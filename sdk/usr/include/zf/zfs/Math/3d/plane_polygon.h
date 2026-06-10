#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <cmath>
#include <memory>

#include <zfs/Math/3d/plane.h>
#include <zfs/Math/math/matrix.h>
#include <zfs/Math/stl/vector_op.h>
#include <zfs/Math/2d/convex_hull.h>

namespace zfs {
    namespace math {
        // =====================================================================

        using vec = std::vector<double>;

        // =====================================================================

        // description:
        // 
        // calculates the area and if desired the center point enclosed by a 
        // contour or convex hull, represented by a vector of ptrs to points.
        // 
        // see also Math/convex_hull.h or math/contour.h
        //
        // note:
        // . the points should have an order such that the region to be circumscribed is not
        //   crossed by a line connecting any two subsequent points of the given set/vector of points.
        // 
        // . point_ptr_t must have the members functions: x(), y(), z(), and pt()
        //   to retrieve the co-ordinates of the points and a point vector in the input vector

        template<typename point_t> struct plane_polygon
        {
            using point_ptr_t = std::shared_ptr<point_t>;
            using point_ptr_vector_t = std::vector<point_ptr_t>;

            enum class region_enclosing_direction_t : unsigned { clockwise, counter_clockwise };

            plane_polygon() = delete;

            plane_polygon(const point_ptr_vector_t& pts_, const plane_t& plane_, bool start_point_replicated_at_end_ = true)
                : pts(pts_)
            {
                // if not replicated, replicate
                if (pts.size() && !start_point_replicated_at_end_)
                {
                    pts.push_back(pts[0]);
                }

                calculate_area(pts, plane_);
            }

            const double& get_area() const { return area; };
            const point_t get_center_point()
            {
                if (!center_point_calculated)
                {
                    calculated_polygon_center_point();
                }
                return center_point;
            }

            region_enclosing_direction_t get_region_enclosing_direction() const
            {
                return region_enclosing_direction;
            }

        private:

            double area{ 0. };
            point_ptr_vector_t pts;
            region_enclosing_direction_t region_enclosing_direction{ region_enclosing_direction_t::clockwise }; // indicates the direction in which the given points describe the given region boundary

            bool center_point_calculated{ false };
            point_t center_point;

            void calculate_area(const point_ptr_vector_t& pts, const plane_t& plane)
            {
                // pts should at least potentially form a triangle
                if (pts.size() > 3)
                {
                    // project points into plane parallel to x-y-axis
                    point_ptr_vector_t projected_pts = project_pts(pts, plane);

                    // calculate area from the given closed edge/border of the region by adding subsequent
                    // area sections from point to point 
                    // area is given, when the curve is given in clockwise order, otherwise negative
                    for (size_t i = 1; i < projected_pts.size(); ++i)
                        area += (projected_pts[i]->y() + projected_pts[i - 1]->y()) * (projected_pts[i]->x() - projected_pts[i - 1]->x()) / 2.;

                    if (area < 0) // polygon was given in counter clockwise order
                    {
                        area = -area;
                        region_enclosing_direction = region_enclosing_direction_t::counter_clockwise;
                    }

                }
            }
            // =====================================================================
            point_ptr_vector_t project_pts(const point_ptr_vector_t& pts, const plane_t& plane)
            {
                // rotate points into plane parallel to x-y-plane
                point_ptr_vector_t projected_pts(pts.size());

                for (size_t i = 0; i < pts.size(); ++i)
                    projected_pts[i] = std::make_shared<point_t>(i);

                matrix<double> rot_matrix(3);

                //vec e_x = plane.get_t1();
                //vec e_y = plane.get_t2();
                //vec e_z = plane.normal;

                rot_matrix.row_vec(0) = plane.get_t1();
                rot_matrix.row_vec(1) = plane.get_t2();
                rot_matrix.row_vec(2) = plane.normal;

                for (size_t i = 0; i < pts.size(); i++)
                    projected_pts[i]->pt = rot_matrix * pts[i]->pt;

                return projected_pts;
            }
            // =====================================================================
            void calculated_polygon_center_point()
            {
                // mind start point is replicated at end
                const size_t n_given = pts.size();
                const size_t n_effective = n_given - 1;

                for (size_t n = 0; n < n_effective; ++n)
                {
                    center_point.x() += pts[n]->x();
                    center_point.y() += pts[n]->y();
                    center_point.z() += pts[n]->z();
                }

                center_point.x() /= n_effective;
                center_point.y() /= n_effective;
                center_point.z() /= n_effective;

                center_point_calculated = true;
            }
        };
        // =====================================================================
    } // end namespace math
} // end namespace zfs
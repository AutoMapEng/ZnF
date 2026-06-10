#pragma once

#include <zfs/zfsapi.h>

#include <cstdlib>
#include <cmath>
#include <list>
#include <vector>
#include <memory>

#include <zfs/Math/stl/vector_op.h>
#include <zfs/Math/math/matrix.h>
#include <zfs/Math/2d/convex_hull.h>
#include <zfs/Math/3d/plane.h>
#include <zfs/Math/3d/plane_polygon.h>

namespace zfs {
    namespace math {

        // ===============================================================================

        using vec = std::vector<double>;

        // ===============================================================================

        struct pt_for_convex_hull_t
        {
            size_t idx{ 0 };
            double u{ 0 }, v{ 0 };
            vec pt{ vec(3,0) };

            pt_for_convex_hull_t() {}
            pt_for_convex_hull_t(size_t i) : idx(i) {}

            double& x() { return pt[0]; }
            double& y() { return pt[1]; }
            double& z() { return pt[2]; }

            const double& x() const { return pt[0]; }
            const double& y() const { return pt[1]; }
            const double& z() const { return pt[2]; }

        };

        using pt_for_convex_hull_ptr_t = std::shared_ptr< pt_for_convex_hull_t >;
        using pt_for_convex_hull_ptr_vector_t = std::vector< pt_for_convex_hull_ptr_t >;

        // ===============================================================================

        template<typename point_t> struct plane_convex_hull : public convex_hull<std::shared_ptr<point_t>>
        {
            using point_ptr_t = std::shared_ptr<point_t>;
            using point_ptr_vector_t = std::vector<point_ptr_t>;

            plane_convex_hull() {}

            plane_convex_hull(const std::vector<point_ptr_t>& pts, const plane_t& plane)
            {
                quickhull(pts, plane);
            }

            void quickhull(const std::vector<point_ptr_t>& pts, const plane_t& plane);

            // area and center point are calculated on first call to functions get_area() and get_center_point()
            const double& get_area();
            const vec& get_center_point();

        private:

            bool center_pt_calculated{ false };
            bool area_calculated{ false };

            double area{ 0. };
            vec poly_center_point{ vec(3, 0) };

            void calculate_area(const point_ptr_vector_t& pts, bool start_point_replicated_at_end);
            void calculated_poly_center_point(const point_ptr_vector_t& pts, bool start_point_replicated_at_end);
            pt_for_convex_hull_ptr_vector_t project_pts(const point_ptr_vector_t& pts, const plane_t& plane);
            double triangle_area(const point_ptr_t& pt1, const point_ptr_t& pt2);
            vec cross(const vec& vec1, const vec& vec2) const;
            double length(const vec& v) const;
            bool pts_are_same(const point_ptr_t& pt1, const point_ptr_t& pt2) const;
        };
        // =====================================================================
        template<typename point_t> void plane_convex_hull<point_t>::quickhull(const point_ptr_vector_t& pts, const plane_t& plane)
        {
            const size_t n_pts = pts.size();

            pt_for_convex_hull_ptr_vector_t projected_pts = project_pts(pts, plane);

#ifdef _DEBUG
            std::vector<double> z_values(n_pts);
#endif

            // assign x and y co-ordinates to pixel co-ordinate variables
            for (size_t i = 0; i < pts.size(); i++)
            {
                projected_pts[i]->u = projected_pts[i]->x();
                projected_pts[i]->v = projected_pts[i]->y();
#ifdef _DEBUG
                z_values[i] = projected_pts[i]->z();
#endif
            }

            // determine convex hull
            convex_hull<pt_for_convex_hull_ptr_t> convex_hull_of_projected_points(projected_pts);

            const size_t n_convex_hull_pts = convex_hull_of_projected_points.convex_hull_pts.size();

            this->convex_hull_pts.reserve(n_convex_hull_pts);

            // result: convex hull of 3d plane: points from input with the same indices as the determined convex hull
            for (size_t i = 0; i < n_convex_hull_pts; ++i)
            {
                for (size_t j = 0; j < n_pts; ++j)
                    if (convex_hull_of_projected_points.convex_hull_pts[i]->idx == pts[j]->idx)
                        this->convex_hull_pts.push_back(pts[j]);
            }
        }
        // =====================================================================
        template<typename point_t> pt_for_convex_hull_ptr_vector_t plane_convex_hull<point_t>::project_pts(const point_ptr_vector_t& pts, const plane_t& plane)
        {
            // rotate points into plane parallel to x-y-plane
            pt_for_convex_hull_ptr_vector_t projected_pts(pts.size());

            for (size_t i = 0; i < pts.size(); ++i)
                projected_pts[i] = std::make_shared<pt_for_convex_hull_t>(i);

            matrix<double> rot_matrix(3);

            //vec e_x = plane.get_t1();
            //vec e_y = plane.get_t2();
            //vec e_z = plane.normal;

            rot_matrix.row_vec(0) = plane.get_t1();
            rot_matrix.row_vec(1) = plane.get_t2();
            rot_matrix.row_vec(2) = plane.normal;

            for (size_t i = 0; i < pts.size(); i++)
                projected_pts[i]->pt = (rot_matrix * pts[i]->pt);

            return projected_pts;
        }
        // =====================================================================
        template<typename point_t> const double& plane_convex_hull<point_t>::get_area()
        {
            if (!area_calculated)
            {
                calculate_area(this->convex_hull_pts, true);
            }

            return area;
        }
        // =====================================================================
        template<typename point_t> const vec& plane_convex_hull<point_t>::get_center_point()
        {
            if (!center_pt_calculated)
            {
                calculated_poly_center_point(this->convex_hull_pts, true);
            }
            return poly_center_point;
        }
        // =====================================================================

        template<typename point_t> void plane_convex_hull<point_t>::calculate_area(const point_ptr_vector_t& pts, bool start_point_replicated_at_end)
        {
            // area calculation is done by adding up triangles formed by the center point
            // and 2 points of the convex hull (alternative approach: see class plane_polygon_area<>)
            if (start_point_replicated_at_end && pts.size() > 3)
            {
                if (!center_pt_calculated)
                    calculated_poly_center_point(pts, start_point_replicated_at_end);

                for (size_t n = 0; n < pts.size() - 1; ++n)
                    area += triangle_area(pts[n], pts[n + 1]);

                area_calculated = true;
            }

            if (!start_point_replicated_at_end && pts.size() > 2)
            {
                if (!center_pt_calculated)
                    calculated_poly_center_point(pts, start_point_replicated_at_end);

                for (size_t n = 0; n < pts.size() - 1; ++n)
                    area += triangle_area(pts[n], pts[n + 1]);

                area += triangle_area(pts[pts.size() - 1], pts[0]);

                area_calculated = true;
            }
        }
        // =====================================================================
        template<typename point_t> void plane_convex_hull<point_t>::calculated_poly_center_point(const point_ptr_vector_t& pts, bool start_point_replicated_at_end)
        {
            if (start_point_replicated_at_end && pts.size() > 3)
            {
                const size_t n_given = pts.size();
                const size_t n_effective = n_given - 1;

                for (size_t n = 0; n < n_effective; ++n)
                {
                    poly_center_point[0] += pts[n]->x();
                    poly_center_point[1] += pts[n]->y();
                    poly_center_point[2] += pts[n]->z();
                }

                poly_center_point[0] /= n_effective;
                poly_center_point[1] /= n_effective;
                poly_center_point[2] /= n_effective;

                center_pt_calculated = true;
            }

            if (!start_point_replicated_at_end && pts.size() > 2)
            {
                const size_t n_given = pts.size();

                for (size_t n = 0; n < n_given; ++n)
                {
                    poly_center_point[0] += pts[n]->x();
                    poly_center_point[1] += pts[n]->y();
                    poly_center_point[2] += pts[n]->z();
                }

                poly_center_point[0] /= n_given;
                poly_center_point[1] /= n_given;
                poly_center_point[2] /= n_given;

                center_pt_calculated = true;
            }
        }
        // =====================================================================
        template<typename point_t> double plane_convex_hull<point_t>::triangle_area(const point_ptr_t& pt1, const point_ptr_t& pt2)
        {
            vec v1(3, 0), v2(3, 0);

            v1[0] = pt1->x() - poly_center_point[0];
            v1[1] = pt1->y() - poly_center_point[1];
            v1[2] = pt1->z() - poly_center_point[2];

            v2[0] = pt2->x() - poly_center_point[0];
            v2[1] = pt2->y() - poly_center_point[1];
            v2[2] = pt2->z() - poly_center_point[2];

            return length(cross(v1, v2)) / 2;
        }
        // =====================================================================
        template<typename point_t> vec plane_convex_hull<point_t>::cross(const vec& v1, const vec& v2) const
        {
            vec cross_prod(3, 0);

            cross_prod[0] = v1[1] * v2[2] - v1[2] * v2[1];
            cross_prod[1] = v1[2] * v2[0] - v1[0] * v2[2];
            cross_prod[2] = v1[0] * v2[1] - v1[1] * v2[0];

            return cross_prod;
        }
        // =====================================================================
        template<typename point_t> double plane_convex_hull<point_t>::length(const vec& v) const
        {
            return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }
        // =====================================================================
        template<typename point_t> bool plane_convex_hull<point_t>::pts_are_same(const point_ptr_t& pt1, const point_ptr_t& pt2) const
        {
            return pt1->x() == pt2->x() && pt1->y() == pt2->y() && pt1->z() == pt2->z();
        }
        // =====================================================================
    } // end namespace math
} // end namespace zfs
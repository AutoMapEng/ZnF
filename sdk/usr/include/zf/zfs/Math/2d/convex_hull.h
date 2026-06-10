#pragma once

#include <zfs/zfsapi.h>

#include <cstdlib>
#include <cmath>
#include <list>
#include <vector>
#include <memory>

namespace zfs 
{
    namespace math {
        // ===============================================================================
        template<typename point_ptr_t> struct convex_hull
        {
            std::vector<point_ptr_t> convex_hull_pts;

            convex_hull() {}

            convex_hull(const std::vector<point_ptr_t>& pts)
            {
                quickhull(pts);
            }

            void quickhull(const std::vector<point_ptr_t>& pts);

        };
        // ===============================================================================
        template<typename point_ptr_t> using convex_hull_ptr_t = std::shared_ptr<convex_hull<point_ptr_t>>;
        template<typename point_ptr_t> using convex_hull_ptr_vector_t = std::vector<convex_hull_ptr_t<point_ptr_t>>;
        // ===============================================================================
        using v_data_t = std::pair<int64_t, int64_t>;
        // ===============================================================================
        template<typename point_ptr_t> struct v_t
        {
            v_t() {};

            v_t(point_ptr_t pt1, point_ptr_t pt2)
            {
                u = double(pt2->u) - pt1->u;
                v = double(pt2->v) - pt1->v;
            }

            double norm() const
            {
                return std::sqrt(u * u + v * v);
            }

            double u{ 0 }, v{ 0 };
        };
        // ===============================================================================
        template<typename point_ptr_t> double z_component_cross_2d(const v_t<point_ptr_t>& v1_, const v_t<point_ptr_t>& v2_)
        {
            return v1_.u * v2_.v - v1_.v * v2_.u;
        }
        // ===============================================================================
        template<typename point_ptr_t> struct contour_side_t
        {
            point_ptr_t start_point{ nullptr }; point_ptr_t end_point{ nullptr };
            point_ptr_t max_dist_pt{ nullptr };
            std::vector<point_ptr_t> pt_list;
        };
        // ===============================================================================
        template<typename point_ptr_t> using contour_side_vector_t = std::vector<contour_side_t<point_ptr_t>>;
        template<typename point_ptr_t> using contour_side_ptr_list_t = std::list<std::shared_ptr<contour_side_t<point_ptr_t>>>;
        // ===============================================================================
        template<typename point_ptr_t> void collect_contour_side_data(point_ptr_t start_point, point_ptr_t end_point, const std::vector<point_ptr_t>& pt_list, contour_side_t<point_ptr_t>& contour_side_data)
        {

            const size_t n_pts = pt_list.size();

            double cur_dist = 0, max_dist = 0;

            v_t<point_ptr_t> contour_side_vector(start_point, end_point);

            contour_side_data.start_point = start_point;
            contour_side_data.end_point = end_point;

            for (size_t pt_idx = 0; pt_idx < n_pts; ++pt_idx)
            {
                point_ptr_t cur_pt = pt_list[pt_idx];

                if (cur_pt != start_point && cur_pt != end_point)
                {
                    double cross_prod = z_component_cross_2d(contour_side_vector, v_t<point_ptr_t>(start_point, cur_pt));

                    if (cross_prod > 0) // point on the desired side
                    {
                        // add point to point list
                        contour_side_data.pt_list.push_back(cur_pt);
                        // calculate distance and add to distance list
                        cur_dist = cross_prod / contour_side_vector.norm();
                        if (cur_dist > max_dist)
                        {
                            max_dist = cur_dist;
                            contour_side_data.max_dist_pt = cur_pt;
                        }
                    }
                }
            }
        }
        // ===============================================================================
        template<typename point_ptr_t> void convex_hull<point_ptr_t>::quickhull(const std::vector<point_ptr_t>& pts)
        {
            convex_hull_pts.clear();

            if (!pts.empty())
            {
                // find first points for convex hull
                const size_t n_pts = pts.size();

                size_t min_u_idx = 0;
                size_t max_u_idx = 0;

                for (size_t pt_idx = 1; pt_idx < n_pts; ++pt_idx)
                {
                    if (pts[pt_idx]->u < pts[min_u_idx]->u)
                    {
                        min_u_idx = pt_idx;
                    }
                    else if (pts[pt_idx]->u == pts[min_u_idx]->u && pts[pt_idx]->v < pts[min_u_idx]->v)
                    {
                        min_u_idx = pt_idx;
                    }

                    if (pts[pt_idx]->u > pts[max_u_idx]->u)
                    {
                        max_u_idx = pt_idx;
                    }
                    else if (pts[pt_idx]->u == pts[max_u_idx]->u && pts[pt_idx]->v > pts[max_u_idx]->v)
                    {
                        max_u_idx = pt_idx;
                    }
                }

                contour_side_vector_t<point_ptr_t> contour_side_vector(2);

                std::shared_ptr<contour_side_t<point_ptr_t>> first_contour_side = std::make_shared<contour_side_t<point_ptr_t>>(), second_contour_side = std::make_shared<contour_side_t<point_ptr_t>>();

                collect_contour_side_data(pts[min_u_idx], pts[max_u_idx], pts, *first_contour_side);
                collect_contour_side_data(pts[max_u_idx], pts[min_u_idx], pts, *second_contour_side);

                contour_side_ptr_list_t<point_ptr_t> contour_side_list;

                contour_side_list.push_back(first_contour_side);
                contour_side_list.push_back(second_contour_side);

                bool b_added_point = true;

                while (b_added_point)
                {
                    b_added_point = false;

                    auto next_contour_side = contour_side_list.begin(); ++next_contour_side;

                    for (auto contour_it = contour_side_list.begin(); contour_it != contour_side_list.end(); )
                    {
                        if (!(*contour_it)->pt_list.empty())
                        {
                            std::shared_ptr<contour_side_t<point_ptr_t>> new_contour_side_1 = std::make_shared<contour_side_t<point_ptr_t>>(), new_contour_side_2 = std::make_shared<contour_side_t<point_ptr_t>>();

                            collect_contour_side_data((*contour_it)->start_point, (*contour_it)->max_dist_pt, (*contour_it)->pt_list, *new_contour_side_1);
                            collect_contour_side_data((*contour_it)->max_dist_pt, (*contour_it)->end_point, (*contour_it)->pt_list, *new_contour_side_2);

                            contour_side_list.erase(contour_it);
                            contour_side_list.insert(next_contour_side, new_contour_side_1);
                            contour_side_list.insert(next_contour_side, new_contour_side_2);

                            b_added_point = true;
                        }

                        contour_it = next_contour_side;
                        if (next_contour_side != contour_side_list.end()) ++next_contour_side;
                    }
                }

                // collect output information
                const size_t n_contour_sides = contour_side_list.size();
                size_t contour_idx = 0;
                convex_hull_pts.resize(n_contour_sides + 1);
                for (auto contour_it = contour_side_list.begin(); contour_it != contour_side_list.end(); ++contour_it, ++contour_idx)
                {
                    convex_hull_pts[contour_idx] = (*contour_it)->start_point;
                    if (contour_idx == (n_contour_sides - 1))
                        convex_hull_pts[contour_idx + 1] = (*contour_it)->end_point;
                }

            }
        }
        // ===============================================================================
    } // end namespace math
} // end namespace zfs
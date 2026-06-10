#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>
#include <vector>
#include <utility>

#include "coordinate.h"
#include <zfs/Math/math/bool_matrix.h>
#include <zfs/Math/3d/pt_matrix.h>

namespace zfs {
    namespace math {
        // =====================================================================================================================================
        struct contour_tracing
        {
            contour_tracing() {}
            contour_tracing(const bool_matrix_t& image_matrix_, const coordinate_t& xs_, bool reduce_pts = false, int d = 0)  // d = 0: outer contour, d= 1: inner contour
            {
                trace_contour(image_matrix_, xs_, reduce_pts, d);
            }

            void trace_contour(const bool_matrix_t& image_matrix_, const coordinate_t& xs_, bool reduce_pts = false, int d = 0)
            {
                contour_pts.clear();

                contour_pts.push_back(xs_);

                coordinate_t xt = find_next_point(xs_, image_matrix_, d);

                bool done = (xs_ == xt);

                if (!done)
                {
                    contour_pts.push_back(xt);

                    coordinate_t xp = xs_;
                    coordinate_t xc = xt;

                    coordinate_t xn;

                    while (!done)
                    {
                        d = (d + 6) % 8;
                        xn = find_next_point(xc, image_matrix_, d);
                        xp = xc;
                        xc = xn;
                        done = xp == xs_ && xc == xt;
                        if (!done)
                            contour_pts.push_back(xn);

                    }
                }

                if (reduce_pts)
                    reduce_points();
            }

            const std::list<coordinate_t>& get_contour() const
            {
                return contour_pts;
            }

        private:

            std::list<coordinate_t> contour_pts;
            coordinate_t delta;


            coordinate_t find_next_point(const coordinate_t& xc, const bool_matrix_t& image_matrix_, int& d)
            {

                coordinate_t x_;

                for (int i = 0; i < 6; ++i)
                {
                    x_ = xc + get_delta(d);

                    if (x_.v() < 0 || x_.u() < 0 || !image_matrix_(x_.v(), x_.u()))
                    {
                        d = (d + 1) % 8;
                    }
                    else
                    {
                        return x_;
                    }
                }

                return xc;
            }

            const coordinate_t& get_delta(int d)
            {
                if (d == 0)      delta = coordinate_t(1, 0);
                else if (d == 1) delta = coordinate_t(1, 1);
                else if (d == 2) delta = coordinate_t(0, 1);
                else if (d == 3) delta = coordinate_t(-1, 1);
                else if (d == 4) delta = coordinate_t(-1, 0);
                else if (d == 5) delta = coordinate_t(-1, -1);
                else if (d == 6) delta = coordinate_t(0, -1);
                else if (d == 7) delta = coordinate_t(1, -1);

                return delta;
            }

            void reduce_points() // deletes pts that form linear elements
            {
                size_t n_contour_pts = contour_pts.size();

                std::vector<bool> delete_indicator(n_contour_pts, false);

                size_t /*pt_idx0 = 0,*/ pt_idx1 = 1, pt_idx2 = 2;

                auto c_pts_it_0 = contour_pts.begin();
                auto c_pts_it_1 = contour_pts.begin(); ++c_pts_it_1;
                auto c_pts_it_2 = contour_pts.begin(); ++c_pts_it_2; ++c_pts_it_2;

                while (pt_idx2 < n_contour_pts)
                {
                    if ((*c_pts_it_2).u() - (*c_pts_it_1).u() == (*c_pts_it_1).u() - (*c_pts_it_0).u() &&
                        (*c_pts_it_2).v() - (*c_pts_it_1).v() == (*c_pts_it_1).v() - (*c_pts_it_0).v())
                    {
                        delete_indicator[pt_idx1] = true;
                    }

                    ++c_pts_it_0;
                    ++c_pts_it_1;
                    ++c_pts_it_2;

                    pt_idx1++; pt_idx2++;
                }

                auto c_pt_it_search = contour_pts.begin(); ++c_pt_it_search;

                for (size_t i = 1; i < n_contour_pts; ++i, ++c_pt_it_search)
                {
                    if (delete_indicator[i])
                    {
                        auto c_pt_it_delete_start = c_pt_it_search;
                        auto c_pt_it_delete_end = c_pt_it_search; ++c_pt_it_delete_end;

                        ++i;
                        while (i < n_contour_pts && delete_indicator[i])
                        {
                            ++c_pt_it_delete_end;
                            ++i;
                        }

                        c_pt_it_search = contour_pts.erase(c_pt_it_delete_start, c_pt_it_delete_end);

                    }
                }

            }
        };
        // =====================================================================================================================================
    } // end namespace math
} // end namespace zfs
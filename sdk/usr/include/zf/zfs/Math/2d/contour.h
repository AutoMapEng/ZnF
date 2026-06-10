#pragma once

#include <zfs/zfsapi.h>

#include <cstdlib>
#include <cmath>
#include <memory>
#include <algorithm>
#include <list>
#include <vector>
#include <set>
#include <list>

#include <zfs/Math/object/multioptions.h>

#include "coordinate.h"
#include <zfs/Math/3d/pt_matrix.h>

namespace zfs {
    namespace math {

        // ===============================================================================
        template<typename pt_ptr_t> struct contour
        {
            std::list<pt_ptr_t> contour_pts;
            pt_matrix_t<pt_ptr_t> pt_matrix;

            enum class mode_t { no_subsample, subsample, selfsubsample };
            enum class option_t { reduce_points, smooth_contour }; // < option smooth_contour: tbd

            struct contour_parameter_t
            {
                std::set<option_t> all_options_set = { option_t::reduce_points, option_t::smooth_contour };
                multi_options<option_t> options{ multi_options<option_t>(all_options_set,{}) }; // options

                mode_t mode{ mode_t::no_subsample }; // mode, default: no_subsample

                uint32_t subsample{ 0 }; // < subsampling, init 0
                uint32_t dsubmax{ 0 }; // < maximum extend before subsampling is done ( used for mode_t::selfsubsample},
                                        // base for computing the subsampling factor, init 0

                contour_parameter_t()
                {
                    options.set_options({});
                }
                contour_parameter_t(const std::set<option_t>& options_)
                {
                    options.set_options(options_);
                }
                contour_parameter_t(const mode_t& mode_, const std::set<option_t>& options_, uint32_t subsample_param_ = 0) : mode(mode_)
                {
                    options.set_options(options_);

                    switch (mode)
                    {
                    case mode_t::subsample:
                    {
                        subsample = subsample_param_;
                        break;
                    }
                    case mode_t::selfsubsample:
                    {
                        dsubmax = subsample_param_;
                        break;
                    }
                    default:
                    {
                        mode = mode_t::no_subsample;
                    }
                    }
                }

            } contour_parameter;
            // ===============================================================================
            contour() {}
            // ===============================================================================
            contour(const contour_parameter_t& contour_parameter_) : contour_parameter(contour_parameter_)
            {}
            // ===============================================================================
            contour(const std::vector<pt_ptr_t>& pt_list_, const contour_parameter_t& contour_parameter_) : contour_parameter(contour_parameter_)
            {
                find_contour(pt_list_);
            }
            // ===============================================================================
            coordinate_t xs;
            pt_ptr_t start_pt;
            void find_contour(const std::vector<pt_ptr_t>& pt_list_, const contour_parameter_t& contour_parameter_)
            {
                contour_parameter = contour_parameter_;

                if (contour_parameter.mode == mode_t::no_subsample)
                    start_pt = pt_matrix.set(pt_list_, xs);
                else
                    start_pt = pt_matrix.set(pt_list_, xs, contour_parameter.subsample, contour_parameter.dsubmax);

                trace_contour(xs);

#if 0
                if (contour_parameter.options.option_set(option_t::smooth_contour))
                    smooth_contour();
#endif

                if (contour_parameter.options.option_set(option_t::reduce_points))
                    reduce_points();
            }
            // ===============================================================================
            void find_contour(const std::vector<pt_ptr_t>& pt_list_)
            {
                if (contour_parameter.mode == mode_t::no_subsample)
                    start_pt = pt_matrix.set(pt_list_, xs);
                else
                    start_pt = pt_matrix.set(pt_list_, xs, contour_parameter.subsample, contour_parameter.dsubmax);

                trace_contour(xs);

#if 0
                if (contour_parameter.options.option_set(option_t::smooth_contour))
                    smooth_contour();
#endif

                if (contour_parameter.options.option_set(option_t::reduce_points))
                    reduce_points();
            }
            // ===============================================================================
        private:

            // ===============================================================================
            bool done{ false };
            coordinate_t xt, xp, xc, xn;
            void trace_contour(const coordinate_t& xs_, int d = 0)
            {
                contour_pts.clear();

                contour_pts.push_back(pt_matrix(xs_.v(), xs_.u()));

                xt = find_next_point(xs_, d);

                done = (xs_ == xt);

                if (!done)
                {
                    contour_pts.push_back(pt_matrix(xt.v(), xt.u()));

                    xp = xs_;
                    xc = xt;

                    while (!done)
                    {
                        d = (d + 6) % 8;
                        xn = find_next_point(xc, d);
                        xp = xc;
                        xc = xn;
                        done = xp == xs_ && xc == xt;
                        if (!done)
                            contour_pts.push_back(pt_matrix(xn.v(), xn.u()));

                    }
                }
            }
            // ===============================================================================
            coordinate_t x_;
            coordinate_t find_next_point(const coordinate_t& xc, int& d)
            {

                for (int i = 0; i < 6; ++i)
                {
                    const coordinate_t& delta = get_delta(d);

                    int64_t cur_u = xc.u() + delta.u();
                    int64_t cur_v = xc.v() + delta.v();

                    if (cur_v < 0 || cur_u < 0 || !pt_matrix(cur_v, cur_u))
                    {
                        d = (d + 1) % 8;
                    }
                    else
                    {
                        x_.set(cur_u, cur_v);
                        return x_;
                    }
                }

                return xc;
            }
            // ===============================================================================
            coordinate_t d0{ coordinate_t(1,  0) };
            coordinate_t d1{ coordinate_t(1,  1) };
            coordinate_t d2{ coordinate_t(0,  1) };
            coordinate_t d3{ coordinate_t(-1,  1) };
            coordinate_t d4{ coordinate_t(-1,  0) };
            coordinate_t d5{ coordinate_t(-1, -1) };
            coordinate_t d6{ coordinate_t(0, -1) };
            coordinate_t d7{ coordinate_t(1, -1) };
            coordinate_t d_{ coordinate_t(0,  0) };

            const coordinate_t& get_delta(int d)
            {
                if (d == 0)      return d0;
                else if (d == 1) return d1;
                else if (d == 2) return d2;
                else if (d == 3) return d3;
                else if (d == 4) return d4;
                else if (d == 5) return d5;
                else if (d == 6) return d6;
                else if (d == 7) return d7;

                return d_;
            }
            // ===============================================================================
            std::vector<bool> delete_indicator;

            void reduce_points()
            {
                size_t n_contour_pts = contour_pts.size();

                if (n_contour_pts >= 3)
                {

                    delete_indicator.resize(n_contour_pts);
                    for (size_t n = 0; n < n_contour_pts; ++n) delete_indicator[n] = false;

                    size_t /*pt_idx0 = 0,*/ pt_idx1 = 1, pt_idx2 = 2;

                    auto c_pts_it_0 = contour_pts.begin();
                    auto c_pts_it_1 = contour_pts.begin(); ++c_pts_it_1;
                    auto c_pts_it_2 = contour_pts.begin(); ++c_pts_it_2; ++c_pts_it_2;

                    while (pt_idx2 < n_contour_pts)
                    {
                        if ((*c_pts_it_2)->u - (*c_pts_it_1)->u == (*c_pts_it_1)->u - (*c_pts_it_0)->u &&
                            (*c_pts_it_2)->v - (*c_pts_it_1)->v == (*c_pts_it_1)->v - (*c_pts_it_0)->v)
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
            }
            // ===============================================================================
            double manhattan_distance(const pt_ptr_t& pt1, const pt_ptr_t& pt2) // < ... or: L1-distance
            {
                return std::abs(double(pt2->u) - pt1->u) + std::abs(double(pt2->v) - pt1->v);
            }

            using pt_ptr_list = std::list<pt_ptr_t>;

            double manhattan_distance(const pt_ptr_list& pt_ptr_list_) // < ... or: L1-distance
            {
                double distance = 0;

                if (pt_ptr_list_.size() > 1)
                {
                    auto pt_it1 = pt_ptr_list_.begin();
                    auto pt_it2 = pt_ptr_list_.begin(); pt_it2++;

                    while (pt_it2 != pt_ptr_list_.end())
                    {
                        distance += manhattan_distance(*pt_it1, *pt_it2);
                        ++pt_it1; ++pt_it2;
                    }

                }

                return distance;
            }

            void smooth_contour()
            {
                size_t n_contour_pts = contour_pts.size();

                if (n_contour_pts >= 3)
                {

                    delete_indicator.resize(n_contour_pts);
                    for (size_t n = 0; n < n_contour_pts; ++n) delete_indicator[n] = false;

                    size_t /*pt_idx0 = 0,*/ pt_idx1 = 1, pt_idx2 = 2;

                    auto c_pts_it_0 = contour_pts.begin();
                    auto c_pts_it_1 = contour_pts.begin(); ++c_pts_it_1;
                    auto c_pts_it_2 = contour_pts.begin(); ++c_pts_it_2; ++c_pts_it_2;

                    while (pt_idx2 < n_contour_pts)
                    {
                        pt_ptr_list pt_list = { *c_pts_it_0 , *c_pts_it_1, *c_pts_it_2 };

                        if (manhattan_distance(pt_list) > manhattan_distance(*c_pts_it_0, *c_pts_it_1))
                            delete_indicator[pt_idx1] = true;

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
            }
            // ===============================================================================
        };
        // ===============================================================================
    } // end namespace math
} // end namespace zfs
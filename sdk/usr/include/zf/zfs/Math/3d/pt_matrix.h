#pragma once

#include <zfs/zfsapi.h>

#include <cstdlib>
#include <cmath>
#include <memory>
#include <algorithm>
#include <list>
#include <vector>

#include <zfs/Math/math/bool_matrix.h>

namespace zfs {
    namespace math {
        // ===============================================================================
        template<typename pt_ptr_t> bool cmp_u(pt_ptr_t pt1, pt_ptr_t pt2)
        {
            return pt1->u < pt2->u;
        }
        // ===============================================================================
        template<typename pt_ptr_t> bool cmp_v(pt_ptr_t pt1, pt_ptr_t pt2)
        {
            return pt1->v < pt2->v;
        }
        // ===============================================================================
        // class for managing (shared) pointers to structures of pixel image/scan information
        // including matrix co-ordinates (u,v) of type uint32_t in a matrix data structure.
        // The structure may additionally contain, e.g. real world 3d co-ordinates (x,y,z)
        // in order to derive and later define a plane enclosing curve - besides its normal
        // and plane offset point, e.g. a convex hull or a contour in 3d space 
        // (see e.g. class convex_hull<point_ptr_t> or contour<point_ptr_t>.)
        //
        // operations include:
        // . setting the matrix from a vector of (shared) pointers to structures containing 
        //   image co-ordinates / pixel positions (u,v). The first () [line, then column wise] 
        //   position and pointer to image pixel data structure is returned.
        // 
        // . deleting pointers from the mentionen vector by allowing to subsample the described
        //   points in the corresponding data structures by a given subsampling factor,
        //   thereby reducing the needed matrix and hence memory size before inserting the
        //   pointers into the matrix.
        //
        // . access operators and functions
        //
        // . possibility to get a occupation indicator for the matrix, showing which
        //   pixels are actually set/pointing to image pixel information structures.
        // ===============================================================================
        template<typename pt_ptr_t> class pt_matrix_t
        {
            using pt_matrix_line_t = std::vector<pt_ptr_t>;
            using pt_matrix_data_t = std::vector<pt_matrix_line_t>;

            const pt_ptr_t empty_dummy;

            uint64_t m_rows{ 0 }, m_cols{ 0 };
            pt_matrix_data_t pt_matrix_data;
            bool_matrix_t indicator_matrix; // < only on request

        public:
            pt_matrix_t() {}

            pt_matrix_t(uint64_t rows, uint64_t cols)
            {
                resize(rows, cols);
            }

            void resize(uint64_t rows, uint64_t cols)
            {
                if (rows)
                {
                    bool is_new = pt_matrix_data.size() == 0 ? true : false;

                    if (is_new)
                    {
                        pt_matrix_data.resize(rows);
                        for (uint64_t i = 0; i < rows; i++)
                            pt_matrix_data[i].resize(cols);
                    }
                    else
                    {
                        uint64_t previous_rows = pt_matrix_data.size();
                        uint64_t previous_cols = pt_matrix_data[0].size();

                        if (rows > previous_rows)
                        {
                            pt_matrix_data.resize(rows);
                            for (uint64_t i = 0; i < rows; i++)
                            {
                                pt_matrix_data[i].resize(cols);
                                for (uint64_t j = 0; j < cols; j++)
                                    pt_matrix_data[i][j] = {};
                            }
                        }
                        else if (cols > previous_cols)
                        {
                            for (uint64_t i = 0; i < previous_rows; i++)
                            {
                                pt_matrix_data[i].resize(cols);
                                for (uint64_t j = 0; j < cols; j++)
                                    pt_matrix_data[i][j] = {};
                            }
                        }
                        else
                        {
                            for (uint64_t i = 0; i < rows; i++)
                                for (uint64_t j = 0; j < cols; j++)
                                    pt_matrix_data[i][j] = {};
                        }
                    }
                    m_rows = rows; m_cols = cols;
                }
            }

            std::vector<pt_ptr_t> subsample_pt_vector(const std::vector<pt_ptr_t>& pt_list_, uint32_t min_u, uint32_t min_v, uint32_t subsample)
            {
                std::vector<pt_ptr_t> subsampled;

                subsampled.reserve(pt_list_.size() / subsample);

                for (auto& pt_ptr : pt_list_)
                {
                    if (((pt_ptr->u - min_u) % subsample == 0) && ((pt_ptr->v - min_v) % subsample == 0))
                        subsampled.push_back(pt_ptr);
                }

                return subsampled;
            }

            const pt_ptr_t& set(const std::vector<pt_ptr_t>& pt_list_, coordinate_t& xs, uint32_t subsample, uint32_t dsubmax = 0) // < xs : start point position in matrix
            {
                auto min_pt_v = *std::min_element(pt_list_.begin(), pt_list_.end(), cmp_v<pt_ptr_t>);
                auto min_pt_u = *std::min_element(pt_list_.begin(), pt_list_.end(), cmp_u<pt_ptr_t>);

                if (subsample == 0) // self subsample
                {

                    subsample = 1;

                    auto max_pt_v = *std::max_element(pt_list_.begin(), pt_list_.end(), cmp_v<pt_ptr_t>);
                    auto max_pt_u = *std::max_element(pt_list_.begin(), pt_list_.end(), cmp_u<pt_ptr_t>);

                    uint32_t dv = max_pt_v->v - min_pt_v->v + 1;
                    uint32_t du = max_pt_u->u - min_pt_u->u + 1;

                    uint32_t dmin = std::min(dv, du);

                    if (dmin && dmin > dsubmax)
                    {
                        subsample = dmin / dsubmax + 1;
                    }

                }

                std::vector<pt_ptr_t> subsampled_pt_vector = subsample_pt_vector(pt_list_, min_pt_u->u, min_pt_v->v, subsample);

                auto max_pt_v = *std::max_element(subsampled_pt_vector.begin(), subsampled_pt_vector.end(), cmp_v<pt_ptr_t>);
                auto max_pt_u = *std::max_element(subsampled_pt_vector.begin(), subsampled_pt_vector.end(), cmp_u<pt_ptr_t>);

                uint32_t upper_left_v = min_pt_v->v;
                uint32_t upper_left_u = min_pt_u->u;

                uint64_t indicator_matrix_rows = (max_pt_v->v - upper_left_v) / subsample + 1;
                uint64_t indicator_matrix_columns = (max_pt_u->u - upper_left_u) / subsample + 1;

                this->resize(indicator_matrix_rows, indicator_matrix_columns);

                size_t u_start_point = max_pt_u->u; // < search

                for (auto pt_it = subsampled_pt_vector.begin(); pt_it != subsampled_pt_vector.end(); ++pt_it)
                {
                    auto cur_pt_ptr = *pt_it;

                    uint32_t cur_v = cur_pt_ptr->v, cur_u = cur_pt_ptr->u;

                    pt_matrix_data[(cur_v - upper_left_v) / subsample][(cur_u - upper_left_u) / subsample] = cur_pt_ptr;

                    if (cur_v == upper_left_v && cur_u < u_start_point)
                        u_start_point = cur_u;
                }

                xs.set((u_start_point - upper_left_u) / subsample, 0);

                return pt_matrix_data[xs.v()][xs.u()]; // < return start point
            }

            const pt_ptr_t& set(const std::vector<pt_ptr_t>& pt_list_, coordinate_t& xs) // < xs : start point position in matrix
            {
                auto min_pt_v = *std::min_element(pt_list_.begin(), pt_list_.end(), cmp_v<pt_ptr_t>);
                auto max_pt_v = *std::max_element(pt_list_.begin(), pt_list_.end(), cmp_v<pt_ptr_t>);
                auto min_pt_u = *std::min_element(pt_list_.begin(), pt_list_.end(), cmp_u<pt_ptr_t>);
                auto max_pt_u = *std::max_element(pt_list_.begin(), pt_list_.end(), cmp_u<pt_ptr_t>);

                uint32_t upper_left_v = min_pt_v->v;
                uint32_t upper_left_u = min_pt_u->u;

                uint64_t indicator_matrix_rows = max_pt_v->v - upper_left_v + 1;
                uint64_t indicator_matrix_columns = max_pt_u->u - upper_left_u + 1;

                this->resize(indicator_matrix_rows, indicator_matrix_columns);

                size_t u_start_point = max_pt_u->u; // < search

                for (auto pt_it = pt_list_.begin(); pt_it != pt_list_.end(); ++pt_it)
                {
                    auto cur_pt_ptr = *pt_it;

                    uint32_t cur_v = cur_pt_ptr->v, cur_u = cur_pt_ptr->u;

                    pt_matrix_data[cur_v - upper_left_v][cur_u - upper_left_u] = cur_pt_ptr;

                    if (cur_v == upper_left_v && cur_u < u_start_point)
                        u_start_point = cur_u;
                }

                xs.set(u_start_point - upper_left_u, 0);

                return pt_matrix_data[xs.v()][xs.u()]; // < return start point
            }

            const bool_matrix_t& get_indicator_matrix()
            {
                uint64_t Rows = rows();
                uint64_t Cols = columns();

                indicator_matrix.resize(Rows, Cols);

                for (uint64_t row = 0; row < Rows; ++row)
                    for (uint64_t col = 0; col < Cols; ++col)
                        if (pt_matrix_data[row][col])
                            indicator_matrix.set(row, col, true);
                        else
                            indicator_matrix.set(row, col, false);

                return indicator_matrix;
            }

            pt_matrix_line_t& operator[](uint64_t line_idx)
            {
                return pt_matrix_data[line_idx];
            }

            const pt_matrix_line_t& operator[](uint64_t line_idx) const
            {
                return pt_matrix_data[line_idx];
            }

            const pt_ptr_t& operator()(const uint64_t& line_idx, const uint64_t& row_idx) const
            {
                if (inside(line_idx, row_idx))
                    return pt_matrix_data[line_idx][row_idx];
                else
                    return empty_dummy;
            }

            uint64_t rows() const
            {
                return m_rows;
            }

            uint64_t columns() const
            {
                return m_cols;
            }

            bool inside(const uint64_t& row, const uint64_t& col) const
            {
                return row < m_rows&& col < m_cols;
            }

        };
        // ===============================================================================
    } // end namespace math
} // end namespace zfs
#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>
#include <vector>
#include <utility>

namespace zfs {
    namespace math {
        // =====================================================================================================================================
        class bool_matrix_t
        {
            using bool_line_data_t = std::vector< bool >;
            using bool_matrix_data_t = std::vector< bool_line_data_t >;

            uint64_t           m_rows{ 0 }, m_cols{ 0 };
            bool_matrix_data_t m_data;

            bool inside(const uint64_t& row, const uint64_t& col) const
            {
                return row < m_rows&& col < m_cols;
            }

        public:
            bool_matrix_t() {}
            bool_matrix_t(uint64_t rows, uint64_t cols) {
                resize(rows, cols);
            }

            void resize(uint64_t rows, uint64_t cols)
            {
                if (rows)
                {
                    bool is_new = m_data.size() == 0 ? true : false;

                    if (is_new)
                    {
                        m_data.resize(rows);
                        for (uint64_t i = 0; i < rows; i++)
                            m_data[i].resize(cols, false);
                    }
                    else
                    {
                        uint64_t previous_rows = m_data.size();
                        uint64_t previous_cols = m_data[0].size();

                        if (rows > previous_rows)
                        {
                            m_data.resize(rows);
                            for (uint64_t i = 0; i < rows; i++)
                            {
                                m_data[i].resize(cols);
                                for (uint64_t j = 0; j < cols; j++)
                                    m_data[i][j] = false;
                            }
                        }
                        else if (cols > previous_cols)
                        {
                            for (uint64_t i = 0; i < previous_rows; i++)
                            {
                                m_data[i].resize(cols);
                                for (uint64_t j = 0; j < cols; j++)
                                    m_data[i][j] = false;
                            }
                        }
                        else
                        {
                            for (uint64_t i = 0; i < rows; i++)
                                for (uint64_t j = 0; j < cols; j++)
                                    m_data[i][j] = false;
                        }
                    }

                    m_rows = rows; m_cols = cols; // < for inside check
                }
            }

            bool operator()(const uint64_t& row, const uint64_t& col) const
            {
                if (inside(row, col))
                    return m_data[row][col];
                else
                    return false;
            }

            void set(const uint64_t& row, const uint64_t& col, bool value)
            {
                if (inside(row, col))
                    m_data[row][col] = value;
            }


        };
        // =====================================================================================================================================
    } // end namespace math
} // end namespace zfs
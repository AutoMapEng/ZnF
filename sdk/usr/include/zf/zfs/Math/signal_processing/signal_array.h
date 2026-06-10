#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <complex>
#include <memory>

namespace zfs
{
    namespace math
    {

        // =================================================================

        template<class T> class signal_array_2D
        {
        public:

            using array_line = std::vector<T>;

            signal_array_2D() = default;

            signal_array_2D(const int& columns);
            signal_array_2D(const int& rows, const int& columns);

            // set size (when using default constructor, or when resize necessary)
            void set(const int& columns);
            void set(const int& rows, const int& columns);

            // set from vector (after setting size using previous set functions
            void set(const array_line& data_vector);

            // set to value (after setting size using previous set functions
            signal_array_2D& operator=(const T& value);

            // enquire sizes
            int rows() const;
            int cols() const;

            // access to elements
            const T& operator()(const int& row_idx, const int& col_idx) const;
            T& operator()(const int& row_idx, const int& col_idx);

            // access to rows
            const array_line& operator[](const int& row_idx) const;
            array_line& operator[](const int& row_idx);

            // access to cols
            array_line col_vec(const int& col_idx) const;
            void col_vec(const int& col_idx, array_line& column_values) const;
            void set_col_vec(const int& col_idx, const array_line& column_values);

            // transpose
            void transp(signal_array_2D& transposed) const;
            signal_array_2D transp() const;
            void transp_inplace();

            // spectrum as vector
            array_line get_as_vector() const;

        private:

            std::vector< array_line > array_data;

        };

        // =================================================================
        template<class T> signal_array_2D<T>::signal_array_2D(const int& n)
        {
            set(n);
        }
        // =================================================================
        template<class T> signal_array_2D<T>::signal_array_2D(const int& m, const int& n)
        {
            set(m, n);
        }
        // =================================================================
        template<class T> void signal_array_2D<T>::set(const int& n)
        {
            set(n, n);
        }
        // =================================================================
        template<class T> void signal_array_2D<T>::set(const int& m, const int& n)
        {
            if (m != rows() || n != cols())
            {
                array_data.resize(m);
                for (int k = 0; k < m; ++k)
                    array_data[k].resize(n);
            }
        }
        // =================================================================
        template<class T> void signal_array_2D<T>::set(const std::vector<T>& data_vector)
        {
            for (int k = 0, line_offset = 0; k < rows(); ++k, line_offset += cols())
            {
                auto copy_start_pos = data_vector.begin() + line_offset;

                if (copy_start_pos + cols() <= data_vector.end())
                    std::copy(copy_start_pos, copy_start_pos + cols(), array_data[k].begin());
                else
                    std::copy(copy_start_pos, copy_start_pos + cols(), array_data[k].begin());
            }
        }
        // =================================================================
        template<class T> signal_array_2D<T>& signal_array_2D<T>::operator=(const T& value)
        {
            for (int k = 0; k < rows(); ++k)
                for (int i = 0; i < cols(); ++i)
                    array_data[k][i] = value;

            return *this;
        }
        // =================================================================
        template<class T> int signal_array_2D<T>::rows() const
        {
            return static_cast<int>(array_data.size());
        }
        // =================================================================
        template<class T> int signal_array_2D<T>::cols() const
        {
            if (rows() > 0)
                return static_cast<int>(array_data[0].size());
            else
                return 0;
        }
        // =================================================================
        template<class T> const T& signal_array_2D<T>::operator()(const int& m, const int& n) const
        {
            return array_data[m][n];
        }
        // =================================================================
        template<class T> T& signal_array_2D<T>::operator()(const int& m, const int& n)
        {
            return array_data[m][n];
        }
        // =================================================================
        template<class T> const typename signal_array_2D<T>::array_line& signal_array_2D<T>::operator[](const int& m) const
        {
            return array_data[m];
        }
        // =================================================================
        template<class T> typename signal_array_2D<T>::array_line& signal_array_2D<T>::operator[](const int& m)
        {
            return array_data[m];
        }
        // ==================================================================================
        template<class T> typename signal_array_2D<T>::array_line signal_array_2D<T>::col_vec(const int& col_idx) const
        {
            signal_array_2D<T>::array_line column_values(rows());

            const signal_array_2D<T>& this_array = *this;

            for (int row_idx = 0; row_idx < rows(); ++row_idx)
            {
                column_values[row_idx] = this_array(row_idx, col_idx);
            }

            return column_values;
        }
        // ==================================================================================
        template<class T> void signal_array_2D<T>::col_vec(const int& col_idx, signal_array_2D<T>::array_line& column_values) const
        {
            const signal_array_2D<T>& this_array = *this;

            column_values.resize(rows());

            for (int row_idx = 0; row_idx < rows(); ++row_idx)
            {
                column_values[row_idx] = this_array(row_idx, col_idx);
            }
        }
        // ==================================================================================
        template<class T> void signal_array_2D<T>::set_col_vec(const int& col_idx, const signal_array_2D<T>::array_line& column_values)
        {
            signal_array_2D<T>& this_array = *this;

            for (int row_idx = 0; row_idx < rows(); ++row_idx)
            {
                this_array(row_idx, col_idx) = column_values[row_idx];
            }
        }
        // ==================================================================================
        template<class T> void signal_array_2D<T>::transp(signal_array_2D& transposed) const
        {
            transposed.set(cols(), rows());

            for (int i = 0; i < transposed.rows(); i++)
                for (int j = 0; j < transposed.cols(); j++)
                    transposed(i, j) = array_data[j][i];
        }
        // =================================================================
        template<class T> signal_array_2D<T> signal_array_2D<T>::transp() const
        {
            signal_array_2D<T> transposed(cols(), rows());

            for (int i = 0; i < transposed.rows(); i++)
                for (int j = 0; j < transposed.cols(); j++)
                    transposed(i, j) = array_data[j][i];

            return transposed;
        }
        // =================================================================
        template<class T> void signal_array_2D<T>::transp_inplace()
        {
            *this = this->transp();
        }
        // =================================================================
        template<class T> typename signal_array_2D<T>::array_line signal_array_2D<T>::get_as_vector() const
        {
            typename signal_array_2D<T>::array_line array_as_vector(size_t(rows()) * cols());

            int vec_cnt = 0;

            for (int m = 0; m < rows(); ++m)
            {
                for (int n = 0; n < cols(); ++n, ++vec_cnt)
                {
                    array_as_vector[vec_cnt] = array_data[m][n];
                }
            }

            return array_as_vector;
        }

        // =================================================================

        template< class T > using  signal_array_2D_ptr = std::shared_ptr<signal_array_2D<T>> ;

        // =================================================================

    }
}
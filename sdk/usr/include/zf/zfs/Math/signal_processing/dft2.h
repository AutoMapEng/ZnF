#pragma once

#include <zfs/zfsapi.h>

#include <set>
#include <algorithm>

#include <zfs/Math/signal_processing/signal_array.h>
#include <zfs/Math/signal_processing/dft.h>

namespace zfs {
    namespace math {
        // ==================================================================================
        /** \class dft2
         * \brief Class for 2D discrete fourier transform.
         *
         * \author Juergen Holzner
         *
         * Class for 2D discrete fourier transform.
         *
         */

        template<class T> struct ZFS_API dft2
        {
            // type definitions
            enum class need_spectrum_type_t { magnitude, real_part, imaginary_part };

            using need_spectrum_options_list_t = std::set<need_spectrum_type_t>;

            need_spectrum_options_list_t all_options{ { need_spectrum_type_t::magnitude, need_spectrum_type_t::real_part, need_spectrum_type_t::imaginary_part } };
            need_spectrum_options_list_t empty_options{ {} };

            need_spectrum_options_list_t needed_spectrum_data_list{empty_options};

            // class

            dft2() = default;

            void initialize(const int& rows, const int& columns, need_spectrum_options_list_t needed_spectrum_data_list_ = {});

            void transform(const signal_array_2D<T>& data);
            void transform(const signal_array_2D<std::complex<T>>& data);

            signal_array_2D_ptr<std::complex<T>> get() const;
            signal_array_2D_ptr<T> get_magnitude() const;
            signal_array_2D_ptr<T> get_imag() const;
            signal_array_2D_ptr<T> get_real() const;

        private:

            bool b_input_data_was_real{ false }; // using symmetries in calculating the dft (internal)

            signal_array_2D_ptr<std::complex<T>> spectrum; // complex dft

            std::vector<std::complex<T>> buffer_line_1, buffer_line_2; // internal buffers 

            // allocated when needed
            signal_array_2D_ptr<T> magnitude; // magnitude of spectrum 
            signal_array_2D_ptr<T> spectrum_real; // real part of spectrum 
            signal_array_2D_ptr<T> spectrum_imag; // imaginary part of spectrum 

            void get_real_part();
            void get_imag_part();
            void calculate_magnitude();

            void symmetric_copy(signal_array_2D<std::complex<T>>& spectrum_);
            void symmetric_copy(signal_array_2D<T>& magnitude_);

        };

        // ==================================================================================
    }
}
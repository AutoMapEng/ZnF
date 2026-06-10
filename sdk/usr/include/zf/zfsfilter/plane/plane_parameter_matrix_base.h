#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>

#include "plane_finder_parameter.h"

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================

        template<class plane_parameter_representant_t> class plane_parameter_matrix_base_t
        {
        protected:

            const planefinder_parameter_t&    m_planefinder_parameter;

            const scan_parameter_ptr_t        m_scan_parameter;

            const plane_thresholds_t&         m_plane_thresholds;
            const plane_connect_thresholds_t& m_plane_connect_thresholds;

            const int32_t                     m_lines, m_pixels;

            bool                              m_cyclic_pixel_indexing;
            bool                              m_cyclic_line_indexing;

            using plane_parameter_line_data_t = std::vector< plane_parameter_representant_t >;
            using plane_parameter_matrix_data_t = std::vector< plane_parameter_line_data_t >;

            plane_parameter_matrix_data_t     m_data;

        public:

            plane_parameter_matrix_base_t() = delete;

            plane_parameter_matrix_base_t(const planefinder_parameter_t& planefinder_parameter,
                const int lines, const int pixels, const bool lines_cyclic, const bool pixel_cyclic);

            virtual ~plane_parameter_matrix_base_t() = default;

            virtual void setup_matrix() = 0;

            bool inside_line_range(int32_t line) const;
            bool inside_pixel_range(int32_t pixel) const;
            bool inside_matrix_range(int32_t line, int32_t pixel) const;

            const plane_parameter_representant_t& operator()(int32_t line, int32_t pixel) const;
            plane_parameter_representant_t& operator()(int32_t line, int32_t pixel);

            int32_t lines() const
            {
                return m_lines;
            }

            int32_t pixels() const
            {
                return m_pixels;
            }

            virtual int32_t any_plane_fit_failed() = 0;
            virtual int32_t n_plane_fits_done() = 0;
            virtual int32_t n_plane_fits_validated() = 0;
            virtual int32_t n_plane_fits_possitively_validated() = 0;

            const plane_connect_thresholds_t& plane_connect_thresholds() const
            {
                return m_plane_connect_thresholds;
            }

            const plane_thresholds_t& plane_thresholds() const
            {
                return m_plane_thresholds;
            }

        private:

            int32_t base_range_pixel(int32_t pixel) const;
            int32_t base_range_line(int32_t line) const;

        };
        // =======================================================================================================================

        template<class plane_parameter_representant_t> plane_parameter_matrix_base_t<plane_parameter_representant_t>::plane_parameter_matrix_base_t(const planefinder_parameter_t& planefinder_parameter,
            const int lines, const int pixels, const bool lines_cyclic, const bool pixel_cyclic)
            : m_planefinder_parameter(planefinder_parameter),
            m_scan_parameter(m_planefinder_parameter.scan_parameter),
            m_plane_thresholds(m_planefinder_parameter.plane_thresholds),
            m_plane_connect_thresholds(m_planefinder_parameter.plane_connect_thresholds),
            m_lines(lines), m_pixels(pixels),
            m_cyclic_pixel_indexing(pixel_cyclic), m_cyclic_line_indexing(lines_cyclic)
        {
        }

        template<class plane_parameter_representant_t> bool plane_parameter_matrix_base_t<plane_parameter_representant_t>::inside_line_range(int32_t line) const
        {
            uint32_t int_part = line / this->lines();

            return (m_cyclic_line_indexing && int_part <= 1) || (line >= 0 && line < this->lines());
        }

        template<class plane_parameter_representant_t> bool plane_parameter_matrix_base_t<plane_parameter_representant_t>::inside_pixel_range(int32_t pixel) const
        {
            uint32_t int_part = pixel / this->pixels();

            return (m_cyclic_pixel_indexing && int_part <= 1) || (pixel >= 0 && pixel < this->pixels());
        }

        template<class plane_parameter_representant_t> bool plane_parameter_matrix_base_t<plane_parameter_representant_t>::inside_matrix_range(int32_t line, int32_t pixel) const
        {
            return inside_line_range(line) && inside_pixel_range(pixel);
        }

        template<class plane_parameter_representant_t> int32_t plane_parameter_matrix_base_t<plane_parameter_representant_t>::base_range_pixel(int32_t pixel) const
        {
            if (m_cyclic_pixel_indexing)
            {
                int32_t mod_pixel = pixel % this->pixels();
                return mod_pixel >= 0 ? mod_pixel : this->pixels() + mod_pixel;
            }
            else
                return pixel;
        }

        template<class plane_parameter_representant_t> int32_t plane_parameter_matrix_base_t<plane_parameter_representant_t>::base_range_line(int32_t line) const
        {
            if (m_cyclic_line_indexing)
            {
                int32_t mod_line = line % this->lines();
                return mod_line >= 0 ? mod_line : this->lines() + mod_line;
            }
            else
                return line;
        }

        template<class plane_parameter_representant_t> const plane_parameter_representant_t& plane_parameter_matrix_base_t<plane_parameter_representant_t>::operator()(int32_t line, int32_t pixel) const
        {
            return m_data[base_range_line(line)][base_range_pixel(pixel)];
        }

        template<class plane_parameter_representant_t> plane_parameter_representant_t& plane_parameter_matrix_base_t<plane_parameter_representant_t>::operator()(int32_t line, int32_t pixel)
        {
            return m_data[base_range_line(line)][base_range_pixel(pixel)];
        }
        // ========================================================================================================================
    }
}
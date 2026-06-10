#pragma once

#include <zfsfilter/filterapi.h>

#include <vector>
#include <memory>
#include <cstdint>
#include <utility>
#include <cmath>

#include <zfs/Math/fit/surfacefit_xyz.h>

#include "plane_finder_scan_section_processing_parameter.h"
#include "plane_parameter_matrix_base.h"
#include "plane_parameter_template.h"

namespace zfsfilter
{
    namespace planes
    {

        // ======================================================================================================================

        using plane_parameter_t = plane_parameter_template_t< xyz_surface_fit_with_error_analysis >;
        using plane_parameter_ptr_t = std::shared_ptr<plane_parameter_template_t< xyz_surface_fit_with_error_analysis >>;

        // ======================================================================================================================

        class FILTERDLL_API plane_parameter_matrix_t: public plane_parameter_matrix_base_t<plane_parameter_ptr_t>
        {

        public:

            plane_parameter_matrix_t() = delete;

            plane_parameter_matrix_t(const plane_finder_scan_section_processing_parameter_t& scan_section_processing_parameter);

            plane_parameter_matrix_t(const planefinder_parameter_t& planefinder_parameter, int lines, int pixels, bool lines_cyclic, bool pixels_cyclic);
            
            void setup_matrix();

            int32_t any_plane_fit_failed();
            int32_t n_plane_fits_done();
            int32_t n_plane_fits_validated();
            int32_t n_plane_fits_possitively_validated();

            plane_parameter_matrix_t get_subsampled(const int factor, int offset_line, int offset_pixel) const;

        };
        
        // ======================================================================================================================

        using plane_parameter_matrix_ptr_t = std::shared_ptr<plane_parameter_matrix_t>;

        // ======================================================================================================================

    }    
}
#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include "plane_finder_parameter.h"
#include "plane_finder_processing_result.h"

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================================
        // produce settings for scan with given base, thresholds, output, operation, and display settings
        planefinder_parameter_t FILTERDLL_API planefinder_set_params(const wstring& scanFn, const planefinder_parameter_t& params);

        // produce standard settings for scan; output, operation, and display settings may be given
        planefinder_parameter_t FILTERDLL_API planefinder_standard_params( const wstring& scanFn, 
                                                                           const planefinder_output_data_generation_settings_t& data_generation_settings_,
                                                                           const planefinder_operation_settings_t& operation_settings_ = planefinder_operation_settings_t(),
                                                                           const planefinder_data_display_settings_t& data_display_settings_ = planefinder_data_display_settings_t());

        // produce plane finder parameters for detailed individual parameter provision (mech-calib-versuche)
        planefinder_parameter_t FILTERDLL_API planefinder_parameter_setting( const int32_t sz_,
                                                                             const int32_t plane_fit_subsample_step_, 
                                                                             const int32_t plane_validation_subsample_step_,
                                                                             const plane_thresholds_t::plane_validation_option_t& plane_validation_option_= plane_thresholds_t::plane_validation_option_t::AUTOMATIC_THRESHOLDING,
                                                                             const planefinder_output_data_generation_settings_t& data_generation_settings_= planefinder_output_data_generation_settings_t(),
                                                                             const planefinder_operation_settings_t & operation_settings_= planefinder_operation_settings_t(),
                                                                             const planefinder_data_display_settings_t& data_display_settings_= planefinder_data_display_settings_t(),
                                                                             const plane_thresholds_t& plane_thresholds_ = plane_thresholds_t(),
                                                                             const plane_connect_thresholds_t& plane_connect_thresholds_= plane_connect_thresholds_t());

        // ========================================================================================================================================
        std::string FILTERDLL_API start_planefinder_with_standard_params(const std::wstring& scanFn, plane_finder_processing_result_t& plane_finder_processing_result);
        // ========================================================================================================================================
        std::string FILTERDLL_API planefinder(const std::wstring& scanFn, planefinder_parameter_t& params, plane_finder_processing_result_t& scan_section_processing_result);
        // ========================================================================================================================================
    }
}
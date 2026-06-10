#pragma once

#include "../filterapi.h"

#include <cstdint>

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================
        struct plane_thresholds_scan_data_t
        {
            bool b_set{ false };

            double minRangeInM{ 0 };
            double maxRangeInM{ 0 };

            plane_thresholds_scan_data_t(){}
            plane_thresholds_scan_data_t(const double& minRangeInM_, const double& maxRangeInM_)
            {
                set(minRangeInM_, maxRangeInM_);
            }

            void set(const double& minRangeInM_, const double& maxRangeInM_)
            {
                if (!b_set && maxRangeInM > minRangeInM)
                {
                    b_set = true;  minRangeInM = minRangeInM_; maxRangeInM = maxRangeInM_;
                }
            }
        };
        // ======================================================================================================================
        struct plane_thresholds_t
        {

            uint32_t n_outlier_max_thresh{ 0 };         // < number of maximum outliers allowed

            double  outlier_rg_distance_thresh{ 0.36 }; // < [m], distance for outlier

            // thresholding type
            enum class plane_validation_option_t: uint32_t {AUTOMATIC_THRESHOLDING=0, MANUAL_THRESHOLDING};
            enum class plane_sample_thresholding_option_t : uint32_t { DEFAULT, INCIDENCE_ANGLE_DEPENDENT, DISTANCE_AND_INCIDENCE_DEPENDENT };

            plane_validation_option_t plane_validation_option{ plane_validation_option_t::AUTOMATIC_THRESHOLDING };
            plane_sample_thresholding_option_t plane_sample_thresholding_option{ plane_sample_thresholding_option_t::DEFAULT };

            // automatic thresholding with range noise figure
            // d < rnff* ( sigma_exp + rnso ) + rno ; alternativ: d < rnff* ( sigma_exp * rnsf ) + rno
            double plane_sample_thresholding_factor{ 3.5 }; // threshold = plane_sample_thresholding_factor* (sigma + plane_sample_thresholding_offset_m)

            bool   use_range_noise_sigma_adaptation_factor_instead_of_constant{ false };
            double range_noise_sigma_thresholding_adaptation_factor{ 1.23 }; // alias: _rnstaf => threshold is modified to : plane_sample_thresholding_factor* (_rnstaf*sigma + plane_sample_thresholding_offset_m)
            double range_noise_sigma_thresholding_adaptation_constant_m{ 0.00010 }; // [m] offset to stddev to capture measured stddev spread around 'sigma_d vs. intensity'- function
                                                                                    // alias: _rnstac => threshold is modified to : plane_sample_thresholding_factor* ((_rnstac+sigma) + plane_sample_thresholding_offset_m)

            double structure_sigma_thresholding_adaptation_factor{ 1.2 }; // for plane validation (measurement of structure sigma has uncertainty) ...

            double sigma_multiple_for_structure_detection{ 2.5 }; // 
            double min_peak_amplitude_for_structure_detection{ 0.015 }; // signal amplitude(s) in spectra must be above this value, absolute, for fixed # of samples

            // manual thresholding with linear threshold over given scan range
            double  plane_validation_threshold_near_range{ 0.0015 }; // < [m], threshold for validating plane in near range (single threshold for comparing residuals, variances from fit)
            double  plane_validation_threshold_far_range{ 0.0015 }; // < [m], threshold for validating plane in far range (single threshold for comparing residuals, variances from fit)

            bool    plane_validation_thresholds_same{ false }; // for faster calculations
            double  plane_validation_threshold_slope_figure{ 0 }; // < [m] for faster calculations
            double  min_range{ 0 };// < [m] for faster calculations
            bool    plane_validation_threshold_slope_figure_set{ false };// < for faster calculations

            plane_thresholds_scan_data_t scan_parameter_for_thresholds;

            double linear_range_dependent_plane_threshold(const double& range_)
            {
                if (plane_validation_thresholds_same)
                {
                    return plane_validation_threshold_near_range;
                }
                else if (plane_validation_threshold_slope_figure_set)
                {
                    return plane_validation_threshold_near_range + plane_validation_threshold_slope_figure* (range_- min_range);
                }
                else if (plane_validation_threshold_far_range == plane_validation_threshold_near_range)
                {
                    plane_validation_thresholds_same = true;
                    return plane_validation_threshold_near_range;
                }
                else if (scan_parameter_for_thresholds.b_set)
                {
                    plane_validation_threshold_slope_figure_set = true;
                    min_range = scan_parameter_for_thresholds.minRangeInM;
                    plane_validation_threshold_slope_figure = (plane_validation_threshold_far_range - plane_validation_threshold_near_range) / (scan_parameter_for_thresholds.maxRangeInM - min_range);

                    return plane_validation_threshold_near_range + plane_validation_threshold_slope_figure * (range_ - min_range);
                }
                // fall backs...
                else if(plane_validation_threshold_near_range)
                {
                    return plane_validation_threshold_near_range;
                }
                else
                {
                    return plane_validation_threshold_far_range;
                }
            }

            plane_thresholds_t(){}

            plane_thresholds_t( const uint32_t                  &n_outlier_max_thresh_, 
                                const double& outlier_rg_distance_thresh_,
                                const plane_validation_option_t &plane_validation_option_,
                                const double                    & plane_sample_thresholding_factor_,
                                const double                    &plane_validation_threshold_near_range_,
                                const double                    &plane_validation_threshold_far_range_)
                : 
                n_outlier_max_thresh(n_outlier_max_thresh_),
                outlier_rg_distance_thresh(outlier_rg_distance_thresh_),
                plane_validation_option(plane_validation_option_),
                plane_sample_thresholding_factor(plane_sample_thresholding_factor_),
                plane_validation_threshold_near_range(plane_validation_threshold_near_range_),
                plane_validation_threshold_far_range(plane_validation_threshold_far_range_)
            {}
        };
        // ======================================================================================================================
    }
}

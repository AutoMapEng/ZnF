#pragma once

#include <zfsfilter/filterapi.h>

#include <zfsfilter/plane/base_vector_definitions.h>
#include <zfsfilter/plane/plane_finder_parameter.h>

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================

        struct FILTERDLL_API plane_validation_parameter_t
        {
            const planefinder_parameter_t& planefinder_parameter;
            const scan_parameter_ptr_t& scan_parameter;
            const zfs::LaserSpotDiameter& laser_spot_diameter; // < laser spot diameter
            const noise_parameter_t& noise_parameter;

            plane_thresholds_t& plane_thresholds; // < threshold object (multiplier), constant given threshold

            bool indoor{ true };
            int  n_plane_edge_samples{ 0 };

            // member functions
            plane_validation_parameter_t() = delete;
            plane_validation_parameter_t(planefinder_parameter_t& planefinder_parameter);

            double expected_range_noise_threshold(const double& intensity_average) const;

            double calc_sample_validation_threshold_from_sigma(const double& some_sigma) const;

            double calc_sigma_validation_threshold(const double& intensity_average) const;
            double calc_sample_validation_threshold(const double& intensity_average) const;
            
            double calc_sigma_validation_threshold_incidence_dependent(const double& intensity_average, const vec& center_position, const vec& normal) const;
            double calc_sample_validation_threshold_incidence_dependent(const double& intensity_average, const vec& center_position, const vec& normal) const;

            double calc_sigma_validation_threshold_range_and_incidence_dependent(const double& intensity_average, const vec& center_position, const vec& normal) const;
            double calc_sample_validation_threshold_range_and_incidence_dependent(const double& intensity_average, const vec& center_position, const vec& normal) const;
        };

        // ======================================================================================================================

    }
}
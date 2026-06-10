#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <limits>

#include <zfs/Math/math/def.h>

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================

        struct plane_connect_thresholds_t
        {

            double n_plane_connect_sigma_mult{ 3.5 };                                                                                       // < multiplier to standard deviations for plane connect condition (3.5 (indoor) - 5 (outdoor))
            double n_plane_connect_sigma_sqr_mult{ n_plane_connect_sigma_mult* n_plane_connect_sigma_mult };                                // < multiplier to variances for plane connect condition
            double n_plane_parameter_noise_max_multiple{ 1.5 };                                                                             // < comparison of plane parameter noise components (1.5 (indoor) - 2.0 (outdoor))
            double n_plane_parameter_noise_max_multiple_sqr{ n_plane_parameter_noise_max_multiple* n_plane_parameter_noise_max_multiple };  // < comparison of plane parameter noise components
            double min_area_connected_plane{ 0.13 * 0.13 };                                                                                 // < [m^2], minimum area of connected planes
            double max_area_connected_plane{ 9999 };                                                                                        // < [m^2], maximum area of connected planes
                                                                                                                                            
            uint32_t min_cnt_connected_plane{ 20 };                                                                                          // < minimum cnt (number of connected subplanes, >=3 ) of connected planes
            uint32_t min_line_cnt_connected_plane{ 5 };                                                                                     // < minimum line cnt (line extend of connected subplanes in subplane matrix) of connected planes
            uint32_t min_pixel_cnt_connected_plane{ 5 };                                                                                    // < minimum pixel cnt (pixel extend of connected subplanes in subplane matrix) of connected planes

            double plane_element_near_horizon_count_angle_rad{ TORAD(5.) };        // < [rad] threshold deciding whether a plane element - as part of a collected plane - is counted as being near the horizon

            plane_connect_thresholds_t() = default;

            plane_connect_thresholds_t(
                const double& n_plane_connect_sigma_mult_,
                const double& n_plane_parameter_noise_max_mult_,
                const double& min_area_connected_plane_,
                const double& max_area_connected_plane_,
                const uint32_t& min_cnt_connected_plane_,
                const double& plane_element_near_horizon_count_angle_)
                : min_area_connected_plane(min_area_connected_plane_),
                  max_area_connected_plane(min_area_connected_plane_),
                  min_cnt_connected_plane(min_cnt_connected_plane_)
            {
                set_plane_parameter_noise_max_multiple(n_plane_parameter_noise_max_mult_);
                set_plane_connect_sigma_multiple(n_plane_connect_sigma_mult_);
                set_plane_horizon_angle_rad(plane_element_near_horizon_count_angle_);
            }

            void set_plane_connect_sigma_multiple(const double& multiple)
            {
                n_plane_connect_sigma_mult = multiple;
                n_plane_connect_sigma_sqr_mult = n_plane_connect_sigma_mult * n_plane_connect_sigma_mult;
            }

            void set_plane_parameter_noise_max_multiple(const double& multiple)
            {
                n_plane_parameter_noise_max_multiple = multiple;
                n_plane_parameter_noise_max_multiple_sqr = n_plane_parameter_noise_max_multiple * n_plane_parameter_noise_max_multiple;
            }

            void set_plane_horizon_angle_rad(const double& angle)
            {
                plane_element_near_horizon_count_angle_rad = angle;
            }

        };
        // ======================================================================================================================
    }
}
#pragma once

#include <cmath>

#include <zfs/noise.h>
#include <zfs/laserspotsize.h>

namespace zfsfilter
{
    // ======================================================================================================================
        
    struct noise_parameter_t
    {
        zfs::RangeNoise noise;
        double          noise_offset_indoor{ 0.003 }; // < [m] / allowed structure sigma, adapted for thresholding of samples with multiplier in plane_thresholds_t
        double          noise_offset_outdoor{ 0.003 }; // < [m] / allowed structure sigma, adapted for thresholding of samples with multiplier in plane_thresholds_t

        double noise_offset(bool indoor= true) const
        {
            if (indoor) return noise_offset_indoor;
            return noise_offset_outdoor;
        }

        double& noise_offset(bool indoor = true)
        {
            if (indoor) return noise_offset_indoor;
            return noise_offset_outdoor;
        }

        double noise_offset(const double& pixel_size, const double& laser_spot_diameter, bool indoor = true) const
        {
            if (pixel_size > 0 && laser_spot_diameter > 0)
            {
                double dp_dl = pixel_size/laser_spot_diameter;
                return std::max(dp_dl,1.) * noise_offset(indoor);
            }
            else
            {
                return noise_offset(indoor);
            }
        }
    };

    // ======================================================================================================================
}
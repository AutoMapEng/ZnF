#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <string>

#include <zfs/noise.h>
#include <zfs/laserspotsize.h>
#include <zfs/linebyline_smart.h>
#include <zfs/zfsheader.h>

#include "scan_calibration_parameter.h"

namespace zfsfilter
{
    // ========================================================================================================================

    struct FILTERDLL_API scan_parameter_t
    {
        bool is_set{ false };

        std::wstring scanFn;

        bool is_indoor_scan{ false };
        bool is_scan_selection{ false };
        bool is_two_layer_scan{ false };
        bool is_multi_layer_scan{ false };

        uint32_t lines{ 0 };
        uint32_t pixels{ 0 };

        uint32_t pixels_360deg{ 0 };

        double min_rg{ 0 };
        double max_rg{ 0 };

        int32_t min_rf{ 0 };
        int32_t max_rf{ 0 };

        double first_low_encoder_angle_rad{ 0 };
        double last_low_encoder_angle_rad{ 0 };

        double elevation_angle_of_first_pixel_rad{ 0 };
        double elevation_angle_of_last_pixel_rad{ 0 };

        double swept_azimuth_angle_rad{ 0 };
        double swept_elevation_angle_rad{ 0 };

        double azimuth_angle_step_per_line_rad{ 0 };
        double azimuth_angle_step_per_pixel_rad{ 0 };

        double elevation_angle_step_per_pixel_rad{ 0 };

        double azimuth_angle_range_one_sided{ 0 };
        double elevation_angle_range_one_sided{ 0 };

        std::string scan_acquisition_date;

        zfs::RangeNoise rg_noise;
        zfs::LaserSpotDiameter laser_spot_diameter;

        scan_calibration_parameter_t scan_calibration_parameter;

        // ------------------------------------------------------------------------------------

        scan_parameter_t() = default;
        scan_parameter_t(const std::wstring& scanFn);
        scan_parameter_t(const zfs::zfslinebyline_ptr& lbl);

        bool set(const std::wstring& scanFn);
        bool set(const zfs::zfslinebyline_ptr& lbl);

        void set_swept_azimuth_angle(const double& swept_azimuth_angle_rad_);

        std::string print();

    protected:

        std::string getScanDate(const ZFSHeader* const scanheader);

    };
    // ========================================================================================================================
    typedef std::shared_ptr<scan_parameter_t> scan_parameter_ptr_t;
    // ========================================================================================================================
}

#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include <zfs/linebyline_smart.h>

namespace zfsfilter
{
    // ========================================================================================================================

    struct FILTERDLL_API scan_calibration_parameter_t
    {
        bool is_set{ false };

        std::wstring scanFn;

        double P00045_RAD{ 0 };// < height index error [rad]
        double P15500_RAD{ 0 };// < pointing/sight axis error [rad]
        double P15501_RAD{ 0 };// < vertical axis error [rad]
        double P15503_RAD{ 0 };// < circle division excentricity error of the horizontal encoder [rad]
        double P15504_RAD{ 0 };// < circle division excentricity error of the low encoder [rad]
        double P15505_RAD{ 0 };// < phase factor

        scan_calibration_parameter_t() = default;
        scan_calibration_parameter_t(const std::wstring& scanFn);
        scan_calibration_parameter_t(const zfs::zfslinebyline_ptr& lbl);

        bool set(const std::wstring& scanFn);
        bool set(const zfs::zfslinebyline_ptr& lbl);
        bool reset(const zfs::zfslinebyline_ptr& lbl);

        std::string print() const;
    };
    // ========================================================================================================================
}
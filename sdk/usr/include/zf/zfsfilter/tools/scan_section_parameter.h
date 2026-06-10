#pragma once

#include <zfsfilter/filterapi.h>

#include <vector>
#include <string>

#include <zfs/linebyline_smart.h>

namespace zfsfilter
{
    // ========================================================================================================================
    using vec_ui64 = std::vector<uint64_t>;
    // ========================================================================================================================
    struct FILTERDLL_API scan_section_parameter_t
    {
        bool is_set{ false };

        vec_ui64 scan_section_start_lines;
        vec_ui64 scan_section_extend_in_lines;

        uint32_t number_of_full_sections{ 0 };
        uint32_t number_of_sections{ 0 };

        scan_section_parameter_t() = default;
        scan_section_parameter_t(const std::wstring& scanFn);
        scan_section_parameter_t(const zfs::zfslinebyline_ptr& lbl);

        bool set(const std::wstring& scanFn);
        bool set(const zfs::zfslinebyline_ptr& lbl);

        static void find_180_degree_scan_section_start_lines(const zfs::zfslinebyline_ptr& lbl, vec_ui64& scan_section_start_lines, vec_ui64& scan_section_extend_in_lines, uint32_t& number_of_sections, uint32_t& number_of_full_sections);

    };
} // namespace zfsfilter

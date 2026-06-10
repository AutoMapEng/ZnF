#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>

namespace zfsfilter
{
    namespace planes
    {
        // ==============================================================================================
        struct scan_section_processing_parameter_t
        {
            std::vector<uint64_t> nv_scan_start_lines_offset;
            std::vector<uint64_t> nv_scan_start_pixel_offset;
        };
        // ==============================================================================================
    }
}
#pragma once

#include <zfsfilter/filterapi.h>

#include <vector>
#include <string>

namespace zfsfilter
{
    struct FILTERDLL_API scan_parameter_modifier
    {
        scan_parameter_modifier() = default;

        static std::string set_double_scan_header_parameter(std::vector<int> parameter_list, std::vector<double> parameter_value_list, const std::wstring& scan);
    };
}
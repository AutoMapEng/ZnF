#pragma once

#include <zfsfilter/filterapi.h>

#include <string>


namespace zfsfilter
{
    namespace algo
    {

        /// calculate quality value for every point
        FILTERDLL_API bool quality4Scan(const std::wstring& a, std::wstring normal_param);
    }
}
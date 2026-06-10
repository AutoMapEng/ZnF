#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

namespace zfsfilter
{

    // create for all supported devices a default filter config file
    FILTERDLL_API void createFilterTempates(const std::wstring& folder);

}
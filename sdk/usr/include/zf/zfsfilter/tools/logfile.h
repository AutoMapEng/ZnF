#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

namespace zfsfilter
{
    // ========================================================================================================================

    bool FILTERDLL_API output_log_to_file(const std::wstring& scanFn, const std::wstring& subPath, const std::wstring& logFn, const std::string& log, bool continuous= false);

    // ========================================================================================================================

}
#pragma once

#include <zfsfilter/filterapi.h>

#include <string>


namespace zfsfilter
{
    namespace algo
    {
        /// run a single algorithm like create xyz layer
        FILTERDLL_API bool run(const std::wstring& algo, const std::wstring& scanname, const std::wstring& params, bool force = false, bool createImages=false, const std::wstring& pnggenparams=L"" );
    }
}

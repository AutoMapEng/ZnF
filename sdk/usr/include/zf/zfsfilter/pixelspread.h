#pragma once

#include "filterapi.h"

#include <string>

class ZFSLinebyline;

namespace zfsfilter
{
    namespace pixelspread
    {
        FILTERDLL_API std::string pixelSpread(const std::wstring& scanFn, int nthreads);
        FILTERDLL_API std::string pixelSpread(ZFSLinebyline*, int nthreads);
    }
}
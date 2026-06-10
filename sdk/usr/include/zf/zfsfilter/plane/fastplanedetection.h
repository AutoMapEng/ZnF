#pragma once

#include <zfsfilter/filterapi.h>

#include <string>



namespace zfsfilter
{
    namespace planes
    {
        FILTERDLL_API std::string fastPlaneDetectionLBL(const std::wstring& scanFn);
    }
}
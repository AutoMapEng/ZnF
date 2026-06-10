#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

namespace zfsfilter
{
    namespace algo
    {
        #define LAYER_NAME_REGISTRATION_MASK L"registration_mask"

        bool segmentation_analysis(const std::wstring& scan, const std::wstring& params);
    }
}
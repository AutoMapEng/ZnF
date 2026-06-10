#pragma once

#include <zfsfilter/filterapi.h>

#include <string>


namespace zfsfilter
{
    namespace algo
    {
        #define LAYER_NAME_CLASS L"class"

        /**
        \brief classification of ground, sky, ...
        */
        FILTERDLL_API bool classification(const std::wstring& a, const std::wstring& params);
   }
}
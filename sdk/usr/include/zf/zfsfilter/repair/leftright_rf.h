#pragma once
#include <zfsfilter/filterapi.h>

#include <string>
#include <memory>

namespace zfsfilter
{
    namespace repair
    {
        /**
        \brief compensate the left-right intensity difference
        \return the name of the new file or empty if it fails
        */
        FILTERDLL_API std::pair<std::wstring,std::string> compensateLeftRightIntensity(const std::wstring& zfsFileName, bool sutOff );
    }
}
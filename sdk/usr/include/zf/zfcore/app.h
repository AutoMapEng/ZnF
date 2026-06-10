#pragma once
#ifdef _WIN32

#include "zfcore_api.h"

#include <string>


namespace zf
{
    /// the application path
    ZFCORE_API std::wstring app_location_path();

}
#endif

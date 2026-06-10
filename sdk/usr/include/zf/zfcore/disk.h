#pragma once
#ifdef _WINDOWS

#include "zfcore_api.h"

#include <string>

namespace zf
{
    // return free disk space MB
    struct DiskSpace
    {
        bool valid{};
        uint64_t freeMB{};
        uint64_t availableMB{};

        double percent_free() const 
        { 
            if (valid && availableMB)
                return (double)freeMB * 100. / availableMB;
            else 
                return {};
        }
    };
    ZFCORE_API DiskSpace disk_space_free_mb( const std::wstring& drive );


} // end namespace zftools

#endif

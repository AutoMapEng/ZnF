#pragma once

#include "zftools_api.h"

#include <stdint.h>

namespace zftools
{

    /// memory usage return status
    struct MemoryUsage
    {
        uint32_t physicalMB{0};
        uint32_t freeMB{0};
        uint32_t freeVirtualMB{0};

        MemoryUsage() = default;

        /// return true if struct is filled up
        bool isValid() const { return physicalMB; }
    };

    /// get information about memory usage
    MemoryUsage ZFTOOLS_API getMemoryStatus();

	/**
    *   Check ob der freier Abreitsspeicher noch ausreicht
    *   \param needed = benoetigter Arbeitsspeicher in MB
    */
    bool ZFTOOLS_API checkRam(int neededMB );




    inline int64_t bytes2mb(int64_t sz) { return sz / (1024 * 1024); }
    inline int64_t bytes2gb(int64_t sz) { return sz / (1024 * 1024 * 1024); }

    inline int64_t mb2gb(int64_t sz) { return sz / 1024; }
    inline int64_t gb2mb(int64_t sz) { return sz * 1024; }


} // end namespace zftools


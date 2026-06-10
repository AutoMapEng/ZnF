#pragma once

#include "zftools_api.h"

#include <stdint.h>


namespace zftools
{
    typedef uint32_t tick_t;

    /// system ms since start platform independet
    ZFTOOLS_API tick_t gettickcount();


} // end namespace zftools


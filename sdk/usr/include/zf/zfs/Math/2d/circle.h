#pragma once

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // =====================================================================
        struct ZFS_API Circle
        {
            double radius{ 0 };
            double xc{ 0 }, yc{ 0 };
        };
    } // namespace math
} // namespace zfs
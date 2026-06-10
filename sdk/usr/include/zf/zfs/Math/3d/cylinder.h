#pragma once

#include <zfs/zfsapi.h>

#include <utility>

#include "../line.h"

namespace zfs {
    namespace math {
        // =====================================================================
        struct ZFS_API Cylinder
        {
            double radius{ 0 };
            double height{ 0 };
            Line3d axis;
        };
    } // namespace math
} // namespace zfs
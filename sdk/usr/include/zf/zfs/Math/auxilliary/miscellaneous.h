#pragma once

/**
\file This file contains mathmatical functions that do not really fit in any other category.
*/

#undef min
#undef max
#include <algorithm>

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {

        inline int maxOfFour(int i1, int i2, int i3, int i4) { return std::max(std::max(i1, i2), std::max(i3, i4)); } ///< returns the largest of four integers.
        inline int minOfFour(int i1, int i2, int i3, int i4) { return std::min(std::min(i1, i2), std::min(i3, i4)); } ///< returns the smallest of four integers.

        bool ZFS_API checkClose(double first, double second, double error = 10e-12);  ///< Check if two doubles are equal within the relative error relError.

    } // end namespace math
} // end namespace zfs
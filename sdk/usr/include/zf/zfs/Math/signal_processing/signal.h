#pragma once

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <math.h>
SWITCH_WARNINGS_ON
#include <cstdint>
#include <algorithm>
#include <vector>
#include <stdint.h>

#include <zfs/zfsapi.h>

typedef unsigned char byte;

namespace zfs {
    namespace math {
        // ===============================================================================================================
        template<class T> class Range;
        // ===============================================================================================================
        template<class T>           ZFS_API const std::vector<T> sawtooth(const T& amplitude, const T& phase, const T& frequency, const T& offset, const T& sampling_frequency, const uint32_t length);
        // ===============================================================================================================
        template<class T>           ZFS_API const std::vector<T> sine(const T& amplitude, const T& phase, const T& frequency, const T& offset, const T& sampling_frequency, const uint32_t length);
        // ===============================================================================================================
        template<class T>           ZFS_API const std::vector<T> subsample(const std::vector<T>& aInputSignal, const uint32_t aSubsamplingFactor, const uint32_t aOffset);
        // ===============================================================================================================
        template<class T>           ZFS_API double               concentration_around_position(const std::vector<T>& distribution, int32_t position);
        // ===============================================================================================================
        template<class T>           ZFS_API std::vector<T>       ordinate_vector(const Range<T>& aRange);
        // ===============================================================================================================
        template<class T, class T1> ZFS_API std::vector<T>       ordinate_vector(const Range<T1>& aRange, const T& aStep);
        // ===============================================================================================================
        template<class T>           ZFS_API double               half_power_width(const std::vector<T>& sampled_signal, int32_t osf);
        // ===============================================================================================================
    } // end namespace math
} // end namespace zfs
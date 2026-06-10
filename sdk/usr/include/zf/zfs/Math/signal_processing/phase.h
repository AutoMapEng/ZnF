#pragma once

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <math.h>
#include <vector>

#include <zfs/zfsapi.h>

#include <zfs/Math/math/tuple.h>

namespace zfs {
    namespace math {
        // forward declarations
        class Polynomial;

        // ==========================================================================================
        void ZFS_API unwrap(std::vector<Tuple2<double> >& tuplevector);
        // ==========================================================================================
        void ZFS_API unwrap(std::vector<double >& phase_vector);
        // ==========================================================================================
        double ZFS_API accumulated_phase(const Polynomial& frequency_poly, int start, int end);
        // ==========================================================================================
    } // end namespace math
} // end namespace zfs

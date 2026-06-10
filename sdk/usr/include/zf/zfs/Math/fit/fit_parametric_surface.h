#pragma once

#include "../../zfsapi.h"
#include <vector>
#include "../3d/parametric_surface.h"

namespace zfs {
    namespace math {
        /**
        \brief Result of a parametric surface fit
        */
        class ZFS_API Parametric3DSurfaceFit
        {
        public:
            zfs::math::Parametric3DSurface parametric_surface;
            bool succ{};  ///< true if a plane could be fitted
            double std{}; ///< std divation of points
            int points{}; ///< number of input points
            int pointsUsed{}; ///< number of points used
            int id{}; ///< user id. not used by planefit. for general usage

            Parametric3DSurfaceFit() = default;
        };

        /// fit parametric surface	
        Parametric3DSurface ZFS_API fit_parametric_surface(const std::vector<zfs::math::Vec3d>& points);

    } // end namespace math
} // end namespace zfs
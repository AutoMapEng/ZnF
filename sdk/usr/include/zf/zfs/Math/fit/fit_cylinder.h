#pragma once

#include <zfs/zfsapi.h>
#include <vector>
#include <memory>

#include "../zfvec.h"
#include "../3d/cylinder.h"
#include "../fit/fit_circle.h"

namespace zfs {
    namespace math {
        // =====================================================================
        struct ZFS_API CylinderFit
        {
            Cylinder cylinder;

            CircleFit circlefit; /// < results from circle fit used in cylinder fit

            bool succ{ false };  ///< true if a plane could be fitted
            double std{ 0. }; ///< std divation of points
            int points{ 0 }; ///< number of input points
            int id{ -1 }; ///< user id. not used by planefit. for general usage

            CylinderFit() = default;
        };
        // =====================================================================
        CylinderFit ZFS_API fit_cylinder(const std::vector<Vec3d>& points, std::vector<Vec3d>& point_normals);
        // =====================================================================
        using CylinderFit_ptr_t = std::shared_ptr<CylinderFit>;
        // =====================================================================
    } // namespace math
} // namespace zfs
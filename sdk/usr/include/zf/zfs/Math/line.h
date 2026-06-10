#pragma once

#include "../zfsapi.h"
#include "zfvec.h"

#include <cmath>

namespace zfs {
	namespace math
	{
        /**
        \brief Line
        */
        class ZFS_API Line3d
        {
        public:
            Vec3d point;
            Vec3d vec;

            Vec3d line_point(const double& lambda) const;
            bool pointIsOnLine(const Vec3d& point) const;

        };

        // ======================================================================================

        inline bool Line3d::pointIsOnLine(const Vec3d& pointArg) const
        {
            return std::fabs((point - pointArg).dotProd(vec)) < zfs::math::eps;
        }

        inline Vec3d Line3d::line_point(const double& lambda) const
        {
            return point + vec * lambda;
        }
	}
}

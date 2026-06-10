#pragma once

#include <cmath>
#include "../zfsapi.h"
#include "zfvec.h"

namespace zfs {
	namespace math
	{
        /// angle [rad] between three 3d points
        inline double angleBetween3Points(const Vec3d& a, const Vec3d& b, const Vec3d& c)
        {
            if (a == b || b == c) return 0.;

            Vec3d ba = a - b;
            Vec3d bc = c - b;

            ba.normalize();
            bc.normalize();

            return std::acos( ba.dotProd(bc) );
        }


        inline bool vectorsAreParallel(Vec3d lhs, Vec3d rhs)
        {
            lhs.normalize();
            rhs.normalize();
            return std::fabs(std::fabs(lhs.dotProd(rhs)) - 1.) < zfs::math::eps;
        }


    }
}

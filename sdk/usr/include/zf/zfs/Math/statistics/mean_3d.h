#pragma once

#include <zfs/zfsapi.h>

#include <zfs/Math/math/mean.h>
#include <zfs/Math/zfvec.h>

#include <cstdint>
#include <vector>

namespace zfs {
    namespace math {
        // =========================================================
        using vec = std::vector<double>;
        // =========================================================
        class ZFS_API mean_3D
        {
        public:
            mean_3D();

            void clear();

            void add(const vec& value);
            void add(const zfs::math::Vec3d& value);
            void add(const double& x, const double& y, const double& z);

            bool calc();

            const vec& getAvr() const;

        private:
            mean mean_x, mean_y, mean_z;

            bool bCalcOK{ false };
            vec avr{ vec(3,0.) };
        };
        // =========================================================
    }
}
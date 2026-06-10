#pragma once

#include <zfs/zfsapi.h>

#include <zfs/Math/math/average.h>
#include <zfs/Math/zfvec.h>

#include <cstdint>
#include <vector>

namespace zfs {
    namespace math {
        // =========================================================
        using vec = std::vector<double>;
        // =========================================================
        class ZFS_API average_3D
        {
        public:
            average_3D();

            void clear();

            void add(const vec& value);
            void add(const zfs::math::Vec3d& value);
            void add(const double& x, const double& y, const double& z);

            bool calc();
            bool calc_negate();

            const vec& getAvr() const;
            const vec& getStd() const;

        private:
            Average average_x, average_y, average_z;

            bool bCalcOK{ false };
            vec avr{ vec(3,0.) };
            vec std{ vec(3,0.) };
        };
        // =========================================================
    }
}
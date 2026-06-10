#pragma once

#include <zfs/zfsapi.h>

#include <zfs/Math/math/average.h>

#include <cstdint>
#include <vector>

namespace zfs {
    namespace math {
        // =========================================================
        class ZFS_API weighted_average : public Average
        {
        public:
            weighted_average();

            void clear();

            void add(const double& value, const double& weight, const double& sigma);
            void add(const double& value, const double& sigma);

            bool calc();

        private:

            long double sum_weight;
        };
        // =========================================================
    } // end namespace math
} // end namespace zfs
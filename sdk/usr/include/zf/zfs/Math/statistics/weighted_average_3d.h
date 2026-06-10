#pragma once

#include <zfs/zfsapi.h>

#include "weighted_average.h"

#include <cstdint>
#include <vector>

namespace zfs {
    namespace math {
        // =======================================================================
        // weighted_average_3D
        // =======================================================================
        using vec = std::vector<double>;
        // =========================================================
        class ZFS_API weighted_average_3D
        {
        public:
            weighted_average_3D();

            void clear();

            void add(const vec& value, const vec& weight, const vec& sigma);
            void add(const vec& value, const vec& sigma);

            bool calc();

            vec getAvr() const;
            vec getStd() const;

            const double& getAvr(uint32_t idx) const;
            const double& getStd(uint32_t idx) const;

        private:
            double return_dummy{ 0 }; // < used in getAvr(idx) and getStd(idx)

            weighted_average weighted_average_x, weighted_average_y, weighted_average_z;
        };
        // =========================================================
    } // end namespace math
} // end namespace zfs
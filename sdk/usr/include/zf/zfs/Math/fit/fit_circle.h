#pragma once

#include <zfs/zfsapi.h>
#include <vector>

#include "../2d/circle.h"

namespace zfs {
    namespace math {
        // =====================================================================
        struct ZFS_API CircleFit
        {
            Circle circle;
        
            bool succ{false};  ///< true if a plane could be fitted
            double std{0.}; ///< std divation of points
            int points{0}; ///< number of input points
            int id{-1}; ///< user id. not used by planefit. for general usage

            double min_theta{ 0 }; ///< min angle of points in 2D plane
            double max_theta{ 0 }; ///< max angle of points in 2D plane
            double range_theta{ 0 }; ///< angle range of points in 2D plane

            CircleFit() = default;
        };

        /// fit circle
        CircleFit ZFS_API fit_circle(const std::vector<double>& x, const std::vector<double>& y);

    } // namespace math
} // namespace zfs
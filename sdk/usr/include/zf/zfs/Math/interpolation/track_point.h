#pragma once

#include <zfs/zfsapi.h>

#include <vector>

namespace zfs {
    namespace math {
        // =============================================================================================
        // TrackSamplePt
        // =============================================================================================
        struct ZFS_API TrackSamplePt
        {
            double x, y, z;
            double t;

            TrackSamplePt() : x(0.), y(0.), z(0.), t(0.) {}
            TrackSamplePt(const double& _x, const double& _y, const double& _z, const double& _t = 0.) : x(_x), y(_y), z(_z), t(_t) {}
        };
        // =============================================================================================
        typedef std::vector<TrackSamplePt> track_sample_points_t;


        struct ZFS_API TrackSamplePt4
        {
            double a, b, c, d;
            double t;

            TrackSamplePt4() : a(0.), b(0.), c(0.), d(0.), t(0.) {}
            TrackSamplePt4(const double& _a, const double& _b, const double& _c, const double& _d, const double& _t = 0.) : a(_a), b(_b), c(_c), d(_d), t(_t) {}
        };
        // =============================================================================================
        typedef std::vector<TrackSamplePt4> track_sample_points4_t;


        // =============================================================================================
        // TrackSamplePt2
        // =============================================================================================
        struct ZFS_API TrackSamplePt2
        {
            double x, y, z;
            double vx, vy, vz;
            double t;
            TrackSamplePt2() : x(0.), y(0.), z(0.), vx(0.), vy(0.), vz(0.), t(0.) {}
            TrackSamplePt2(const double& _x, const double& _y, const double& _z,
                const double& _vx, const double& _vy, const double& _vz,
                const double& _t = 0.) : x(_x), y(_y), z(_z), vx(_vx), vy(_vy), vz(_vz), t(_t) {}
        };
        // =============================================================================================
        typedef std::vector<TrackSamplePt2> track_sample2_points_t;
        // =============================================================================================

        // =============================================================================================
        // Interpolation
        // =============================================================================================
        typedef  TrackSamplePt TrackInterpolationPt;
        typedef  TrackSamplePt4 TrackInterpolationPt4;

        // =============================================================================================
        typedef std::vector<TrackInterpolationPt> interpolation_points_t;
        // =============================================================================================
        typedef  TrackSamplePt2 TrackInterpolationPt2;
        // =============================================================================================
        typedef std::vector<TrackInterpolationPt2> interpolation_points2_t;
        // =============================================================================================
    } // end namespace math
} // end namespace zfs
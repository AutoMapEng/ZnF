#pragma once
#include "../zfsapi.h"

#include <utility>
#include <vector>

#include "../Math/zfvec.h"

class ZFSLinebyline;
class ZFScan;


namespace zfs
{
    ZFS_API std::vector< zfs::math::Vec3d > collectPointsArroundCurrentPosition(ZFSLinebyline* lbl);
    ZFS_API std::vector< zfs::math::Vec3d > collectPointsArroundCurrentPosition(ZFScan* zfs);

    /**
    \brief compute normal vector
    pts 3 or more points
    \return succ,normalvector
    */
    ZFS_API std::pair<bool, zfs::math::Vec3d> computeNormal(const std::vector< zfs::math::Vec3d >& pts);
}

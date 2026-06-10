#pragma once

#include <zfs/zfsapi.h>
#include <zfs/Math/zfvec.h>

/// deprecated. Use ZFHessePlane instead
int ZFS_API zf_cut_pointnormal_onto_plane_3D( const double *hesse, zfs::math::Vec3d& Point);


// ---------------------------
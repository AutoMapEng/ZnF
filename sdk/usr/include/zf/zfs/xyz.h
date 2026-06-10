#pragma once
#include "zfsapi.h"

#include "Math/zfvec.h"



namespace zfs
{
    /**	\brief From cartesian coordinates to polar coordinates -- 1.09.2010 --
        \param henc_rad = high speed encoder (elevation,vertical) in scanner system
        \param lenc_rad = low speed encoder (horizontal) in scanner system
    */
    ZFS_API
        bool XYZ2polar(const zfs::math::Vec3d& v, double& henc_rad, double& lenc_rad, double* rg_m = nullptr);

    inline bool XYZ2polar(double x, double y, double z, double& henc_rad, double& lenc_rad, double* rg_m = nullptr)
    {
        return XYZ2polar( zfs::math::Vec3d( x, y, z ), henc_rad, lenc_rad, rg_m);
    }



    /**	From cartesian coordinates to polar coordinates (force back)  -- 01.09.2010 -- */
    ZFS_API
        bool XYZ2polar_back(double x, double y, double z, double& henc_rad, double& lenc_rad, double* rg_m = nullptr);
}

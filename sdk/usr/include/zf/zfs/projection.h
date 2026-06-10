#pragma once

#include "zfsapi.h"

#include <string>

#include <zfs/Math/zfvec.h>

namespace zf
{

    /**
    @brief holds params for projection
    */
    class ZFS_API ProjectionParams
    {
    public:
        std::string ellipsoidName;      ///< 'WGS84'
        std::string datumShiftName;     ///< 'default'
        int projectionEPSG{};       ///< 1001
        std::string projectionParams; /// '{18}=32.000000'
        zfs::math::Vec3d constantOffset;
        std::wstring geoidCorrectionFile; ///< optional
        std::string userDefined; ///< optional a user defined string
    };
}

#pragma once
#include "zfsapi.h"
#include "zfvector.h"

namespace zfs
{
    //#######################################################################################################################
    /**
    \brief Defines a line in 3d-space.
    */
    struct ZFS_API ZFLine3d
    {
        ZFVec3d point;          ///< A point on the line.
        ZFVec3d spanningVec;    ///< Vector that spans the line.
        bool valid;             ///< This is false when the plane was constructed with the default constructor.

        ZFLine3d( ZFVec3d pointArg, ZFVec3d spanningVector) : point(pointArg), spanningVec(spanningVector), valid(true){}
        ZFLine3d() : point(0.,0.,0.), spanningVec(0.,0.,0.), valid(false){}

        bool pointIsOnLine( const ZFVec3d& point);
        bool isValid();
    };
}

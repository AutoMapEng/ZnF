#pragma once
#include "zfsapi.h"

#include <utility>

#include "map.h"

class ZFScanZFS;
class ZFSLinebyline;

namespace zfs
{
    /**
    \brief determine the vertical shift
    \return shift [increments], center pixel
    */
    ZFS_API std::pair<int,int> determineVertShift( ZFScanZFS* z, ZFScanMap::mode_t, bool forceOld=false);
    ZFS_API std::pair<int, int> determineVertShift( ZFSLinebyline* lbl, ZFScanMap::mode_t, bool forceOld=false);
}
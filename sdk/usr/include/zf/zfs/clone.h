#pragma once
#include "zfsapi.h"

class ZFScan;

namespace zfs
{
    /// check whether cloneing is supported for this file type
    ZFS_API_F bool cloneSupported(const ZFScan* originalScan);

    /// clone a scan for multithreaded access for supported scan types
    ZFS_API_F ZFScan* ZFSClone(ZFScan*, bool copyRgbImageForEachClone = false);
}

#pragma once

#include "zfsapi.h"

class ZFSHeader;

namespace zfs
{
    /// check if this software is able to open this scan
    ZFS_API bool CheckDLLVer(ZFSHeader& h);
    ZFS_API bool CheckDLLVer(ZFSHeader& h, int this_software_major, int this_software_chkminor, int this_software_chkpatch);
}

#pragma once

#include <vector>
#include "zflineaccessimpl315.h"

#ifdef ZFS_5024_SUPPORT
#define ZFS_LINEACCESS_5024
#endif


#ifdef ZFS_LINEACCESS_5024

class ZFModel;


namespace zfs
{
    class SubEncoderFractionalValMappingTables;
}


class ZFS_API ZFLineAccess514 : public ZFLineAccess314
{
public:

    ZFLineAccess514(ZFLineInfo* inf);
    ZFLineAccess514(ZFLine*, ZFLineInfo* inf);

protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
};


class ZFS_API ZFLineAccess515 : public ZFLineAccess315
{
public:

    ZFLineAccess515(ZFLineInfo* inf);
    ZFLineAccess515(ZFLine*, ZFLineInfo* inf);


protected:
};


#endif // ZFS_LINEACCESS_5024

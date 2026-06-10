#pragma once

#include "zfsapi.h"
#include "config.h"

#include "zfs315.h"



#ifdef SUPPORT_ZFS515

class ZFS_API ZFScanZFS514 : public ZFScanZFS314
{
    DEFINE_ZFBASE(ZFScanZFS314)

public:

    ZFScanZFS514();

    void Init(ZFRect*, bool phys) override;

    virtual const char* DataTyp() const override { return "ZFSImager 514"; }

    bool Angle0(double& henc, double& lenc) const override;

protected:
    int m_vertoffset{-1};
};



class ZFS_API ZFScanZFS515 : public ZFScanZFS315
{
    DEFINE_ZFBASE(ZFScanZFS315)

public:

    ZFScanZFS515();

    virtual const char* DataTyp() const override { return "ZFSImager 515"; }


protected:
};



#endif // SUPPORT_ZFS515

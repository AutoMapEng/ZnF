#pragma once

#include "zfsapi.h"
#include "config.h"

#include "zfscanimpl.h"



#ifdef SUPPORT_ZFS315

/// sync by enc
class ZFS_API ZFScanZFS314 : public ZFScanZFS214
{
    DEFINE_ZFBASE(ZFScanZFS214)


public:

    ZFScanZFS314();

    virtual const char* DataTyp() const override { return "ZFSImager 314"; }

    virtual bool    Angle0(double& henc, double& lenc) const override;

    void correct(bool en);
    bool correcting() const;

    virtual double RefRg(int line) const override;
    virtual double RefRf(int line) const override;
    virtual double MOffset() const override;

protected:
    virtual int		_Rf() const override;
    virtual double  _Rgm() const override;
};

/**
* sync by lara clock
*/
class ZFS_API ZFScanZFS315 : public ZFScanZFS215
{
    DEFINE_ZFBASE(ZFScanZFS215)

public:

    ZFScanZFS315();

    virtual const char* DataTyp() const override { return "ZFSImager 315"; }

    virtual void    OnPostInit() override;

    virtual bool    Angle0(double& henc, double& lenc) const override;

    void correct(bool en);
    bool correcting() const;

protected:
    virtual int		_Rf() const override;
    virtual double  _Rgm() const override;
};





#endif // SUPPORT_ZFS315

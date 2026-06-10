#pragma once
#include "zfscan.h"

//#define SUPPORT_XYZim


#ifdef SUPPORT_XYZim

class ZFS_API XYZVector
{
public:
    double	x, y, z;
};

class ZFS_API XYZFVector
{
public:
    float	x, y, z;
};


class ZFS_API ZFScanXYZim : public ZFScan
{
    friend class ZFProject;

protected:
    unsigned short 	*iim;
    unsigned short 	*rim;
    XYZVector	    *im;
    XYZFVector	    *fim;
    double		    fovAzimuth;
    double		    fovElevation;


public:
    ZFScanXYZim();
    ~ZFScanXYZim();

    virtual const char* FileTyp() const override { return "XYZim"; };
    virtual const char* DataTyp() const override { return "Imager"; };

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const char* wchar_t = NULL, ZFRect* rc = NULL);
    virtual void CloseFile();
    void FreeMem();

    virtual bool HasIntens() { return true; }
    virtual bool HasRange() { return true; }
    virtual bool HasAngle() { return false; }
    virtual bool HasXYZ() { return true; }

    virtual int    Rgmm();
    virtual int    Rf();
    virtual bool   Angle0(double& henc, double& lenc);
    virtual bool   XYZ0(double& x, double& y, double &z);

protected:
};

#endif // SUPPORT_XYZim

#pragma once

#include "zfsapi.h"
#include "config.h"

#include "trmatrix.h"
#include "tol.h"
#include "encpixel2angle.h"
#include "Math/zfvec.h"
#include "way.h"
#include "mcalib.h"

#include <string>
#include <vector>
#include <memory>
#include <math.h>

#include "zfdef.h"

#ifdef ZFS_5003_SUPPORT
#define ZFS_MODEL_5003
#endif

#ifdef ZFS_5010_SUPPORT
#define ZFS_MODEL_5010
#endif

#ifdef ZFS_COLOR_SUPPORT
#define ZFS_MODEL_OSC
#endif



class ZFSHeader;
class ZFMechCalib;
class ZFLineAccess;
class ZFSLinebyline;
namespace zfs {
    class WayCounting;
}


class ZFS_API ZFModel
{
    friend class ZFSLinebyline;

protected:
    ZFSLinebyline* pParent{};	// may NULL
    int ench{}; // resolution encoder high-speed
    int encl{};
    bool useLProfiler{}; // use l-encoder for 3D calculation in profiler mode (imager in profiler mode. lmotor rotates)
    ZFTRMatrix proMat{}; // rotation matrix for profiler model

    double m_xAxis{}; // obsolete  x-direction value for profiler if not 0.
    std::shared_ptr<zfs::WayCounting> way{};

    zfs::VertPixel2Angle pixel2angle{};

    // for fast xyz calculation inc calibration
    bool calibLut{};		//< if possible
    bool calibLutUse{};	//< if used
    float* xerr{};	//< internal pointer to correctionvectors
    float* yerr{};
    float* zerr{};



public:
    std::string info;

    zfs::TOF tol{};	///< time of light correction

public:
    ZFModel(ZFSLinebyline* parent, const ZFSHeader* head); // parent could be NULL
    virtual ~ZFModel();

    // do a recalibration of a taken scan like quality temperature calibration
    // This recalib will modify the zfsheader items of the mechcalib
    void recalib( ZFSHeader* );

    virtual void PreInit(const ZFSHeader*) {}

    ZFSLinebyline* Parent() { return pParent; }
    const ZFSLinebyline* Parent() const { return pParent; }
    void AddInfo(const char*);

    virtual const char* Typ() const = 0;	// name
    bool IsCalibrated() const;
    const char* CalibInfo() const;

    bool SupportLUT() const { return calibLut; }
    bool UseLUT() const { return calibLutUse; }

//    void SetXDistance(double dis) { xAxis = dis; } // obsolete
//    double GetXDistance() const { return xAxis; } // obsolete
    void UseLProfiler(bool use = true, double rotX = 0., double rotY = 0., double rotZ = 0.); // deg

    /// depending on profiler or imager or way mode, adjust the low-speed / horizontal encoder value
    void adjustHorz(double& horz)
    {
        if (useLProfiler == false || way->getMod() == zfs::WAY_IMG_NO)
            horz = 0;
    }


    void InitWay(ZFSHeader*, int m, double v = .01, bool helix = true);
    int WayMod() const
    {
        return way->getMod();
    }
    double CalcWay(int line, double pixel__ = 0.) const // xAxis     pixelvalue from 0..1 for helix calculation if enabled
    {
        return way->CalcWay(line, pixel__);
    }
    void WayModEnable(bool);
    bool IsWayModeEnabled() const;
    double Way( const ZFLineAccess* line) const
    {
        return way->GetWay(line);
    }
    zfs::WayCounting& wayObject();




    /// calibrate the angle and range. Thread safe
    bool Angle(int pixel, const ZFLineAccess* /*line*/, double rg, double& /*henc_rad*/, double& /*lenc_rad*/) const;
    /// calibrate the angle and range. NOT thread safe
    bool Angle( const ZFLineAccess* /*line*/, double rg, double& /*henc_rad*/, double& /*lenc_rad*/) const;


    /// get the 3d value for a pixel
    /// Thread safe
    zfs::math::Vec3d XYZ0(int pixel__, const ZFLineAccess* line, double rg) const
    {
        return __XYZ0(pixel__, line, rg);
    }
    // obsolete NOT thread safe
    bool XYZ0(const ZFLineAccess* line, double rg, double& x, double& y, double& z) const;


    zfs::math::Vec3d XYZ0(double henc, double lenc, double rg) const
    {
        return __XYZ0(henc, lenc, rg);
    }
    // old interface
    bool XYZ0(double henc, double lenc, double rg, double& x, double& y, double &z) const
    {
        const auto v = __XYZ0(henc,lenc, rg);
        x = v.x(); y = v.y(); z = v.z();
        return true;
    }



    static bool angleFrontToAngleBack(double& henc, double& lenc);


    virtual bool ReadEncFile() { return true; }

    // internal to access the LUT
    bool FastXYZ(bool fast);
    bool IsFastXYZ() const;
    bool ErrVec(const ZFLineAccess* line, double& x, double& y, double& z);

    int getErrVects(const float **x, const float **y, const float **z) const;

protected:
    void make_calib_class(int typ, int ctyp);

    virtual bool __Angle(int pixel, const ZFLineAccess* /*line*/, double rg, double& /*henc_rad*/, double& /*lenc_rad*/) const = 0;

    virtual zfs::math::Vec3d __XYZ0(int pixel__, const ZFLineAccess* /*line*/, double rg) const { return{}; }

    // if angle is already calculated proceed with the rest
    virtual zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const { return{}; }


public:
    ZFMechCalib* calib{};		// Kalibriermodel
    int lines{};				//!< Number of lines in scan.
    int pixel{};				//!< Number of pixel of line.
};


//#######################################################################################################################################################
//#######################################################################################################################################################
//#######################################################################################################################################################




// ###################################################################################
// -------------------------------------------------------------------------------------------------
// D: Transformormation from Front KOS to Back KOS
// -------------------------------------------------------------------------------------------------
// I: henc, lenc:		angle described in front KOS
// O: henc, lenc:		angle described in back KOS
// -------------------------------------------------------------------------------------------------
inline bool ZFModel::angleFrontToAngleBack(double &henc, double &lenc)
{
    henc = 2.*PI - henc;
    if (lenc > PI)
        lenc -= PI;
    else
        lenc += PI;

    return true;
}


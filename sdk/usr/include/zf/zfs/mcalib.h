#pragma once

#include <string>
#include <memory>

#include "zfsapi.h"
#include "config.h"
#include "Math/zfvec.h"
#include "../Common/def.h"


#ifdef ZFS_5003_SUPPORT
#define ZFS_CALIB_UK
#endif

class ZFSHeader;
class ZFSensorModel;
class ZFLineAccess;
class ZFModel;
class ZFTheoModel;
class ZFAngleToPixel;
namespace zfs
{
	class ProfilerErrorVector;
}

//===================================================================================
//
// Basic abstract class for calibration
//

class ZFS_API ZFMechCalib
{
protected:
	bool calibrated{};
	bool enabled{};
	ZFModel* model{};  //!< Parent class, could be NULL


public:
	ZFMechCalib( ZFModel* pParent=NULL );
	virtual ~ZFMechCalib();

	virtual const char* Typ() const = 0;	// name
	virtual int Idx() const = 0;			// typ index

	virtual double ElevOffset() const  // [rad]
        { return 0;}

	ZFModel* Model() { return model; }
	const ZFModel* Model() const { return model; }

	// override this if a recalibration like temperaturecalibration is required
	virtual void recalib(ZFSHeader*) {}

	/** Loads calib data from header **/
	virtual bool Load( const ZFSHeader* ) 
        { return true; }  // must be implemented and called for init calibration

	/** Checks if calibration is available. **/
	bool IsCalibrated() const 
        { return calibrated; }

	void Enable( bool b )	
        { if( IsCalibrated() && b ) enabled=true; else enabled=false;  }

	/** Returns true if it has a calibration and calibration is enabled. **/
	bool IsEnabled() const	{ return enabled; }

	/** Calculates from uncalibrated encoder values to XYZ **/
    virtual zfs::math::Vec3d XYZ0(double UNUSED(henc) /*rad*/, double UNUSED(lenc) /*rad*/, double UNUSED(rg) /*mm*/) const = 0;
    bool XYZ0(double henc/*rad*/, double lenc /*rad*/, double rg /*mm*/, double& x /*m*/, double& y /*m*/, double& z /*m*/, bool fast = false) const
    {
        const auto v = XYZ0(henc, lenc, rg);
        x = v.x(); y = v.y(); z = v.z();
        return true;
    }

	/** Calculates from uncalibrated angle-rad values to calibrated encoder values. **/
    virtual bool toEncCal( double UNUSED(henc), double UNUSED(lenc), double UNUSED(rg), double& UNUSED(h), double& UNUSED(l), bool UNUSED(fast)=false ) const 
        { return false; } // rad

	/** Calculates from XYZ to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
    virtual bool xyzToEncUncal( double UNUSED(x), double UNUSED(y), double UNUSED(z), double& UNUSED(henc), double& UNUSED(lenc), bool UNUSED(front), bool UNUSED(fast)=false ) const 
        { return false; }

	/** Calculates from calibrated encoder values to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
    virtual bool encCalToEncUncal( double UNUSED(h), double UNUSED(l), double UNUSED(rg), double& UNUSED(henc), double& UNUSED(lenc), bool UNUSED(fast)=false ) const 
        { return false; }
};

//===================================================================================
/**
*	Basic calibration (only elev offset)
*/

class ZFS_API ZFMechCalib_Basic : public ZFMechCalib
{
protected:
	double elevOffset{}; // rad

public:
	ZFMechCalib_Basic( ZFModel* pParent );

	virtual const char* Typ() const override { return "ZF Basic"; }
	virtual int Idx() const override { return 10; }

	virtual bool Load( const ZFSHeader* ) override;

	virtual double ElevOffset() const override { return elevOffset; } // rad
	void SetElevOffset( double r ) { elevOffset = r; } // rad

	/** Calculates from uncalibrated encoder values to XYZ **/
	virtual zfs::math::Vec3d XYZ0( double henc /*rad*/, double lenc /*rad*/, double rg /*mm*/  ) const override;

	/** Calculates from uncalibrated encoder values to calibrated encoder values. **/
	virtual bool toEncCal( double henc, double lenc, double rg, double& h, double& l, bool fast=false ) const override;
	
    /** Calculates from XYZ to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
    virtual bool xyzToEncUncal( double UNUSED(x), double UNUSED(y), double UNUSED(z), double& UNUSED(henc), double& UNUSED(lenc), bool UNUSED(front), bool UNUSED(fast)=false ) const override
        {return false;}

	/** Calculates from calibrated encoder values to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
    virtual bool encCalToEncUncal( double UNUSED(h), double UNUSED(l), double UNUSED(rg), double& UNUSED(henc), double& UNUSED(lenc), bool UNUSED(fast)=false ) const override
        {return false;}
};


//===================================================================================

namespace zfs
{
	/**
	\brief checks for the datatyp if calibration typ is supported
	*/
	ZFS_API bool isCalibSupported(const int datatyp, const int caltyp);
}

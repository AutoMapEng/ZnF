#pragma once

#include "mcalib.h"



//===================================================================================
/**
*	Imager 5003 ZF-Uk calibration typ 1
*/

#ifdef ZFS_CALIB_UK
class ZFS_API ZFMechCalib_UK : public ZFMechCalib_Basic
{
protected:
	ZFSensorModel* pSensorModel{};

public:
	ZFMechCalib_UK( ZFModel* pParent );
	~ZFMechCalib_UK();

	virtual const char* Typ() const override { return "ZF-UK 1"; }
	virtual int Idx() const override { return 1; }

	virtual bool Load( const ZFSHeader* ) override;

	static void SetPath4Calibfile( const wchar_t* path ) { path4Calibfile = path; }

	virtual zfs::math::Vec3d XYZ0( double henc /*rad*/, double lenc /*rad*/, double rg /*mm*/  ) const override;
	/** Calculates from XYZ to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/

	/** Calculates from uncalibrated encoder values to calibrated encoder values. **/
	virtual bool toEncCal( double henc, double lenc, double rg, double& h, double& l, bool fast=false ) const override;

	/** Calculates from uncalibrated encoder values to XYZ **/
	virtual bool xyzToEncUncal( double x, double y, double z, double &henc, double &lenc, bool front, bool fast=false ) const override;
	
    /** Calculates from calibrated encoder values to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
	virtual bool encCalToEncUncal( double h, double l, double rg, double& henc, double& lenc, bool fast=false ) const override;

    ZFSensorModel* model() { return pSensorModel; }

private:
	static std::wstring path4Calibfile;
};
#endif // ZFS_CALIB_UK

//===================================================================================
/**
*	Imager 5003 ZF-Theo calibration typ 1005
*/

class ZFS_API ZFMechCalib_Imager5003_Theo : public ZFMechCalib_Basic
{
protected:
	ZFTheoModel* theo{};
	std::shared_ptr<zfs::ProfilerErrorVector> m_profiler_error_vector{};
	friend class ZFSLinebyline;

	//friend bool xyz2Pix(double x, double y, double z, ZFAngleToPixel &a2p, ZFTheoModel *theo, bool isProfiler, double xAxis, double &pixel, double &line, bool forceScdLayer, int lineszfs);

public:
	ZFMechCalib_Imager5003_Theo(ZFModel* pParent );
	~ZFMechCalib_Imager5003_Theo();

	virtual const char* Typ() const override { return "Imager 5003 Theo"; }
	virtual int Idx() const override { return 1100; }

	void recalib(ZFSHeader*) override;

	virtual bool Load( const ZFSHeader* ) override;

	/** Calculates from uncalibrated encoder values to XYZ **/
	virtual zfs::math::Vec3d XYZ0( double henc /*rad*/, double lenc /*rad*/, double rg /*mm*/ ) const override;

    // lowlevel
	/** Calculates from uncalibrated angle-rad values to calibrated encoder values. **/
	virtual bool toEncCal( double henc, double lenc, double rg, double& h, double& l, bool fast=false ) const override;

	/** Calculates from XYZ to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
	virtual bool xyzToEncUncal( double x, double y, double z, double &henc, double &lenc, bool front, bool fast=false ) const override;

	/** Calculates from calibrated encoder values to uncalibrated encoder values; if fast=true then LUT tables are used for fast calculation **/
	virtual bool encCalToEncUncal( double h, double l, double rg, double& henc, double& lenc, bool fast=false ) const override;

	// manipulation
	ZFTheoModel* getTheo();
    const ZFTheoModel* getTheo() const;
    virtual bool setTheo(ZFTheoModel* th);

};


//===================================================================================
/**
*	Profiler 5003
*/

class ZFS_API ZFMechCalib_Profiler5003 : public ZFMechCalib_Imager5003_Theo
{
	double a0{};
	double a1{};
	double a2{};

public:
    ZFMechCalib_Profiler5003( ZFModel* pParent ) : ZFMechCalib_Imager5003_Theo( pParent ) { a0 = a1 = a2 = 0.; }

	virtual const char* Typ() const override { return "Profiler 5003"; }
	virtual int Idx() const override { return 1100; }

	virtual bool Load( const ZFSHeader* h) override;

	/** Calculates from uncalibrated encoder values to XYZ **/
    virtual zfs::math::Vec3d XYZ0(double henc /*rad*/, double UNUSED(lenc) /*rad*/, double rg /*mm*/) const override;

    //lowlevel
	/** Calculates from uncalibrated angle-rad values to calibrated encoder values. **/
    virtual bool toEncCal( double henc, double UNUSED(lenc), double rg, double& h, double& l, bool fast=false ) const override {ZFMechCalib_Imager5003_Theo::toEncCal(henc, 0., rg, h, l, fast); return true;}
};


//===================================================================================
/**
*	Imager 5006 ==> TheoModel
*/

class ZFS_API ZFMechCalib_Imager5006_Theo : public ZFMechCalib_Imager5003_Theo
{
public:
    ZFMechCalib_Imager5006_Theo( ZFModel* pParent ) : ZFMechCalib_Imager5003_Theo( pParent ) {;}

	virtual const char* Typ() const override { return "Imager 5006 Theo"; }
	virtual int Idx() const override { return 2000; }
};

//===================================================================================
/**
*	Imager 5010
*/

#ifdef ZFS_5010_SUPPORT

class ZFS_API ZFMechCalib_Imager5010 : public ZFMechCalib_Imager5006_Theo
{
public:
    ZFMechCalib_Imager5010( ZFModel* pParent ) : ZFMechCalib_Imager5006_Theo( pParent ) {;}

	virtual const char* Typ() const override { return "Imager 5010"; }
	virtual int Idx() const override { return 2000; }
};

#endif // ZFS_5010_SUPPORT


//===================================================================================
/**
*	Imager 5016
*/

#ifdef ZFS_5016_SUPPORT

class ZFS_API ZFMechCalib_Imager5016 : public ZFMechCalib_Imager5006_Theo
{
public:
    ZFMechCalib_Imager5016(ZFModel* pParent) : ZFMechCalib_Imager5006_Theo(pParent) { ; }

    virtual const char* Typ() const override { return "Imager 5016"; }
    virtual int Idx() const override { return 2000; }
};

#endif // ZFS_5016_SUPPORT

//===================================================================================
/**
*	Imager 5024
*/

#ifdef ZFS_5024_SUPPORT

class ZFS_API ZFMechCalib_Imager5024 : public ZFMechCalib_Imager5016
{
public:
	ZFMechCalib_Imager5024(ZFModel* pParent) : ZFMechCalib_Imager5016(pParent) { ; }

	virtual const char* Typ() const override { return "Imager 5024"; }
	virtual int Idx() const override { return 2000; }
};

#endif // ZFS_5016_SUPPORT

//===================================================================================
/**
*	Imager 5006 ==> UKModel
*/

#ifdef ZFS_CALIB_UK
class ZFS_API ZFMechCalib_Imager5006_UK : public ZFMechCalib_UK
{
public:
	ZFMechCalib_Imager5006_UK( ZFModel* pParent ) : ZFMechCalib_UK( pParent ) {;}

	virtual const char* Typ() const override { return "Imager 5006 UK"; }
	virtual int Idx() const override { return 2000; }
};
#endif

//===================================================================================
/**
*	Imager 5006 ==> UKModel
*/

class ZFS_API ZFMechCalib_Profiler6000 : public ZFMechCalib_Basic
{
protected:
	double a{},b{},n{},c{};
	ZFTheoModel* theo{};

public:
	ZFMechCalib_Profiler6000( ZFModel* pParent );
	~ZFMechCalib_Profiler6000();

	virtual const char* Typ() const override { return "Profiler6000"; }
	virtual int Idx() const override { return 6000; }

	virtual bool Load( const ZFSHeader* ) override;

	/** Calculates from uncalibrated encoder values to XYZ **/
	virtual zfs::math::Vec3d XYZ0( double henc /*rad*/, double lenc /*rad*/, double rg /*mm*/  ) const override;
};

//===================================================================================

namespace zfs
{
	/**
	\brief checks for the datatyp if calibration typ is supported
	*/
	//ZFS_API bool isCalibSupported(const int datatyp, const int caltyp);
}

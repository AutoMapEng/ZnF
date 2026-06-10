#pragma once

#include "zfsapi.h"
#include "config.h"

#include <vector>
#include <cfloat>
#include <memory>
#include <math.h>

#include "zfdef.h"

#include "calib/mcalib_qcal.h"
#include "Math/zfvec.h"

class ZFSHeader;
namespace zfs {
    namespace calib {
        class MCalibTemp;
} }


/// check if there is a calibration set
ZFS_API
bool theomodel_check4params_set(const ZFSHeader* h);



class ZFS_API ZFTheoModel
{
public:

	typedef struct ZFS_API calibParams
	{
        double Theo_henc[4]{};
		double Theo_lenc[6]{};

		double Theo_AzimSin[5]{};
		double Theo_ElevSin[5]{};
		double Theo_ElevDroop[3]{};
		double Theo_AzimRange[3]{};
		double Theo_ElevRange[3]{};
		double Theo_AzimBearing[4]{};
		double Offset{};
		double storedRefValue{};
		double level[6]{};
        //double rps;
        zfs::calib::QCalParams qcal{};
        zfs::calib::QCalResult qcalResult{};
        std::shared_ptr<zfs::calib::MCalibTemp> mcaltemp{};

        //Standardmessunsicherheit
        double Theo_henc_std[4]{};
        double Theo_lenc_std[6]{};

        double Theo_AzimSin_std[5]{};
        double Theo_ElevSin_std[5]{};
        double Theo_ElevDroop_std[3]{};
        double Theo_AzimRange_std[3]{};
        double Theo_ElevRange_std[3]{};
		double Theo_AzimBearing_std[4]{};
        double Offset_std{};

		bool useLUTs{};
#ifdef RT_CALIBRATION
        bool useRFLUTs{};     // if "true" => realtime calibration is used to correct the angles "rt_angleUncalToAngleCal"
#endif // RT_CALIBRATION

        calibParams();
	} calibParams_t;

public:

    calibParams_t p{};

	ZFTheoModel( ZFSHeader* h, double elevOffset);
    ZFTheoModel() {}
    virtual ~ZFTheoModel() {}

    void init() { p = calibParams(); } ///< normalerweise nicht noetig

    void recalib( ZFSHeader* );

	void updateLUTs();

    /// return true if lut was created and enabled. Calculation via LUT
//not used    bool isFastCalculationPossible() const;

    void enable(bool en) { m_enable = en; }
    bool isEnabled() const { return m_enable; }

    // write calib zfs header values. Just for simulation and calibration
	bool write_zfsheader(ZFSHeader *h);
    
    // !!! function will modify zfsheader and so calibration !!!
	bool Load_via_zfsheader(ZFSHeader* h, double elevOffset);
    bool Load_const_via_zfsheader(const ZFSHeader* h, double elevOffset);

    bool Load_via_params(const ZFTheoModel::calibParams &p_);

	void angleUncalToAngleCal( double lenc, double henc, double rg, double &new_lenc, double &new_henc) const;
	bool angleUncalToAngleCal_Profiler( double lenc, double henc, double rg, double &new_lenc, double &new_henc) const;

    zfs::math::Vec3d angleCalToXYZ(double henc /*rad*/, double lenc /*rad*/, double rg /*mm*/) const;
    void angleCalToXYZ( double henc /*rad*/, double lenc /*rad*/, double rg /*mm*/, double &x /*mm*/, double &y /*mm*/, double &z /*mm*/ ) const;

    zfs::math::Vec3d angleUncalToXYZ( double henc /*rad uncal*/, double lenc /*rad uncal*/, double rg /*mm*/ ) const;
    void angleUncalToXYZ(double henc /*rad uncal*/, double lenc /*rad uncal*/, double rg /*mm*/, double &x, double& y, double& z) const
    {
        const auto v = angleUncalToXYZ(henc, lenc, rg);
        x = v.x(); y = v.y(); z = v.z();
    }
    zfs::math::Vec3d angleUncalToXYZ( double henc /*rad uncal*/, double rg /*mm*/ ) const; // without lenc (Profiler)
    void angleUncalToXYZ(double henc /*rad uncal*/, double rg /*mm*/, double &x, double& y, double& z) const
    {
        const auto v = angleUncalToXYZ(henc, rg);
        x = v.x(); y = v.y(); z = v.z();
    }

	bool xyzToAngleUncal( double x, double y, double z, double &henc, double &lenc, bool front ) const;
	bool xyzToAngleCal( double x, double y, double z, double &henc, double &lenc ) const;
	//bool angleCalToAngleUncal( double& calL, double& calH, double rg );

	// Profilerergaenzung
	bool xyzToAngleCalProfiler( double x, double y, double z, double &henc, double &lenc ) const;	//SL 18.10.2012

    bool angleCalToAngleUncal(double calL, double calH, double rg, double& ucalL, double& ucalH, bool getall = false) const;
	bool angleCalToAngleUncal_approx( double& calL, double& calH ) const;

	bool angleBackToAngleFront(double &henc, double &lenc) const;
	bool angleFrontToAngleBack(double &henc, double &lenc) const;
	//inline double toRad(double inc) { return inc*(2.*PI)/thisHenc;} ;

private:

    void recalib__(ZFSHeader* h);

    double getElevVsRangeErr(const double *a, double rg) const;
    double getElevVsElevErr_ii(const double *a, double henc) const;
    double getElevVsAzimErr(const double *a, double lenc) const;
    double getElevVsElevErr(double henc, const double *a) const;
    double getAzimVsElevErr(double henc, const double *a) const;
    double getAzimVsRangeErr(const double *a, double rg) const;
    double getAzimVsAzimErr(const double *a, double lenc) const;
    double getAzimVsAzimElevErr_ii(const double *a, double lenc, double henc) const;


private:

    // --------------------
    // uses LUTs ==> fast
    // --------------------
    bool generateLUTS();

    bool m_enable{ true };

    std::vector<float> LUT_elevVsElev_Front;					// mrad
    std::vector<float> LUT_azimVsElev_Front;					// mrad
    std::vector<float> LUT_azimVsRange_Front;					// [mm]
    std::vector<float> LUT_azimVsAzim_Front;					// mrad
    std::vector<float> LUT_elevVsRange_Front;					// [mm]

    std::vector<float> LUT_elevVsElev_Bck;						// mrad
    std::vector<float> LUT_azimVsElev_Bck;						// mrad
    std::vector<float> LUT_azimVsRange_Bck;					    // [mm]
    std::vector<float> LUT_azimVsAzim_Bck;						// mrad
    std::vector<float> LUT_elevVsRange_Bck;					    // [mm]

#ifdef RT_CALIBRATION
    // --------------------
    // uses rt_LUTs ==> real time calibration
    // --------------------
    bool rt_generateLUTS();

    struct AngelVsRange
    {
        float azim {}; // [mm]
        float ele {}; // [mm]
    };

    std::vector<AngelVsRange> RT_LUT_azimEleVsRange;

    struct AngleVsAngle
    {
        float hencVsHenc {}; // [mrad]
        std::vector<float> lencVsLenc; // [mrad]
    };

    std::vector<AngleVsAngle> RT_LUT_AzimEle_Front;
    std::vector<AngleVsAngle> RT_LUT_AzimEle_Back;
public:
    void rt_angleUncalToAngleCal(double lenc, double henc, double rg, double &new_lenc, double &new_henc) const;
    zfs::math::Vec3d rt_XYZ0(double henc, double lenc, double rg) const;

#endif //RT_CALIBRATION
};


//################################################################################################################################

inline double ZFTheoModel::getElevVsElevErr( double henc, const double *a) const
{
    return (a[0] + a[1] * sin(henc + PI / 2.) + a[2] * sin(3. * henc + a[3]));
}

// -----------------------------------------------------------------------------------
inline double ZFTheoModel::getAzimVsElevErr( double henc, const double *a) const
{
    try
    {
        double sinhenc = sin(henc);
        double tanhenc = tan(henc);
        if (fabs(sinhenc) < fabs(a[0] / 2.) || fabs(tanhenc) < fabs(a[0] / 2.) ||
            fabs(sinhenc) < DBL_EPSILON || fabs(tanhenc) < DBL_EPSILON)
        {
            henc = fabs(a[0] / 2.);
            if (henc == 0) henc = 0.0001;

            sinhenc = sin(henc);
            tanhenc = tan(henc);
        }

        if( sinhenc && tanhenc )
            return ((a[0] / sinhenc + a[1] / tanhenc + a[2] * sin(4. * henc + a[3]) + a[4] * sin(2. * henc + a[5])));		    // a[4] && a[5] ist neu am 02. Mai 2017

    }
    catch (...) { }
    return 0.;
}
// ----------------------------------------------------------------------------------

inline double ZFTheoModel::getElevVsElevErr_ii( const double *a, double henc) const
{
	if (henc<=PI)
		return( a[0] * sin ( henc  + a[1]) + a[2]);
	else if ( henc>PI && henc<=2.*PI )
		return( -getElevVsElevErr_ii(a, PI-(henc-PI)) );
	else
		return 0.;
}

// ----------------------------------------------------------------------------------

inline double ZFTheoModel::getAzimVsAzimElevErr_ii(const double *a, double lenc, double henc) const
{
	return (a[0] / 1000. * sin(2. * lenc + a[1]) + a[2] / 1000. * sin( henc + a[3]));
}

// ----------------------------------------------------------------------------------

inline double ZFTheoModel::getElevVsAzimErr( const double *a, double lenc) const
{
	return(
		a[0] * sin ( 2. * lenc  + a[1]) +
		a[2]
		);
}
// -------------------------------------------------------------------------------------
inline double ZFTheoModel::getAzimVsAzimErr( const double *a, double lenc) const
{
	return(
		a[0] * sin ( a[1]  * (2. * PI) + lenc )  +
		a[3] * sin ( a[4]  * (2. * PI) + 3.*lenc ) +
		a[2]
		);
}
// -------------------------------------------------------------------------------------
inline double ZFTheoModel::getAzimVsRangeErr( const double *a, double rg ) const
{
	if (rg<0.25)
		return getAzimVsRangeErr( a, 0.25 );

    if(a[1])
        return a[0] * pow(rg, a[1]) + a[2];
    return a[0] * pow(rg, -1) + a[2]; // a[1] = -1
}
// -------------------------------------------------------------------------------------
inline double ZFTheoModel::getElevVsRangeErr( const double *a, double rg) const
{
	// 0.25 !!
	if (rg<0.25)
		return getElevVsRangeErr( a, 0.25);

    if (a[1])
        return a[0] * pow(rg, a[1]) + a[2];
    return a[2];
}

// -------------------------------------------------------------------------------------

inline zfs::math::Vec3d ZFTheoModel::angleCalToXYZ(double henc, double lenc, double rg) const
{
    if (henc > PI)
    {
        henc = fabs(2. * PI - henc);
        if (lenc > PI)
            lenc -= PI;
        else
            lenc += PI;
    }

    henc -= TORAD(90);
    double coshenc2;

    if (cos(henc) < fabs(p.Theo_lenc[0] / 2.))
        coshenc2 = fabs(p.Theo_lenc[0] / 2.);
    else
        coshenc2 = cos(henc);


    const double rg_cos_ele = rg * cos(henc);

    return{
        sin(lenc)* rg* coshenc2,
        cos(lenc) * rg_cos_ele,
        sin(henc) * rg };
}

// -------------------------------------------------------------------------------------

inline zfs::math::Vec3d ZFTheoModel::angleUncalToXYZ(double henc, double lenc, double rg) const
{
    double new_lenc, new_henc;
    angleUncalToAngleCal(lenc, henc, rg, new_lenc, new_henc);
    return angleCalToXYZ(new_henc, new_lenc, rg);
}

// ------------------------------------------------------------------------------------------------

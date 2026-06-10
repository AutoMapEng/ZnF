#ifndef ZFS_THEOMODEL5016_H
#define ZFS_THEOMODEL5016_H

#include "zfsapi.h"
#include "config.h"
#include <vector>
#include <cfloat>
#include <math.h>

#include "../Common/def.h"
#include "theoModel.h"

class ZFSHeader;
class ZFSensorModel;
class ZFLineAccess;
class ZFModel;


class ZFS_API ZFTheoModel5016 : public ZFTheoModel
{
public:

    typedef struct calibParams
    {
        double Theo_henc[4];
        double Theo_lenc[4];
        double Theo_AzimSin[5];
        double Theo_ElevSin[5];
        double Theo_ElevDroop[3];
        double Theo_AzimRange[3];
        double Theo_ElevRange[3];
        double Offset;
        double storedRefValue;
        double level[6];

        //Standardmessunsicherheit
        double Theo_henc_std[4];
        double Theo_lenc_std[4];
        double Theo_AzimSin_std[5];
        double Theo_ElevSin_std[5];
        double Theo_ElevDroop_std[3];
        double Theo_AzimRange_std[3];
        double Theo_ElevRange_std[3];
        double Offset_std;

        bool useLUTs;
    } calibParams_t;

private:

    double getElevVsRangeErr(double *a, double rg);
    double getElevVsElevErr_ii(double *a, double henc);
    double getElevVsAzimErr(double *a, double lenc);
    double getElevVsElevErr(double henc, double *a);
    double getAzimVsElevErr(double henc, double *a);
    double getAzimVsRangeErr(double *a, double rg);
    double getAzimVsAzimErr(double *a, double lenc);


private:

    // --------------------
    // uses LUTs ==> fast
    // --------------------
    bool generateLUTS();
    bool releaseLUTs();

    double *LUT_elevVsElev_Front;					// mrad
    double *LUT_azimVsElev_Front;					// mrad
    double *LUT_azimVsRange_Front;					// [mm]
    double *LUT_azimVsAzim_Front;					// mrad
    double *LUT_elevVsRange_Front;					// [mm]

    double *LUT_elevVsElev_Bck;						// mrad
    double *LUT_azimVsElev_Bck;						// mrad
    double *LUT_azimVsRange_Bck;					// [mm]
    double *LUT_azimVsAzim_Bck;						// mrad
    double *LUT_elevVsRange_Bck;					// [mm]

public:

    calibParams_t p;

    ZFTheoModel5016(ZFSHeader* h, double elevOffset);
    ZFTheoModel5016();
    ~ZFTheoModel5016();

    virtual bool init(/*default*/);
    virtual void updateLUTs();

    virtual bool write(ZFSHeader *h);
    virtual bool Load(ZFSHeader* h, double elevOffset);

    virtual bool angleUncalToAngleCal(double lenc, double henc, double rg, double &new_lenc, double &new_henc);
    virtual bool angleCalToAngleUncal(double calL, double calH, double rg, double &ucalL, double &ucalH);
    virtual bool angleCalToAngleUncal_approx(double& calL, double& calH);
};

//################################################################################################################################
inline double ZFTheoModel5016::getElevVsElevErr(double henc, double *a)
{
#ifdef USE_ELEV_VS_ELEV_MORE
    return (a[0] + a[1] * sin(henc + PI / 2.) + a[2] * sin(3. * henc));
#else
    return (a[0] + a[1] * sin(henc + PI / 2.) + a[2] * sin(3. * henc + a[3]));
#endif // USE_ELEV_VS_ELEV_MORE

}

// -----------------------------------------------------------------------------------
inline double ZFTheoModel5016::getAzimVsElevErr(double henc, double *a)
{
    try
    {
        register double sinhenc = sin(henc);
        register double tanhenc = tan(henc);
        double fac = (fabs(henc) < DBL_EPSILON) ? 1.0 : henc / fabs(henc);

        if (fabs(sinhenc) < fabs(a[0] / 2.) || fabs(tanhenc) < fabs(a[0] / 2.) ||
            fabs(sinhenc) < DBL_EPSILON || fabs(tanhenc) < DBL_EPSILON)             // Anpassung wegen ProfilerJump180 SL
            return getAzimVsElevErr(henc + (0.003*fac), a);                         // Anpassung wegen ProfilerJump180 SL

        return ((a[0] / sinhenc + a[1] / tanhenc + a[2] * sin(4.*henc + a[3])));	// a[2] && a[3] ist neu am 07. Feb 2007
    }
    catch (...)
    {
        return 0.;
    }
}
// ----------------------------------------------------------------------------------
inline double ZFTheoModel5016::getElevVsElevErr_ii(double *a, double henc)
{
    if (henc <= PI)
        return(a[0] * sin(henc + a[1]) + a[2]);

    else if (henc > PI && henc <= 2.*PI)
        return(-getElevVsElevErr_ii(a, PI - (henc - PI)));
    else
        return 0.;
}
// ----------------------------------------------------------------------------------

inline double ZFTheoModel5016::getElevVsAzimErr(double *a, double lenc)
{
    return(
        a[0] * sin(2. * lenc + a[1]) +
        a[2]
        );
}
// -------------------------------------------------------------------------------------
inline double ZFTheoModel5016::getAzimVsAzimErr(double *a, double lenc)
{
    return(
        a[0] * sin(a[1] * (2. * PI) + lenc) +
        a[3] * sin(a[4] * (2. * PI) + 3.*lenc) +
        a[2]
        );
}
// -------------------------------------------------------------------------------------
inline double ZFTheoModel5016::getAzimVsRangeErr(double *a, double rg)
{
    double flt = a[0] * pow(rg, a[1]) + a[2];

    if (rg < 0.25)
        return getAzimVsRangeErr(a, 0.25);

    return(flt);
}
// -------------------------------------------------------------------------------------
inline double ZFTheoModel5016::getElevVsRangeErr(double *a, double rg)
{
    double flt = a[0] * pow(rg, a[1]) + a[2];

    // 0.25 !!
    if (rg < 0.75)
        return getElevVsRangeErr(a, 0.75);

    return(flt);
}

#endif //ZFS_THEOMODEL5016_H


#ifndef ZFS3CALIB_H
#define ZFS3CALIB_H

#include <zfcore/filename.h>


#define EXT_CALIBFILE   ".par"  // File-Extension


#define EPTS 200
#define APTS 200


#ifndef ULONG
typedef unsigned long ULONG;
#endif



class CCalData
{
protected:
    bool    m_bAvailable;

    void    ExtrapolateAziLUT (float *Angle, float *Error, int *npts, float Ubound, float Lbound);
    void    ExtrapolateLUTs ();
    float   InterpolateAngle (float *Angle, float *Error, int npts, float ActAngle);

public:
    struct CCal{
	int   nElev ;               /* Number of Points in Elevation List  */
	int   nAzi  ;               /* Number of Points in Azimuth List    */
	int   nDpI ;                /* Number of Points for Edpi and Eoff  */
	float VcalAziOff ;          /* Azimuth Offset [deg] of VCAL */
	float ElevOffRef ;          /* Elevation Offset of reference pole [deg]*/
	float Elev      [EPTS] ;    /* Elevation                     */
	float ElErrEle  [EPTS] ;    /* Elevation Error wrt Elevation */
	float Azi       [APTS] ;    /* Azimuth                       */
	float AzError   [APTS] ;    /* Azimuth Error wrt Azimuth     */
	float ElError   [APTS] ;    /* Elevation Error wrt Azimuth   */
	float AzEl_m    [APTS] ;    /* m  of Azimuth Err Fct wrt Elev */
	float AzEl_c    [APTS] ;    /* c  of Azimuth Err Fct wrt Elev */
	float AziV      [APTS] ;    /* Azimuth for Eoff and Edpi     */
	float Eoff      [APTS] ;    /* Delta Elevation Offset wrt Ref P */
	float Edpi      [APTS] ;    /* Elev DpInc at each Pole P'    */
    } CDat;

    CCalData();
    virtual ~CCalData();

    ULONG Version();
    void  Reset();
    bool  Init();
    bool  Init( const wchar_t* file );
    bool  Available() { return m_bAvailable; }
    bool  CorrectAngles( short encEle, double* azi, double* ele ); // correct data
    bool  Compare( CCalData* );
};




#endif // ZFS3CALIB_H

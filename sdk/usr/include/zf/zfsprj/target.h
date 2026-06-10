#pragma once
#include <zfcore/compiler.h>

#include "ZfsPrjBasic.h"

#include <memory>
#include <string>
#include <vector>

#include <zfs/zfbase.h>
#include "vecname.h"

#include "target_err.h"
#include "target_types.h"
#include "target_statistics.h"


//
// zfs stores only the subsampled coordinates in the header
// the read function calculates the 3D values for uncalibrated, calibrated and
// world calibrated targets the advantage of storing not 3d in the gheader is
// that a changed calibration
//   is automatically attached to the targets as well
//

class ZFSPRJ_API TargetRegisterValues 
{
protected:
    ZFVec3d mMargin; //// Unsicherheit der Target-Lokalisierung

public:
    ZFVec3d registerXYZDiff;   //!< x/y/z Distance zur durchschnittlichen
                                //!< Targetposition berechnet von LC
    double registerDifLC;      //!< Distance zur durchschnittlichen Targetposition
                                //!< berechnet von LC. -1 => Kein Wert vorhanden.
    ZFVec3d registerXYZKTDiff; //!< x/y/z Distance zum bekannten Target, falls
                                //!< vorhanden.
    double registerDiffKN; //!< Distanz zum bekannten Target, falls vorhanden. -1
                            //!< => Kein Wert vorhanden.

    bool mStdValues; ///!< True Standartwerte werden verwendet. False = vom User
                    /// eingegebene Werte.

    enum eTargetType { VAL_TACHY, VAL_SCAN, VAL_GPS };

public:
    TargetRegisterValues() { clear(); }

    void clear() 
    {
        registerXYZDiff.x() = registerXYZDiff.y() = registerXYZDiff.z() = -1;
        registerDifLC = -1;
        registerDiffKN = -1;

        mMargin.setXYZ( -1., -1., -1. );
        mStdValues = false;
    }

    void setMargin(double x, double y, double z) 
        { mMargin.setXYZ(x,y,z); }

    void setMarginsX(double x) { mMargin.setX(x); }
    void setMarginsY(double y) { mMargin.setY(y); }
    void setMarginsZ(double z) { mMargin.setZ(z); }

    double getMaringX() const { return mMargin.x(); }
    double getMaringY() const { return mMargin.y(); }
    double getMaringZ() const { return mMargin.z(); }

    bool sameMargin(const TargetRegisterValues &trv)  
        { return mMargin == trv.mMargin ? true : false; }
};



//######################################################################################################################

/** \class ZFTarget
 *
 * \brief	This class holds one target. It includes read and write
 *funktions for zfs files. \ToDo Target hinzufuegen/loeschen/umbennennen
 *ueberarbeiten damit die aktuell geoeffneten Scans immer aktualisiert werden
 **/
class ZFSPRJ_API ZFTarget : public VecName, public ZFBase
{
    DEFINE_ZFBASE(ZFBase)

protected:
    std::shared_ptr<zfprj::TargetStatistic> statisticFitting{}; ///< the statistic of the targetfitting
                        //!< und wird nur fuer nachtraegliche Korrekturen
                        //!< gespeichert.

    std::wstring scanName;     //!< Name of the Scan that contains this target
    std::wstring scanNamePath; //!< Path to the Scan that contains this target
    std::wstring scanExt; //!< Extension of the Scan that contains this target

public:
    double xwc{}; //!< world coordinates x value
    double ywc{}; //!< world coordinates y value
    double zwc{}; //!< world coordinates z value

    TargetRegisterValues mTargetRegInfo{}; ///< information abaut the registration

    bool success{}; //!< status of fitting
                // int    quality;			//!< 0=unused, 1=bad,
                // 100=good

    // calculated subsampled angle (uncalibrated)
    double angleh{}; // rad
    double anglel{}; // rad
    double alpha{};  // rad Winkel mit dem laser auf target schaut
    double fitquality{};

    static double quality_rg;
    static double quality_angle;
    static double quality_res;
    static double quality_fit;

    double xstart{}; // start point for target fitting
    double ystart{};

public:
    ZFTarget();
    ZFTarget(const ZFTarget &t);
    ZFTarget(const VecName &v);
    ZFTarget(const std::wstring &name, double x, double y, double z);
    ZFTarget(eTargetTyp t, const std::wstring &name, double x, double y, double z);
    virtual ~ZFTarget() {}

    /// typ of target
    int getTyp() const { return typ; }
    void setTyp(int tNr);

    const std::wstring &getScanName() const { return scanName; }
    std::wstring getScanNamePath() const { return scanNamePath + L"/" + scanName + scanExt; }
    void setScanName(const std::wstring &fileName, const std::wstring &path, const std::wstring &ext);

    const char *getNameC() const;    ///< return name of target
    const wchar_t *getNameW() const; ///< return name of target
    std::wstring getNameWString() const;
    void setName(const char *s);    ///< set name of target
    void setName(const std::wstring& s); ///< set name of target

    void setMargin(TargetRegisterValues::eTargetType type);

    //zfprj::TargetStatistic* statistic(); ///< the statistic of the targetfitting
    const std::shared_ptr<zfprj::TargetStatistic> getStatistic() const; ///< the statistic of the targetfitting
    void setStatistic(std::shared_ptr<zfprj::TargetStatistic> o);
    void setStatistic( const std::string& str );

    void readXml(const tinyxml2::XMLNode *targetNode) override;
    tinyxml2::XMLElement* writeXml(tinyxml2::XMLNode *targetElement, bool allParameter) const override;
    void writeXmlToNode(tinyxml2::XMLElement *targetNode, bool allParameter, bool saveUse) const override;

    std::wstring getTargetImageName() const;

public:
    static std::string Flags2String(int flags);
    static std::string Typ2String(int t);

public:
    void clear();
   
    static std::wstring getFlagText(int flag);
};


typedef std::vector<ZFTarget> targetlist_t;


//############################################################################################################################################
//############################################################################################################################################

inline ZFTarget::ZFTarget() 
    : VecName(), ZFBase() 
{
}

inline ZFTarget::ZFTarget(const std::wstring& name_, double x_, double y_, double z_)
    : VecName(), ZFBase() 
{
    typ = (int)TARGET_TYP_UNDEF;
    setName(name_.c_str());
    quality = 200;
    set(x_, y_, z_);
}

inline ZFTarget::ZFTarget(eTargetTyp t, const std::wstring& name_, double x_,double y_, double z_)
    : VecName(), ZFBase() 
{
    typ = (int)t;
    setName(name_.c_str());
    quality = 200;
    set(x_, y_, z_);
}

/**
 *	Kopierkontruktor fuer VecName objekte. Variablen die im VecName Objekt
 *nicht vorhanden sind bleiben auf den default werten.
 */
inline ZFTarget::ZFTarget(const VecName& v_) 
    : VecName(v_), ZFBase() 
{
    typ = (int)TARGET_TYP_UNDEF;
    quality = 200;
}

inline ZFTarget::ZFTarget(const ZFTarget& t) 
    : VecName(t), ZFBase() 
{ 
    *this = t; 
}


inline void ZFTarget::clear()
{
    *this = {};
    quality = 200;
    mTargetRegInfo.clear();
}



inline const char* ZFTarget::getNameC() const
{
    return name.c_str();
}

inline const wchar_t* ZFTarget::getNameW() const
{
    return name.w_str();
}

inline std::wstring ZFTarget::getNameWString() const
{
    return name.WString();
}

inline void ZFTarget::setName(const char* s)
{
    name = s;
}

inline void ZFTarget::setName(const std::wstring& s)
{
    name = s;
}


/**
 *	set the target type by number
 *	\param tNr = new target type number
 */
inline void ZFTarget::setTyp(int tNr) {
    typ = tNr;
    if (typ <= 9) {
        typ = tNr;
        switch (typ)
        {
        case 0:  typ = TARGET_TYP_AUTO; break;
        case 1:  typ = TARGET_TYP_APRIL; break;
        case 2:  typ = TARGET_TYP_CHESS; break;
        case 3:  typ = TARGET_TYP_SPHERE; break;
        case 4:  typ = TARGET_TYP_XYZ; break;
        case 5:  typ = TARGET_TYP_SPHERE; break;
        case 6:  typ = TARGET_TYP_XYZ; break;
        case 9:  typ = TARGET_TYP_PROFILER; break;
        case -1: typ = TARGET_TYP_FIT; break;
        }
    }
}

inline const std::shared_ptr<zfprj::TargetStatistic> ZFTarget::getStatistic() const
{
    return statisticFitting;
}

inline void ZFTarget::setStatistic(std::shared_ptr<zfprj::TargetStatistic> o)
{
    if (o)
    {
        statisticFitting = o;
    }
}

inline void ZFTarget::setStatistic(const std::string& str)
{
    statisticFitting = zfprj::TargetStatistic::create(str);
}


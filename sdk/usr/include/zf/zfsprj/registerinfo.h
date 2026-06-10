#ifndef ZFSPRJ_REGISTERINFO_H
#define ZFSPRJ_REGISTERINFO_H

#include "ZfsPrjBasic.h"
#include "vecnamelist.h"

#include "../zfs/trmatrix.h"

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QImage>
#include <QString>
#include <QObject>
#include <QColor>
SWITCH_WARNINGS_ON
#include <limits>

#define ZFDARKYELLOW QColor( 150, 150, 0 )
#define ZFDARKYELLOW_PRE QColor( 200, 150, 0 )

#define REGINF_TYP  "typ"
#define REGINF      "RegisterInfo"

namespace tinyxml2 {
    class XMLNode;
    class XMLElement;
}

class QImage;

/**
*   Virtuelle Basisklasse zur speicherung und klassiviezierung von Registrierergbnissen.
*/

class ZFSPRJ_API RegisterResultInfo
{
public:
    int typeWeight;                     //!<Gewicht des individuellen typs ist fuer jede Klasse unterschiedlich.
    QString mConnectionName;            //!<Name des VP auf den die Connection zeigt
    QString mInfoText;                  //!<Textausgabe aus der Berechnung
    int mErrorFlag;                     //!<Fehlernummer 0 => kein Fehler
    enum eQuality {
        QUAL_NOT_REG = 0,       //!<Nicht registriert
        QUAL_NOT_POS = 1,		//!<Registrierung nicht moeglich
        QUAL_BAD = 2,           //!<Feher bei der Registrierung
        QUAL_APPROXIMATED = 3,  //!<Naeherungswert
        QUAL_OK = 4,            //!<Registriert mit annehmbarem Ergebnis
        QUAL_GOOD = 5          //!<Registriert mit gutem Ergebnis
    };

    enum eWeightDef {
        REGINF_NOTREG = 810,        //!< Nicht registrierte und automatisch angelegte Connection
        REGINF_NOTREG_USER = 500,   //!< Nicht registrierte vom Benutzer angelegte Connection
        REGINF_BAD_REG = 200,       //!< Nicht erfolgreich registeriert.
        REGINF_TARGET = 10,        //!< Offset fuer die Target registrierung
        REGINF_PLANE = 20,        //!< Offset fuer die Plane to Plane registrierung
        REGINF_CLOUD = 30,        //!< Offset fuer die Cloud to Cloud registrierung
        REGINF_POLYGON = 0,        //!< Offset fuer die Polygonzug
        REGINf_OPTIMIZE = 10        //!< Offset fuer VR Optimierung
    };


#ifdef _DEBUG
    static int lfdNr;
    static int nrOpen;
    int nr;
#endif

protected:
    eQuality mQual;

public:
    RegisterResultInfo();
    virtual ~RegisterResultInfo();


    virtual void clear()
    {
        mConnectionName = "";
        mInfoText = "";
        mErrorFlag = 0;
        mQual = QUAL_NOT_REG;
    }
    virtual void calcQual() = 0;

    virtual eQuality getQuality() const { return mQual; }

    /**
    *   Liest die als XML gespeicherte RegisterInfo aus, erstellt die Entsprechende Klass und uebernimmt die Daten.
    *   \param registerDate => RegisterResultInfo Element im XML Dokument.
    */
    static RegisterResultInfo* LoadRegisterInfo( tinyxml2::XMLElement* registerData);
    static RegisterResultInfo* CopyRegisterInfo(const RegisterResultInfo* org);

    //Rein virtuelle Methoden
    virtual QString getString(bool longText = false)  const = 0;
    virtual QString getStringWithouXML() const = 0;

    /**
    *	Speichern des Objektes in der XML Strucktur des Projektes
    *	\param registerData => XMLNode an den die Info angehengt werden soll
    *	\param newMode =>
    *	\param saveVersion => Version in der das Projekt abgespeichert werden soll.
    */
    virtual void Save( tinyxml2::XMLNode* registerData, bool newNode, double saveVersion) const = 0;

    virtual double getWeight() = 0;

    virtual bool operator>(RegisterResultInfo* info) = 0;

    enum { Type = 0 };
    virtual int type() const = 0;



    virtual QString getValue1( bool unit ) const = 0;
    virtual QString getValue2( bool ) const = 0;
    virtual QString getTooltipTextV1() const = 0;
    virtual QString getTooltipTextV2() const = 0;


    virtual QColor getColor() const;
    virtual QString getHexCol() const;


protected:
    virtual bool Load( tinyxml2::XMLElement* regInfo) = 0;
};

/**
*   Klasse speicherung und klassiviezierung von Registrierergbnissen mit targets (Scan to Scan und Scan to Tachy)
*/

class ZFSPRJ_API TargetRegisterInfo : public RegisterResultInfo
{
public:
    //Grenzwerte
    static double maxTargetAverageGood;       //!<Maximale standartabweichung um qualit?t Good zu erhalten
    static double maxTargetAverageOK;        //!<Maximal standartabweichung um qualitaet Ok zu erhalten

    enum eRegTyp { UNKNOWN, ZF, SCANTRA, NEPTAN_PRJ };  //!<Registriert mit Typ

private:
    RegisterResultInfo* mBasicRegisterInfo;		//!< Basic RegisterInfo Objekt

    VecNameList mKnownTargetResultList;
    VecNameList mTargetResultList;
    QString errorString;
    bool mUseKnownTarget;                   //!< true => bekannte Targets (Tachy) verwendet
    eRegTyp mRegTyp;
    double mStdDef;                         //!< stdDef aller Targets

public:
    TargetRegisterInfo() : RegisterResultInfo(), mBasicRegisterInfo(nullptr), mUseKnownTarget(false), mRegTyp(UNKNOWN) { typeWeight = REGINF_TARGET; }
    TargetRegisterInfo(RegisterResultInfo* basicResult, eRegTyp regTyp, bool knownTarget, const QString& name, VecNameList& targetResultList, VecNameList& knownTargetResultList) : RegisterResultInfo(), mBasicRegisterInfo(NULL) { typeWeight = 100; clear(); init(basicResult, regTyp, knownTarget, name, targetResultList, knownTargetResultList); }
    TargetRegisterInfo(const TargetRegisterInfo* o);
    virtual ~TargetRegisterInfo();

    const RegisterResultInfo* getBasigRegInfo() const { return mBasicRegisterInfo; }

    virtual eQuality getQuality() const { return (mQual != QUAL_NOT_REG || !mBasicRegisterInfo) ? mQual : mBasicRegisterInfo->getQuality(); }

    void init(RegisterResultInfo* basicResult, eRegTyp regTyp, bool knownTarget, const QString& name, VecNameList& targetResultList, VecNameList& knownTargetResultList)
    {
        if (basicResult != this)
            mBasicRegisterInfo = RegisterResultInfo::CopyRegisterInfo(basicResult);
        mRegTyp = regTyp;
        mUseKnownTarget = knownTarget;
        mConnectionName = name;
        mTargetResultList = targetResultList;
        mKnownTargetResultList = knownTargetResultList;
        calcQual();
    }

    virtual void calcQual();
    virtual void clear()
    {
        delete mBasicRegisterInfo;
        mBasicRegisterInfo = nullptr;
        RegisterResultInfo::clear();
        mTargetResultList.clearValues();
        mKnownTargetResultList.clearValues();
        errorString = "";
        mRegTyp = UNKNOWN;
        mUseKnownTarget = false;
    }

    virtual QString getString(bool longText = false) const;
    virtual QString getStringWithouXML() const { return ""; }
    virtual QString getValue1( bool unit) const;
    virtual QString getValue2( bool ) const { return ""; }
    virtual QString getTooltipTextV1() const;
    virtual QString getTooltipTextV2() const { return ""; }

    virtual void Save( tinyxml2::XMLNode* registerData, bool newNode, double saveVersion) const;

    enum { Type = 1 };
    virtual int type() const { return Type; }

    const VecNameList* getTargetResultList() { return &mTargetResultList; }
    const VecNameList* getKnownTargetResultList() { return &mKnownTargetResultList; }
    bool useKnownTarget() { return mUseKnownTarget; }

    virtual double getWeight();

    virtual bool operator>(RegisterResultInfo* info);

protected:
    virtual bool Load( tinyxml2::XMLElement* regInfo);
};

/**
*   Klasse speicherung und klassiviezierung von Registrierergbnissen mittels der VR Bibliothek (Cloud to Cout)
*/

class ZFSPRJ_API CToCRegisterInfo : public RegisterResultInfo
{
public:
    static double distanceStdDev_Good;
    static double distanceStdDev_OK;
    static double identityness_RGood;
    static double identityness_ROk;
    static double identityness_tGood;
    static double identityness_tOk;
    static double agreePercentageGood;
    static double agreePercentageOk;

    static double checkValue_t;
    static double checkValue_R;

private:
    bool mMaster;               //Flag ob es sich um den Masterscan handelt
    bool preRegistered;         //!<Flag ob die Berechnung unter Beruecksichtigung der Vorregistrierung erfolgte
    bool mOnlyPreRegisted;      //!<Flag ob nur die Vorregistrierung berechnet wurde.
    /**Kriterium fuer die Grobregistrierung. Dazu wird
    eine Registrierung von Scan s1 nach Scan s2 durchgefuehrt, und dann umgekehrt von s2
    nach s1. Diese beiden Transformationen sollten sich im Idealfall aufheben. identityness_R
    liegt im Wertebereich [0, Pi], identityness_t ist unbegrenzt und hat die Einheit Millimeter.
    Deutlich von 0 verschiedene Werte (also beispielsweise identityness_R > 0.2 oder
    identityness_t > 1500) koennen auf eine fehlerhafte Grobregistrierung hinweisen.*/
    double identityness_R;          //~0
    double identityness_t;             //~0

    /**Hat der Nutzer verifiziert, dass die Scans grob richtig registriert sind, geben
    diese 3 Werte Auskunft ueber die Genauigkeit (Standardabweichung) der Feinregistrierung in
    x, y und z Richtung. Bei unseren Tests wurden dabei bei korrekter Feinregistrierung jeweils
    Werte < 7mm ermittelt. Ungenauigkeiten spiegelten sich in unseren Tests jeweils in deutlich
    hoeheren Werten wider. Auch eine fehlerhafte Grobregistrierung kann (muss aber nicht zwingend)
    zu erhoehten stdDevs Werten fuehren.*/
    double stdDevs[3];              // <7 wenn vorregistreierung vorhanden
    double d;						// Berechnete distanz aus stdDevs

    /**Anzahl der Punkte, die bei der Feinregistrierung eine Korrespondenz
    gefunden haben (Inliers) und die keine Korrespondenz gefunden haben (Outliers).
    Eine hohe Anzahl von Outliern im Verh?ltnis zu den Inliern kann ein Indiz fuer eine fehlerhafte
    Grobregistrierung sein. Die Werte sind allerdings nur ein grober Anhaltspunkt, da die Anzahl
    an Outliern auch in Situationen mit wenig Ueberlappung zwischen den aufeinanderfolgenden
    Scans auftreten kann (z. B. in einem Tuerdurchgang).*/
    int numInliers;               //nur Anzeige
    int numOutliers;              //nur Anzeige

    /**Grad der Uebereinstimmung nach der Grobregistrierung. Die Uebereinstimmung
    sinkt, wenn sich Messpunkte aus einem Scan im als frei gemessennen Raum
    des jeweils anderen befinden. Der Wert liegt im Bereich [0, 1] und kann zur Erkennung von
    Fehlern bei der Grobregistrierung verwendet werden. Werte nahe 1 deuten auf eine erfolgreiche
    Registrierung hin. In unseren Tests deutete einWert < 0.9 bereits auf Registrierungsfehler
    hin.*/
    double agreePercentage;      // > 0.9

    /**Dieser Wert beschreibt die Abweichung zwischen der vom
    Anwender als Startwert uebergebenen und der von der Bibliothek berechneten Drehung. Diese
    Abweichung ist nur aussagekr?ftig, wenn der gesch?tzte Startwert tats?chlich dicht an der
    Wirklichkeit war.*/
    double deviationFromGuess_R;    //nur bei vorregistrierung und als info

    bool fineRegistrationConverged; //! Whether or not the fine registration converged. Lack of convergence is often an indicator for a failed coarse registration.

    int version;		//vr version  0: < 2.0.0 stdDevs wird in mm uebergeben
                        //			  1: >= 2.0.0 stdDevs wird in m uerbergeben
                        //			  2: >= 2.2.2 Historgramm wird mit negativen und possitiven Werten uebergeben

    int deviationHistogram[100];
    double stdDevMatchingThreshold;

    double mConfidence;

public:
    CToCRegisterInfo() : RegisterResultInfo()
    {
        typeWeight = REGINF_CLOUD; clear(); 
    }
    CToCRegisterInfo(bool master, const QString& name, bool onlyPreRegisted, bool prereg, double id_R, double id_t, double stdD[3], int inlier, int outlier, double agreePerc, double devGuess_R, int vers, bool fineRegConverged, int devHistogram[100], double stdDevMatchingThresh)
        : RegisterResultInfo()
    {
        typeWeight = REGINF_CLOUD;
        clear();
        init(master, name, onlyPreRegisted, prereg, id_R, id_t, stdD, inlier, outlier, agreePerc, devGuess_R, vers, fineRegConverged, devHistogram, stdDevMatchingThresh, -1 );
    }
    virtual ~CToCRegisterInfo();

    double getStdD() const { return d; }
    double getStdX() const { return stdDevs[0]; }
    double getStdY() const { return stdDevs[1]; }
    double getStdZ() const { return stdDevs[2]; }

    bool isPreregistered() { return preRegistered; }

    QImage getHistogramImage(int width, int height, const QFont& font, const QColor& backgroundCol = Qt::white, bool heading = true, bool fullScale = true ) const;
    bool hasHistogram() const;

    void init(bool master, const QString& name, bool onlyPreRegisted, bool prereg, double id_R, double id_t, double stdD[3], int inlier, int outlier, double agreePerc, double devGuess_R, int vers, bool fineRegConverged, int devHistogram[100], double stdDevMatchingThresh, double confidence );

    virtual void calcQual();
    virtual void clear();

    virtual QString getString(bool longText = false) const;
    virtual QString getStringWithouXML() const;

    virtual void Save( tinyxml2::XMLNode* registerData, bool newNode, double saveVersion) const;

    void setConfidence( double con ) { mConfidence = con; }
    double getConfidence() { return mConfidence; }

    enum { Type = 2 };
    virtual int type() const { return Type; }

    virtual double getWeight();
    virtual bool operator>(RegisterResultInfo* info);


    virtual QColor getColor() const;

    virtual QString getValue1( bool unit ) const;
    virtual QString getValue2( bool expert ) const;

    virtual QString getTooltipTextV1() const;
    virtual QString getTooltipTextV2() const;

    double getOverlab();

protected:
    virtual bool Load( tinyxml2::XMLElement* regInfo);
};


/**
*   Klasse speicherung und Klassiviezierung von Registrierergbnissen mittels der Scantra (Plan to Plan)
*/

class ZFSPRJ_API PlaneToPlaneRegisterInfo : public RegisterResultInfo
{
public:
    static double mSigma_tGood;
    static double mSigma_tOk;

private:
    bool mVPInfo;                  //true => Registerinfo fuer Scan false => fuer Connection
    bool mApproximation;            //true => Naeherungswert
    bool mHasParam;                 //true => Wert wurde berechnet
    double mSigma_t;                //Gesamtstandardabweichung des Translationsvektors
    bool mOnlyTarget;               //true => Nur target Registrierung
    bool mUseKnownTarget;           //true => Bekante Targets wurden verwendet.

public:
    PlaneToPlaneRegisterInfo();
    PlaneToPlaneRegisterInfo(const QString& name, bool hasParam, bool approx, double sigma_t, bool vpInfo, bool onlyTarget, bool useKnownTarget );
    virtual ~PlaneToPlaneRegisterInfo();

    virtual void clear();

    virtual void calcQual();

    virtual QString getString(bool longText = false) const;
    virtual QString getStringWithouXML() const;
    virtual void Save( tinyxml2::XMLNode* registerData, bool newNode, double saveVersion) const;

    enum { Type = 3 };
    virtual int type() const { return Type; }

    virtual double getWeight();
    virtual bool operator>(RegisterResultInfo* info);


    virtual QString getValue1( bool unit ) const;
    virtual QString getValue2( bool ) const { return ""; }

    virtual QString getTooltipTextV1() const;
    virtual QString getTooltipTextV2() const { return ""; }

protected:
    virtual bool Load( tinyxml2::XMLElement* regInfo);
};

/**
*   Klasse speicherung und Klassiviezierung von Registrierergbnissen mittels des Polygonzuges
*/

class ZFSPRJ_API TraverseRegisterInfo : public RegisterResultInfo
{
public:
    static double mCoordinateCloseConflictGood;
    static double mCoordinateCloseConflictOk;

    enum eTraverseTyp { NONE, CIRCLE, CIRCLE_FP, LINE };

private:
    eTraverseTyp mRegTyp;   //Art der Polygonregistrierung
    bool mMaster;       //Erster scan in der reihe
    double mvX;         //Koordinatenabschlusswiederspruch proportional zur Laenge der Polygonseite
    double mvY;         //Koordinatenabschlusswiederspruch proportional zur Laenge der Polygonseite

public:
    TraverseRegisterInfo();
    TraverseRegisterInfo(const QString& name, double vX, double vY, bool master, eTraverseTyp regTyp);
    TraverseRegisterInfo(const TraverseRegisterInfo& o);
    virtual ~TraverseRegisterInfo();

    void init(double vX, double vY, bool master, eTraverseTyp regTyp);
    virtual void calcQual();
    virtual void clear();

    virtual QString getString(bool longText = false) const;
    virtual QString getStringWithouXML() const;
    virtual void Save( tinyxml2::XMLNode* registerData, bool newNode, double saveVersion) const;

    enum { Type = 4 };
    virtual int type() const { return Type; }

    virtual double getWeight();
    virtual bool operator>(RegisterResultInfo* info);

    virtual QString getValue1( bool unit ) const;
    virtual QString getValue2( bool expert ) const;

    virtual QString getTooltipTextV1() const;
    virtual QString getTooltipTextV2() const;

protected:
    virtual bool Load( tinyxml2::XMLElement* regInfo);
};


class ZFSPRJ_API VROptimizeRes
{
public:
    ZFTRMatrix mResiduals;
    double mStdDevX;
    double mStdDevY;
    double mStdDevZ;
    double mStdDev;

public:
    VROptimizeRes( const ZFTRMatrix& res ) 
        : mResiduals(res), 
        mStdDevX(std::numeric_limits<double>::max()), 
        mStdDevY(std::numeric_limits<double>::max()), 
        mStdDevZ(std::numeric_limits<double>::max()),
        mStdDev(std::numeric_limits<double>::max())
    {}
    VROptimizeRes() 
        : mStdDevX(std::numeric_limits<double>::max()), 
        mStdDevY(std::numeric_limits<double>::max()),
        mStdDevZ(std::numeric_limits<double>::max()), 
        mStdDev(std::numeric_limits<double>::max()) 
    {}
    VROptimizeRes( double stdDevX, double stdDevY, double stdDevZ ) : mStdDevX( stdDevX ), mStdDevY( stdDevY ), mStdDevZ( stdDevZ ), mStdDev( 0. )
    {
        calc();
    }

    bool hasStdDevValues() { return mStdDevX != std::numeric_limits<double>::max() ? true : false; }

    void calc()
    {
        mStdDev = sqrt( ( mStdDevX*mStdDevX ) + ( mStdDevY*mStdDevY ) + ( mStdDevZ * mStdDevZ ) );
    }
};

class ZFSPRJ_API VROptimizeRegisterInfo : public RegisterResultInfo
{
    VecNameList mDistMapTarget;   //!< Tabelle mit allen abweichungen von der berechneten Targetposition der vorhandenen und genuetzten Targets des Scantpunktes.
    VecNameList mDistMapKnownTarget;   //!< Tabelle mit allen abweichungen von der berechneten Targetposition der vorhandenen und genuetzten Targets des Scantpunktes zu den bekannten Targets.
    std::map<std::wstring, VROptimizeRes> mstdDevMap; //!< Tabelle mit allen hereinkommenden ueberarbeiteten stdDef Werten aus den Connections

    double mStdDevConnection;                     //!<Durchschnitt aus allen Werten aus der Connection Tabelle
    double mDiffGes;                              //!< Durchschnitt aus beiden Tabellen
    bool masterScanPos;                 //!<Feste Scan position

    ZFVec3d mStdDiffPos;
    double mStdDiffDist;

public:
    static double mOptimizeGood;
    static double mOptimizeOk;

public:
    VROptimizeRegisterInfo();
    VROptimizeRegisterInfo( const ZFVec3d& difPos, const std::map<std::wstring, VROptimizeRes>& stdDevMap, const VecNameList& targetMap, const VecNameList& knownTargetMap, bool isFix );
    VROptimizeRegisterInfo( const VROptimizeRegisterInfo& o );

    virtual void calcQual();
    virtual void clear();

    bool hasStdDefInfo() const { return mStdDiffPos.x() != -1 ? true : false; }
    /**
    *   Rueckgabe des Registrierungsergebnisses als Text
    *   \param longText => wird nicht benuetzt
    */
    virtual QString getString( bool longText = false )  const;
    virtual QString getStringWithouXML() const;

    /**
    *	Speichern des Objektes in der XML Strucktur des Projektes
    *	\param registerData => XMLNode an den die Info angehengt werden soll
    *	\param newMode =>
    *	\param saveVersion => Version in der das Projekt abgespeichert werden soll.
    */
    virtual void Save(  tinyxml2::XMLNode* registerData, bool newNode, double saveVersion ) const;
    virtual double getWeight();
    virtual bool operator>( RegisterResultInfo* info );

    enum { Type = 5 };
    virtual int type() const { return Type; }



    virtual QString getValue1( bool unit ) const;
    virtual QString getValue2( bool ) const { return ""; }

    virtual QString getTooltipTextV1() const;
    virtual QString getTooltipTextV2() const { return ""; }

    /**
    *   Gibt die Summe aller Standardabweichungen der Connections und die Anzahl zurueck
    *   \param[out] ges = Summe aller Standardabweichungen (C2C)
    *   \param[out] nr = Anzahl der Standardabweichungen (C2C)
    */
    void getStdDefConnection( double& ges, int& nr, VecNameList& list ) const;
    /**
    *   Gibt die Summe aller Standardabweichungen im Quadrat der Connections und die Anzahl zurueck
    *   \param[out] ges = Summe aller Standardabweichungen (C2C)
    *   \param[out] nr = Anzahl der Standardabweichungen (C2C)
    */
    void getStdDefConnectionSqr( double& ges, int& nr_ ) const;

    /** 
    *   Gibt die Maximale std einer Connection zurück.
    */
    double getMax();

    void getStdDefConnectionD_X_Y_Z( double& d, double& x, double& y, double& z ) const;

    const VecNameList& getTargetMap() { return  mDistMapTarget; }
    const VecNameList& getKnownTargetMap() { return  mDistMapKnownTarget; }

protected:
    virtual bool Load(  tinyxml2::XMLElement* regInfo );

    QString getTargetListHTML( bool longText ) const;

};
#endif //ZFSPRJ_REGISTERINFO_H

#pragma once

#include <mutex>

//#include "err.h"
#include <map>
#include <vector>
#include "Project.h"

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QObject>
#include <QString>
#include <QMutex>
#include <QObject>
#include <QColor>
#include <QStringList>

#include <boost/graph/adjacency_list.hpp>
SWITCH_WARNINGS_ON

#include "ZfsPrjBasic.h"
#include "register.h"
#include "target.h"
#include "vecnamelist.h"
#include "registerinfo.h"
#include <zfs/level/level_errors.h>

#ifdef SUPPORT_PRJ_UNDO
#include "../zftools/undo.h"
#endif

#include "groups.h"

#include "err.h"

// XML id's



class ZFSHeader;
class ViewPoint;
class RegisterVecName;
class ZFRegisterObjNode;
class ZFUnit;
class RegisterResultInfo;

typedef boost::property<boost::edge_weight_t, double> EdgeWeightProperty;
typedef boost::adjacency_list < boost::listS, boost::vecS, boost::undirectedS, QPointer<ViewPoint>, EdgeWeightProperty > Graph;

//#####################################################################################

/**
*	Fehlerklasse fuer die Registrierung. Die Fehler sind in zfsprj/err.h definiert
*/

class RegisterErrorProj
{
protected:
    regerr_t mErrNr;
    std::wstring mText;

public:
    RegisterErrorProj( regerr_t nr, const std::wstring& text )
    {
        mErrNr = nr;
        mText = text;
    };
    RegisterErrorProj()
    {
        mErrNr = REGERR_UNKNOWN;
    };

    void setNr( regerr_t nr ) { mErrNr = nr; mText =L""; }
    regerr_t getNr() { return mErrNr; }
    std::wstring& errText() { return mText; }

};


//#####################################################################################
/**
 * Hilfsklasse zum Bewerten der Genauigkeit der Vorregistrierung
 */
class ZFSPRJ_API RegisterPrecision {

public:
    static double precisionTransitionMax;
    static double precisionRotationMax;
    static double precisitonHeightMax;
    static double precisionTransitionMin;

    double precisionTransition{20};
    double precisionRotation{180};
    double precisionHeight{20};

    //Ergebnisse aus der letzten vorregistrierung mit CtoC
    double identityness_t{};
    double identityness_R{};

    RegisterPrecision() = default;

    void defaultValues() { precisionTransition = 20; precisionRotation = 180; precisionHeight = 20; /*regFlag = 0;*/ }

    void checkValues();
};

//#####################################################################################

#ifdef SUPPORT_PRJ_UNDO

#define UNDO_ADD_CONNECTION                 L"UndoAddConnection"
#define UNDO_DELETE_CONNECTION              L"UndoDeleteConnection"
#define UNDO_CHANGE_CONNECTION_REGINFO      L"UndoChangeConnectionReginfo"
#define UNDO_CHANGE_REGISTRATION            L"UndoChangeRegistration"
#define UNDO_CHANGE_GLOBAL_MATRIX           L"UndoChangeGlobalMatrix"
#define UNDO_CHANGE_GLOBAL_MATRIX_NAME      L"UndoChangeGlobalMatrixName"
#define UNDO_SET_MASTER                     L"UndoSetMaster"
#define UNDO_CHANGE_REGISTRATION_PRECISION  L"UndoChangeRegistrationPresicion"
#define UNDO_LOCK_SCANPOS                   L"UndoLockScanpos"
#define UNDO_CHANGE_TARGET_RESULT           L"UnodChangeTargetResult"
#define UNDO_REG_FLAG_SCANPOS               L"UndoChangeRegisterFlag"
#define UNDO_CHANGE_CONNECTION_STATUS       L"UndoChangeConnectionStatus"
#define UNDO_CHANGE_SCANPOS_NAME            L"UndoRenameScanPos"


class RegisterVPConnection;
/**
* @brief UndoItem for delete connection.
*/
class UndoDeleteConnection: public zftools::BaseUndoItem
{
    RegisterVPConnection* connection{};        //Geloeschte Connection
    std::wstring viewpointName;

public:
    UndoDeleteConnection();
    UndoDeleteConnection( const std::wstring& viewpoint, const RegisterVPConnection& con );
    virtual ~UndoDeleteConnection();

    virtual std::wstring getId() override { return L"UndoDeleteConnection"; }
    virtual std::wstring getMessage() override;
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoDeleteConnection>(); }
};


//#####################################################################################
/**
* @brief UndoItem for RegInfo changes of a connection.
*/
class ZFSPRJ_API UndoChangeConnectionReginfo: public zftools::BaseUndoItem
{
    /**
     * @brief Start VP name of the connection
     */
    std::wstring vpName;

    /**
     * @brief Target VP name of the connection
     */
    std::wstring targetVPName;

    /**
    * @brief the original RegisterResultInfo of the connection
    */
    RegisterResultInfo* registerInfo;

    /**
     *  Registration flag bevor change
     */
    int mRegFlag = 0;

    /**
    *   Register matrix bevore change
    */
    ZFTRMatrix matrix;

public:
    UndoChangeConnectionReginfo() { registerInfo = NULL; }
    UndoChangeConnectionReginfo(const std::wstring& vpName, const std::wstring& targetVPName, const RegisterResultInfo* newRegisterInfo, int flag, ZFTRMatrix mat);
    virtual ~UndoChangeConnectionReginfo() { if (registerInfo) delete registerInfo; }

    virtual std::wstring getId() override { return L"UndoChangeConnectionReginfo"; }
    virtual std::wstring getMessage() override { return QObject::tr("Undo change connection register info").toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeConnectionReginfo>(); }
};

//#####################################################################################
/**
* @brief UndoItem for add connection.
*/
class UndoAddConnection: public zftools::BaseUndoItem
{
    /**
     * @brief Start VP name of the connection
     */
    std::wstring vpName;

    /**
     * @brief Target VP name of the connection
     */
    std::wstring targetVPName;

public:
    UndoAddConnection() {}
    UndoAddConnection(const std::wstring& vpName, const std::wstring& targetVPName) : zftools::BaseUndoItem(), vpName(vpName), targetVPName(targetVPName) {}
    virtual ~UndoAddConnection() {}

    virtual std::wstring getId() override { return L"UndoAddConnection"; }
    virtual std::wstring getMessage() override { return QObject::tr("Undo add connection from %1 to %2").arg(QString::fromStdWString(vpName)).arg(QString::fromStdWString(targetVPName)).toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoAddConnection>(); }
};

//#####################################################################################
/**
* @brief UndoItem for add connection.
*/
class UndoSTDChangeConnection : public zftools::BaseUndoItem
{
    /**
    * @brief Start VP name of the connection
    */
    std::wstring vpName;

    /**
    * @brief Target VP name of the connection
    */
    std::wstring targetVPName;

    bool mStdConnection = false;        //!< Flag ob es sich um eine Single connection oder eine multyconnection handelt

public:
    UndoSTDChangeConnection() {}
    UndoSTDChangeConnection( const std::wstring& vpName, const std::wstring& targetVPName, bool stdConnection) 
        : zftools::BaseUndoItem(), vpName( vpName ), targetVPName( targetVPName ), mStdConnection(stdConnection ){}
    virtual ~UndoSTDChangeConnection() {}

    virtual std::wstring getId() override { return L"UndoStdConnection"; }
    virtual std::wstring getMessage() override { return QObject::tr( "Undo change multi connection from %1 to %2" ).arg( QString::fromStdWString( vpName ) ).arg( QString::fromStdWString( targetVPName ) ).toStdWString(); }
    virtual void getUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void setUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void undoItem() override;
    virtual void afterUndo( std::list<int>& actions ) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoAddConnection>(); }
};

//#####################################################################################
/**
*   UndoItem fuer das setzten der Target ergebnisse
*/
class ZFSPRJ_API UndoChangeTargetRegisterValues : public zftools::BaseUndoItem
{
    TargetRegisterValues tRegVal;
    std::wstring scanPosName;   //Name der Scanposition
    std::wstring targetName;    //Name des Targets

public:
    UndoChangeTargetRegisterValues() : zftools::BaseUndoItem() {}
    UndoChangeTargetRegisterValues( const std::wstring& vpName, const std::wstring& tName, const TargetRegisterValues& v );
    virtual ~UndoChangeTargetRegisterValues() { }

    virtual std::wstring getId() override { return UNDO_CHANGE_TARGET_RESULT; }
    virtual std::wstring getMessage() override { return QObject::tr( "Undo change target result" ).toStdWString(); }
    virtual void getUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void setUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void undoItem() override;
    virtual void afterUndo( std::list<int>& actions ) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeTargetRegisterValues>(); }
};

//#####################################################################################

/**
* @brief UndoItem for registration precision changes.
*/
class ZFSPRJ_API UndoChangeRegistrationPrecision: public zftools::BaseUndoItem
{
    /**
     * @brief Start VP name of the connection
     */
    std::wstring vpName;

    /**
     * @brief Precision before the registration change
     */
    RegisterPrecision precision;

public:
    UndoChangeRegistrationPrecision() : zftools::BaseUndoItem() {}
    UndoChangeRegistrationPrecision(const std::wstring& vpName, RegisterPrecision& precision);
    virtual ~UndoChangeRegistrationPrecision() { }

    virtual std::wstring getId() override { return UNDO_CHANGE_REGISTRATION_PRECISION; }
    virtual std::wstring getMessage() override { return QObject::tr("Undo change registration precision for scan position %1").arg(QString::fromStdWString(vpName)).toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeRegistrationPrecision>(); }
};

//#####################################################################################

/**
* @brief UndoItem for registration changes.
*/
class UndoChangeRegistration: public zftools::BaseUndoItem
{
    /**
     * @brief Start VP name of the connection
     */
    std::wstring vpName;

    /**
     * @brief Matrix before the registration change
     */
    ZFTRMatrix matrix;

    /**
     * @brief Precision before the registration change
     */
    RegisterPrecision precision;

    /**
     * @brief Flag if the registration flag has been updated
     */
    bool setFlag = false;

    /**
     * @brief Registration flag before the registration change
     */
    int regFlag = 0;

    /**
    * @brief the RegisterResultInfo of the viewpoint
    */
    RegisterResultInfo* registerInfo = nullptr;

    /**
     * @brief Error before the registration change
     */
    RegisterErrorProj error;

public:
    UndoChangeRegistration() : zftools::BaseUndoItem() { registerInfo = NULL; }
    UndoChangeRegistration(const std::wstring& vpName, ZFTRMatrix& matrix, RegisterPrecision& precision,
        RegisterResultInfo* newRegisterInfo , bool setFlag, int regFlag, const RegisterErrorProj& err);
    virtual ~UndoChangeRegistration();

    virtual std::wstring getId() override { return UNDO_CHANGE_REGISTRATION; }
    virtual std::wstring getMessage() override { return QObject::tr("Undo change registration for scan position %1").arg(QString::fromStdWString(vpName)).toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeRegistration>(); }

};

//#####################################################################################

/**
* @brief UndoItem for changes to the global matrix
*/
class ZFSPRJ_API UndoChangeGlobalMatrix: public zftools::BaseUndoItem
{
    /**
     * @brief Matrix before the change
     */
    ZFTRMatrix matrix;
    std::wstring systemName;

public:
    UndoChangeGlobalMatrix() : zftools::BaseUndoItem() { }

    UndoChangeGlobalMatrix(const std::wstring& _name, const ZFTRMatrix& _matrix);
    UndoChangeGlobalMatrix(std::shared_ptr<zfprj::CoordinateSystem> _system);

    virtual ~UndoChangeGlobalMatrix() { }

    virtual std::wstring getId() override { return UNDO_CHANGE_GLOBAL_MATRIX; }
    virtual std::wstring getMessage() override { return QObject::tr("Undo change global matrix").toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeGlobalMatrix>(); }
};

//#####################################################################################

/**
* @brief UndoItem for changes to the currently used global matrix
*/
class ZFSPRJ_API UndoChangeGlobalMatrixName : public zftools::BaseUndoItem {
    std::wstring system;

public:
    UndoChangeGlobalMatrixName() : zftools::BaseUndoItem() {}
    UndoChangeGlobalMatrixName(const std::wstring& system);
    virtual ~UndoChangeGlobalMatrixName() {}

    virtual std::wstring getId() override { return UNDO_CHANGE_GLOBAL_MATRIX_NAME; }
    virtual std::wstring getMessage() override { return QObject::tr("Undo change current global matrix").toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;
    virtual void afterUndo(std::list<int>& actions) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeGlobalMatrixName>(); }
};

//#####################################################################################

/**
* @brief UndoItem for changes to the global matrix
*/
class ZFSPRJ_API UndoSetMaster: public zftools::BaseUndoItem
{
    /**
     * @brief Name of the original master scan
     */
    QString master;

public:
    UndoSetMaster() : zftools::BaseUndoItem() { }
    UndoSetMaster( const QString& master);
    virtual ~UndoSetMaster() { }

    virtual std::wstring getId() override { return UNDO_SET_MASTER; }
    virtual std::wstring getMessage() override { return QObject::tr( "Undo set master" ).toStdWString(); }
    virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
    virtual void undoItem() override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoSetMaster>(); }
};


//#####################################################################################

/**
* Undo fuer das festsetzten von Scanpositionen
*/
class ZFSPRJ_API UndoRegFlagScanpos : public zftools::BaseUndoItem
{
    int  mRegflag = 0;                      //!< Registrierungs Status vor der Aenderung
    std::wstring mScanPosName;          //!< Name of the Scanposition

public:
    UndoRegFlagScanpos() : zftools::BaseUndoItem() { }
    UndoRegFlagScanpos( int flag, const std::wstring& scanPosName );
    virtual ~UndoRegFlagScanpos() { }

    virtual std::wstring getId() override { return UNDO_REG_FLAG_SCANPOS; }
    virtual std::wstring getMessage() override { return QObject::tr( "Undo set registration type scan position" ).toStdWString(); }
    virtual void getUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void setUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void undoItem() override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoRegFlagScanpos>(); }
};

//#####################################################################################

/**
* Undo fuer das festsetzten von Scanpositionen
*/
class ZFSPRJ_API UndoLockScanpos : public zftools::BaseUndoItem
{
    bool  mLock = false;                     //!< Lock status auf den zurueckgesetzt werden soll.
    std::wstring mScanPosName;       //!< Name of the Scanposition

public:
    UndoLockScanpos() : zftools::BaseUndoItem() { }
    UndoLockScanpos( bool lock, const std::wstring& scanPosName );
    virtual ~UndoLockScanpos() { }

    virtual std::wstring getId() { return UNDO_LOCK_SCANPOS; }
    virtual std::wstring getMessage() { return QObject::tr( "Undo lock scan position" ).toStdWString(); }
    virtual void getUndoAttributes( zftools::undo_attributes_t& attributes );
    virtual void setUndoAttributes( zftools::undo_attributes_t& attributes );
    virtual void undoItem();

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoLockScanpos>(); }
};

//#####################################################################################

/**
* Undo fuer das aktivieren bzw deaktivieren von Connections
*/
class ZFSPRJ_API UndoChangeConnectionStatus : public zftools::BaseUndoItem
{
    bool  mStatus = false;          //!< Lock status auf den zurueckgesetzt werden soll.
    std::wstring mStartPos;         //!< Name of the Scanposition
    std::wstring mStopPos;          //!< Name of the Scanposition

public:
    UndoChangeConnectionStatus() : zftools::BaseUndoItem() { }
    UndoChangeConnectionStatus( bool status, const std::wstring& startPos, const std::wstring& stopPos );
    virtual ~UndoChangeConnectionStatus() { }

    virtual std::wstring getId() override { return UNDO_CHANGE_CONNECTION_STATUS; }
    virtual std::wstring getMessage() override { return mStatus ? QObject::tr( "Undo disable connection" ).toStdWString() : QObject::tr( "Undo enable connection" ).toStdWString(); }
    virtual void getUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void setUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void undoItem() override;
    virtual void afterUndo( std::list<int>& actions ) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoChangeConnectionStatus>(); }
};

//#####################################################################################

/**
* Undo fuer das umbenennen der Namen der Scanposition
*/
class ZFSPRJ_API UndoRenameScanPos : public zftools::BaseUndoItem
{
    std::wstring mScanName;                 //!< Name of the scan
    std::wstring mOldScanPosName;           //!< Old name of the scanposition

public:
    UndoRenameScanPos() : zftools::BaseUndoItem() { }
    UndoRenameScanPos( const std::wstring& scanName, const std::wstring& oldScanposName );
    virtual ~UndoRenameScanPos() { }

    virtual std::wstring getId() override { return UNDO_CHANGE_SCANPOS_NAME; }
    virtual std::wstring getMessage() override { return QObject::tr( "Undo rename scanposition" ).toStdWString(); }
    virtual void getUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void setUndoAttributes( zftools::undo_attributes_t& attributes ) override;
    virtual void undoItem() override;
    virtual void afterUndo( std::list<int>& actions ) override;

    static std::shared_ptr<UndoItem> Create() { return std::make_shared<UndoRenameScanPos>(); }
};


#endif

//#####################################################################################
/**
*   Schraegstellungs Klasse. Die Klasse ist abgeleitet von der Klasse registerLevel und erweitert diese um die 
*   Methoden um bereits im Projekt vorhandene Werte auszulesen.
*/
class ZFSPRJ_API ProjectRegisterLevel 
{
public:
    enum CorrectionTyp {
        NON = 0,
        COMPENSATOR = 1,
        LEVEL = 2,
        IMU = 4,
        DEAKTIVATED = 8
    };

    static int defaultTyps;

private:
    ZFTRMatrix levelMartrix;    //!<Matrix fuer die Horizontierung des Scanners
    int error{};                 //!<Fehler aus der Berechnugn zfsdll::registerLevel::eErrorCode //ToDo (Noch nicht implementiert muss noch im Projekt gespeichert werden)
    CorrectionTyp status{NON};       //!< Status der Horizontierung

    double verticalX{};
    double verticalY{};
     
public:
    ProjectRegisterLevel() = default;

    /**
    *   Wenn die Werte bereits in der Projektdatei stehen werden diese genommen und 
    *   die Werte nicht neu berechnet. Es wird erst nach Werten aus dem Kompensator und dann nach Werten der Libelle geschaut.
    *   \param scan => Scan fuer den die Matrix fuer die Horizontierung berechent werden soll.
    *   \return True wenn eine Matrix vorhanden ist //ToDo Fehlerwert aus der Berechnung (zfsdll::registerLevel::eErrorCode) 
    */
    zfs::level::eErrorCode init( const ZFProjectItemScan* scan, int corrTyp );

    const ZFTRMatrix* getLevelMatrix() { return &levelMartrix; }
    int getError() { return error; }
    double getVerticalX() {return verticalX;}
    double getVerticalY() { return verticalY; }

    CorrectionTyp getStatus() { return status;}

};

/**
*   Viewpoint Connection Klasse. Sie enthaelt den Namen des Viewpoints auf den verwiesen wird und
*   ein registerResultInfo objekt in dem das Registrierergebnis fuer die Verbindung gespeichert ist
*/
class ZFSPRJ_API RegisterVPConnection : public QObject
{
    friend class UndoDeleteConnection;
    friend class ViewPoint;

    bool mEnabled;                                          //!< True Connection wird für die Registrierung verwendet. False Connection wird nicht verwendet.
    bool mSelected;                                         //!< True = Connection ist ausgewaelt

protected:
    RegisterResultInfo::eQuality  mScoutCheck;              //!< Info falls ein Scout check fuer die Connection durchgefuert wurde.
    RegisterResultInfo* mRegisterInfoConection;             //!< Register Info Objekt zu der Connection
    ZFTRMatrix mMatrixResult;                               //!< Ergebnis dieser Connection. Da es mehrere Connections auf eine Scanposition geben kann muss dies nicht die Distanz zwieschen dem Scanpositionen sein.
    ZFTRMatrix mMatrixDiff;                                 //!< Diffenenzmatrix zwischen Vorgaenger und Nachfolger die sich aus dieser Registrierung ergibt.
    int mRegFlag;                                           //!<Register Flag der registrierung.

    double mDiffToPos;                                      //!< Differenz wert zur Aktuellen Position des ReverVP Standpunktes.
    bool mDiffPosIsSet;                                     //!< Flag ob der Differenzwert gesetzt ist.
    bool mStdConnection;                                    //!< Flag ob es sich um eine Single connection oder eine multyconnection handelt


public:
    std::wstring mReverdVP;                                 //!< Name des VP auf den die Connection zeigt
    bool mAuto;                                             //!< Bool ob die Connection automatisch oder manuell erstellt wurde

public:
    RegisterVPConnection()											                  : QObject(), mEnabled( true ),    mSelected( false ), mScoutCheck(RegisterResultInfo::QUAL_NOT_REG), mRegisterInfoConection(NULL), mRegFlag(0), mDiffToPos( 0 ), mDiffPosIsSet( false ), mStdConnection(false),                   mAuto(true)    {}
    RegisterVPConnection(std::wstring name)						                      : QObject(), mEnabled( true ),    mSelected( false ), mScoutCheck(RegisterResultInfo::QUAL_NOT_REG), mRegisterInfoConection(NULL), mRegFlag(0), mDiffToPos( 0 ), mDiffPosIsSet( false ), mStdConnection(false),  mReverdVP(name), mAuto(true)    {}
    RegisterVPConnection(std::wstring name, bool single, bool autoCon, bool enabled ) : QObject(), mEnabled( enabled ), mSelected( false ), mScoutCheck(RegisterResultInfo::QUAL_NOT_REG), mRegisterInfoConection(NULL), mRegFlag(0), mDiffToPos( 0 ), mDiffPosIsSet( false ), mStdConnection(single), mReverdVP(name), mAuto(autoCon) {}
    RegisterVPConnection( const RegisterVPConnection& o );

    virtual ~RegisterVPConnection();

    /** Kopiert den uebergebenen Wert in das Objekt und updated den Connection Baum im ZFRegisterObjNode objekt*/
    void addReginfo( ZFRegisterObjNode* node, const std::wstring spName, const RegisterResultInfo* info, int flag,
        const ZFTRMatrix& matRes, const ZFTRMatrix& diffMatrix, bool undo );
    void updateDiffMatrix( const ZFTRMatrix& diffMatrix ) {
        mMatrixDiff = diffMatrix;
        mDiffPosIsSet = false;
        mDiffToPos = 0;
    }
    void addP2PInfo( RegisterResultInfo::eQuality qual ) { mScoutCheck = qual; }

    void setConnectionDiff( double diff ) { mDiffToPos = diff; mDiffPosIsSet = true; }
    bool isConnectionDiffIsSet() const { return mDiffPosIsSet; }
    double getConnectionDiff() const { return mDiffToPos; }

    bool isUsed() const  { return mEnabled; }
    void setSelected( bool b )
    {
        mSelected = b;
    }
    bool isSelected()
    {
        return mSelected;
    }

    /** Einlesen der Registerinformation aus der XML Datei*/
    void readRegInfo(  tinyxml2::XMLElement* element )
    {
        if( mRegisterInfoConection )
            delete mRegisterInfoConection;
        mRegisterInfoConection = RegisterResultInfo::LoadRegisterInfo( element );
    }

    ZFTRMatrix getResultMatrix() const { return mMatrixResult; }
    ZFTRMatrix getDifferenceMatrix() const { return mMatrixDiff; }
    int getRegFlag() const { return mRegFlag; }

    bool isRegistered( bool preRegistered ) const;


    /**
    *	Gibt das Resultat aus dem ScantraScout check zurueck. Falls keine Daten vorliegen ist  mQual = QUAL_NOT_REG;
    */
     RegisterResultInfo::eQuality getP2PInfo() { return mScoutCheck; }


    const RegisterResultInfo* getRegInfo() const { return mRegisterInfoConection; }
    void setConfidence( double con );
    void regInfoCalcQual() { if( mRegisterInfoConection ) mRegisterInfoConection->calcQual(); }



    double getWeight();

    void setStdConnection( bool b, bool undo, std::wstring vpName );
    bool isStdConnection() const { return mStdConnection; }

    bool operator==( const std::wstring& vpName ) const { return vpName == mReverdVP? true: false; }
    bool operator==( const RegisterVPConnection& connection ) const { return connection.mReverdVP == mReverdVP? true: false; }

    bool operator>( const RegisterVPConnection& conn );

    RegisterVPConnection& operator=(const RegisterVPConnection& o );

protected:
    void use( bool b )
    {
        mEnabled = b;
    }
};

//############################################################################################################################################
//############################################################################################################################################
//############################################################################################################################################

/**
*   Viewpoint classe. Beinhaltet alle Scans die zum VP gehoeren und die Connections
*/

class ZFSPRJ_API ViewPoint : public zfprj::GroupItem
{
    Q_OBJECT

    friend class ZFRegisterObjNode;

#ifdef SUPPORT_PRJ_UNDO
    // friend declarations for undo items to have access to the member variables
    friend class UndoChangeRegistration;
    friend class UndoDeleteConnection;
    friend class UndoAddConnection;
    friend class UndoRegFlagScanpos;
#endif


protected :
    //Informationen wie die Scanposition registriert werden soll
    bool mUse{};                                              //Flag ob der VP fuer die registrierung genutzt werden soll
    std::string uuid = "";
    QColor color; ///< the color the renderer should use, stored in the project and showed in the browser

    //Registrier informationen
    ZFTRMatrix mRegisterMatrix;                             //Registrierungs Matrix fuer diesen VP
    ZFTRMatrix mDistanceMatrix;                             //Matrix mit der Distanz zum letzten Scan
    RegisterResultInfo* mRegisterInfo;                      //Enthaelt die registrier informationen fuer den VP
    RegisterErrorProj mRegisterError;						//Enthaelt die fehler Information fuer die Registrierung.


    int mRegFlag{};                                           //Registrierungs Flag fuer diesen VP
    RegisterPrecision mPreRegisterPrecision;                // Genauigkeitsangaben der PreRegistrierung

    std::list<std::wstring> mConnectionBack;                //Liste zu allen Vorgaengern( VP zu VP )
    std::list<RegisterVPConnection> mConnectionList;        //Liste aller Connections (VP zu VP)

    //Allgemeine Informationen
    ZFRegisterObjNode* mParent{};                             //Parent Objekt (ZFRegisterObjNode)
    bool mMaster{};                                           //Bool ob der VP getade Master ist.
    std::list<GuardetZFProjectItemScan> mScanList;          //Alle scans die zu diesem VP gehoeren
    bool mHasBadScan{};										//Bool ob ein corrupter scan im VP ist
    bool mHasSortedScans{};									    //Bool ob ZFScan Scans im VP sind
    //bool mHasOtherScans{};									//Bool ob ein fremdformat scan im VP ist.
    bool mHasUnsortedScans{};                                 //Bool ob unsortierte kleine Punktwoken vorhanden sind z.B. Handscanner

    double mConnectionDiff{};                                 //!< Max Distanz Wert der verschiedenen Connections [m]. -1 Wert ist nicht gesetzt.
    double mConnectionDiffVis{};                              //!< Max Distanz Wert der verschiedenen sichtbaren Connections [m]. -1 Wert ist nicht gesetzt.

    /////////////
    //Die Klasse besitzt eine copy Methode neu hinzugefuegte Variablen muessen auch dort hinzugefuegt werden!!!
    ////////////
public:
    bool mUpsideDown{};                                       //Bool ob die Scans an der Scanpossition ueberkopf aufgenommen wurden.
    std::wstring mViewPointName;                            //Name des Viewpoints

    QMutex mMutexChanges;                                   //Alle aenderungen in der Scan Liste und in der Connection
                                                            //Liste muessen ueber diesen mutex gehen

    targetlist_t mTargets;                                  //Alle zu diesem Vp gehoerenden Targets
                                                            //Falls ein Target aus mehreen Scans vorhanden ist
                                                            //wird das best aufgeloeste verwedet.

    VecNameList mHelpTargetList;                            //Zusaetzliche liste um beliebige Informationen zu finden

    int mVPNr{};                                              //Nr des VP 0 = Masterscan //ToDo
    int mPredecessorNr{};                                     //Nummer des Vorgaengers (ergibt sich aus den connections)

    double mDateNr{};                                         //Nr des ersten Scans setzte sich aus dem Datum der Aufnahme zusammen JJJJMMDDhhmm
                                                            //oder falls kein Datum im Header ist eine fortlaufende nummer 0 bis n
    Graph::vertex_descriptor mGraphVert; //Zeiger auf das Element im Graph

public:
    ViewPoint( ZFRegisterObjNode* parent );

    void setParent( ZFRegisterObjNode* p ) { mParent = p; }

    ZFProject* getProject();

    ZFRegisterObjNode* getParent(){ return mParent; }

    virtual void Save() {}
    virtual void Load() {}

    bool hasVisibleGroup() const;

    bool isVisible() const override;
    int nrVisible() const override;
    void checkVisbility() override;

    // color of the viewpoint for the renderer and browser
    QColor getColor() const { return color; }
    void setColor( const QColor& c ) { color = c; }

    void copy( ViewPoint& vpNew, bool all, bool createConnections ) const;
    virtual ~ViewPoint()
    {
        if( mRegisterInfo )
        {
            delete mRegisterInfo;
            mRegisterInfo = NULL;
        }
    }

    /**
    *   Gibt die Liste aller DB ID's der Scans in der Scanposition
    */
    std::vector<std::string> getDBIdList() const;

    /**
    * @brief Get the UUID of this item.
    */
    const std::string& getUUID() { return uuid; }

    /**
    * @brief Set the UUID of the item.
    *
    * @param _uuid the new uuid of the item.
    */
    void setUUID(const std::string& _uuid) { uuid = _uuid; }

    RegisterResultInfo* getRegInfo() { return mRegisterInfo; }
    void setRegInfo( RegisterResultInfo* info, const RegisterErrorProj& err, bool useUndo = true );
    RegisterErrorProj& getRegisterError() { return mRegisterError; }
    
    /**
    *   Abfrage ob der VP fuer die Registrierung genutzt werden soll.
    */
    bool isUse() const
    {
        return mUse;
    }
    virtual void setUse( bool use );
    void updateRegistration();
    void updateConectionName( std::wstring newName, std::wstring oldName );
    void updateBackConectionName( std::wstring newName, std::wstring oldName );
    void addScan( ZFProjectItemScan* scan, bool delTempFiles );
    void deleteScan( ZFProjectItemScan* scan );
    const std::list<GuardetZFProjectItemScan>& getScanList() const { return mScanList; }
    std::wstring getFirstScan();
    bool hasSortedScans() const;
    bool hasScans() const { return hasSortedScans() || hasUnsortedScans(); }
    bool hasIncompleteScan() const;
    bool hasUnsortedScans() const { return mHasUnsortedScans; }
    virtual bool containsScan( const std::wstring& scanName ) const;
    bool allScansAreAvailable();


    QString getRegisterInfoString();
    RegisterResultInfo::eQuality getRegisterQual();
    
    void deleteTempFiles( const QString& spName );

    const std::list<std::wstring>* getBackConnectionList() const { return &mConnectionBack; }
    void updateBackConnection( const std::wstring& orgName, const std::wstring& newName );

    std::list<RegisterVPConnection>* getConnectionList() { return &mConnectionList; }
    const std::list<RegisterVPConnection>* getConnectionList() const { return &mConnectionList; }
    const RegisterVPConnection* getConnection( const std::wstring& connectionVP ) const;
    RegisterVPConnection* getConnection( const std::wstring& connectionVP );
    void setAllConnectionsToMulti();
    virtual void enableConnection( bool b, RegisterVPConnection* con, bool checkConnection, bool undo );
    virtual bool addConnection( bool useUndo, const std::wstring& connectionVP, bool singleMode, bool single, bool autoCon, bool updateSingle, bool onlyChange );
    virtual bool deleteConnection( bool deleteReg, bool useUndo, const std::wstring& connectionVP, bool updateSingl, bool signal = true );
    virtual bool deleteConnection( bool deleteReg, bool useUndo, RegisterVPConnection* connection, bool updateSingle, bool signal = true );
    virtual bool deleteAllConnections( bool useUndo, bool deleteReg, bool checkConnection );
    virtual void deleteAllBackConnections( bool useUndo, bool deleteReg, bool updateSingl );
    virtual bool clearFollowningRegistration();
    void deleteUnusedConnections( bool updateSingl );
    //void checkSingleConnections();
    //void sincConnectionsWithList( std::list<std::wstring> list );
    void sincConnectionsWithListAdd( bool useUndo, std::list<std::wstring> list );
    virtual void sincConnectionsWithListDelete( bool useUndo, std::list<std::wstring> list, std::map<std::wstring, std::list<std::wstring> > connectionList, bool onlyZFS );
    size_t nrOfConnections( bool onlyEnabled );
    size_t nrOfBackconnections( bool onlyEnabled );
    virtual bool hasBackConnections( bool onlyVisible, bool onlyEnabled );
    bool hasOnlyNontVisibleBackConnections( bool& hasNotVisBackCon );
    bool hasNontVisibleBackConnections();
    bool hasBackConnection( const std::wstring& name );
    virtual bool hasOneRegisteredBackConnections( bool usePrereg );
    virtual int nrRegisteredBackConnections( bool usePrereg );

    void updateRegisterInfo();

    unsigned int nrScans() { return (unsigned int)mScanList.size(); }

    ZFTRMatrix getCurrentMatrix( bool useGlobal, const std::wstring system = L"") const;

    ZFTRMatrix getLevelMatrix( ProjectRegisterLevel::CorrectionTyp typ ) const;

    const ZFTRMatrix* getRegisterMatrix() const { return &mRegisterMatrix; }

    const ZFTRMatrix* getDistanceMatrix() const { return &mDistanceMatrix; }
    void setRegisterPrecision( const RegisterPrecision& precision);

    bool changeVPName( const QString& newName, QString& error );
    //void updateViewPointName( const QString& oldName );
    void setViewPointName(std::wstring name);
    std::wstring getViewPointName() const { return mViewPointName; }
    virtual std::vector<std::wstring> getFullScanFilenamesOfViewpoint() const;

    void rearangeFromPredecessor();

    void setDistanceMatrix( const ZFTRMatrix& matrix );
    void setRegistrationFromConnection( const RegisterVPConnection* con );
    //bool setRegisterMatrix( bool useMaster, bool write, const ZFTRMatrix& matrix, int flag, bool clear = true );
    virtual std::pair<bool, bool> setRegisterMatrix( bool useUndo, bool write, const ZFTRMatrix& matrix, int flag, bool clear, bool updateFollow, bool useMaster,
                                    bool clearRegInfo, bool updateSameMatrix, bool calcResDiff, ZFProject::eProjectError* err = nullptr );
    virtual bool moveRegisterMatrix( bool useUndo, bool write, const ZFTRMatrix& newMatrixPred, const ZFTRMatrix& orgMatrixPred, bool onlyRegisted );
    virtual bool updateFollowingRegistrations( bool useUndo, bool saveToScans, const ZFTRMatrix* diffMatrix, std::list<ViewPoint*>& moveList, bool useSingle, bool calcRegDiff );
    void updatePossitionFromPredecessor( bool saveToScans, bool calcResDiff );

    const RegisterPrecision* getRegisterPrecision() const { return &mPreRegisterPrecision; }
    void addMatrixToRegistration( bool useUndo, const ZFTRMatrix& matrix, int flag, bool calcResDiff );

    virtual void chatchUpNotRegistedSingleConnection( std::map< std::wstring, ZFTRMatrix>* updateListNotRegSP, std::map<std::wstring, bool>* alreadyCheckedList );
    void updateConnectionDiffMatrix( bool all );
    virtual void updateConnectionDiffMatrixBack();
    void calcConnectionDiff();
    void calcConnectionDiffVis();
    void calcConnectionDiffAll( bool calcFrontBack );
    double getConnectionDiff() { return mConnectionDiff; }
    double getConnectionDiffVis() { return mConnectionDiffVis; }
    void clearConnectionDiffs();

    void clearRegMatrix();
    QColor getRegisterQualColor() const;
    bool isRegisted() const;
    bool isPreRegisted( bool useManuell ) const;
    bool isP2POrTargetPreregisted() const;
    int getRegFlag() const { return mRegFlag; }
    

    /**
    *   Gibt true zurueck wenn der Scan bei einer Registierung als startscan verwendet werden soll. Eine Ausnahme ist wenn Tachy verwendet wird
    *   dann gibt es keine anderen Startscans.
    *   1. Master Scan
    *   2. Fix Scan
    *   3. Scan mit mindestens einer Registrierten Verbindung von einer nicht sichtbaren oder nicht ausgewaelten Scanposition
    *   \param useVis => Die Rueckgabe erfolgt unter Berücksichtigung von sichtbaren und unsichtbaren Scanpositionen
    *   \param usSelected => Die Rueckgabe erfolgt unter Berücksichtigung von ausgewaelten und nicht ausgewaelten Scanpositionen
    *   \param[out] hasNotVisBackConnections => True= Die Scanposition hat mindestens eine nicht sichtbare oder nicht ausgewaelte hereinkommende Verbindung.
    */
    virtual bool isStartScan( bool useVis, bool usSelected, bool& hasNotVisBackConnections, bool& hasNotVisConnections, bool useTachy ) const;

    /**
    *   Locket einen Scan. Der Scann kann danach nicht mehr verschoben werden.
    */
    void setLock( bool enabled );

    /**
    *   Gibt an ob der Scan fix ist. Entweder der Masterscan oder vom user auf fix gesetzt.
    *   \param useMaster => True der Masterscan wird automatisch als gelockt angesehen.
    */
    bool isLocked( bool useMaster ) const;

    bool isMaster() const { return mMaster; } //< Rueckgabe ob der SP master ist.
    /**
    *   Locket einen Scan. Der Scann kann danach nicht mehr verschoben werden. Connections werden nicht kontrolliert. (Nur fuer undo)
    */
    void setLockBase( bool enabled, bool lock );

    /**
    * @brief Reads in all targets of a scan.
    *
    */
    void updateTargets(std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock);

    bool writeRegisterToScans();

    //Targets
    void addTarget( const ZFTarget& vn );
    /**
    *   Loescht das Target aus dem Standpunkt und den Scans des Standpunktes. Wenn das Target in mehreren Scans vorhanden ist wird es in allen geloescht.
    */
    void deleteTarget( const QString& name );

    /**
    *   Benennt das Target in der Klasse, im Projekt und in allen Scans um in denen es vorkommt.
    *   Es werden keine Signale gesendet.
    */
    bool renameTarget( const QString& oldName, const QString& newName );

    /**
    *   Enable ore disable a target of the scanposition
    *   \param target => Name of the target 
    *   \param use => true = Enables the target in all scans of this scanposition
    */
    void useTarget( const std::wstring target, bool use );

    void enableAllRedTargets( bool b, bool sendSignal );
    void enableAllYellowAndRedTargets( bool b, bool sendSignal );
    void enableAllTargets( bool b, bool sendSignal );

    void clearBackConnections();

    /**
    *   Loescht alle automatisch erstellten hereinkommenden Connections von dem Scannerstandpunkt.
    */
    void deleteAllAutoBackConnections( bool signal, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );

    /**
    *   Loescht alle multi Connections fuer diesen Standpunkt.
    */
    void deleteAllMultiConnections( bool signal, bool updateSingle, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );

    void clearTargetRegisterResult();
protected:

    std::pair<bool, bool>  setRegisterMatrixIntern(bool useUndo, bool write, const ZFTRMatrix& matrix, int flag, bool clear, bool updateFollow, bool useMaster,
        bool clearRegInfo, bool updateSameMatrix, bool calcResDiff, ZFProject::eProjectError* err = nullptr);
    void setRegFlag(int flag, bool useUndo);
    QColor getRegCol( int flag ) const;
    void setVisible(bool _visible, bool updateScans) override;

    void createBackList( bool useUndo );
    void clearConnections();
    void resetSingleConnection();
    void deleteBackConnection( const std::wstring& vpName );
    void addBackConnection( bool useUndo, const std::wstring& vpName );
    virtual bool moveConnections( bool useUndo, bool write, const ZFTRMatrix& orgMatrixPred, bool onlyRegisted );

private:
    /*
    ViewPoint( const ViewPoint& vp)
    : zfprj::GroupItem(vp.mViewPointName)
    {}
    */
    ViewPoint& operator=(const ViewPoint &) { return *this; }
};


class ZFSHeader;

typedef void (*cbZFRegisterObjProgress)(int max, int cur, const char* ); // progressbar callback


class ZFSPRJ_API ZFRegisterObjNode : public zfprj::ZFPrjObjNode
{
    DEFINE_ZFBASE(ZFPrjObjNode)

    friend class ZFRegisterParamBasic;
    friend class ViewPoint;
    
    //Parameter Floorplan
    zf::FileName mFloorplanName;        //Name der Floorplan Datei relativ vom Projekt aus
    double mTop;                            //Umschliessendes Rechteck in m
    double mBottom;
    double mLeft;
    double mRight;

    int sLastVPNr;
    static std::wstring defaultVPName;					//Default name fuer Vp

    ZFProject::eProjectError err;           //Fehler beim laden

    Graph g;
public:
    std::mutex viewpointMutex;
    std::map<std::wstring, ViewPoint*> mViewPointMap;       //Liste aller im Projekt vorhandenen Viewpoints
    targetlist_t mKnownTargets;                             //Liste aller im Projekt vorhandenen known Targets

    enum eRegisterError { NO_REG_ERR,               //Kein Fehler
        NO_CORRECT_POSITON,       //Master VP der Auswahl ist nicht der Projekt master scan und
        //hat auch keine registrierung bzw vorregistrierung
        MORE_THEN_ONE_START_VP,   //VP sind nicht durchgehend verknuepft es gibt mehrere moegliche master VP
        CANT_FIND_MASTER,         //Master VP konnte nicht gefunden werden
        WRONG_SELECT_SIZE         //Nicht genug oder zu viel Scans ausgewaelt
    };

public:

    QString Typ() const override { return "Register Object"; }
    std::string Xml() const override { return REGXML_DATA; }

    ZFRegisterObjNode( ZFProject* proj ) : zfprj::ZFPrjObjNode(proj)
    {
        mTop=mBottom=mLeft=mRight=0;sLastVPNr=0;
        err = ZFProject::PROJECT_OK;
    }
    virtual ~ZFRegisterObjNode(void);

    ZFProject::eProjectError getErr() { return err; }

    //const std::list<std::wstring>& getTachyfile();

    int updateProject( bool sendCallback, bool createVP, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    int addRegister2Proj( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool sendCallback, double height, const wchar_t* posinfofn=(const wchar_t*)0  );

    // \param unit additional conversation of input
    int addTachyToProj(const std::wstring& tachy, std::wstring& error, ZFUnit* unit = {});
    
    /**
    *   Auslesen einer Scaner positions Datei und hinzufuegen zum Projekt.
    *   Die informationen werden geloescht.
    *   \param tachy => Name der Datei die ausgelesen werden soll.
    *                   Wenn NULL oder "" uebergeben wird nur der vorhanden Eintrag geloescht.
    *   \return Gibt einen Fehlerwert zurueck falls beim einlesen der Datei Fehler aufgetreten sind.
    *           REGERR_TACKY_LOAD => keine Targets in der Datei
    *           REGERR_FAILED => unbekannter Fehler beim einlesen der Datei
    */
    int addScanPos( const std::wstring& posinfofn, double height );

    bool isScanRegisted( const QString& scan );
    double calcTargetDiff( bool useVisible );
    void calcTargetStd( double& average, double& std, double& max );
    void getTargetResultList( bool useCurrentSystem, bool useVisible, std::list<VecNameList>& scanTargetList, bool incluedUnused = false );
    void calcTachyTargetStd( double& average, double& std, double& max );
    void getTachyTargetResultList( bool useCurrentSystem, bool useVisible, std::list<VecNameList>& knownTargetList, bool incluedUnused = false );
    void calcConnectionMaxDiff();
    void calcOptimizeResult( double& average, double& std, double& maxDiff, double& averageStdC2C, double& stdC2C, double& minC2C, double& maxC2C );
    void calcTargetList( std::list<VecNameList>& targets, std::list<VecNameList>& knownTargets );
    void calcTargetResult( double& targetStd, double& knownTargtetStd );
    /** Abfrage ob im Projekt Targets vorhanden sind.
    *   /return True => Targets sind vorhanden.
    */
    bool hasTargets() const;
    void renameAllTargetsAtRect( const QRectF& rect, const std::wstring& name, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void disableAllRedTargets( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void disableAllYellowAndRedTargets( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void enableAllTargets( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );

    int createViewPoints( bool sendCallback, bool update, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    bool newVPName( ZFProjectItemScan* scanItem, std::wstring name, QString& errorTxt, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool update=true );
    //bool newVPName( ViewPoint* scanPos, const QString& newName, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool updateProject );
    
    bool addScanToViewpoints( ZFProjectItemScan* scanItem,
        std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, 
        const ScanPositionNamingOptions& options);

    void removeScanFromViewpoints( const std::wstring& newMaster, bool useUndo, bool deleteReg, ZFProjectItemScan* scanItem, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool update=true );
    void removeScanFromViewpoint( const std::wstring& newMaster, bool deleteReg, ZFProjectItemScan* scanItem, std::wstring viewPoint, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool update=true );
    void removeScanFromViewpoint( const std::wstring& newMaster, bool useUndo, bool deleteReg, ZFProjectItemScan* scanItem, ViewPoint* viewPoint, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool update=true );
    bool removeViewPoint( const std::wstring& vpName );
    void updateViewpointTargets( bool update, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock);
    QString getNewVPName();
    void addSuperiorMatrixToVP( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void setRegister( const std::wstring& viewpoint, const ZFTRMatrix& m, int flag );

    void checkConnectionErrors();
    void chatchUpNotRegistedConnections();
    void updateConnectionDiffMatrix( bool all );
    //void checkSingleConnections( bool update, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void checkConnections( bool update, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb=NULL );
    //bool vpHasConnectionToMaster( ViewPoint* vp );
    void checkConnectionsFromScanPos( ViewPoint* master, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb, bool update );
    void createMultyConnectionsToNeighbor( bool allVisible, double dist, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb );
    void createMultyConnectionsToNeighbor( ViewPoint* scanPos, double dist, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb );
    void createConnections( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb=NULL );
    void createMultyConnections( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb=NULL );
    void addMissingConnections( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb=NULL );
    void deleteUnusedConnections( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb=NULL );
    void deleteConnections( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, cbZFRegisterObjProgress cb=NULL );

    void createVRNumbers();
    void deleteVRNumbers();
    int vrRegInfo( ViewPoint* vp, int& nr, int predecessor, int& add );
    void updateRegisterInfo();
    void updateRegistrationFromPredecessor( bool calcRegDiff );

    void showAllViewPoints( bool updateAllPreRegisted, bool sendCallback, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock);

    /**
    * @brief Reset all register matrices to the values stored in the scan header starting from index 380.
    * Deleted, because the matrix starting from index 380 holds a preset matrix and not the original matrix.
    *
    * @param sendCallback   Flag, if a PRJ_REGISTER_CHANGED event should be issues after the changes
    * @param lock           The lock for the register nodes    
    */
    // void restoreOriginalRegisterMatrix(bool sendCallback, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock);

    /**
    * @brief Reset all register matrices to the IMU values stored in the scan header.
    *
    * @param sendCallback   Flag, if a PRJ_REGISTER_CHANGED event should be issues after the changes
    * @param lock           The lock for the register nodes
    * @param viewpointName  the name of the viewpoint to be reset or an empty string to restore all
    */
    void restoreIMURegisterMatrix(bool sendCallback, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, const QString& viewpointName = "");

    void getIMUMatrix(ZFSHeader* header, ZFTRMatrix& matrix, double& lastHeight, bool first = true);

    /**
    * @brief Reset all register matrices to the GPS values stored in the scan header.
    *
    * @param sendCallback   Flag, if a PRJ_REGISTER_CHANGED event should be issues after the changes
    * @param lock           The lock for the register nodes
    * @param viewpointName  the name of the viewpoint to be reset or an empty string to restore all
    */
    void restoreGPSRegisterMatrix(bool sendCallback, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, const QString& viewpointName = ""    );

    void moveScansToSingelVP( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void moveScansToSingelVPScanName( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock );
    void setDefaultVPName( const QString& defaultName );

    void addFloorplan( const std::wstring& name, double top, double bottom, double left, double right );
    const wchar_t* getFloorplan() const { return mFloorplanName.w_str(); }
    double getFloorplanTop() const { return mTop; }
    double getFloorplanBottom() const { return mBottom; }
    double getFloorplanLeft() const { return mLeft; }
    double getFloorplanRight() const { return mRight; }

    void writeGlobalMatrixToScans();
    void setMaster( ViewPoint* vp, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock, bool update, bool force, bool addMatrixToSystems );

    ViewPoint* getViewpoint( const QString& name );
    ViewPoint* findViewpoint( const ZFProjectItemScan* scan );
    const ViewPoint* findViewpointConst( const ZFProjectItemScan* scan ) const;
    ViewPoint* findViewpoint( const QString& scanName );
    ViewPoint* findMaster( ZFRegisterObjNode::eRegisterError& error, bool useOnlySelected, bool seekMaster );
    ZFRegisterObjNode::eRegisterError checkMasterVP( ViewPoint* master, bool useOnlySelected );

    ViewPoint* lastViewpoint();

    /**
    * @brief Generates a new viewpoint object based on the data stored in the given xml node.
    *
    * @param element    the xml node used to create the viewpoint from
    *
    * @return the name of the newly generated viewpoint or an empty string if something went wrong
    */
    virtual std::wstring createViewpointFromXmlNode( tinyxml2::XMLElement* element, ZFProject::eProjectError& err );

    /**
    * @brief Loads the objects from the associated XML document
    */
    virtual bool Load();

    /**
    * @brief Stores the objects to the associated XML document
    */
    virtual bool Store( double versNr );

    void cleanup();

    const wchar_t* path();

    ZFProject* getParent() { return parent;}

    void clearVPList();

    void readLabels( ZFSHeader* h, std::map<std::wstring, int>& labelMap );
    void checkVPNr( const std::wstring& vpName );
    void updateLastVPNr();

    /*
    *   Gibt eine Liste aller ausgewaelten Scanpositionen zurueck. 
    *   \param[out] hasStartScan => True = In der Auswahl befindet sich mindestens
    *                               eine Start Scanposition fuer die Registierung. 
    */
    std::list<std::wstring> getAllSelectedVP( bool useTachy, bool& hasStartScan );

    /**
    *   Gibt die Liste aller ausgewaelten Scanpositionen zurueck
    */
    std::vector<std::wstring> getSelectedVPs();

    QStringList getAllSelectedScans(bool onlySorted, bool fullFilename = true) const;
    bool selectedScans( int min, int max );
    int nrSelectedVP();
    bool allSortedVPSelected( bool onlyVisible, bool sortet );
    bool notSortedVPSelected();
    void deselectAllNotSortedScans();
    void deselectAllVP();
    void selectAllVP( bool onlyVisible );

    void updateGraph();
protected:

    void writeMasterToProject( const std::wstring& master );
};


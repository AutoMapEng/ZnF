#pragma once
#include "ZfsPrjBasic.h"
#include <zfcore/compiler.h>
#include "config.h"

#include <zfs/zfs.h>
#include <zfs/scaninfo.h>

#include <list>
#include <vector>

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QString>
#include <QStringList>
#include <QObject>
#include <QList>
#include <QPointer>
#include <QDateTime>
SWITCH_WARNINGS_ON

#include "obj.h"
#include "layer.h"
#include "vecname.h"
#include "target.h"
#include "metadata.h"
#include "project_params.h"



namespace tinyxml2 {
    class XMLNode;
}

class ZFScanBasic;
class ZFProject;
class ZFProjectItemScan;
class ViewPoint;
class ZFProjectItemScan;

//==============================================================================

/**
* @brief Class to define how the viewpoint name of a scan is optained.
*
* DEFAULT      the configured value is used as base value
* HEADER       the name is taken from the scan header (only ZFS scans)
* SCANNAME     the scan name is taken
* USERDEFINED  the name stored in viewpointName is taken
*
* If writeToHeader is true, the viewpoint name is stored in the
* scan header (ZFS scans only).
*/
class ZFSPRJ_API ScanPositionNamingOptions {

public:
    enum ScanposNaming {
        DEFAULT,
        HEADER,
        SCANNAME,
        USERDEFINED
    } namingOption = DEFAULT;

    std::wstring viewpointName;
    bool writeToHeader = false;
    bool useExternalMatrix = true;
    bool noteGlobalMatrix = false;

    ScanPositionNamingOptions() {}

};

//===============================================================================


class ZFSPRJ_API ZFProjectItemBase : public ZFBase, public ProjectParams
{
    DEFINE_ZFBASE(ZFBase)

        friend class ZFProject;

protected:
    ItemStatus  status = ItemStatus::ITEM_OK;      // Status of the scans

    ZFProject* parent;  // back pointer
    std::wstring  filename;     // filename
    std::wstring  ext;          // file extension
    std::wstring  subfolder;    // unterverzeichnis prj -> scan e.g. prj/<subfolder>/scan
    std::wstring  subcolortemppath;// unterverezichnis scan -> hdr-pictures e.g. prj/<subfolder>/scan/<subfoldercolor>/pics
    ZFProjectItemBase* back;
    tinyxml2::XMLNode* mNodeBase{};

    std::string uuid = "";   // UUID of the scan
    bool visible = true;

    static bool useTempVis;
    bool tempVis = false;

protected:
    virtual void myInit( tinyxml2::XMLNode* parent,  tinyxml2::XMLNode* nodeBase) = 0;

public:
    ZFProjectItemBase(
        tinyxml2::XMLNode* parent, tinyxml2::XMLNode* nodeBase,
        ZFProject*, const std::wstring& fn,
        const std::wstring& ext = L"", const std::wstring& sub = L"");
    virtual ~ZFProjectItemBase();

    void setVisible(bool b);
    void setTempVis(bool b);
    bool isVisible() const
    {
        bool vis = visible;
        if (useTempVis)
            vis = tempVis;
        return vis;
    }

    /**
    *   Aktivate the use of the tempVis Variable to show ore hide scans.
    *
    *   ZFProject::PRJ_GROUP_CHANGED
    */
    static void useTempVisible(bool useTemp);

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

    /**
    * @brief Get the status of this item.
    */
    ItemStatus getStatus() { return status; }

    /**
    * @brief Set the status of the item. This can be used to mark an item as deleted.
    *
    * @param newStatus the new status of the item.
    */
    void setStatus(ItemStatus newStatus) { status = newStatus; }


    ZFProject* Parent() const { return parent; }
    ZFProjectItemBase* Back() const { return back; }
     tinyxml2::XMLNode* xNode() const { return mNodeBase; }
    const  tinyxml2::XMLNode* xNodeConst() const { return mNodeBase; }

    virtual bool Open() { return false; }
    virtual void Close() { ; }
    virtual bool IsOpen() const { return false; }
    virtual bool writeXML(float projectFileVersion = PROJECT_FILE_VERSION) = 0;
    virtual bool readXML(ZFProject* prj,  tinyxml2::XMLNode* n) = 0;

    /** Returns Filename without path and extension */
    const wchar_t* Filename() const { return filename.c_str(); }
    const std::wstring& getExtension() const { return ext; }
    void setExtension(const std::wstring& e) { ext = e; }
    void setFilename(const std::wstring& n) {
        filename = n;
    }
    /** Returns Filename including extension without path*/
    std::wstring FileNameExt() const;
    /** Returns Filename including extension and path*/
    std::wstring FullFilename() const;

    std::wstring PreviewName() const;

    const wchar_t*  ParentPath() const;
    std::wstring getFilePath() const;

    void setSubFolder(std::wstring _subfolder) {
        subfolder = _subfolder;
        if (subfolder.empty()==false && subfolder.front() == '/') {
            subfolder.erase(0, 1);
        }
    }

    const std::wstring& SubFolder() const { return subfolder; }
};

typedef std::list<ZFProjectItemBase*> itemlist_t;
typedef itemlist_t::iterator itemlist_iterator;



//------------------------------------------------------------------------------------------------------------------

class ZFSPRJ_API ZFProjectItemPicture : public ZFProjectItemBase {
    DEFINE_ZFBASE(ZFProjectItemBase)
        friend class ZFProject;
    friend class ZFProjectItemScan;

public:
    bool isImgAvailable();
    bool ImgAvailable();
protected:
    virtual void myInit( tinyxml2::XMLNode* parent,  tinyxml2::XMLNode* nodeBase);
    bool		 imgAvailable;	// true if img can be read ( availbale on disk )
    static QStringList extList;

public:
    ZFProjectItemPicture( tinyxml2::XMLNode* parent, ZFProject*,
        ZFProjectItemBase* base, const std::wstring& fn,
        const std::wstring& ext = L"", const std::wstring& sub = L"");
    ZFProjectItemPicture( tinyxml2::XMLNode* baseNode, ZFProject* prj, ZFProjectItemBase* base);
    virtual ~ZFProjectItemPicture();

    virtual bool writeXML(float projectFileVersion = PROJECT_FILE_VERSION);
    virtual bool readXML(ZFProject* prj,  tinyxml2::XMLNode* n);

    /* List of all importable extensions */
    static const QStringList* getExtList() { return &extList; }

};

//------------------------------------------------------------------------------------------------------------------

class ZFSPRJ_API GuardetZFProjectItemScan {
    friend class ZFProjectItemScan;

private:
    ZFProjectItemScan* mScanPtr;

protected:
    void delPtr() { mScanPtr = NULL; }


public:
    GuardetZFProjectItemScan(ZFProjectItemScan* scan) : mScanPtr(NULL) {
        setPtr(scan);
    }
    GuardetZFProjectItemScan() : mScanPtr(NULL) {}
    GuardetZFProjectItemScan(const GuardetZFProjectItemScan& ob) : mScanPtr(NULL) { setPtr(ob.mScanPtr); }
    virtual ~GuardetZFProjectItemScan() { setPtr(NULL); }

    ZFProjectItemScan* getPtr() const { return mScanPtr; }
    void setPtr(ZFProjectItemScan* scan);

    bool operator==(const GuardetZFProjectItemScan& ob) const {
        if (mScanPtr == ob.mScanPtr)
            return true;
        return false;
    }
};

struct DBInfo
{
    std::string id;       //!< Id des Scans
    ZFTRMatrix addMat;    //!< Orginal Matrix des Scans aus der zfmdb Datei.
};


class ZFSPRJ_API ZFProjectItemScan : public ZFProjectItemBase 
{
    DEFINE_ZFBASE(ZFProjectItemBase)

    friend class ZFProject;
    friend class GuardetZFProjectItemScan;

protected:
    QPointer<ViewPoint> viewPoint;  // Viewpoint object
    std::wstring   filePath;		// full filename with path
    std::wstring   shortfilePath;	// short filename without path and extension
    std::wstring   shortfileExt;	// short filename with extension and without path
    //ZFTRMatrix     matrix;			// translation/rotationsmatrix
    bool           scanAvailable{};	// true if scan can be read ( available on disk )
    bool		   mIsSelected{};		// flag ob der Scan ausgewaelt ist.

    std::vector<DBInfo> dbIdList;           //!< Liste aller db id's die zu diesem Scan gehoeren.

    QList<GuardetZFProjectItemScan*> mGuardList; //Liste aller sicheren Objekte die auf dieses element zeigen.
    static QStringList extList;			//Liste aller moeglichen extensions (z.B. .zfs)
    static QStringList fullExtList;		//Liste aller meoglichen extensions komplett (z.B. Z+F Scan (*.zfs))

    int mNrPoints{};			//!< Anzahl der Pixel im Scan
    zfs::resolution_t mResolution;		//!< Aufloesung des Scans
    bool mHasColor{}; //!< Flag ob farbinformationen zu dem Scan vorhanden sind
    bool updateRegister{};					//!< Hilfsflag ob die registrierung aus dem Projekt uebernommen werden soll
    std::wstring scannerName;         	//!< Name des Scanners, der den Scan aufgenommen hat

    QDateTime scanDate;

    /*
    * @brief The user defined color for this scan.
    *        This is used to color the point cloud in 3D view or the entry in the scan browser
    */
    ZFrgb pointcloudColor;

    /**
    * @brief Set the full filename of the item taking the project path and subfolder settings into account.
    *        The calculated filename is set to the field filePath.
    */
    void setFilePath(const std::wstring &projectPath, const std::wstring& fileName);
    void setShortFileName();

public:
    zfprj::ZFMetadataList metadataList;

    virtual void myInit( tinyxml2::XMLNode* parent,  tinyxml2::XMLNode* nodeBase);

    void addGuardPtr(GuardetZFProjectItemScan* ptr) {
        if (mGuardList.size()) {
            for (auto it = mGuardList.begin(); it != mGuardList.end(); ++it) {
                if (*it == ptr)
                    return;
            }
        }
        mGuardList.push_back(ptr);
    }
    void removeGuardPtr(GuardetZFProjectItemScan* ptr) {
        mGuardList.removeOne(ptr);
    }

    void setDBIDList( const std::vector<DBInfo> idList ) { dbIdList = idList; }
    const std::vector<DBInfo>& getDBIDList() const { return dbIdList; }

    /**
    * @brief Accessor-Method to the color in which the scan is represented in e.g. the 3D view
    */
    ZFrgb getPointCloudColor() const {
        return pointcloudColor;
    }

    /**
    * @brief Set the color in which the scan is represented in e.g. the 3D view and the scan browser.
    *        The color is not changed automatically. This has to be done separately
    *
    * @color the new color to be used for this scan
    */
    void setPointCloudColor(ZFrgb color) {
        pointcloudColor = color;
    }

public:
    ZFProjectItemScan( tinyxml2::XMLNode* parent, ZFProject*,
        const std::wstring& fn, const std::wstring& ext = L"", const std::wstring& sub = L"");
    ZFProjectItemScan( tinyxml2::XMLNode* node, ZFProject*);
    virtual ~ZFProjectItemScan();

    /* List of all importable extensions */
    static const QStringList* getExtList() { return &extList; }
    /* Filter list for all importabel data types*/
    static const QStringList* getFullExtList() { return &fullExtList; }

    virtual void deleteFiles(std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock) {};
    virtual void copyFiles(const std::wstring& newPath, bool overwrite) {};
    virtual bool Open();
    virtual void Close();
    bool IsOpen() const override;

    virtual bool writeXML(float projectFileVersion = PROJECT_FILE_VERSION);
    virtual bool readXML(ZFProject* prj,  tinyxml2::XMLNode* n);

    const wchar_t* getShortFileName() const { return shortfilePath.c_str(); }  ///< Returns the filename without extension and path
    std::wstring getShortFilenameWithExtension() const;   ///< Returns the filename with extension

    void setVP(ViewPoint* vp);
    ViewPoint* getVP();
    const ViewPoint* getVP() const;
    QString getVPName() const;

    QString getMulticloudName() const;
    virtual bool hasMulticloud() const;

    bool ScanAvailable(bool forceCheck = false);
    bool isSelected() const { return mIsSelected; }
    void setSelected(bool sel) { mIsSelected = sel; }

    virtual std::wstring FullFilenameSubscan() { return L""; }
    virtual bool subscanAvailable() { return false; }

    bool Intersect(ZFProjectItemScan*);			// not implemented yet


    // Register
    /**
    *	Rueckgabe des Zeitpunktes an dem der Scan aufgenommen wurde als ein double wert im Format jj, mm, dd, hh, min
    *	Gibt 0 zurueck wenn kein Datum bekannt ist
    */
    QDateTime getScanDate() const {
        return scanDate;
    }

    virtual double getDateTimeNr() = 0;
    /**
    *	Gibt die Registrierungsmatirx des Scans zurueck
    *	\param global => true = Es wird die Matrix des Scans/Scanposition inklusive der Superior Matrix zurueckgegeben. Default = false
    */
    ZFTRMatrix Matrix(bool global = false) const;

    /**
    *   Liest die Registrierung aus dem Scan ein geht nur fuer ZFS-Scans
    *   \param regFlag => RegistrierungsFlag aus dem header
    *   \return eingelesene Matrix falls der Scan registriert wurde ansonsten Einheitsmatrix
    */
    virtual ZFTRMatrix readMatrixFromScan(int& regFlag);


    void updateRegisterFromProject();

    /**
    *	Gibt die Anzahl der im Scan vorhandenen Punkte zurueck
    */
    int getNrPoints() const { return mNrPoints; }

    /**
    *   Setzten der anzahl der im Scan vorhandenen Punkte.
    */
    void setNrPoints(int points) { mNrPoints = points; }

    /**
    *	Gibt die Aufloessung zuruck oder 0 wenn diese nicht bekannt ist
    */
    zfs::resolution_t getResolution() const { return mResolution; }

    /**
    *	Gibt true zurueck wenn farbinformationen zu dem Scan vorhanden sind
    */
    bool hasColor() const { return mHasColor; }
    void setColor(bool col) { mHasColor = col; }

    /**
    *	Einlesen der allgeimeinen Informationen aus dem Scan
    */
    virtual void readInformation() = 0;

    /**
    *	Falls vorhanden Color status updaten
    */
    virtual void updateColorInfo(bool updateChilds = true) {}

    /**
     * @brief Get the name of the scanner which has taken the scan
     */
    std::wstring getScannerName() const { return scannerName; }

    /**
    *   Liest die vorhandnen Informationen aus der Datei ein und schreibt diese in das Projekt.
    */
    virtual bool updateScanFromFile() { return true; }

    //Targets
    /**
    *   Target aus dem Projekt loeschen.
    *   \param targetName => Name des Targets das geloescht erden soll.
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    *   \return True wenn das Target geloescht werden konnte.
    */
    virtual bool deleteTarget(const std::wstring& name, bool updateProject, bool sendSignal);

    /**
    *   Loeschen aller Targets fuer den Scan aus der Projektdatei
    *   \param head => Falls der Header des Scans offen ist kann er hier uebergeben werden. Nur fuer ZFS Scans.
    *   \param targets => Liste der Targets die in das Projekt eingetragen werden sollen.
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    virtual bool deleteAllTargets( bool updateProject, bool sendSignal);

    /**
    *	Hinzufuegen eines Targets zu dem Scanobjekt in der Projektdatei
    *   \param target => Target Objekt
    *   \param updateProject => True das Target wird zum VP hinzugefueg.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    *   \param overwrite => True falls das Target bereits existiert wird es ueberschrieben. False das Target wird nicht veraendert und wird zurueckgegeben.
    *   \param createImage => True Image zu dem Target erstellen. Nur für zfs Scans
    */
    virtual int addTarget( const ZFTarget& target, bool updateProject, bool sendSignal, bool overwrite, bool createImage );

    /**
    *   Target im Projekt umbenennen.
    *   \param head => Falls der Header des Scans offen ist kann er hier uebergeben werden. Nur fuer ZFS Scans.
    *   \param oldName => Name des Targets
    *   \param newName => Neuer Name des Targets
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    virtual bool renameTarget( const std::wstring& oldName, const std::wstring& newName, bool updateProject, bool sendSignal);


    /**
    *   Check if the gifen target exists in the Scan
    *   \param targetName => Name of the target.
    *   \raturn true = Target exists
    */
    virtual bool targetExists( const std::wstring& targetName );

    /**
    *   Setzt das Target auf use/notUse.
    *   \param  name => Name des Targets
    *   \param use => true = Target wird buetzt
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    void setTargetUse(const std::wstring name, bool use, bool updateProject, bool sendSignal);

    /**
    *   Setzt alle Targets auf use/notUse.
    *   \param  name => Name des Targets
    *   \param use => true = Target wird buetzt
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    void setTargetUseAll(bool use, bool updateProject, bool sendSignal);

    /**
    *   Schreibt die Targets der uebergebenen Liste in die Projetkdatei und fuegt sie der Scanposition hinzu.
    *   \param list => Liste mit den Targets die in das Projekt geschrieben werden sollen.
    */
    void addTargets( const targetlist_t& list, bool overwrite);

    /**
    *	Liest die Targets aus dem Header des Scans aus und schreibt sie in die Projektdatei.
    *	\param registerNode => Node zu dem die Targets geschrieben werden falls NULL uebergeben wird wird der node aus dem scanItem Objekt geholt.
    */
    virtual bool updateScanTargets( tinyxml2::XMLNode* registerNode);


    /**
    *   Liest alle Targets des Scans aus der Projektdatei aus.
    *   \param[out] lst => Liste der eingelesenen Targets
    *   \param additional => True slam targets werden mit zurueckgelesen.
    */
    virtual errTarget_t readTargetsFromProject( targetlist_t &lst, bool additional, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock) const;

    /**
    *   Sammelt fuer den Scan die registrier Informationenen und feugt diese dem Projekt hinzu
    *	\param viewpoint => Scanpositions Name falls einer im Scan steht wird er in die Variable geschrieben.
    *   \param upsideDown => Rueckgabevariable ob der Scan auf dem Kopf steht.
    *   \param useLevelAll = >
    *	\return Zurueckgegeben wird der Fehlerwert  (regerr_t)
    */
    virtual int addScan2Project(std::wstring& /*viewpoint*/, bool& /*upsideDown*/, bool /*useLevelAll*/) { return 0; }

    bool addRegisterItem(const std::wstring& viewpoint);

};



//================================================================================================================

/**
*	Scans des Typs ZFScan
*/
class ZFSPRJ_API ZFProjectItemScanSorted : public ZFProjectItemScan {
    DEFINE_ZFBASE(ZFProjectItemScan)

protected:
    int mPixel;				//!< Anzahl der Pixel pro Zeile
    int mLines;				//!< Anzahl der Zeilen im Scan

public:
    ZFProjectItemScanSorted( tinyxml2::XMLNode* parent, ZFProject*, const std::wstring& fn,
        const std::wstring& ext = L"", const std::wstring& sub = L"");
    ZFProjectItemScanSorted( tinyxml2::XMLNode* node, ZFProject*);

    bool IsOpen() const override;
    int getPix() const;
    int getLine() const;
    int getNrPoints() const;
    bool hasColor() const;
    zfs::resolution_t getResolution() const;


    ZFScanBasic* Scan();
    const ZFScanBasic* Scan() const;
};

//================================================================================================================

/**
* @brief Factory to generate scan object instances.
*/
class ScanFactory {

private:

    ScanFactory() {}
    ~ScanFactory() {}

public:

    /**
    * @brief Access-Method to the ScanFactory instance.
    * @return the instance
    */
    static ScanFactory *getInstance() {
        static ScanFactory instance;
        return &instance;
    }

    /**
    * @brief Helpermethod to determine the scan typ by its name.
    *
    * The extension of the given filename is examined and the typ name is returned.
    *
    * @param the filename of the scan
    * @return the scan typ as string
    */
    QString getScanTypeByFilename(const QString& filename);

    /**
    * @brief	Creates a ZFProjectItemScan instance from the given data.
    *
    * @param nrPoints   number of points in the scan
    * @param res        resolution of the scan
    * @param color      flag if it is a color scan
    * @param parent     scans node of the project
    * @param prj        correlated project
    * @param fn         filename of the scan to be added (can contain the path). The name must contain the extention!
    *
    * @return the created ZFProjectItemScan instance or NULL if no instance could be created
    */
    ZFProjectItemScan* create( int nrPoints, zfs::resolution_t res, bool color,
                               tinyxml2::XMLNode* parent, ZFProject* prj,
                               const std::wstring& fn );

    /**
    * @brief	Creates a ZFProjectItemScan instance from the given XML node data.
    *
    * @param node   XML node to be used to create the scan instance
    * @param prj    project of the scan
    *
    * @return the created ZFProjectItemScan instance or NULL if no instance could be created
    */
    ZFProjectItemScan* create( tinyxml2::XMLNode* node, ZFProject* prj);

};

//================================================================================================================

typedef std::vector<ZFProjectItemScan*> scans_list_t;

//================================================================================================================


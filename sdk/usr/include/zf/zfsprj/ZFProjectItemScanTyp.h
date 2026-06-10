#pragma once
#include <zfcore/compiler.h>

#include "project_items.h"
#include "config.h"


#include <string>
#include <mutex>


class ZFProject;

//======================================= Sortiert =========================================================================

/**
*	Scans des Typs ZFScan
*/
class ZFSPRJ_API ZFProjectItemZFS : public ZFProjectItemScanSorted
{
    DEFINE_ZFBASE(ZFProjectItemScanSorted)

private:
    itemlist_t   pic;			// verknuepfung zu pictures
    bool imagesInitialised;
    bool hasImageFiles;

    std::mutex mutGetHeader;

protected:
    ZFSHeader      header;			// header of scan if not open

  public:
    int mTyp;                       // Scan typ aus dem Header

    bool mUpsideDown;

    //Registrierungs status aus alten Projektdateien
    enum eRegStatus {
        REG_NOREG = 0,
        REG_POS = 100,
        REG_TACHY = 110,
        REG_SCAN = 120,
        REG_NEPTAN = 130,
        REG_EXTERN = 140,
        REG_HEADER = 150 //REG_HEADER Registrierung wurde nur aus dem Headereintrag uebernommen
    };

public:
    ZFProjectItemZFS(  tinyxml2::XMLNode* parent, ZFProject*,
        const std::wstring& fn, const std::wstring& ext = L"", const std::wstring& sub = L"");
    ZFProjectItemZFS(  tinyxml2::XMLNode* node, ZFProject*  );

    virtual ~ZFProjectItemZFS();

    bool writeXML(float projectFileVersion = PROJECT_FILE_VERSION) override;
    bool readXML( ZFProject* prj,  tinyxml2::XMLNode* n ) override;

    ZFScan* getScanObject( bool& useOpen );

    /*
    *   Gibt einen Zeiger auf das Header Objekt zurueck.
    */
    ZFSHeader* getZFSHeader();

    /// header has to be read in advance otherwise return zero
    const ZFSHeader* getZFSHeader() const;

    /// return wheather zfs-header is already in memory loaded
    bool isHeaderOpen() const;

    /// remove the cached header
    void destroyHeader();

    const itemlist_t& getPictures(bool updateItems = true);

    bool hasPictureFiles();

    void renameImages(const std::wstring& oldScanName, const std::wstring& newScanName);
    void loadImages( tinyxml2::XMLNode* scanNode, ZFProject * prj, bool force = false);

    bool AddPic( const std::wstring& fullfn);
    bool AddPicAndCalib( const std::wstring& fullfn, const std::wstring& cameraname, int mapmode, double lenc, double beta);

    bool DelPic( const std::wstring& shortfn);
    bool DelAllPic();

    ZFProjectItemPicture* Find( const std::wstring& shortfn );

    std::wstring FullFilenameSubscan() override;
    bool subscanAvailable() override;
    static std::wstring removeSubFromFileName( std::wstring fn );
    static std::wstring addSubToFileName( const std::wstring& fn );

    void updateColorInfo(bool updateChilds = true) override;
    double getDateTimeNr() override;
    ZFTRMatrix readMatrixFromScan( int& regFlag ) override;
    void readInformation() override;

    bool updateScanFromFile() override;

    //Target
    /**
    *	Hinzufuegen eines Targets zu dem Scanobjekt in der Projektdatei und zum Scan
    *   \param target => Target Objekt
    *   \param updateProject => True das Target wird zum VP hinzugefueg.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    int addTarget( const ZFTarget& target, bool updateProject, bool sendSignal, bool overwrite, bool createImage ) override;

    /**
    *   Target aus dem Projekt und dem Scan loeschen.
    *   \param targetName => Name des Targets das geloescht erden soll.
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    *   \return True wenn das Target geloescht werden konnte.
    */
    bool deleteTarget( const std::wstring& name, bool updateProject, bool sendSignal ) override;

    /**
    *   Loeschen aller Targets fuer den Scan aus der Projektdatei und dem Scanheader.
    *   \param targets => Liste der Targets die in das Projekt eingetragen werden sollen.
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    bool deleteAllTargets( bool updateProject, bool sendSignal ) override;

    /**
    *   Target im Projekt und ZFS Header umbenennen.
    *   \param oldName => Name des Targets
    *   \param newName => Neuer Name des Targets
    *   \param updateProject => True der VP wird aktualisiert.
    *   \param sendSignal => True das Signal ZFProject::PRJ_REGISTER_TARGET wird gesendet.
    */
    bool renameTarget( const std::wstring& oldName, const std::wstring& newName, bool updateProject, bool sendSignal ) override;

    /**
    *	Liest die Targets aus dem Header des Scans aus und schreibt sie in die Projektdatei.
    *   Ausserdem werden alle Informationen zur 1TO Registrierung aus dem Scanheader ausgelesen.
    *
    *   <FreePos Height="1.360000" X="0.000000" Y="0.000000" Z="0.000000">
    *       <ConnectionPoints>  //Auflistung aller Connection Points (Targets)
    *           <ConnectionPoint name="dt2" Height="1.340000" X="12.535000" Y="0.000000" Z="0.000000" exist="1" /> //exists => 1 = Target ist beieits gefittet
    *       </ConnectionPoints>
    *   </FreePos>
    *	\param registerNode => Node zu dem die Targets geschrieben werden falls NULL uebergeben wird wird der node aus dem scanItem Objekt geholt.
    */
    bool updateScanTargets( tinyxml2::XMLNode* registerNode ) override;

    /**
    *   Sammelt fuer den Scan die registrier Informationenen und feugt diese dem Projekt hinzu
    *	\param viewpoint => Scanpositions Name falls einer im Header steht wird er in die Variable geschrieben.
    *   \param upsideDown => Rueckgabevariable fuer den Header Eintrag 22435
    *   \param useLevelAll = >
    *	\return Zurueckgegeben wird der Fehlerwert  (regerr_t)
    */
    int addScan2Project( std::wstring& viewpoint, bool& upsideDown, bool useLevelAll ) override;


 protected:
     void readLabels( ZFSHeader* h, std::map<std::wstring, int>& labelMap );
     void clearImageList();

};



//#################################### Pointclouds ################################################

/**
@basic base class for holding any pointcloud
*/
class ZFSPRJ_API ZFProjectItemPointcloud : public ZFProjectItemScan
{
    DEFINE_ZFBASE(ZFProjectItemScan)

protected:
    std::wstring m_typ;	///< fileextension = typ
    double m_unit;      ///< faktor beim import fuer einheitenumrechnung

  public:
    ZFProjectItemPointcloud(const wchar_t* typ, int nrPoints, bool color,  tinyxml2::XMLNode* parent, ZFProject*, const std::wstring& fn, const std::wstring& sub);
    ZFProjectItemPointcloud(  tinyxml2::XMLNode* node, ZFProject* );

    void readInformation() override {}

    double getUnit() const { return m_unit; }
    void setUnit( double u ) { m_unit = u; }

    bool writeXML(float projectFileVersion = PROJECT_FILE_VERSION) override;
    bool readXML( ZFProject* prj, tinyxml2::XMLNode* n ) override;

    /**
    *	Rueckgabe des Zeitpunktes an dem der Scan aufgenommen wurde als ein double wert im Format jj, mm, dd, hh, min
    *	Gibt 0 zurueck wenn kein Datum bekannt ist
    **/
    virtual double getDateTimeNr() { return 0.; }

    std::wstring getType() { return m_typ; }
};

//-------------------------------------------------------------------------------------------

/**
@brief Holds sorted pointclouds like ptx
*/
class ZFSPRJ_API ZFProjectItemSortedPointcloud : public ZFProjectItemPointcloud
{
    DEFINE_ZFBASE(ZFProjectItemPointcloud)

public:
    ZFProjectItemSortedPointcloud(const wchar_t* typ, int nrPoints, bool color,
        tinyxml2::XMLNode* parent, ZFProject*, const std::wstring& fn, const std::wstring& sub);
    ZFProjectItemSortedPointcloud(  tinyxml2::XMLNode* node, ZFProject*);
};

//-------------------------------------------------------------------------------------------

/**
@brief Holds unsorted pointclouds like las
*/
class ZFSPRJ_API ZFProjectItemUnsortedPointcloud : public ZFProjectItemPointcloud
{
    DEFINE_ZFBASE(ZFProjectItemPointcloud)

public:
    ZFProjectItemUnsortedPointcloud(const wchar_t* typ, int nrPoints, bool color,
        tinyxml2::XMLNode* parent, ZFProject*, const std::wstring& fn, const std::wstring& sub);
    ZFProjectItemUnsortedPointcloud(  tinyxml2::XMLNode* node, ZFProject*);
};


#ifdef ZFPRJ_ZFDB

/**
*    Multicloud
*/

class ZFSPRJ_API ZFProjectItemMDB : public ZFProjectItemUnsortedPointcloud
{
    DEFINE_ZFBASE( ZFProjectItemUnsortedPointcloud )

    std::vector<std::string> fileList; //Liste aller ID's in utf8

public:
    ZFProjectItemMDB( const wchar_t* typ, int nrPoints, bool color, tinyxml2::XMLNode* parent,
                        ZFProject*, const std::wstring& fn, const std::wstring& sub );

    ZFProjectItemMDB( tinyxml2::XMLNode* node, ZFProject* );

    /**
    *   Loesscht alle zu dem scan gehoerenden zfdb Dateien inklusive der zfmdb
    */
    void deleteFiles(std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock) override;
    /**
    *   Kopiert alle zu dem scan gehoerenden zfdb Dateien inklusive der zfmdb
    */
    void copyFiles(const std::wstring& newPath, bool overwrite) override;

    bool writeXML( float projectFileVersion = PROJECT_FILE_VERSION ) override;
    bool readXML( ZFProject* prj, tinyxml2::XMLNode* n ) override;

    /// find cloud in the fileList
    bool findCloud( const std::wstring& cloudFileName ) const;

    const std::vector<std::string>* getIdList() const { return &fileList; }

    errTarget_t readTargetsFromProject(targetlist_t& lst, bool additional, std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock) const override;

    bool isSlamResult( std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> lock);

    int addTarget(const ZFTarget& target, bool updateProject, bool sendSignal, bool overwrite, bool createImage) override;
    bool deleteTarget(const std::wstring& name, bool updateProject, bool sendSignal) override;

    bool hasMulticloud() const override;

};
#endif // ZFPRJ_ZFDB

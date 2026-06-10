#pragma once

#include "ZfsPrjBasic.h"
#include "config.h"
#include "ZFProjectItemScanTyp.h"
#include "defines.h"
#include "obj.h"
#include "project_lock.h"
#include "project_params.h"
#include "register_defines.h"

#include <zfs/rainbow.h>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QObject>
#include <QString>

#include <list>
#include <vector>
#include <memory>
#include <mutex>
SWITCH_WARNINGS_ON

#define COLORTEMPPATH L"Color/"
#define COLORTEMPPATHSHORT L"Color"
#define REPLACECOLORTEMP

#ifdef _WINDOWS
#pragma warning(disable : 4251)
#ifndef NOMINMAX
#define NOMINMAX
#endif

#endif

//=======================================================================================================================

class ZFPrjParser;
namespace zfprj { class ProjectZfdb; }

namespace tinyxml2 {
class XMLDocument;
class XMLNode;
} // namespace tinyxml2

class PrjGroups;
class RegisterPrecision;
class ZFProject;
class ZFScan;
class ZFProjectItemScan;
class ZFProjectItemBase;

namespace zfprj {
#ifdef ZFPRJ_FILEEXPORT
class ProjectObjectsToFileExporter;
class LogIndent;
#endif
template <typename T> class WritableLockedProjectObjects_Ptr;
template <typename T> class ReadableLockedProjectObjects_Ptr;
class Layers;
class LogIndent;

class CoordinateSystems;
class ZFProjectWatcher;
} // namespace zfprj

#include "groups.h"

namespace zfs {
    namespace math {
        namespace gps {
            class GPSCoordinate;
        }// end namespace gps
    }// end namespace math    
} // end namespace zfs

/**
 * @brief Enum to define the mode, how scans are positioned.
 */
enum track_t {
  TRACK_MOVE,           // move using IMU
  TRACK_SET,            // set directly with accurate position
  TRACK_SET_INACCURATE, // set directly with inaccurate position
  TRACK_GPS,            // use GPS coordinate
  TRACK_DEFAULT         // default distance (1 meter in each axis)
};

//#####################################################################################################################################
class BackGroundImg;

/**
 * @brief Class to store the parameters for a background image.
 */
class ZFSPRJ_API BackgroundImageParameter {

public:
    BackgroundImageParameter() = default;

  QString id;
  QString imageName; // name of the background images
  double imageX{};     // xPos of image
  double imageY{};     // yPos of image
  double imageZ{};     // zPos of image
  double scaleX{1.};     // scale for x axis
  double scaleY{1.};     // scale for y axis
  double rotation{};   // rotation (grad)
  double opacity{60};    // opacity

  bool warning = true; //True warnungen ausgeben z.b. dass das Bild nicht vorhanden ist. (wird nicht gespeichert)

  /**
   * @brief Read in parameters from the given node.
   *
   * @param viewNode the node to be read
   */
  void readParamFromNode(tinyxml2::XMLElement *viewNode, QString path = "");

  /**
   * @brief Write parameters from the given node.
   *
   * @param viewNode the node to be written
   */
  void writeParamToNode(tinyxml2::XMLElement *viewNode, QString path = "");
};

//############################################################################################################################
//############################################################################################################################
//############################################################################################################################


/**
 * \brief Handles "meta" data for a collection of registered scans.
 *
 * The project stores the results of the work, that a user does to when working
 * with Z+FLaserControl. This work may include: Registration, finding targets,
 * measuring, etc..
 *
 * The implementation is split between several files.
 *
 * Project.cpp                   all methods for general properties and
 * parameter handling and object access project_io.cpp                all
 * methods related with file io project_util.cpp              collection of
 * utility methods project_registration.cpp      all methods related with
 * project registration project_callback.cpp          all methods related with
 * callbacks
 *
 */
class ZFSPRJ_API ZFProject : public ZFBase, public ProjectParams {
  DEFINE_ZFBASE(ZFBase)

  friend class ZFPrjParser;
  friend class ZFProjectItemScan;
  friend class zfprj::LogIndent;

protected:
  void savePlanParam(tinyxml2::XMLElement *viewNode);
  void readPlanParam(tinyxml2::XMLElement *viewNode);

public:
  ZFProject(int properties);
  ZFProject(bool showAllVP = true, bool bCB = true, bool bLog = true,
            bool bBak = true);
  virtual ~ZFProject();

  enum prop_t      // Project properties
  { P_NOTHING = 0, ///< just the prj
    P_CB = 1,      ///< callbacks
    P_LOG = 2,     ///< logfile erzeugen
    P_BAK = 4,     ///< backups anlegen
    P_IGNORVER_READONLY =
        8, ///< do not display message open read only if version missmatch, it
           ///< will open scan in readonly without asking always
    P_IGNORVER_OVERWRITE =
        0x10, ///< do not display message open read only if version missmatch,
              ///< it will open scan in overwrite mode
    P_WATCH_FILESYSTEM =
        0x20, ///< watch for changes of filesystem in project path

    P_DEFAULT = (P_CB | P_LOG | P_BAK | P_WATCH_FILESYSTEM) };

  enum eProjectError {
    PROJECT_OK,
    PROJECT_UNKNOWNERROR,
    PROJECT_ISOPEN,
    PROJECT_CANTOPNEN,
    PROJECT_CANTREAD,
    WARNING_DELETE_DOUBLE_CONNECTIONS,
    WARNING_ORTHONORMALIZE_MATRIX
  };

  //================================================================================================================================
  //=========
  //========= project creation and read write
  //=========
  //================================================================================================================================

  /**
   * @brief Creates an empty XML document.
   */
  bool makeProject();

  /**
   * @brief Creates a new project file with the given filename.
   *
   * @param filename the filename for the project
   * @return true if the project file was created, false otherwise
   */
  virtual bool Neu(const std::wstring &filename);

  /// Only sets mFilename
  bool Create(const wchar_t *filename);

  /**
   * @brief The recomended open of project with the given filename and flags
   */
  enum {
    OPEN_LOCK = 1,        ///< lock the project (default yes)
    OPEN_USECB = 2,       ///< call callbacks
    OPEN_UPDATESCANS = 4, ///<
    OPEN_UNDO = 8,        ///< undo support
    OPEN_READONLY = 16,   ///< open project as read only

    OPEN_DEFAULT = (OPEN_LOCK | OPEN_USECB | OPEN_UPDATESCANS | OPEN_UNDO),
    OPEN_VIEWER = (OPEN_LOCK | OPEN_USECB | OPEN_READONLY),
  };
  virtual bool Open(const std::wstring &filename, int &errorCode,
                    int openflags);

  // open the cloned project. Do not create lock or use callbacks. No undo.
  bool openClone(const std::wstring& filename, bool readOnly=true );

  // allow to open a already open project as read only
  bool openReadonly(const std::wstring& filename, int& errorCode );


  /**
   * @brief Opens the project with the given filename.
   *
   * @param filename       the filename of the project
   * @param errorCode      error code (out)
   * @param lock           flag, if the project should be locked
   * @param useCB          flag, if callbacks are used
   * @param updateScans    flag, if the contained scans should be refreshed
   *
   * @return true, if the project was opened, false otherwise
   */
  virtual bool Open(const std::wstring &filename, int &errorCode,
                    bool lock = true, bool useCB = true,
                    bool updateScans = true, bool useUndo = true);

  /**
   * @brief Opens the project with the given filename.
   *
   * @param filename       the filename of the project
   * @param errorCode      error code (out)
   * @param lock           flag, if the project should be locked
   * @return true, if the project was opened, false otherwise
   */
  bool Open(const zf::FileName &filename, int &errorCode, bool lock = true);

  /**
   * @brief Saves the project.
   *
   * @param msSaveTime         maximal time so try saving
   * @param filename           filename of the project
   * @param useCB              flag, if callbacks should be called
   * @param trySave            flag, if save should fail if project is locked
   * @param projectFileVersion project version in which we want to store the
   * project (used for export for older LRC versions)
   *
   * @return true if the project was saved, false if project was locked and
   * trySave was true or if saving failed
   */
  virtual bool Save(int msSaveTime = 0, const wchar_t *filename = nullptr,
                    bool useCB = true, bool trySave = false,
                    float projectFileVersion = PROJECT_FILE_VERSION);

  /**
   * @brief Check, if we need a backup file. Time check is against BACKUP_TIME.
   *        If the time is reached a new backup is created.
   *        If the number of backupfiles is bigger than MAX_BACKUP_FILES the
   * last file is removed.
   */
  void createBackup();

  /**
   * generate a clone file and return the string.
   * if no project is open, a empty string will be returned
   * 
   * \param tempPath define if it should be stored in a temp path or in the root path of the project
   * \return the abs file name of the project created
   */
  std::wstring storeClone( bool tempPath=true, bool storeMaster=true );
  /// build a filename for clone. project must have a filename otherwise a empty string is returned
  std::wstring createCloneFilename(bool tempPath);


  /**
   * @brief Check, if the given file has the same hash value than the project
   * file of the current project.
   *
   * @param backupFile the file to be checked
   */
  bool checkHash(const QString &backupName);

  /**
   * @brief Close the project.
   *
   * @param save  Flag, if the underlying XML document should be saved
   * @param useCB Flag, if callbacks should be fired
   */
  virtual void Close(bool save = true, bool useCB = true);

  /** returns temp path of project, only if project is open,
  *   creates folder if not exist
  */
  const std::wstring TempPath();
                                

  void SetPath(const std::wstring &newp) { path = newp; }
  bool IsOpen(const wchar_t *filename = nullptr) const; //!< check if defined or any project is open
  bool IsOpening() const;         //!< check if project is currently opened
  const wchar_t *OpenErrorString();

  /**
   * @brief Check if the given project was saved with a version higher than
   * current LRC. If the project is newer a message box is displayed.
   *
   * @return true if the current project has a fitting version number or the
   * user choosed to open it in write mode anyway, false if the user choose to
   * open in read only mode
   */
  bool checkSaveProgram();

  //===================================================================================
  //=========
  //========= project properties
  //=========
  //====================================================================================

  const wchar_t *Filename() const {
    return mFilename.w_str();
  } //!< filename of project (*.zfprj)

  const wchar_t *FilenameShort() const {
    return fileNameShort.c_str();
  } //!< filename of project with out path and ext

  const char *FilenameC() const {
    return mFilename.c_str();
  } //!< filename of project (*.zfprj)

  void SetFilename(const std::wstring &newfn) { mFilename = newfn; }

  const wchar_t *Path() const { return path.c_str(); }

  const std::wstring PathW() const { return path; }


  /**
   * @brief Get the project name.
   *
   * @return the project name
   */
  std::wstring getProjectName() const { return projectName; }

  /**
   * @brief Set the name of the project stored in the project XML node
   */
  void setProjectName(const std::wstring &name) { projectName = name; }

  /**=====================================================================================
  * access methods for basic project parameters and properties (implemented in
  Project.cpp) \return true if write protected or read only
  */
  bool IsWriteProtected();

  /// get project properties see P_..
  int getProperties() const { return m_properties; }
  void setProperties(int p) { m_properties = p; }
  void setPropertiesBits(int bits) { m_properties |= bits; }
  void clrPropertiesBits(int bits) { m_properties &= ~bits; }

  /**
   * @brief Define the project to be a multiscanner project or not
   *
   * @param multi  the new state for multiscanner support
   */
  void setMultiScannerSupport(bool multi) { multiScannerSupport = multi; }

  /**
   * @brief Check if the project is a multiscanner project.
   *
   * @return true, if the project supports multiscanner, false otherwise
   */
  bool isMultiScannerProject() const { return multiScannerSupport; }

  /**
   *   Gibt die eingelesene version der Projektdatei zurueck.
   */
  std::string projectFileVersion() const { return version; }

  /// flags used by open with flags
  int openflags() const { return m_openflags; }

  /**
   * @brief Set the version of the project.
   *
   * @param first      Major
   * @param secound    Minor
   * @param third      Typ
   * @param fourth     Path
   */
  static void setCurrentVersion(int first, int secound, int third, int fourth) {
    mLCVersion1 = first;
    mLCVersion2 = secound;
    mLCVersion3 = third;
    mLCVersion4 = fourth;
  }

  //================================================================================================================================
  //=========
  //========= project global items
  //=========
  //================================================================================================================================

  PrjGroups *Groups() { return groups.get(); }
  const PrjGroups* Groups() const { return groups.get(); }

  std::shared_ptr<zfprj::GroupList> GroupList() { return groupList; }
  const std::shared_ptr<zfprj::GroupList> GroupList() const { return groupList; }

  zfprj::Layers *Layers() { return layers.get(); }
  const zfprj::Layers* Layers() const { return layers.get(); }


  //================================================================================================================================
  //=========
  //========= project access items
  //=========
  //================================================================================================================================

  unsigned int N() const; //!< scans in project
  int Size() const { return (int)N(); }

  /**
   *	Anzahl der Scans im Projekt zurueckgeben
   *	\return Nummer aller Scans im Projekt
   */
  int getNrOfScans() const { return (int)N(); }

  /**
   * @brief Get a scan.
   *
   * @param ZFScan     the scan we look for
   * @param allScans   Flag if we want any scan or only scans without status
   * ITEM_DELETED
   *
   * @return the scans or NULL
   */
  ZFProjectItemBase *Find(ZFScan *, bool allItems = false, bool forceLock = true);

  /**
   * @brief Get a scan by its name.
   *
   * @param typ        types of scans we want to have
   * @param filename   the filename of the item
   * @param allScans   Flag if we want any item or only items without status
   * ITEM_DELETED
   *
   * @return the scans or NULL
   */
  ZFProjectItemBase *Find(const char *typ, const std::wstring &filename, bool allItems = false, bool forceLock = true) const;
  
  /**
   * @brief Get a scan by its filename.
   *
   * @param typ        types of scans we want to have
   * @param filename   the filename of the item
   * @param allScans   Flag if we want any item or only items without status
   * ITEM_DELETED
   *
   * @return the scans or NULL
   */
  ZFProjectItemBase *FindExact(const char *typ, const std::wstring &filename,
                               bool allItems = false, bool forceLock = true);

  /**
   * @brief Get a list of scans.
   *
   * @param typ        types of scans we want to have
   * @param allScans   Flag if we want any scan or only scans without status
   * ITEM_DELETED
   *
   * @return the list containing the scan names including the path or an empty
   * list
   */
  scans_list_t GetScansList(const char *typ = ZFProjectItemZFS::getClassName(), bool allScans = false, bool forceLock = true) const;
  
  /**
   * @brief Get a list of scan names.
   *
   * @param typ        types of scans we want to have
   * \param inclMaster => True list includes master and fixed scans.
   *
   * @return the list containing the scans or an empty list
   */
  std::vector<std::wstring>
  GetScanNameList(const char *typ = ZFProjectItemZFS::getClassName(), bool inclMaster = true, bool fullFilename = true, bool forceLock = true) const;


  ZFProjectItemBase *GetFirstScan(const char *type = ZFProjectItemZFS::getClassName());


  /**
  * Set all scans to visible
  */
  void setAllScansVisible(bool visible);
  /**
  * Set all scans to visible
  */
  void setAllScansVisibleTemp(bool visible);

  /**
   * @brief Adds a scan to the project with callbacks.
   *
   * @param filename   the filename of the scan
   * @param nrPoints   number of points of the scan
   * @param res        resolution of the scan
   * @param color      flag, if color images are available
   * @param regMat     Only for point clouds. Initial register matrix.
   *
   * @return true if the scan was added, false otherwise
   */
  bool Add(const std::wstring &filename, int nrPoints, zfs::resolution_t res,
           bool color, const ZFTRMatrix &regMat);

  bool Add(const std::wstring &filename, int nrPoints, zfs::resolution_t res,
           bool color, const ScanPositionNamingOptions &options,
           const ZFTRMatrix &regMat);

  bool Add(const std::wstring& filename, const ZFTRMatrix& regMat );

  /**
   * @brief Adds a scan to the project without calling the callback interfaces
   * registered with the project.
   *
   * @param filename   the filename of the scan
   * @param nrPoints   number of points of the scan
   * @param res        resolution of the scan
   * @param color      flag, if color images are available
   * @param regMat     Only for point clouds. Initial register matrix.
   *
   * @return true if the scan was added, false otherwise
   */
  bool AddNoCB(const std::wstring &filename, int nrPoints,
               zfs::resolution_t res, bool color, const ZFTRMatrix &regMat);

  bool AddNoCB(const std::wstring &filename, int nrPoints,
               zfs::resolution_t res, bool color,
               ScanPositionNamingOptions options,
               const ZFTRMatrix &regMat );

  /**
   * @brief Adds the ZFProjectItemScan object to the project without calling the
   * callback interfaces registered with the project.
   *
   * @param scan   a scan object not included in the project yet. This could
   * have been retrieved from the ScanFactory for instance
   *
   * @return true if the scan was added, false otherwise
   */
  bool AddNoCB(ZFProjectItemScan *scan);

  /**
   * @brief Removes a scan from the project.
   *
   * @param filename   the filename of the scan to remove
   * @param cb         flag, if callback interfaces should be called
   *
   * @return true if the scan was added, false otherwise
   */
  bool Remove(QString& errTxt, const std::wstring& filename, bool cb = true,
              bool markAsDeleted = true, bool allowDeleteMaster = false, bool deleteFiles = false);

  //================================================================================================================================
  //=========
  //========= callback methods (implemented in project_callback.cpp)
  //=========
  //================================================================================================================================

  typedef void (*cbPrjNode_t)(
      ZFProject *prj, zfprj::ZFPrjObjRoot *_objsRoot,
      bool read); // definition of the node callback function

  typedef std::vector<cbPrjNode_t> cbNodelist_t; // typdef of the node callback list

  /**
   * @brief Adding a project callback for ZFPrjObjNode objects if it is not
   * already registered.
   *
   * @param cb the new callback interface
   */
  void installProjectNodeCallback(cbPrjNode_t cb);

  // Available callback actions
  enum {
    PRJ_,
    PRJ_OPEN,
    PRJ_PRESAVE,
    PRJ_PRECLOSE,
    PRJ_UPDATE,
    PRJ_CLOSED,
    PRJ_ADD,
    PRJ_ADD_CREAT,
    PRJ_DEL,
    PRJ_GROUP_ADD,
    PRJ_GROUP_DEL,
    PRJ_GROUP_CHANGED,
    PRJ_OBJ_ADD,
    PRJ_OBJ_DEL,
    PRJ_OBJ_CHANGED,
    PRJ_NODE_ADD,
    PRJ_NODE_DEL,
    PRJ_NODE_CHANGED,
    PRJ_LAYER_ADD,
    PRJ_LAYER_DEL,
    PRJ_LAYER_CHANGED,
    PRJ_REGISTER_CHANGED,    // Registrierung eines VP wurde veraendert
    PRJ_REGISTER_CONNECTION, // Connection geloescht oder hinzugefuegt
    PRJ_REGISTER_CHANGE_MULTI_CONNECTION, // Multi oder Singel connection
    PRJ_REGISTER_MASTER,     // Master VP veraendert
    PRJ_REGISTER_TARGET,     // Targets in den Scans hinzugefuegt oder geloescht
    PRJ_VP_UPDATE,           // Browser update
    PRJ_GLOBALMATRIX,        // Global Matrix wurde geaendert
    PRJ_PARAMETER_CHANGED,   // Projekt-Parameter geaendert
    PRJ_PLAN_CHANGED,        // Projektplan wurde geaendert
    PRJ_SHOW_PROJECTPLAN_PROPERTIES, // Projektplan-Properties anzeigen
    PRJ_FILESYSTEM_FILE_ADDED,       ///< watch filsystem for changes
    PRJ_FILESYSTEM_FILE_REMOVED,     ///< watch filsystem for changes
    PRJ_SCANPOS_NAME_CHANGE,        //!< Scan positon name changed. For update in Scantra
    PRJ_DB_CHANGE,                  //!< Anzeige der datenbank im 3D hat sich geaendert
    PRJ_SLAMTARGETADD               //!< Slamtarget wurde hinzugefuegt. Kann ein neuer Fit eine Annotation oder ein neues Target sein.
  };

  typedef void (*cbPrj_t)(
      ZFProject &, int action,
      const QString &); // definition of the callback function

  typedef std::vector<cbPrj_t> cblist_t; // typdef of the callback interface list

  /**
   * @brief Adding a project callback if it is not already registered.
   *
   * @param cb the new callback interface
   */
  void installProjectCallback(cbPrj_t cb);

  /**
   * @brief Notify all callback listeners.
   *
   * @param project    reference to the affected project
   * @param action     callback action
   */
  void Cb(ZFProject &project, int action, const QString &param = "");

  /**
   * @brief Enable or disable project callbacks.
   *
   * @param state the new enabled state for callbacks
   */
  void EnableCB(bool state);

  /**
   * @brief Activate the projekt callbacks.
   *
   * @param action callback action
   */
  void activateCB(int action) { Cb(*this, action); }

  //================================================================================================================================
  //=========
  //========= Lock
  //=========
  //================================================================================================================================

  //================================================================================================================================
  //=========
  //========= registration methods (implemented in project_registration.cpp)
  //=========
  //================================================================================================================================

  void setGlobalMatrixName(const std::wstring &matrixName = L"");

  const std::wstring &getGlobalMatrixName();

  /**
   * @brief Set the global matrix.
   *        The matrix is set in the project and the XML document of the
   * project.
   *
   * @param globalMatrix the new global matrix
   */
  void SetGlobalMatrix(const ZFTRMatrix &globalMatrix);

  /**
   *   Set the global matrix.
   *   The translation of the matrix and the Z angle is set in the project and
   * the XML document of the project.
   *
   *   \param mat => The new global matrix.
   *   \return Remainder of the X/Y angles.
   */
  ZFTRMatrix SetGlobalMatrixWithoutRotXY(const ZFTRMatrix &mat);

  /**
   * Get the current global matrix.
   *
   * \param name => Name of the coordinate system
   * \return the global matrix.
   */
  ZFTRMatrix GetGlobalMatrixByName(const std::wstring& name);

  /**
   * @brief Get the current global matrix.
   *
   * @return the global matrix.
   */
  ZFTRMatrix GetGlobalMatrix();
  const ZFTRMatrix *GlobalMatrixPtr();

  //void setCorrectionmatrix(const ZFTRMatrix &mat);
  //bool isSetCorrectionMatrix();
  //const ZFTRMatrix *getCorrectionMatrix();

  /**
   * @brief Calculate the distance of the two positions.
   *
   * @param firstMatrix            the first position matrix
   * @param secondMatrix           the second position matrix
   * @param distanceMatrix         reference to distance matrix (out)
   * *
   * @return true, if the distance was calculated successfully, false otherwise
   */
  bool calculateDistance(const ZFTRMatrix *firstMatrix,
                         const ZFTRMatrix *secondMatrix,
                         ZFTRMatrix &distanceMatrix);

  /**
   * @brief Calculate Precision based on GPS values.
   *
   * @param precision reference to the resulting precision (out)
   * @param zfs       the scan name
   */
  void calculateGPSPrecision(RegisterPrecision &precision, const QString &zfs);

  /**
   * @brief Calculate precision based on IMU values.
   *
   * @param precision reference to the resulting precision (out)
   * @param zfs       the scan name
   */
  void calculateIMUPrecision(RegisterPrecision &precision, const QString &zfs);

  /**
   * @brief Write registration into zfs file (project gets updated).
   *
   * @param zfs            the absolut filename of the scan
   * @param mat            the new matrix
   * @param flag           the registration flag
   * @param useCallback    flag, if callbacks should be used
   *
   * @return true, if the matrix was set, false otherwise
   */
  bool registerWriteMatrix(ZFScan *zfs, double *mat, long flag,
                           bool useCallback);

  /**
   * @brief Set the position matrix and the distance matrix of the given scan
   * relative to the worldMatrix (position) and the predecessor scan (distance).
   *
   * @param zfs               the given scan
   * @param set               the positioning mode
   * @param curVP             the current viewpoint
   * @param worldMatrix       the global matrix of the project
   * @param positionMatrix    the position matrix relative to the world matrix
   * @param distanceMatrix    the distance matrix calculated to the predecessor
   * scan.
   * @param serialNumber      serial number of the scanner to run calculations
   * only on the scans of the same scanner
   *
   * @return true, if the values are set, false otherwise
   */
  bool calculateScanPositionAndDistance(
      const QString &zfs, track_t set, const QString &curVP,
      ZFTRMatrix &worldMatrix, ZFTRMatrix &positionMatrix,
      ZFTRMatrix &distanceMatrix, const QString &serialNumber = "",
      double defaultDisplaceX = 1, double defaultDisplaceY = 1);

  /**
   * @brief Iterates from currentViewPoint to firstViewPoint and subtracts the
   * relative positions from the given UTM values for north and east.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param firstViewPoint   the stop point
   * @param currentViewPoint the start point
   * @param north            the changed north value (out)
   * @param east             the changed east value (out)
   * @param serialNumber     serial number of the scanner to run calculations
   * only on the scans of the same scanner
   *
   * @return true, if the calculation finished successfully, false otherwise.
   */
  bool calculateRelativePosition(QString &firstViewPoint,
                                 QString &currentViewPoint, double &north,
                                 double &east,
                                 const QString &serialNumber = "");

  /**
   * @brief Calculate a new world matrix and update all distances.
   *
   * @param save save the changes to the XML document after the distances have
   * been changed
   * @param useCallbacks   Flag, if callbacks should be send after the changes
   * @param force          if true, the global matrix is updated even if it is
   * disabled in the settings
   *
   * @return true, if the distances has been changed successfully, false
   * otherwise
   */
  bool updateScanDistances(bool save, bool useCallbacks, bool force = false);

  /**
   * @brief Calculate the world matrix based on the available GPS coordinates.
   *
   * @param matrix  the calculated matrix (out)
   *
   * @return true, if the matrix was retrieved successfully, false otherwise
   */
  bool getGlobalMatrixFromGPS(ZFTRMatrix &matrix);

  /**
   * @brief Add the GPS matrix based on the available GPS coordinates.
   *
   * @return true, if the matrix was added successfully, false otherwise
   */
  bool addGPSMatrix();

  /**
   * @brief Calculate precision values of a scan.
   *
   * @param zfs       the scan name
   * @param vp        the viewpoint name
   * @param trackMode the current track mode
   * @param precision reference to the resulting precision (out)
   *
   * @return true, if calculation was successful, false otherwise
   */
  bool calculatePrecisionValues(const QString &zfs, const QString &vp,
                                track_t trackMode,
                                RegisterPrecision &precision);

  /**
   * @brief Get some scan values from the scan header.
   *
   * @param zfs        the full scan file name
   * @param rotation   the rotation value from the header
   * @param height     the height value from the header
   * @param upsideDown flag, if the scanner is mounted upside down
   *
   * @return true, if the values where extracted from the header successfully,
   * false otherwise
   */
  bool getSensorValuesForScan(const QString &zfs, ZFVec3d &rotation,
                              double &height, bool &upsideDown);

  /**
   * @brief Get rotation stored in the navigation section of the scan in the
   * project file.
   *
   * @param zfs        the scan name
   * @param rotation   the rotation value stored in the project file
   *
   * @return true, if the values where extracted from the project successfully,
   * false otherwise
   */
  bool getRotationForScan(const QString &zfs, double &rotation);

  static bool getRotationForScanNode(tinyxml2::XMLNode *scanNode,
                                     double &rotation);

  /**
   * @brief Calculates the north direction for the given scan.
   *
   * @param zfs the scan name
   * @param northRotation the resulting rotation (out)
   * @param addMasterRotation flag, if the rotation of the master should be
   * taken into account
   *
   * @return true, if the project has a north rotation value and the value for
   * the scan was calculated
   *
   */
  bool getNorthDirection(const QString &zfs, double &northRotation,
                         bool addMasterRotation, bool useGlobal = true);

  /**
  * @brief Calculates the north direction for the given scan.
  *
  * @param zfs the scan name
  * @param northRotation the resulting rotation (out)
  * @return true, if the project has a north rotation value and the value for
  * the scan was calculated
  *
  */
  bool getScanNorthDirection( const QString &zfs, double &northRotation );

  /**
   * @brief Calculate the distance of the current scan to its predecessor scan.
   * There are several scenarios, each handled in a different manner:
   *
   * 1.) Current scan has no valid GPS coordinate
   * --> The scan is placed at the default distance of 1 meter
   *
   * 2.) Current scan has a valid GPS coordinate
   * 2.1) Scan is to far away from last coordinate
   * --> The scan is placed to the default position and distance
   *
   * 2.2) predecessor scan has no valid GPS coordinate
   * --> The scan is placed using the position matrix of the predecessor scan
   *
   * 2.2.1) the distance based on the position matrix is to far away
   * --> Set default position and distance to the scan.
   *
   *
   * If indoor tracking is used, these values are already precalculated in the
   * scan header. If outdoor tracking is used, the distance values are written
   * to the scan header.
   *
   * @param zfsLast                    the file name of the last scan
   * @param zfsCurrent                 the filename of the current scan
   * @param previousPositionMatrix     position matrix of the predecessor scan
   * @param positionMatrix             position matrix of the current scan
   * @param distanceMatrix             reference to empty distance matrix (out)
   * *
   * @return true, if the distance was calculated successfully, false otherwise
   */
  bool calculateGPSDistance(const QString &zfsLast, const QString &zfsCurrent,
                            const ZFTRMatrix &previousPositionMatrix,
                            const ZFTRMatrix &positionMatrix,
                            ZFTRMatrix &distanceMatrix);
  bool calculateGPSDistanceFromHeader(const QString &zfsLast,
                                      const QString &zfsCurrent,
                                      const ZFTRMatrix &previousPositionMatrix,
                                      const ZFTRMatrix &positionMatrix,
                                      ZFTRMatrix &distanceMatrix);

  /**
   * @brief Adjust the given position to the offset of the scan defining the
   * world matrix to its current position. This handles situations, where the
   * scan which originally sets the world matrix has been moved to another
   * position by registration or the user.
   *
   * @param worldMatrix       the current world matrix
   * @param positionMatrix    the position matrix to adjust (out)
   *
   * @return true, if the offset was added to the position matrix.
   */
  bool adjustWorldMatrixOffset(const ZFTRMatrix &worldMatrix,
                               ZFTRMatrix &positionMatrix);

  /**
   * @brief Get the coordinate values from the scan header.
   *
   * @param zfs               the file name of the scan
   * @param dx                realtive movement on the x-axis from the scan
   * header (out)
   * @param dy                realtive movement on the y-axis from the scan
   * header (out)
   * @param dz                realtive movement on the z-axis from the scan
   * header (out)
   *
   * @return true, if the movement was retrieved, false otherwise
   */
  bool getRelativeMovement(const QString &zfs, double &dx, double &dy, double &dz);

  /**
   * @brief Get the coordinate values from the scan header.
   *
   * @param zfs               the file name of the scan
   * @param coord             coordinate from the scan header (out)
   *
   * @return true, if the coordinate was found, false otherwise
   */
  bool getGPSCoordinateForScanFromHeader(const QString &zfs, zfs::math::gps::GPSCoordinate &coord);

  /**
   * @brief Get the coordinate values for the scan from the navigation node.
   *
   * @param zfs               the file name of the scan
   * @param coord             coordinate from the scan header (out)
   *
   * @return true, if the coordinate was found, false otherwise
   */
  bool getGPSCoordinateForScan(const QString &zfs, zfs::math::gps::GPSCoordinate &coord);

  /**
   * @brief Get the best coordinate for this project. This is calculated by
   * taking the HDOP values and the satelites count into account.
   *
   * @param scanName          the file name of the scan with the best coordinate
   * (out)
   * @param coord             best coordinate of the project (out)
   * @param lastScanAllowed   flag, if the last taken scan should be used or not
   * @param serialNumber      only check scans of the scanner with the given
   * serial number
   *
   * @return true, if a coordinate was found, false otherwise
   */
  bool getBestGPSCoordinate(QString &scanName,
                            zfs::math::gps::GPSCoordinate &coord,
                            bool lastScanAllowed = true,
                            const QString &serialNumber = "");

  /**
   * @brief Check the coordinate if it is valid. Satelite count and HDOP value
   * is taken into account.
   *
   * @param coord             coordinate to check
   *
   * @return true, if a coordinate is valid, false otherwise
   */
  bool useCoordinate(const zfs::math::gps::GPSCoordinate &coord);

  void setGPSLimits(const zfs::math::gps::GPSCoordinate& limits);

  /**
   * @brief Set the flag, if the coordinate of the given scan should be used or
   * not
   *
   * @param zfs    the name of the scan
   * @param use    the flag if the coordinate should be used or not
   *
   * @return true, if the flag was set, false otherwise
   */
  bool setGPSCoordinateUseFlag(const QString &zfs, bool use);

  /**
   * @brief Set the position matrix and the distance matrix to the last scan to
   * the given viewpoint.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param vp                the viewpoint to be changed
   * @param precision         the precision for the preregistration matrix
   * @param positionMatrix    the new position matrix of the viewpoint
   * @param distanceMatrix    the new distance matrix of the viewpoint
   *
   * @return true, if the position and the distance was set, false otherwise
   */
  bool changeRegisterMatrix(const QString &vp,
                            const RegisterPrecision &precision,
                            const ZFTRMatrix &positionMatrix,
                            const ZFTRMatrix &distanceMatrix);

  /**
   * @brief Looks up the given viewpoint and set the matrices.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param viewpoint             the viewpoint to look up
   * @param registerMatrix        the registration matrix of the VP (out)
   * @param distanceMatrix        the distance matrix of the VP (out)
   *
   * @return true, if the lookup succeeded, false otherwise
   */
  bool getPositionOfViewpoint(QString &viewpoint, ZFTRMatrix &registerMatrix,
                              ZFTRMatrix &distanceMatrix);

  static bool getPositionOfViewpoint(tinyxml2::XMLElement *root,
                                     QString &viewpoint,
                                     ZFTRMatrix &registerMatrix,
                                     ZFTRMatrix &distanceMatrix);

  //================================================================================================================================
  //=========
  //========= Tools
  //=========
  //================================================================================================================================

  //=======================================================================================================================
  // Logging stuff based on project (implemented in Project.cpp)
  //=======================================================================================================================
  enum loglevel_t { LOG_OFF, LOG_USER, LOG_DEBUG };
  void EnableLogging(bool en=true);
  void SetLogging(loglevel_t loglevel);
  void Log(loglevel_t level, const QString &s);
  void Log(const QString &s) { Log(LOG_USER, s); }
  std::wstring &LogFile() { return sLogfile; }

  /**
   * @brief Check if the given version is higher than the version of the
   * project.
   *
   * @param first      Major
   * @param secound    Minor
   * @param third      Typ
   * @param fourth     Path (or -1 to ignore)
   *
   * @return true if the current project has a higher version number than the
   * given one
   */
  bool prjVersionBiggerThan(int first, int secound, int third, int fourth) const;

  /**
   *   Target und Register eintraege im Projekt aus den Scans aktualisieren.
   */
  void updateScansInProjektFile();

  /**
   * @deprecated, use ProjectCombiner instead
   */
  bool combineProjFiles(ZFProject &proj,
                        const std::wstring &newFilName =
                            L""); //!< combine proj with this project file

  /**
   * @deprecated, use ProjectCombiner instead
   */
  bool combineProjFiles(
      const std::wstring &prjName,
      const std::wstring
          &newFilName); //!< opens the project and combine is with this projekt

  /**
   * Erzeuge einen relativen dateiname zum Project.
   * Dateien/Scans koennen auch in Unterverzeichnissen liegen.
   */
  std::wstring makeRelFile(const std::wstring &fullFileName) const;
  QString makeRelFile(const QString &fullFileName) const;

  //================================================================================================================================
  //=========
  //========= Misc
  //=========
  //================================================================================================================================

  void setResiver(QObject *resiver) { mResiver = resiver; }

  int UpdateJpg(
      const std::wstring &zfsfilename,
      const std::wstring &scanName = L"",
      int mapmode = 1); /// update images assigned to scan to projectfile as well.
                    /// return number of images added to project
  void Repaint();   // calls all callbacks
  void updateScanObject(const std::wstring &scan);

  //=======================================================================================================================
  // methods to access the objects of a project (implemented in Project.cpp)
  //=======================================================================================================================

  bool dellObjRec(zfprj::ZFPrjObj *obj);
  bool dellObjRec(const std::wstring &name);
  std::shared_ptr<zfprj::ZFPrjObjRootReadWrite> ObjsRoot() {
    return std::shared_ptr<zfprj::ZFPrjObjRootReadWrite>(
        new zfprj::ZFPrjObjRootReadWrite(objsRoot.get()));
  }
  std::shared_ptr<zfprj::ZFPrjObjNodeRead> ObjsRootRead() {
    return std::shared_ptr<zfprj::ZFPrjObjNodeRead>(
        new zfprj::ZFPrjObjNodeRead(objsRoot.get()));
  }
  std::shared_ptr<zfprj::ZFPrjObjReadWrite>
  GetObjectItem(const std::wstring &name);

  std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> GetObjLock(const char *typ);
  std::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> TryObjLock(const char *typ);
  /**
  \brief Use this version of GetObjectLock to avoid repetitive downcasts to the
  requested container type. T must be a subtype of zfprj::ZFPrjObjNode and must
  be explicitly stated in the instantiation.
  */
  template <typename T>
  zfprj::WritableLockedProjectObjects_Ptr<T> GetWriteableLockedObjects();
  /*
  \brief The returned pointer will be empty if the objects are already locked.
  */
  template <typename T>
  zfprj::WritableLockedProjectObjects_Ptr<T> TryGetWriteableLockedObjects();

  std::shared_ptr<zfprj::ZFPrjObjNodeRead> GetObjRead(const char *typ);
  std::shared_ptr<zfprj::ZFPrjObjNodeRead> TryGetObjRead(const char *typ);
  zfprj::ZFPrjObjNode *GetObj(const char *typ);
  /**
  \brief Works like GetWriteableLockedObjects() but the objects can not be
  altered.
  */
  template <typename T>
  zfprj::ReadableLockedProjectObjects_Ptr<T> GetReadableLockedObjects();
  /**
  \brief The returned pointer will be empty if the objects are already locked.
  */
  template <typename T>
  zfprj::ReadableLockedProjectObjects_Ptr<T> TryGetReadableLockedObjects();

  /**
   * @brief Returns the master scan or nullptr, if no master could be found
   *
   * @return the master scan or nullptr
   */
  ZFProjectItemScan *GetMasterScan();

  int GetScanNr(ZFProjectItemScan *scan) const;

  /**
   * @brief The objects of the given type are reloaded from the XML document.
   *
   * @param type   the name of the class typ to reload
   *
   * @return true, if the objects are reloaded, false otherwise
   */
  bool loadObjectsFromDocument(const char *type);

  /**
   * @brief The objects of the given type are written to the XM document of the
   * project. The XML document is not saved at this point!
   *
   * @param type  the name of the class typ to stored
   *
   * @return true, if the objects are stored, false otherwise
   */
  bool storeObjectsToDocument(const char *type);

  void setDimensions(double centerX, double centerY, double centerZ, double w,
                     double h);
  double getCenterX() const { return mPrjCenterX; }
  double getCenterY() const { return mPrjCenterY; }
  double getCenterZ() const { return mPrjCenterZ; }
  double getCenterWidth() const { return mPrjWidth; }
  double getCenterHeight() const { return mPrjHeight; }

  void setShowAllVP(bool showAll) { mShowAllVP = showAll; }
  bool getShowAllVP() { return mShowAllVP; }

  /**
   *   Auswahl aller Scanpositionen die Visible sind. Nicht in nicht angezeigten
   * Gruppen.
   */
  void selectAllVisbleScanPositions();


  /**
  * Gibt die aktuell ausgewaelte Scanposition zurueck. Wenn mehr als einer ausgewaelt ist
  * wird keiner zurueckgegeben. 
  */

  std::wstring currentSelectedScan();

  //=======================================================================================================================
  // IO methods (implemented in project_io.cpp)
  //=======================================================================================================================

  enum ExportFormat { /*EXPORT_ASC,*/ EXPORT_DXF, /*EXPORT_VRML,*/ _EXPORT_ };

  /**
   * @brief Exports all project objects to a file with format.
   *
   * @param exportfilename the filename of the export
   * @param format         the format to export
   *
   * @return true if the file was exported, false otherwise
   */
  bool Export(const std::wstring &exportfilename, 
      const ZFTRMatrix& worldMatrix, ExportFormat format);

  std::wstring GetExportExt(ExportFormat format);
  const char *GetExportTxt(ExportFormat format);

  /**
   * @brief Initialise the project from an  tinyxml2::XMLDocument.
   * This is mainly usefull to init test projects from XML-Strings.
   *
   * @param doc the  tinyxml2::XMLDocument to use (ownership is taken)
   */
  bool initFromXMLDocument(tinyxml2::XMLDocument *doc, ZFProject::eProjectError& err );

  /**
   * @brief Stores the XML-Document of the project with mutex lock.
   *
   * @param filename   the filename to use
   *
   * @return true, if the project was saved, false otherwise
   */
  virtual bool SaveXMLDocument(const wchar_t *filename);
  virtual bool SaveXMLDocument(const char *filename);

  //=======================================================================================================================

  /**
   * @brief Get the project root node.
   *
   * @return the root node of the XML document
   */
  tinyxml2::XMLNode *getBaseNode() { return mNodeBase; }
  const tinyxml2::XMLNode *getBaseNode() const { return mNodeBase; }

  /**
   * @brief Get document for reading other values, need zftools library with xml
   * parser.
   *
   * REMARK: The document should not be manipulated directly. Use the helper
   * function of the project to be thread safe!
   *
   * @return the XML document of the project.
   */
  tinyxml2::XMLDocument *getXMLDocument() { return doc.get(); }
  const tinyxml2::XMLDocument *getXMLDocument() const { return doc.get(); }

  zfs::BoundingBox3D CalcProjectArea();

  /**
   *   Aktuelle eingestelle Zeitzohnen diffenenz vom Comupter ermitteln.
   *   \return Zeitdiffenenz. UTC + return = Computerzeit
   */
  int getCurrentTimeZone();

  /**
   * @brief Returns the name of the first viewpoint of the project. The first
   * viewpoint is the VP of the first zfs scan.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param serialNumber   if serial number is set the first viewpoint created
   * by this scanner is returned
   *
   * @return the name of the first viewpoint of the project
   */
  QString getFirstViewpoint(const QString &serialNumber = "");

  /**
   * @brief Returns the name of the last viewpoint of the project. The last
   * viewpoint is the VP of the last zfs scan.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param serialNumber   if serial number is set the last viewpoint created by
   * this scanner is returned
   *
   * @return the name of the last viewpoint of the project
   */
  QString getLastViewpoint(const QString &serialNumber = "");

  /**
   * @brief Returns the name of the last scan of the project.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param serialNumber   if serial number is set the last scan created by this
   * scanner is returned
   *
   * @param parentScansOnly do not list selection scans
   *
   * @return the name of the last scan of the project
   */
  QString getLastScan(const QString &serialNumber = "",
                      bool parentScanOnly = false);

  /**
   * @brief Returns the name of the viewpoint of the given scan.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @return the name of the viewpoint of the scan
   */
  QString getViewpointForScan(const QString &scanName);

  /**
   * @brief Returns a list containing the scan names of the viewpoint from the
   * project file.
   *
   * @return the list with the scan names
   */
  QStringList getScans4Viewpoint(const QString &vp);

  static QStringList getScansForViewpoint(tinyxml2::XMLElement *root,
                                          const QString &vp);

  /**
   * @brief Looks up the scan node for the given scan name. If the scan node is
   * not found, it is created
   *
   * @param zfs the file name of the scan
   *
   * @return the scan node
   */
  tinyxml2::XMLNode *getScanNode(const std::wstring &zfs,
                                 bool createMissing = false,
                                 bool allItems = false,
                                 bool forceLock = true) const;

  static tinyxml2::XMLNode *getScanNodeFromRoot(tinyxml2::XMLNode *node,
                                                std::wstring &scanName,
                                                bool createMissing);

  /**
   * @brief Checks, if the targets for a scan has been searched or not.
   *
   * @param zfs the file name of the scan
   *
   * @return true, if the targets has not been searched before
   */
  bool needsTargetSearch(const std::wstring &zfs);

  /**
   * @brief Insert the child node into the parent node. This is protected with a
   * mutex lock (thread safe)
   *
   * @param parent the parent node
   * @param child the new child node
   */
  void insertNode(tinyxml2::XMLNode *parent, tinyxml2::XMLNode *child);

  /**
   * @brief Get the predecessor scan of the given viewpoint.
   *
   * This method runs on the XML document. Make sure, all changes of the project
   * are stored in the project before calling this!
   *
   * @param currentScan       the name of the current scan (out)
   * @param predecessorScan   the name of the predecessor scan (out)
   * @param currentVP         the current viewpoint name (in)
   * @param predecessorVP     the predecessor viewpoint name (out)
   * @param serialNumber      the serial number of the scan which hat taken the
   * scan
   *
   * @return true, if the check succeeded, false otherwise. The return value of
   * predecessorScan is empty, if the scan has no predecessor.
   */
  bool getPredecessorScan(QString &currentScan, QString &predecessorScan,
                          const QString &currentVP, QString &predecessorVP,
                          const QString &serialNumber = "");

  static bool getPredecessorScanFromRoot(tinyxml2::XMLElement *root,
                                         const QString &currentVP,
                                         QString &currentScan,
                                         const QString &serialNumber,
                                         QString &predecessorVP,
                                         QString &predecessorScan);

  /**
   * @brief Read out the serial number of a scan
   *
   * @param scanName  the name of the scan
   *
   * @return the serial number of the scan
   */
  QString getSerialNumber(const QString &scanName);

  //=======================================================================================================================
  // helper methods (implemented in project_util.cpp)
  //=======================================================================================================================
  
  int lastColorIndex = 0;
  zfs::ColorTable lut;

  void resetViewposColor();
  QColor generateViewposColor();

  /**
   * @brief Set the text value for the given node to the matrix. The matrix is
   * transformed into a space separated list of 16 double values.
   *
   * @param node      the node to change
   * @param matrix    the matrix to set
   */
  void setMatrix(tinyxml2::XMLNode &node, const ZFTRMatrix &matrix);

  /**
   * @brief Read matrix
   *
   * @param node      the parent node
   * @param name      the name of the matrix
   * @param matrix    the resulting matrix as double array
   *
   * @return true if everything is OK, otherwise false
   */
  static bool readMatrix(tinyxml2::XMLNode *node, const char *name,
                         double *matix);

  void writeGlobalMatrixToHeader(const ZFTRMatrix &globalMatrix);

  /**
   * @brief Get the UUID of the given project without opening a ZFProject.
   *
   * @param projectName   the full name of the XML document
   *
   * @return the uuid or an empty string if the project is not found
   */
  static QString getProjectUUID(const QString &projectName);

  //=======================================================================================================================

  // Background image
  bool hasFloorplan();

  void initBackgroundImageFromFloorplan(BackgroundImageParameter &param);
  const BackgroundImageParameter &getBackgroundImageParameter() const;
  void setBackgroundImageParameter(BackgroundImageParameter &param);
  const QString getPlaneParamGBImpName(bool addPath = true) const;
  void setBackgroundImageParameterWarning(bool w);
  bool useBackgroundImageParameterWarning();

  void setPlaneParamGBImpName(const QString &name);
  void setPlaneParamBGImgPosX(double x) { backgroundImageParam.imageX = x; }
  void setPlaneParamBGImgPosY(double y) { backgroundImageParam.imageY = y; }
  void setPlaneParamBGImgPosZ(double z) { backgroundImageParam.imageZ = z; }
  void setPlaneParamBGImgScaleX(double x) { backgroundImageParam.scaleX = x; }
  void setPlaneParamBGImgScaleY(double y) { backgroundImageParam.scaleY = y; }
  void setPlaneParamBGImgRot(double rot) {
    backgroundImageParam.rotation = rot;
  }
  void setPlaneParamOpacity(double op) { backgroundImageParam.opacity = op; }

  // Grid
  void setGridX(double x) { mGridX = x; }
  void setGridY(double y) { mGridY = y; }
  void setGridZ(double z) { mGridZ = z; }
  void setGridRotationX(double x) { mGridRX = x; }
  void setGridRotationY(double y) { mGridRY = y; }
  void setGridRotationZ(double z) { mGridRZ = z; }
  void setGridStep(double step) { mGridStep = step; }
  void setGridN(double n) { mGridN = n; }
  void drawGrid(bool draw) { mGridDraw = draw; }

  double getGridX() const { return mGridX; }
  double getGridY() const { return mGridY; }
  double getGridZ() const { return mGridZ; }
  double getGridRotationX() const { return mGridRX; }
  double getGridRotationY() const { return mGridRY; }
  double getGridRotationZ() const { return mGridRZ; }
  double getGridStep() const { return mGridStep; }
  double getGridN() const { return mGridN; }
  bool isGridDrawn() const { return mGridDraw; }

  //=======================================================================================================================

  /**
  @brief Add a file system watcher to the project for tracking creation, rename
  and removing files
  */
  void addFilesystemWatcher(zfprj::ZFProjectWatcher &);
  void removeFilesystemWatcher();

  //#####################################################################################################################################
  //#####################################################################################################################################
  //#####################################################################################################################################

protected:
  void saveGridParam(tinyxml2::XMLElement *viewNode);
  void readGridParam(tinyxml2::XMLElement *viewNode);
  void checkXMLNode(tinyxml2::XMLNode *node, tinyxml2::XMLNode *check);

  void _init(); ///< internal class init

  /**
   * @brief Initialise all object lists of the project.
   */
  void createEmptyObjs();

  //#####################################################################################################################################
  //#####################################################################################################################################
  //#####################################################################################################################################

public:
  QObject *mResiver{}; // Empfaenger => Application z.B. zum ausgeben von Texten

  QReadWriteLock mRWMutex; // synchronisation mutex for project access

  mutable std::timed_mutex mutPrjModify;

#define PrjModifyLock std::lock_guard<std::timed_mutex> lock(mutPrjModify);

  // std::string unknown;        // unknown part of xml file
  bool isOpening{};             // flag, if project is currently opened
  std::wstring projectUUID;   // unique UUID of the project
  std::wstring m_performance; // some info abot performance on loading project
                              // for example

  zfs::BoundingBox3D CalcProjectArea( bool global );

  // get current coordinate system or create a empty one if no exist
  std::shared_ptr<zfprj::CoordinateSystems> getCoordinateSystems();
  
#ifdef ZFPRJ_ZFDB
  // return the class for handling the zfdb stuff
  std::shared_ptr<zfprj::ProjectZfdb> getZfdb();
#endif

  bool mUpdateScans{}; // Flag ob die Scans beim einlesen des Projektes auf
                     // registrierung gecheckt werden

protected:
#ifdef ZFPRJ_ZFDB
  std::shared_ptr<zfprj::ProjectZfdb> zfdb;
#endif
  std::shared_ptr<zfprj::CoordinateSystems> coordinateSystems;

  int m_properties{}; ///< properties see prop_t_

  //bool bcorrectionMatrixIsSet; // Flag ob die Korrekturmatrix bereits gesetzt
                               // wurde.
  //ZFTRMatrix correctionMatrix; // matrix zum korrigieren zu grosser koordinaten
                               // (vorderer teil wird abgeschnitten)

  // current LC version
  static int mLCVersion1;
  static int mLCVersion2;
  static int mLCVersion3;
  static int mLCVersion4;
  int mPrjVersion1{-1};
  int mPrjVersion2{-1};
  int mPrjVersion3{-1};
  int mPrjVersion4{-1};

  // Dimensions
  double mPrjCenterX{}; // center of project X
  double mPrjCenterY{}; // center of project Y
  double mPrjCenterZ{}; // center of project Z
  double mPrjWidth{}; // Breite des Projektes X (halbe Breite +/- vom Mittelpunkt aus)
  double mPrjHeight{}; // Hoehe des Projektes X (halbe Hoehe +/- vom Mittelpunkt aus)

  bool mLockOpenProject{}; // flag if the project is locked on open
  std::unique_ptr<tinyxml2::XMLDocument> doc; // XML Document (NULL if other format)
  std::string version;        // version Dof Project file
  zf::FileName mFilename; // project filename including path
  std::wstring path;          // project path
  std::wstring fileNameShort; // name of the project without path and extension

  bool m_openedReadonly{}; // flag, if the project was opened in read only mode
                         // (i.e. if it was saved with a newer version of LRC)
  bool m_writeProtected{}; // flag, if project or folder is write protected
  bool m_check4WriteProtect{}; // do check for write protection only once
  int m_openflags{};           ///< flags used by Open() if open with flags

  std::wstring projectName; // name of the project (from Project-Node of the XML-File)
  itemlist_t itemlist; // List of all scans in project
  std::wstring error;  // open project error

  tinyxml2::XMLNode *mNodeBase{};

  std::unique_ptr<PrjGroups> groups; // groups of messurements etc.

  zfprj::project_group_list_t
      groupList; // list of objekt groups, containing e.g. Viewpoints

  std::unique_ptr<zfprj::ZFPrjObjRoot>
      objsRoot; // objects like planes, layers, targets, regiserdatabase,...
  std::unique_ptr<zfprj::Layers> layers;

#ifdef ZFPRJ_FILEEXPORT
  std::unique_ptr<zfprj::ProjectObjectsToFileExporter>
      objectsToFileExporter; ///< Handles the export of the project objects to
                             ///< other formats.
#endif

  bool enableCb{};   // flag of the callbacks state (enabled/disabled)
  bool mShowAllVP{}; // Flag ob, nicht registrierte, hinzugefuegte VP's
                   // verschobenwerden sollen um alle anzeigen zu koennen
  bool multiScannerSupport{}; // flag if the project has multi scanner support

  BackgroundImageParameter backgroundImageParam;

  double mGridX{};    // X-Value fuer den Mittelpunkt des Grids
  double mGridY{};    // Y-Value fuer den Mittelpunkt des Grids
  double mGridZ{};    // Z-Value fuer den Mittelpunkt des Grids
  double mGridRX{};   // Drehung fuer das Grid
  double mGridRY{};   // Drehung fuer das Grid
  double mGridRZ{};   // Drehung fuer das Grid
  double mGridStep{1.}; // Grid
  double mGridN{99.};    // Grid
  bool mGridDraw{};   // Flag ob das Grid dargestellt werden soll.


#ifdef LINUX
  int fileHandle{-1};
#endif

  cblist_t cblist;        // callback list
  cbNodelist_t objNodeCB; // node callback list

  int m_indention{}; ///< Logausgabe einruecken
  loglevel_t loggingLevel{LOG_OFF};
  std::wstring sLogfile;

  /// watch for file changes (experimental)
  zfprj::ZFProjectWatcher* watcher{};
};

//###################################################################################################
//###################################################################################################
//###################################################################################################

template <typename T>
zfprj::WritableLockedProjectObjects_Ptr<T>
ZFProject::GetWriteableLockedObjects() {
  return zfprj::WritableLockedProjectObjects_Ptr<T>(
      GetObjLock(T::getClassName()));
}

template <typename T>
zfprj::WritableLockedProjectObjects_Ptr<T>
ZFProject::TryGetWriteableLockedObjects() {
  return zfprj::WritableLockedProjectObjects_Ptr<T>(
      TryObjLock(T::getClassName()));
}

template <typename T>
zfprj::ReadableLockedProjectObjects_Ptr<T>
ZFProject::GetReadableLockedObjects() {
  return zfprj::ReadableLockedProjectObjects_Ptr<T>(
      GetObjRead(T::getClassName()));
}

template <typename T>
zfprj::ReadableLockedProjectObjects_Ptr<T>
ZFProject::TryGetReadableLockedObjects() {
  return zfprj::ReadableLockedProjectObjects_Ptr<T>(
      TryGetObjRead(T::getClassName()));
}

//###################################################################################################
//###################################################################################################
//###################################################################################################


/// current project, maybe closed. Just only one project supported. This is the
/// project opend in lasercontrol
ZFSPRJ_API ZFProject &CurrentProject();

/// get a shared pointer to the current project
ZFSPRJ_API std::shared_ptr<ZFProject> CurrentProject_ptr();


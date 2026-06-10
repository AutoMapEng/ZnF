#pragma once

#include "zfsapi.h"
#include "zfserr.h"

#include <tuple>
#include <memory>
#include <list>
#include <vector>
#include <utility>
#include <math.h>

#ifdef _WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif


#include <zfcore/filename.h>
#include "Math/zfvec.h"
#include "clone.h"
#include "flags.h"
#include "mask.h"
#include "pixel.h"
#include "rf.h"
#include "tol.h"
#include "trmatrix.h"
#include "way_defines.h"
#include "world.h"
#include "zfbase.h"
#include "zfdef.h"
#include "zfsdefs.h"
#include "zfserr.h"


class ZFScan;
class ZFScanList;
class ZFProject;
class ZFScanBasic;
class ZFSHeader;
class ZF2DSelections;
class ZFRect;
class ZFRfComp;
class ZFSFilter;
class CxImage;
class CUnahe;
class ZFMechCalib;
class ZFScanZFS;
class ZFScanMap;

namespace zfs
{
    class WayCounting;
    class ZFLock;
    class Compensator;
    class ZFPixelAccess;
    class AtmosphereCorrection;
    class RawParams;
    struct LevelSystem;
}
namespace std
{
	class mutex;
}

#define ERR_ZFS_OK			0
#define ERR_ZFS				1	// undefined error
#define	ERR_ZFS_UNKBNOWNTYP 2	// unnown/not implemented typ, please update software
#define ERR_ZFS_MEMORY		3	// out of memory
#define ERR_ZFS_ZFSHEADER	4	// problem reading zfs hedaer
#define ERR_ZFS_JPG			5	// error reading color


//================================================================================================================


#define ZFSLOCK_FILE_EXT L".lock.inf"

/**
*	\brief Abstact class for scanfile.
*
*   Basic access to scanfile
*/
class ZFS_API ZFScanBasic : public zfs::ZFWorld
{
    DEFINE_ZFBASE(zfs::ZFWorld)

    friend class ZFScanList;
    friend class ZFProject;
    friend class zfs::WayCounting;
    friend ZFScan* zfs::ZFSClone(ZFScan*, bool);
    friend class zfs::Compensator;

protected:
    bool bClone{};					//!< true if object is just a clone
    zfs::ZFLock* pLockParent{};		// the locking for the parent scan

    zfs::ZFPixelAccess* a{};	 //!< holds current pixel

//    std::shared_ptr<ZFScanMap> mapView{}; ///< used for viewing
    std::shared_ptr<ZFScanMap> mapData{}; ///< used for accessing mask,jpg,...

    int levelMatrixStatus{};
    ZFTRMatrix  levelMatrix;

    zf::FileName fileName;		//!< Name of the scanfile
    zf::FileName fileNameShort;	//!< Name of the scanfile without path

    static std::wstring path4Calibfile;

    int  minRf{};				//!< Minimum intensity
    int  maxRf{};				//!< Maximum intensity (overloaded pixel has more)
    int zfstyp{};				//!< Datatyp
    int  openflags{};         //!< Define how to open scan
    bool isOpen{};            //!< True if file is ok.
    bool writeProtected{};    //!< If file comes from cd it is witeprotected.
    int nSubsample{1};         //!< Subsample scan while reading on 0=off.
    ZFRect* sel{};            //!< Selection if only partial is loaded.
    bool dirty{};             //!< flag will be set when scan has been changed

    bool hasRf{};
    bool hasRg{};
    bool hasRGB{};

    int pixel{};		//!< Number of pixels in view.
    int lines{};		//!< Number of lines in view.
    int pixel360{};	//!< Number of pixels for 360°.
    int lines360{};	//!< Number of lines for 360° (not allways used i.e. profiler).

    double maxrange{};	      //!< Maximum range of scan.
    double minrange{};	      //!< Minimum range of scan.
    double maxviewstartangle{}; //!< Maximum view start angle of scan.

    double rgadd{};       //!< Additional range offset added by every Rgm() call
    double offsetH{};     //!< High speed motor offset in rad.
    double offsetL{};     //!< Low speed motor offset in rad.
    double offsetM{};     //!< Range offset in m.
    double scaleM{};      //!< Scale range default:1.0.

    int lockCounter{};    //!< Lock if backgroundtasks are running on file.

    std::string info;   //!< info about calib, ...
    static std::string error;  //!< 3rrors on open scan.

    std::shared_ptr<zfs::AtmosphereCorrection> atmosphere{}; ///< atmosphere correction
    std::shared_ptr<zfs::RawParams> straylight{};

public:
    std::shared_ptr<zfs::LevelSystem> m_stehachse{};
    zfs::Compensator* compensator{};
    zfs::zfs_flags_t  flags{};             //!< Flags see flags.h zfs::zfs_flags_t
    zfs::TOF tol;						///< time of light correction

protected:
    void SetFileName(const std::wstring& name);

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const wchar_t* param = NULL, ZFRect* r = NULL) = 0;
    virtual void CloseFile() { ; }

public:
    ZFScanBasic();
    virtual ~ZFScanBasic();

    virtual bool allok();
    virtual const char* FileTyp() const = 0;
    virtual const char* DataTyp() const = 0;

    int ZFSTyp() const { return zfstyp; }
    bool IsClone() const { return bClone; }


    /** Check scanname is name */
    bool isScan(const std::wstring& name) const
    {
        if (fileName == name)
            return true;
        return false;
    }

    const char* FileNameC() const { return fileName.c_str(); }
    const char* FileNameShortC() const { return fileNameShort.c_str(); }
    const wchar_t* FileNameW() const { return fileName.w_str(); }
    const wchar_t* FileNameShortW() const { return fileNameShort.w_str(); }

    static void SetPath4Calibfile(const wchar_t* path);
    static void SetPath4Calibfile(const char* path);

    // File
    /**
     * Open file with subsample (if file is very big, you can load it with less memory)
     * \param s = subsample value
     */
//    void setSubsample(int s) { nSubsample = s; }
//    int subsample() const { return nSubsample; }
    zfs::zfserr_t Open(const std::wstring&, const wchar_t* param = NULL, ZFRect* r = NULL, int openflags = OPEN_DEFAULTFLAGS_ZFSCAN);
    zfs::zfserr_t Open(const std::wstring&, const char* param, ZFRect* r = NULL, int openflags = OPEN_DEFAULTFLAGS_ZFSCAN);
    zfs::zfserr_t Open(const std::string&, const char* param = NULL, ZFRect* r = NULL, int openflags = OPEN_DEFAULTFLAGS_ZFSCAN);
    zfs::zfserr_t Open(ZFScanBasic*, ZFRect&);
    void Close(bool useCallback = true);
    bool IsOpen() const { return isOpen; }
    virtual zfs::zfserr_t OnOpen() { return zfs::ERR_OK; }
    virtual void OnPreClose() { ; }
    virtual bool IsWriteProtected() const { return writeProtected; }
    virtual bool OpenHeaderOnly(const wchar_t*) { return false; }
    bool IsDirty() const { return dirty; } ///< return true if scan has been modified

    const char* Info() const { return info.c_str(); }
    static const char* Error() { return error.c_str(); }
    void AddInfo(const char* inf);
    static void AddError(const char* inf);
    static void ClearError() { error = ""; }

    /** Lock counter. Use to lock file while background process is in use.*/
    void IncLock() { lockCounter++; }
    /** Unlock counter. See also IncLock().*/
    void DecLock() { if (lockCounter) lockCounter--; }
    /** Returns the lockCounter */
    int LockCounter() const { return lockCounter; }

    /** Get access to zfs header if availoable. */
    ZFSHeader* GetZFSHeader() { return GetZFSHeader__(); }
    const ZFSHeader* GetZFSHeader() const { return GetZFSHeader__(); }

    virtual bool WriteHeader();
    virtual bool HasHeader() const { return true; }


    // Basic properties of scan
    int Pixel() const { return pixel; }   // number of pixel in gray-image
    int Lines() const { return lines; }   // number of lines in gray-image
    int Pixel360() const { return pixel360; } //!< Number of pixels for whole 360 deg.
    int Lines360() const { return lines360; }

//    virtual bool extension(double& /*starth*/, double& /*startv*/, double& /*h*/, double& /*v*/) { return false; }

    virtual bool HasIntens() const { return hasRf; }
    virtual bool HasRGB() const { return hasRGB; }
    virtual bool HasRange() const { return hasRg; }
    virtual bool HasAngle() const { return false; }
    virtual bool HasXYZ() const { return false; }

    virtual int  HasCalibration() const { return 0; } //!< 0 = no, otherwise typ of calibration
    virtual bool IsCalibrated() const { return false; } //!< Check if calibration is on.
    virtual void InitCalib(int UNUSED(newtyp) = 0) { ; }

    // point to pixel
    zfs::ZFPixelAccess* PixAcc() { return a; }
    virtual bool Set(int pix, int lin);   // pano-image (gray-view) coordinates
    void Set(int lin) { Set(0, lin); }
    int CurrentPixel() const;
    int CurrentLine() const;
    virtual char* AdrLine(int /*lin*/) { return NULL; } //!< Adress of line (!Maybe header as first


    //
    // Mapping
    //
    ZFScanMap& MapObject() const; ///< return view mapping object

    /// transfer/map a uv coordinate to a physical coordinate independing on current view mapping mode
    bool image2physical(const std::pair<int32_t, int32_t> uv, std::pair<int32_t, int32_t>& pxln) const;

    /// transfer/map a pixel+line to a image view coordinate independing on current view mapping mode
    bool physical2image(const std::pair<int32_t, int32_t> pxln, std::pair<int32_t, int32_t>& uv) const;



    /// Transform from flat-view to physical scan. CAUTION!! Map and UnMap have a little glitch.
    /// they take a small bar from the bottom of the left half of the pano-image and add it to the bottom of the right half.
    /// Map depending on current mode
    virtual bool    Map(int32_t& /*pixel*/, int32_t& /*line*/ ) const { return true; }

    /// map depending on current mode. Transform physical scan coordinats to flat-view coordinates. CAUTION! this function can return y == scan->Lines() (invalid) coordinates if the physical scan has an uneven number of pixels. This is probably a side effect of the above mentioned glitch.
    virtual bool    UnMap(int32_t& /*pixel*/, int32_t& /*line*/, short UNUSED(mX) = -1, short UNUSED(mY) = -1) const { return true; }
    virtual int     _CurrentPixelMap() const;
    virtual int     _CurrentLineMap() const;



    /// checks rf and rg for zero
    virtual bool isZero() const { return false; }

    /// range of pixel from center [meter]
    double  Rgm() const;

    /// intensity if pixel [increments]
    int  Rf() const;

    /// raw value of range without any correction [millimeter]
    double Rgmm() const;

    // local 3d coordinate (no registration) [meter]
    zfs::math::Vec3d XYZ0() const { zfs::math::Vec3d v; XYZ0(v[0], v[1], v[2]); return v; }


    /** X, y, z values without translation/rotation but with calibration (+compensator correction)  if available. Need range and angle.*/
    virtual bool XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const;

    /// X, y, z values without translation/rotation but with calibration (+compensator correction) if available.
    bool XYZ0(double& /*x*/, double& /*y*/, double &/*z*/) const override
    {
        return false;
    }

    virtual zfs::math::rgb rgb()
    {
        return zfs::math::rgb(0, 0, 0);
    }


    /// Angles, without any correction/calibration. [rad]
    virtual bool Angle0(double& /*henc*/, double& /*lenc*/) const
        { return false; }
    virtual bool Angle(double& henc, double& lenc) const;
    /// return pair<vert,horz>
    std::pair<double, double> Angle() const { double vert = 0., horz = 0.; Angle(vert, horz); return std::pair<double, double>(vert, horz); }


    /** Calculate back from 3D point to 2D pixel. If the 3d point lies within the >180° overlap-area,
    it is not mapped to the upper-right part of the scan data, but to the lower left.
    * !!! this is not very accurate   <- ??? Whats the error?
    * All coordinates are in scan system.
    *	\return Returns true if implemented and pixel valid.*/
    virtual bool XYZ2POS(double /*x*/, double /*y*/, double /*z*/, int& /*xpos*/, int& /*ypos*/, bool forceScdLayer = false, bool forCal = false) { return false; }
    /// access via zfs::mat::Vec3d
    bool XYZ2POS(const zfs::math::Vec3d& xyz, int& u, int& v, bool secondLayer = false) { return XYZ2POS(xyz.x(), xyz.y(), xyz.z(), u, v, secondLayer); }



    /** \brief XYZ0 with compensator correction
     *	\author Susanne Lipkowski
     *	\param [in,out] x, y, z Cartesian coodinates of the point
     *	\return true on success
     */
    virtual bool Compensator(double& x, double& y, double& z) const;	//SL



    // set new values
    virtual bool    SetRgm(double m) { return false; } ///< take care about rangecalibration (new from 8.2)
    virtual bool    SetRgmm(int /*pix*/, int /*lin*/, int /*rg*/) { return false; } ///< direct set raw value
    virtual bool    SetRf(int /*pix*/, int /*lin*/, int /*rf*/) { return false; }
    virtual bool    SetRGB(int /*pix*/, int /*lin*/, zfs::math::rgb&) { return false; } // return true if implemented

    // misc
    double MaxRange() const { return maxrange; } /// maximum ambigue interval  [meter]
    void SetMaxRange(double rg) { maxrange = rg; } // m

    double MinRange() const { return minrange; } /// minimum range of laser for accurate measurment [meter]
    void SetMinRange(double rg) { minrange = rg; } // m

    double MaxViewStartAngle() const { return maxviewstartangle; } // m
    void SetMaxViewStartAngle(double angle) { maxviewstartangle = angle; } // m



    // Offsets
    void SetRgAdd(double meter) { rgadd = meter; }  // meter
    double RgAdd() const { return rgadd; }
    virtual double HOffset() const { return offsetH; } // rad
    virtual double LOffset() const { return offsetL; } // rad
    virtual double MOffset() const { return offsetM; } // meter
    virtual double MScale() const { return scaleM * 1000.; }
    virtual void SetHOffset(double o);
    virtual void SetLOffset(double o);
    virtual void SetMOffset(double o);
    virtual void SetMScale(double s);

    double getOffsetRef_m() const { return offsetM; } // const reference offset
    void setOffsetRef_m(double m) { offsetM = m; } // const reference offset

    // use callbacks progress
    bool m_progress;

    /// Callback Registered XYZ transformation for different calculation of the xyz
    typedef bool(*cbXYZ_f)(const ZFScanBasic*, int u, int v, double& x, double& y, double& z);
    cbXYZ_f cbXYZ = nullptr; // callback initialised to avoid undefined behaviour
    void installXYZCallback(cbXYZ_f cb) { cbXYZ = cb; }


    ZFTRMatrix& LevelMatrix() { return levelMatrix; }		///< returns matrix of leveling system if available
    const ZFTRMatrix& LevelMatrix() const { return levelMatrix; }		///< returns matrix of leveling system if available
    int LevelMatrixStatus() const { return levelMatrixStatus; }

    // internal access
    zfs::AtmosphereCorrection& atmosphereCorrection();
    const zfs::AtmosphereCorrection& atmosphereCorrection() const;

    // internal access
    zfs::RawParams& strayLight();
    const zfs::RawParams& strayLight() const;

    // Childs
protected:
    ZFScanBasic* pParent{};
    std::list<ZFScanBasic*> childs;

public:
    ZFScanBasic* parent() { return pParent; }   //!< Returns parent NULL if root.
    const ZFScanBasic* parent() const { return pParent; }   //!< Returns parent NULL if root.
    int numChilds() const { return (int)childs.size(); }
    void addChild(ZFScanBasic*);

    /** Remove child if NULL remove all. !!! not for future implementation !!! */
    void removeChild(ZFScanBasic* c);


    // access point plus value
    double  Rgmm(int pix, int lin) { Set(pix, lin); return Rgmm(); }
    double  Rgm(int pix, int lin) { Set(pix, lin); return Rgm(); }
    int     Rf(int pix, int lin) { Set(pix, lin); return Rf(); }

    bool    Angle0(int pix, int lin, double& henc, double& lenc) { Set(pix, lin); return Angle0(henc, lenc); }
    bool    Angle(int pix, int lin, double& henc, double& lenc) { Set(pix, lin); return Angle(henc, lenc); }
    bool    XYZ0(int pix, int lin, double& x, double& y, double &z) { Set(pix, lin); return XYZ0(x, y, z); }
    bool    XYZat(int pix, int lin, double& x, double& y, double &z, k_t k = K_ZFS) { Set(pix, lin); return zfs::ZFWorld::XYZ(x, y, z, k); }

protected:
    virtual void PostXYZ0Action(double& x, double& y, double& z) const {}
    virtual bool XYZ_(double& x, double& y, double &z) const override; // World XYZ

    virtual double _Rgmm() const { return 0; }
    virtual double _Rgm() const { return Rgmm() / 1000.; }
    virtual int _Rf() const { return 0; }
    virtual bool _XYZ0(double rg, double henc, double lenc, zfs::math::Vec3d& vec) const { return false; }

    virtual ZFSHeader* GetZFSHeader__() = 0;
    virtual const ZFSHeader* GetZFSHeader__() const = 0;
};


//================================================================================================================



class ZFSpline;



/**
*	\brief ZFScan  Abstact class for scanfile.
*
*   Advanced functions of scan.
*/
class ZFS_API ZFScan : public ZFScanBasic
{
    DEFINE_ZFBASE(ZFScanBasic)

    friend class ZFScanList;
    friend class ZFProject;
    friend class ZFRfComp;
    friend class ZFRf;
    friend class zfs::WayCounting;
    friend ZFScan* zfs::ZFSClone(ZFScan*, bool);

private:
    CxImage* rgbmemCopyForClones{};  ///< clones have their own copy of the overlay image, so we can change the image in the mother scan while clones exist.
    bool rgbmemIsLocked{};    /**< This is set to true if a scan was was cloned without the copy rgb image option. The image can not be replaced while this is true. */

protected:
    //PG
    int start1{};
    bool profiler{};		//scan is a profiler scan
//    double xAxis{};	    //difinet xAxis distance for profiler scans

    CUnahe* pUnahe{};
    ZFSpline* pRfComp{};

    ZF2DSelections* selections{};

    ZFMask* globalmask{};
    mask_lst mask_list{};
    std::shared_ptr<std::mutex> mutexMaskList{};

    void CBClose();
    virtual double RfComp() const;	//!< Reflection compensating with temperature and range. Returns 0 .. 1=100%. Values more than 1 are possible
    zfs::zfserr_t LoadRGBFile(CxImage* image, zf::FileName& fn);

    ZFMechCalib* m_calib{}; // used by InitCalib


public:
    ZFRf   rf{};	///< mode for viewing image

    const ZFMechCalib* calibObj() const { return m_calib; }

public:
    typedef std::vector<ZFBase*> objlist_t;
    ZFBase* addObj(ZFBase*); //!< Add new object.
    void addObj( std::shared_ptr<ZFBase> ); //!< Add new object.
    bool removeObj(ZFBase*); //!< Remove the object.
    void removeObjAll(const char* name = NULL); //!< Remove all objects. Use name to remove all objects of one typ.
    enum objaction_t { OBJ_UPDATE, OBJ_REMOVED, OBJ_ADDED };
    typedef void(*cbObj_t)(objaction_t);
    static cbObj_t cbObj;
    static void installObjCallback(cbObj_t cb) { cbObj = cb; }

    std::shared_ptr<zfs::WayCounting> way{};
    void InitWay(zfs::way_t m = zfs::WAY_NO, double v = .01, bool helix = true);
    zfs::way_t WayMod();
    double CalcWay(int line, double pixel = 0.); // xAxis     pixelvalue from 0..1 for helix calculation if enabled

    bool maskHasChanged{}; //PG 7.2.0.0

protected:
    objlist_t objlist;  //!< Holds variable kind of objects assosiated with scan. Destructor frees objects.
    std::list< std::shared_ptr<ZFBase> > objects{};
    volatile bool objListLock;
    std::shared_ptr<std::mutex> mutexObjects{};

public:
    ZFScan();
    ~ZFScan();

    bool allok() override;


    /// get all values at once (Rf,Rg,RGB,XYZ) use zfs::PData or zfs::PdataRGB
    template<class T>
    T pdata(int px, int lin);


    objlist_t* getObjListLock(); //!< Returns the ObjekctList and locks it. You need to call releaseObjListLock() to unlock!
    void releaseObjListLock();   //!< Release ObjListLock
    void setObjListLock();       //!< Set ObjListLock

    std::vector< std::shared_ptr<ZFBase> > getObjects( const char* objectClassName);
    int countObjects(const char* objectClassName="ZFBase");
    int removeAllObjects(const char* objectClassName ="");

    bool IsProfiler() { return profiler; }

    virtual long getRegisterFlag();
    zfs::zfserr_t OnOpen() override;
    void OnPreClose() override;


    // Callback on close of any can
    typedef void(*cbClose_t)(ZFScan*);
    static std::vector<cbClose_t> cbClose;
    static void installCloseCallback(cbClose_t cb) { cbClose.push_back(cb); }

    // Callback on close of this scan
    typedef void(*cbCloseScan_t)();
    std::vector<cbCloseScan_t> cbCloseScan;
    void installCloseCallback(cbCloseScan_t cb) { cbCloseScan.push_back(cb); }


    int MinRf() const { return minRf; }
    int MaxRf() const { return maxRf; }
    void SetMinRf(int rf);
    void SetMaxRf(int rf);
    int RfPercentToRf(double rfPercent);   ///< Enter values between 0 and 100
    double getMinRfPercent();

    // minimum rf defined in the filter settungs or if not the MinRf from the zfs header
    // slow access
    int FilterMinRf() const;

    void unahe(int inc = 0, ZFRect* rc = NULL); // UniformNonAdaptiveHistogramEqualization

    virtual double RfLinear1() const; //!< Linear interpolation of intensity 0..1
    virtual double RfNorm() const { return RfComp(); } // Never use this value for filtering
    virtual double RfNorm1() const { double rf_ = RfNorm(); if (rf_ > 1.) rf_ = 1.; return rf_; } //!< normalized Intensity value max 1. . Never use this value for filtering

    double subPixRf(double x, double y, bool rfCol);

    /// are data of this data type changable
    virtual bool IsChangeable() const  { return true; }


    // Color
    enum {
        RGB_NO,		// no colorinformation available
        RGB_OK,		// color available and loaded correctly
        RGB_FAILED	// color available but problem loading, mayby out of memory
    };
    int rgbStatus{};		//!< Status of colorinformation of scan
    std::wstring rgbfile;//!< currently used file for rgb image.
    std::wstring originalRgbFile; //!< filename of the original color file.
    bool hasPhoto();    //!< return true if a color image from a camera is available
    CxImage* rgbmem{};		//!< the image
    int rgbdeep{};        //!< deep of image
    int rgbrot{};         //!< stored as linebyline=2 of zfscan=1
    bool rgbPano360{};    //!< panorama image is stored as 360deg image, no overlapp
    zfs::zfserr_t LoadRGB(const std::wstring&, int x = 0, int y = 0); //!< load color or overlap image for scan at position x,y
    zfs::zfserr_t LoadRGB();		//!< load color image if available for scan
    void DetachRGB();  //!< unload image
    bool rgbjpg(zfs::math::rgb&);		//!< get the color of the image
    zfs::math::rgb rgb() override;//!< get the color of the pixel
    int rgb_startx{}, rgb_starty{}, rgb_w, rgb_h{}; //!< dimensions of the colorimage



    //
    // 2D Selections
    //
    ZF2DSelections* Selections() { return selections; } // exist always
    const ZF2DSelections* Selections() const { return selections; } // exist always
    // return all selections or one if nothing selected with max size
    std::vector<ZFRect> getSelectionsRect() const;

    ZFSpline* PtrRfComp() { return pRfComp; } //!< not null if available
    const ZFSpline* PtrRfComp() const { return pRfComp; } //!< not null if available
    bool HasRfComp() const { return pRfComp ? true : false; }

    // Referenz from lineheader if supported
    virtual double RefRg(int /*line*/) const { return 0; } //!< meter
    virtual double RefRf(int /*line*/) const { return 0; } //!< increments


    //
    // Masks
    //
    bool MaskHasChanged() { return maskHasChanged; } //PG 7.2.0.0
    void MaskHasChanged(bool bo) { maskHasChanged = bo; } //PG 7.2.0.0
    virtual ZFMask* AddMask(int x, int y, int w, int h, zfs::math::rgb rgb, const wchar_t* name = NULL, bool set = true) { return AddMask_(x, y, w, h, rgb, name, set, NULL); }
    virtual ZFMask* AddMask(zfs::math::rgb rgb, const char* name = NULL, bool set = true) { return AddMask_(rgb, name, set, NULL); }
    virtual ZFMask* AddMask(zfs::math::rgb rgb, const wchar_t* name = NULL, bool set = true) { return AddMask_(0, 0, Pixel(), Lines(), rgb, name, set, NULL); }
    ZFMask* AddMask(zfs::math::rgb rgb, const std::wstring& name, bool set = true) { return AddMask_(0, 0, Pixel(), Lines(), rgb, name.c_str(), set, NULL); }

    bool RemoveMask(ZFMask*, bool sync = true);
    int RemoveAllMasks(const std::list<std::wstring>& exclusionList = {});
    bool RemoveMask(const wchar_t*, bool sync = true);
    bool RemoveMask(const std::wstring& m, bool sync) { return RemoveMask(m.c_str(), sync); }

    void SyncMask();
    void ClrMask(const char* name = NULL);
    ZFMaskPartial* FindMask(const char* name);
    ZFMaskPartial* FindMask(const wchar_t* name);
    ZFMaskPartial* FindMask(const std::wstring& name);
    bool IsSet(int x, int y) const { return globalmask->IsSet(x, y); }   //!< returns true if the point is not masked and available.
    ZFMask& GlobalMask() { return *globalmask; }
    mask_lst& Masks() { return mask_list; }
    mask_lst MaskList() { return mask_list; }
    typedef void(*cbMask_t)(int action); //!< Called on add or remove mask.
    static cbMask_t cbMask;
    static void installMaskCallback(cbMask_t cb) { cbMask = cb; }
    // following only possible if scan has zfs-header
    virtual zfs::zfserr_t readMasks(int* n = NULL);
    virtual int writeMasks2File(zfs::zfserr_t& err); //!< Attach all masks to file.
    virtual int writeMasks2otherFile(ZFSHeader* ho, const std::wstring& fname); //!< Attach all masks to file.

    /// get the color and name if a mask is set for the defined pixel
    std::pair< zfs::math::rgb, std::wstring> getPixelMask(int pixel, int line) const;

protected:

    ZFMask* AddMask_(int x, int y, int w, int h, zfs::math::rgb rgb, const wchar_t* name = NULL, bool set = true, ZFScanZFS* z = NULL);
    ZFMask* AddMask_(zfs::math::rgb rgb, const char* name = NULL, bool set = true, ZFScanZFS* z = NULL);
    ZFMask* AddMask_(zfs::math::rgb rgb, const wchar_t* name = NULL, bool set = true, ZFScanZFS* z = NULL) { return AddMask_(0, 0, Pixel(), Lines(), rgb, name, set, z); }
    ZFMaskPartial* FindMask__(const wchar_t* name);

private:
    void throwExceptionWhenRgbImageCanNotSafelyBeExchanged();
};




//================================================================================================================
//=== Inline-Implementaion =============================================================================================================
//================================================================================================================


inline zfs::math::rgb ZFScan::rgb()
{
    return rf.rgbrgb();
}




inline bool ZFScanBasic::Angle(double& henc, double& lenc) const
{
    if (Angle0(henc, lenc))
    {
        henc = henc + offsetH;
        lenc = lenc + offsetL;
        if (henc > (PI*2.)) henc -= PI*2.; else if (henc < 0.) henc += PI*2.;
        if (lenc > (PI*2.)) lenc -= PI*2.; else if (lenc < 0.) lenc += PI*2.;
        return true;
    }
    return false;
}

template<> ZFS_API zfs::PDataRaw ZFScan::pdata(int, int);
template<> ZFS_API zfs::PData ZFScan::pdata(int, int);
template<> ZFS_API zfs::PDataRGB ZFScan::pdata(int, int);
template<> ZFS_API zfs::PDataDetail ZFScan::pdata(int, int);

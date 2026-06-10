#pragma once

/// \file
///
/// Line by line access for zfs scans
///
///   very fast access to scan-lines
///
///   this is the main headerfile for customer-access to scans
///
///   (C) Zoller & Froehlich GmbH
///

#include "zfsapi.h"
#include "config.h"

#ifndef LBL_MASK
#define LBL_MASK
#endif


#include <string>
#include <list>
#include <memory>
#include <utility>
#include <functional>

#include "zfbase.h"
#include "zfserr.h"
#include "zfsdefs.h"
#include "flags.h"

#include "dataaccess.h"
#include "zfvector.h"
#include "trmatrix.h"
#include "zflineaccess.h"
#include "model.h"
#include "world.h"
#include "pixel.h"
#include "zflineaccessimpl.h"
#include "linebyline_mask.h"
#include "rf.h"
#include "Math/zfvec.h"
#include "way_defines.h"
#include <zfcore/filename.h>
#include "../Common/inttypes_.h"
#include "filter/filter_result.h"

//#############################################################################################################

class ZFReader;
class ZFReadFileRollBuffer;
class ZFReadFileSingleLine;
class ZFSFilter;
class ZFScanMap;
class ZFRfComp;
class ZFAngleToPixel;
class ZFSHeader;
class ZFSpline;
class PartInf;
namespace zfs
{
    class LineHeader;
    class LineHeaderBuf;
    class Compensator;
    class AtmosphereCorrection;
    class ZFSLinebylinePixel;
    class RawParams;
    namespace math { class rgb; }
    class FilterParams;
    class TiledImage;
}

//#############################################################################################################


/** \class ZFSLinebyline
    \brief Basic class for lineByline access
*/
class ZFS_API ZFSLinebyline : public ZFBase
{
    DEFINE_ZFBASE(ZFBase)

    friend class ZFRfComp;
    friend class ZFRf;
    friend class zfs::ZFSLinebylinePixel;

public:
    ZFReader* reader{};	// filereader
    zfs::zfs_flags_t flags{};

protected:
    ZFScanMap* pixLineMap{}; //RGB jpg
    ZFLineInfo* lineInfo{};

    friend class ZFReadFileRollBuffer;
    friend class ZFReadFileSingleLine;
    friend class ZFReadFile;
    void FreeLines();


    bool DetectJpg();
    void DestroyJpg();

    ZFSpline * pRfComp{};
    std::shared_ptr<zfs::TiledImage> image{};

    bool mFilterPreNext{};

public:
    std::string info;		//!< Info about scan, ...

    ZFSHeader* fileHeader{};	//!< Pointer to current fileheader
    bool ok;				//!< Flag if access is ok

    ZFLineAccess* linePre{};	//!< Line bevor current line in roll mode
    ZFLineAccess* zeile{};	//!< Current line
    ZFLineAccess* lineNext{};	//!< Line after current line. in roll mode

    ZFModel* model{};			//!< 3D Model class for xyz calculation
    ZFTRMatrix matrix;		//!< World matrix

    int32_t levelMatrixStatus{};
    ZFTRMatrix levelMatrix; //!< Leveling system matrix if supported

    zfs::Compensator* compensator{};


    int32_t openflags{};			//!< File open flags
    int32_t typ{};				//!< File typ

    ZFSFilter* mxpx{};		//!< holds filtersettings

    int32_t clones{};
    ZFSLinebyline* cloneParent{};

    ZFRf   rf;	            //!< mode for viewing image

    ZFAngleToPixel* a2p{};
    bool supportXYZ2POS = false;	//!< mode for viewing image

public:
    zf::FileName fileName;	//!< Name of the open .zfs file. (first name if scan exist of more files)


private:
    zfs::zfserr_t Open_( const std::wstring& fileName );

protected:
    zfs::zfserr_t DetectModel( bool checkImage = true );
    bool Unahe(); /// this will create a nice gray image but will take some time
    double RfComp() const;
    double RfComp( ZFLineAccess* z ) const;
    double RfComp1( ZFLineAccess* z ) const { double rf_=RfComp(z); if(rf_>1.) rf_=1.; return rf_; }

private:
    ZFSLinebyline& operator=( const ZFSLinebyline& o ) {return *this;}
    ZFSLinebyline(const ZFSLinebyline& o) {}

public:
    ZFSLinebyline();
    virtual ~ZFSLinebyline();

    ZFTRMatrix& Matrix() { return matrix; }		// World matrix
    const ZFTRMatrix& Matrix() const { return matrix; }		// World matrix

    ZFTRMatrix& LevelMatrix() { return levelMatrix; } // returns matrix of leveling system if available
    const ZFTRMatrix& LevelMatrix() const { return levelMatrix; } // returns matrix of leveling system if available
    int32_t LevelMatrixStatus() const { return levelMatrixStatus; }

    double getMinRfPercent() const;
    int32_t RfPercentToRf( double rfPercent) const;

    static double getMinRfPercent( const ZFSHeader* h );
    static int32_t RfPercentToRf( double rfPercent, const ZFSHeader* h);


    /** Returns the file name inclusive path*/
    const char* FileName() { return fileName.c_str(); }
    const wchar_t* FileNameW() const { return fileName.WString().c_str(); }


    /**
    *   Check if zfs Scan is a color scan
    *   \return true if color is available
    */
    bool HasRGB() const;
    bool LoadRGB( const std::wstring& imageFilename ); // replace the standard color image with another one.
    std::wstring fileNameColorPictureW() const; // convinience overload

    uint32_t getImageWidth() const;
    uint32_t getImageHeight() const;

    zfs::math::rgb rgb_col(bool& ok); /** get color of pixel, otherwise r,g,b has the same value */

    /// get color of pixel of previous line, otherwise r,g,b has the same value 
    zfs::math::rgb rgb();
    // faster access because rf and range are still calculated
    zfs::math::rgb rgb(const ZFLineAccess* l);
    // fastest one if you have already a line pointer
    zfs::math::rgb rgb(const ZFLineAccess* l, zfs::PData& px);




    /** Returns the typ number of the scanner.*/
    virtual int32_t Typ() const { return typ; }


    virtual zfs::zfserr_t Open( const std::wstring&, int32_t flags=0 ); ///< open a zfs scan
    void Close();							                            ///< close

    /** Returns the zfs header acces status for the open file. */
    bool Ok() const { return ok; }

    bool IsProfiler(); ///< return true if scan was taken in profilermode or scanner is a profiler

    ZFSLinebyline* Clone();
    bool IsClone() const { return cloneParent?true:false; }


    bool Create( const std::wstring&, ZFSHeader* ); // create header for new file

    /** Returns a pointer to the current line. */
    ZFLineAccess* GetLine() { return zeile; }
    const ZFLineAccess* GetLine() const { return zeile; }

    /** Returns a pointer to the line information class. */
    ZFLineInfo* LineInfo() { return lineInfo; }
    const ZFLineInfo* LineInfo() const { return lineInfo; }

    zfs::AtmosphereCorrection& atmosphereCorrection();

    zfs::RawParams& strayLight();
    const zfs::RawParams& strayLight() const;

    virtual void LoadCalibration(); // LowLevel

//    void SetXDistance( double d );	// obsolete. set distance between lines in profiler mode
    void UseLProfiler( bool use=true, double rotX=0., double rotY=0., double rotZ=0. ); //deg  use for 3D calculation in profiler mode (imager in profiler mode. for example lmotor rotates)

    // Track calculation for Profiler
    void InitWay( zfs::way_t m=zfs::WAY_NO, double v=.01, bool helix=true );
    /// current way calculation mode
    int32_t WayMod() const;
    double CalcWay( int32_t line, double pixel=0. ); // xAxis     pixelvalue from 0..1 for helix calculation if enabled
    /// switch on / off calculation
    void WayModEnable( bool );
    /// whether is enabled
    bool IsWayModeEnabled() const;
    double Way() const;
    double Way( ZFLineAccess* ) const;


    bool IsCalibrated() const;
    const char* CalibInfo() const;
    const char* ModelInfo() const;

    int32_t SkipTo( int32_t line, bool reload = false );
    virtual int32_t LoadLines( int32_t lines=1 ); // Load next line into memory, or skip lines if lines >1
    /** Loads next line into memory. <BR> See also LoadNextLine() \return current line index */
    int32_t operator ++ () { return LoadLines(1); }
    int32_t LoadLineHeader( int32_t line, char* buf, int32_t bufsize ); ///< get lineheader, random access posssible. does not read linedata itself. return number of bytes read, 0 if not successfull
    int32_t LoadLineHeader( int32_t line, zfs::LineHeader& ); ///< get lineheader, random access posssible. does not read linedata itself. return number of bytes read, 0 if not successfull
    int32_t LoadLineHeader( int32_t line, zfs::LineHeaderBuf& ); ///< get lineheader, random access posssible. does not read linedata itself. return number of bytes read, 0 if not successfull
    int64_t determineFilePos(int32_t ln); ///< get file position return 0 if not possible
    int64_t LoadRawLine( std::vector<char>& buf); // buf must have size to hold whole line. Just for internal usage, Line is in binary format which depends on other settings. On success return number of bytes read otherwise 0

    /** Returns a pointer to the header. If file not open returns NULL. Typically it is the header of the last part if there are severel parts */
    ZFSHeader* GetZFSHeader() { return fileHeader; }
    const ZFSHeader* GetZFSHeader() const { return fileHeader; }

    /** Returns a pointer to the first header. If file not open returns NULL */
    ZFSHeader* GetCurrentZFSHeader();
    /** Returns a pointer to the first header. If file not open returns NULL */
    ZFSHeader* GetFirstZFSHeader();
    const ZFSHeader* GetFirstZFSHeader() const;


    /** Returns number of lines in scan. */
    int32_t Lines() const;
    /** Returns number of lines in current file */
    int32_t LinesCurrentFile() const;
    /** Returns number of pixel of line. */
    int32_t Pixel() const { return model->pixel; }



    /** Returns number of startline of the current file */
    int32_t GetStartLine();

    /** Returns the number of lines from the first file to the n file */
    int32_t GetLinesTo( int32_t fileNr );

    /** Returns true if current line not null. */
    bool HasLine() const			{ return zeile && zeile->Ok() ? true : false; }
    /** Returns true if line before current line not null. */
    bool HasLinePre() const		{ return linePre && linePre->Ok() ? true : false; }
    /** Returns true if next line not null. */
    bool HasLineNext() const		{ return lineNext && lineNext->Ok() ? true : false; }
    /** Returns a pointer to current line. <BR> See also CurrentLine()*/
    ZFLineAccess* L() const				{ return zeile; }
    /** Returns a pointer to line before current line. */
    ZFLineAccess* LPre() const			{ return linePre; }
    /** Returns a pointer to next line. */
    ZFLineAccess* LNext() const			{ return lineNext; }


    int32_t MinRf() const;	///< minimum usable intensity
    int32_t MaxRf() const;	///< maximum usable intensity

    double MinRange() const { return L()->Info()->minrange; } ///< minimum range [meter]
    double MaxRange() const { return L()->Info()->maxrange; } ///< maximum range [meter]

    //
    // Basic access functions
    //
    const wchar_t* CurrentFileNameW();
    const char* CurrentFileName();
    bool Set( int32_t pix, int32_t l = 0 ); /** Set pointer to pixel. Uses scan coordinates. l does nothing. Use SkipTo( l ) instead */
    int32_t CurrentPixel() const; /** current pixel */
    int32_t CurrentLine() const;  /** current line of the total scan*/
    int32_t LineInCurrentPart() const; //PG //> returns line number of the current part

    /// checks rf and rg for zero
    virtual bool isZero() const;

    int32_t Rf(ZFLineAccess* l=nullptr) const;          /** real intensity from device */
    double RfNorm() const { return RfComp(); }	/** Intensity as a float from 0.0 ... 1.0 for 100% but can be more than 100%. Never use this value for filtering */
    double RfNorm1() const;	/** Intensity as a float from 0.0 ... 1.0. Never use this value for filtering */
    bool isRfNormValid() const;
    
    double Rgm(ZFLineAccess* l=nullptr) const;      /** Range of Pixel unit=m */

    /// return the whole line already calculated (fasted access of line)
    enum {
        PDATA_GET_XYZ = (1<<0),
        PDATA_GET_MASK = (1 << 1),
        PDATA_GET_POST = (1 << 2),

        PDATA_GET_EVERYTHING = 0xffff,
    };

    void getPData(ZFLineAccess*, zfs::PDatas<zfs::PDataRaw>& line);
    void getPData(ZFLineAccess*, zfs::PDatas<zfs::PDataDetail>& line, int get = PDATA_GET_EVERYTHING);





    void Angle( double& h, double& l ) const; // get calibrated angle (rad)
    std::pair<double, double> Angle() const { double vert = 0., horz = 0.; Angle(vert, horz); return std::pair<double, double>(vert, horz); }
    void Angle( double range, double& h, double& l ) const; // get calibrated angle (rad) for a selected range

    void AngleUncal( double& h /*rad*/, double& l /*rad*/ ); // get un-calibrated angle (rad)
    void AngleUncal( double rgm, double& h /*rad*/, double& l /*rad*/ );// get un-calibrated angle (rad) for a selected range

    // Mapping from physical space to view space and reverse
    typedef std::pair<int32_t, int32_t> pxln;
    bool physical2image(const pxln orginal, pxln& transfered ) const; ///< replacement for Map
    bool image2physical(const pxln orginal, pxln& transfered) const; ///< replacement for UnMap
    bool Map( int32_t& p, int32_t& l ) const; ///< outdated, please use physical2image
    bool UnMap( int32_t& p, int32_t& l ) const; ///< outdated, please use image2physical
    ZFScanMap* MapObj() { return pixLineMap; }
    const ZFScanMap* MapObj() const { return pixLineMap; }


    // Pre line
    bool SetPreLine( int32_t pix );
    int32_t PreRf();
    double PreRgm();

    // Next Line
    bool SetNextLine( int32_t pix );
    int32_t NextRf();
    double NextRgm();

    // write values back. Values only stored in memory
    void SetRgm( double m );
    void SetRf( int32_t inc );

    /** Returns the reflection of point p in current line. The pixel pointer in the line are set to p.<BR> See also Rf()*/

    int32_t Rgmm(ZFLineAccess* l=NULL) const; /** unsafe, please do not use, use Rgm() instead */

    virtual void XYZ0( double& x, double& y, double& z, const ZFLineAccess* l=NULL ) const;
    virtual void PreXYZ0( double& x, double& y, double& z );
    zfs::math::Vec3d PreXYZ0() { zfs::math::Vec3d v; PreXYZ0(v[0], v[1], v[2]); return v; }
    virtual void NextXYZ0( double& x, double& y, double& z );
    zfs::math::Vec3d NextXYZ0() { zfs::math::Vec3d v; NextXYZ0(v[0], v[1], v[2]); return v; }


    /** Calculate form 2D pixel to 3D point for current pixel p. World XYZ.
     * \see XYZ0( double& x, double& y, double& z )
     * \param [out] x,y,z Coordinates of current pixel
     * \param l Line to load pixel from. If nullptr, use current line
     */
    void XYZ(double& x, double& y, double& z, const ZFLineAccess* l=nullptr) const;
    void PreXYZ( double& x, double& y, double& z );
    void NextXYZ( double& x, double& y, double& z );

    void Angle0( double& henc_rad, double& lenc_rad);
    void Angle0( int32_t u, int32_t v, double& henc_rad, double& lenc_rad );

    /** X, y, z values without translation/rotation but with calibration (+compensator correction) if available.*/
    void XYZ0( double henc, double lenc, double rg, double& x, double& y, double &z ) const;


    zfs::math::Vec3d XYZ0() const { zfs::math::Vec3d v; XYZ0(v[0], v[1], v[2]); return v; }
    zfs::math::Vec3d XYZ () const { zfs::math::Vec3d v; XYZ (v[0], v[1], v[2]); return v; }



    /**
    \brief backtransformation from xyz too pixel and line if supported. return true if success
            scan has to be open with flag ..XYZ2POS
    */
    bool SupportXYZ2POS() const { return supportXYZ2POS; }
    bool XYZ2POS( double x_local, double y_local, double z_local, double& pixel, double& line, bool forceScdLayer=false, bool forCal = false); 
    bool XYZ2POS( double x_local, double y_local, double z_local, int32_t& pixel, int32_t& line, bool forceScdLayer=false, bool forCal = false); 
    /// access via zfs::mat::Vec3d
    bool XYZ2POS(const zfs::math::Vec3d& xyz, int32_t& u, int32_t& v, bool secondLayer = false, bool forCal = false) { return XYZ2POS(xyz.x(), xyz.y(), xyz.z(), u, v, secondLayer, forCal); }




    /**
    *   \brief XYZ0 with compensator correction.
    *   \author Susanne Lipkowski
    *   \return bool
    */
    bool Compensator(zfs::math::Vec3d&, const ZFLineAccess* l) const;
    bool Compensator( double& x, double& y, double &z, const ZFLineAccess* l) const;

    /// setup the params for filtering
    void FilterDefine( const ZFSFilter& param );
    void FilterDefine( const zfs::FilterParams& );

    /// use 3-line filtering
    void usePreNextForFilter( bool use ) { mFilterPreNext = use; }
    
    /// Filter the line. \return number of pixels masked out
    bool PrepareFilter(); ///< will be called by Filter()
    zfs::LineFilterResult Filter(); // depending von lines 3 or single line filter
    /// force to filter using just one line, ignoring re and next
    zfs::LineFilterResult FilterForceOneLine();
    /// Filter just a part of the line. \return number of pixels masked out
    int32_t Filter( uint32_t fromPixel, uint32_t toPixel ); 
    /// clear the filter result so next filter call on the same line will call the filter with maybe changed params again and create a new result
    void ClearFilterResult();

    /// Check if pixel is enabled or disabled by a filter
    bool IsSet( int32_t x )	const { return zeile && zeile->IsSet( x ); } 

    //------------------------------------------------------------------------

    /// add a layer file to the scan
    bool attachLayer( const std::wstring& name = L"mask" );
    
    /// /// check if a layer file was loaded. return number of layers (currently just one supported)
    int hasLayerLoaded() const;
    
    /// /// get a list of layers loaded on number of available in the parts
    std::vector< std::pair<std::wstring, int> > layersLoaded() const;

    /// check if layer is set. Layer has to be available and loaded with attachLayer
    /// return -1 if not available, 0 = valid pixel no layer   1..n bit set in layer n (inverted to meaning of the layer itself)
    int isSetLayer( int line, int px) const;
    
    /// /// set a new value to the layer. Need write access to layer. Open scan with option OPEN_RW
    bool setLayer( int line, int px, int val, int layer=0);

    //------------------------------------------------------------------------


    bool IsZeroPixel() const; /// checks whether pixel is a filled pixel at the end of a line (exist only in lara mode)

    //
    // Low Level functions
    //
    /** Returns the offset for the height speed motor in rad.*/
    double HOffset() const { return zeile->Info()->offsetHrad; }
    double LOffset() const { return zeile->Info()->offsetLrad; }

    void SetHOffset(double o);	// [rad]

    int32_t pixel360() const { return zeile->Info()->pixel360; }

    double MOffset() const;
    void SetMOffset( double o);
    /** Returns the scale range.*/
    double MScale() const;
    void SetMScale( double s );

    // Referenz from lineheader if supported
    ZFSpline* PtrRfComp() { return pRfComp; } //!< not null if referenz correction available
    const ZFSpline* PtrRfComp() const { return pRfComp; } //!< not null if referenz correction available
    bool HasRfComp() const { return pRfComp?true:false; }

    virtual double RefRg() const; //!< referenz of current line (unit:meter)  return 0 if not available
    virtual double RefRf() const ; //!< referenz intensity of current line (unit:increments)  return 0 if not available

    void SetRgmm( int32_t mm );

    /** Returns the Information of the file as char string. */
    const char* Info() { return info.c_str(); }
    void AddInfo( const char* inf ); // Add new line to info string

    /** Write changes in fileheader back to file */
    bool WriteHeader();

    // only typ 201
    char getRgc() const;

    int32_t getParts() const; ///< return number of parts

    /// return a list of all possible parts
    const std::vector<PartInf>* getPartList() const;

    int32_t getCurrentPart(); ///< return current part of scan

    /**
     * get part and line in part having a absolute line number
     * \return part and rel-linenumber  part=-1 if not found
     */
    std::pair<int,int> getPartAndRellinenumber( int line_abs ) const;

#ifdef LBL_MASK //7.0.0.10 PG
    bool WriteMask( int32_t aktLineFrom, int32_t aktLineTo );
    bool WriteMask();

//    bool AddMask(int32_t aktLineFrom, int32_t aktLineTo, zfs::math::rgb rgb, const std::string& name, bool set = true);
    bool AddMask(const std::wstring& name, zfs::math::rgb rgb = {255,0,0} );
    bool AddMask(int32_t aktLineFrom, int32_t aktLineTo, zfs::math::rgb rgb, const std::wstring& name, bool set = true);

    bool FindMask(const std::string& name) const;
    bool FindMask(const std::wstring& name) const;

    // run over all masks
    void parseMasks(std::function< void(ZFMask*) >);

    ZFMaskPartial* findMask(int part, const std::wstring& mask_name);

    bool RemoveMask(const std::string& name);
    bool RemoveMask(const std::wstring& name);

    std::vector<ZFMaskInfo> getMaskInfoList() const;

    int32_t IsSetCurrentMask( int32_t x );

    bool IsSetMask( int32_t x ) const;

    void SetMask(int32_t p, int32_t l, const std::wstring& name=L"");

    void ClrMask(int32_t p, int32_t l, const std::wstring& name=L"");

    bool HasMask() const;

    /// reset/clear all masks with defined name in all parts. Retunr number of masks in parts reseted
    int ResetMask(const std::wstring& name);

    ZFMaskPartial* maskOfPart(int part, const std::wstring& maskName );
#endif

    // Objectlist
    typedef std::vector<ZFBase*> objlist_t;
    objlist_t objlist;  //!< Holds variable kind of objects assosiated with scan. Destructor frees objects.
    ZFBase* addObj( ZFBase* ); //!< Add new object.
    bool removeObj( ZFBase* ); //!< Remove the object.
    void removeObjAll( const char* name=NULL ); //!< Remove all objects. Use name to remove all objects of one typ.
    typedef void (*cbObj_t)( objlist_t&, bool );

    // Callback Registered XYZ transformation
    ZFBase* cbObjXYZ{};
    typedef bool (*cbXYZ_f)( const ZFSLinebyline*, const ZFLineAccess*, const ZFBase*, int32_t u, int32_t v, double& x, double& y, double& z );
    cbXYZ_f cbXYZ{};
    void installXYZCallback( cbXYZ_f cb, ZFBase* o=NULL ) { cbXYZ=cb; cbObjXYZ=o; }

    ZFBase* cbObjLine{};
    typedef void (*cbLine_f)( const ZFSLinebyline*, const ZFBase*, int32_t ln );
    cbLine_f cbLine{};
    void installLineCallback( cbLine_f cb, ZFBase* o=NULL ) { cbLine=cb; cbObjLine=o; }


    bool FastXYZ( bool fast ); ///< Use fast xyz calculation for 5006 Profiler lara-sync (is used by default if possible)
    bool IsFastXYZ() const;
    bool ErrVec( double& x, double& y, double& z );

    //Write
    bool writeCurrentLine();
    bool writwLineHeader();
    bool writwPreLineHeader();

    bool writeAllHeader( int32_t idx, int32_t value );
    bool writeAllHeader( int32_t idx, double value );

    /// will be called automatically after XYZ0. Just internal usage
    void PostXYZ0Action( double& x, double& y, double& z, const ZFLineAccess* l ) const; 
    void PostXYZ0Action(zfs::math::Vec3d&, const ZFLineAccess* l) const;

    /// just for unit test
    bool createSimulatedLines(ZFSHeader&);

private:
    void Copy( ZFSLinebyline& );
};


//........................................................................

inline int32_t ZFSLinebyline::CurrentPixel() const
{
    if (zeile)
        return zeile->CurrentPixel();
    return 0;
}

inline double ZFSLinebyline::Rgm(ZFLineAccess* l) const
{
    if (l)
        return l->Rgm();
    else
        return zeile->Rgm();
}

/**
    return referenz range
*/
inline double ZFSLinebyline::RefRg() const
{
    if (zeile)
        return zeile->RefRg();
    return 0;
}

/**
    return referenz intensity
*/
inline double ZFSLinebyline::RefRf() const
{
    if (zeile)
        return zeile->RefRf();
    return 0;
}

//#############################################################################################################


/*template void ZFS_API ZFSLinebyline::getPData<zfs::PDataRaw>(zfs::PDatas<zfs::PDataRaw>& line);
template void ZFS_API ZFSLinebyline::getPData<zfs::PData>(zfs::PDatas<zfs::PData>& line);
template void ZFS_API ZFSLinebyline::getPData<zfs::PDataRGB>(zfs::PDatas<zfs::PDataRGB>& line);
template void ZFS_API ZFSLinebyline::getPData<zfs::PDataDetail>(zfs::PDatas<zfs::PDataDetail>& line);*/


/**
 * \brief Open scan (only one line is loaded into memory) for very fast access
 *
 * main function to open a scan line by line.
 * Use delete after access to destroy
 *
 * \param filename  path to file to load
 * \param flags     open flags
 * \param [out] err is set to ERR_OK on success
 * \return Owning pointer to ZFSLinebyline object
 */
ZFS_API_F ZFSLinebyline* OpenLineByLine(const std::wstring& filename, int32_t flags=OPEN_DEFAULTFLAGS_LBL, zfs::zfserr_t* err=nullptr);

/**
 * \overload
 */
ZFS_API_F ZFSLinebyline* OpenLineByLine(const wchar_t* filename, int32_t flags=OPEN_DEFAULTFLAGS_LBL, zfs::zfserr_t* err=nullptr);

/**
 * \overload
 * Use Filename encoded using UTF8
 */
ZFS_API_F ZFSLinebyline* OpenLineByLine(const std::string& filename, int32_t flags=OPEN_DEFAULTFLAGS_LBL, zfs::zfserr_t* err=nullptr);

/**
 * \overload
 * Use library own abstraction of paths
 */
ZFS_API_F ZFSLinebyline* OpenLineByLine(const zf::FileName& filename, int32_t flags=OPEN_DEFAULTFLAGS_LBL, zfs::zfserr_t* err=nullptr);

/**
 * \brief Close scan and release all resources
 * \param lbl scan to close
 */
ZFS_API_F void CloseLineByLine( ZFSLinebyline* lbl );


//#############################################################################################################

#include "linebyline_inl.h"

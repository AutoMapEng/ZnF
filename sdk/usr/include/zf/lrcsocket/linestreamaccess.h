#pragma once

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

#include "lrcsocketapi.h"

#include <string>
#include <memory>
#include <vector>

class ZFLinedataStream;
namespace lrcsocket
{
    class StreamImpl;
}

#include "../zfs/zfs.h"
#include "../zfs/pixel.h"
#include "../zfs/filter/mixpix.h"
#include "../zfs/linebyline.h"
#include "../zfs/filter/linebyline_filter.h"
#include "../zfs/zflineaccessimpl.h"
#include "../zfs/zflineaccessimpl315.h"
#include "../zfs/zflineaccessimpl515.h"
#include "../zfs/mcalib.h"
#include "../zfs/Math/zfvec.h"


#define LH_POS_REF	20	// position referenz im lineheader


/*
    class for interpreting and calibrating scanline
    ZFLinedataStreamAccess is for internal use only
        Please never use this class.
        ZFLinedataStreamAccess will try to calculate and calibrate a line if zfs-header is available
*/
class LRCSOCKET_API ZFLinedataStreamAccess
{
    friend class ZFLinedataStream;
    friend class lrcsocket::StreamImpl;

public:
    ZFLinedataStreamAccess();
    virtual ~ZFLinedataStreamAccess();

    void Setup(int serie, int lh, int b);

    /// create the internal line buffers
    bool Create(const char* header, int sz);
    bool Create(const ZFSHeader* header);
    bool Create( int type, int bpp, int lhsz, int pixel );

    /// check wheter create was called and was successfull
    bool created() const;


    /*
    \brief change the default setting for xyz calculation (new in 2.0.1)
    */
    void ChangeDefaultXYZCalculation(bool l, double rotx = 0., double roty = 0., double rotz = 0.)
    {
        if (model)	model->UseLProfiler(l, rotx, roty, rotz);
    }

    /// data type of the stream
    int Typ() const { return typ; }

    /// serial number of the device
    const char* SNo() const { return sno.c_str(); }

    /// check if everything is received to filer and/or recording
    bool filter_possible() const { return HasZFSHeader() && created(); }
    bool record_possible() const { return HasZFSHeader() && created(); }

    /// access to linedata. NULL if problem
    ZFLineAccess* LineAccess() { return lineaccessCur; }
    /// access to linedata. NULL if problem
    const ZFLineAccess* LineAccess() const { return lineaccessCur; }
    /// access to model. NULL if problem
    const ZFModel* Model() const { return model; }

    void Destroy();

    ZFSHeader& GetZFSHeader() { return zfsheader; }
    const ZFSHeader& GetZFSHeader() const { return zfsheader; }
    bool HasZFSHeader() const { return bHeader; }

    // check if calibration is available
    int IsCalib() const;

    /*
    * use datablock for new line
    * do filtering as well
    * \return filterresult
    */
    std::wstring UseLine( const char* data, int size);
    std::wstring UseLine( const std::vector<char>& data);

    // set pointer to pixel
    void Set(int px);

    // check mask bit
    bool IsSet(int pixel) const;

    // checks the pdata structure if pixel is filtered out (nonzero) returns the id of the filter
    int isFilteredOut(int pixel) const;

    // number of points in current calculated line
    size_t size2() const { return pd2.points.size(); }

    const zfs::PDatas<zfs::PDataDetail>& pb1() const { return pd1; }
    const zfs::PDatas<zfs::PDataDetail>& pb2() const { return pd2; }
    const zfs::PDatas<zfs::PDataDetail>& pb3() const { return pd3; }

    const zfs::PDataDetail& px1(int p) const { return pd1.points[p]; }
    const zfs::PDataDetail& px2(int p) const { return pd2.points[p]; }
    const zfs::PDataDetail& px3(int p) const { return pd3.points[p]; }

    // Get Range [meter]  (use Set() before)
    double Rgm() const;

    // Get intensity 0..1   just linear interpolated
    double RfNorm() const;

    // Get intensity (real) [inc]
    int Rf() const;

    // calculate xyz
    bool XYZ0(double& x, double& y, double& z);
	zfs::math::Vec3d XYZ0();
	bool XYZ(double& x, double& y, double& z);	/// global

    // elevation offset of line
    double eleOffset() const;

    /// return reference intensity if available otherwise zero [inc]
    double RefRf() const;

    /// return reference range if available otherwise zero [meter]
    double RefRg() const;

    // Pointer to pixel
    char* Ptr();

    // low speeed Encoder byte
    // if enocder overflow you have to handle it by your own
    DEPRECATED(int16_t LEnc() const);

    // return additional byte of pixel if scanparam has enabled it
    // zfs.dll support function ZfsDetectCounterbyte(ZFSHeader*) for getting counterbyte
    unsigned char Add() const;

    // return additional lowest 2 bits of byte of pixel if scanparam has enabled it
    // this 2 bits repesent the lowest 2 bits of a counter
    // Please read documentation for more information about this bits
    // depending on system and scancommand this can be different
    unsigned char Add_Least2Bits() const
    {
        return Add() & 3;
    }

    double OffsetM() const;

    uint32_t getLineNumber() const { return lineNumber; }

    int64_t MissedLines() const { return missedLines; }


    //
    // Filter Stuff
    //    this will work only with some special projects like Profiler 600x
    //

    std::wstring Filter(); ///< standardfilter internaly used.
    ZFSFilter* GetFilterParam() { return &filterParams; }
    const ZFSFilter* GetFilterParam() const { return &filterParams; }
    void SetFilterParam(const ZFSFilter& param) { nextFilterParams = param; newFilterParams = true; }

    void enableFilter(bool en) { m_filterEnable = en; }
    bool isFilterEnabled() const { return m_filterEnable; }

    void FilterResetMask();
    double FilterTimeUsed() const { return tmFilter; }

    /// tells wheather filter defined and call filter was sucessfull
    bool isFiltered() const { return filtered; }

    /// return the line for storing
    ZFLineAccess* lineStreamOut() const { return lineaccessPre ? lineaccessPre : lineaccessCur; }
    ZFLineAccess* lineStreamCurrent() const { return lineaccessCur; }

    // check if new filter params should be attached (typically at the beginning of a line)
	void checkFilterParams();

    /// rotate the 3line buffer to next line to use
    void rotatePtr();

protected:
    void initPtr();
    bool Create_();
    bool Create_and_BuildLUT();
    void fillPData();

protected:
    // calibrierung
    int           typ = 0;			    ///< datatyp
    int           caltyp = 0;		    ///< calibrationstyp
    std::string   sno;              ///< device number
    ZFSHeader     zfsheader;	    //
    std::unique_ptr<ZFLineInfo>   info;			    ///< some information about scan
    std::shared_ptr<ZFSpline> spline{};
    bool m_filterEnable{ true };

    // buffer for 3 lines
    ZFLineAccess* lineaccess1 = nullptr;	    ///< linebuffer (the buffer)
    ZFLineAccess* lineaccess2 = nullptr;	    ///< linebuffer
    ZFLineAccess* lineaccess3 = nullptr;	    ///< linebuffer

    // rotating pointer point to lineaccess1 to 3
    ZFLineAccess* lineaccessCur = nullptr;  ///< ritation pointer pointing to lineaccess1..3
    ZFLineAccess* lineaccessPre = nullptr;
    ZFLineAccess* lineaccessNext = nullptr;
    //    ZFLineAccess* lineaccess = nullptr; ///< for access of line to process

    ZFModel* model = nullptr;		    ///< 3D Model
    ZFTRMatrix    matrix;		    ///< whole matrix
    ZFSFilter	  filterParams;     ///< current filterparams
    ZFSFilter	  nextFilterParams;	///< filterparams from programm use for next line
    bool newFilterParams = false;           ///< flag that nextFilterParams should be used for next line

    bool bHeader = false;				    ///< true if header available

    int pix = 0;					    ///< pixel per line
    int bpp = 0;					    ///< bytes per pixel
    int lhsz = 0;					    ///< lineheadersize
    double ref = 0.;
    double offsetM = 0.;  ///< will be set via UseLine for each line
    int  henc = 0;					    ///< encoder highspeed typical vertical
    int  hsub = 1;					    ///< subsample
    uint32_t lineNumber = 0;

    char* pLine = nullptr;				    // current line
    char* pPixel = nullptr;				    // current pixel position

    // memory if there is no lineaccess available, fallback
    int memSize = 0;
    std::unique_ptr<char> pMem;

    // check
    uint64_t lastLine = 0;		    // internal
    int64_t missedLines = 0;      // counter for missed lines

    uint32_t start1 = 0, start2 = 0; // encoder position start encoder if not 360deg

    // calculated and filtered line
    zfs::PDatas<zfs::PDataDetail> pd1, pd2, pd3;

    /// time filter used [ms]
    double tmFilter{};
    bool filtered{}; // tells if filter is active and works successfull

    double tmCalc{};

};


//#######################################################################################################################################################################

inline std::wstring ZFLinedataStreamAccess::UseLine(const std::vector<char>& data)
{
    return UseLine(&data[0], static_cast<int>(data.size()));
}


inline void ZFLinedataStreamAccess::Destroy()
{
    pLine = pPixel = NULL;
    bHeader = false;
}


inline int ZFLinedataStreamAccess::IsCalib() const
{
    if (lineaccessCur && model)
        return caltyp;
    return 0;
}


inline double ZFLinedataStreamAccess::eleOffset() const
{
    if ( model && model->calib )
        return model->calib->ElevOffset();
    return 0;
}

// set pointer to pixel
inline void ZFLinedataStreamAccess::Set(int px)
{
    if (lineaccessCur)
        lineaccessCur->Set(px);
    else if (pLine)
        pPixel = pLine + lhsz + px * bpp;
}

inline bool ZFLinedataStreamAccess::IsSet(int pixel) const // check mask bit
{
    if (lineaccessCur)
        return lineaccessCur->IsSet(pixel);
    return true;
}

inline int ZFLinedataStreamAccess::isFilteredOut(int pixel) const
{
    if (pixel >= 0 && pixel < static_cast<int>(pd2.points.size()))
    {
        const auto& pt = pd2.points[pixel];
        return pt.filterID;
    }
    return 0;
}


inline double ZFLinedataStreamAccess::Rgm() const
{
    if (lineaccessCur)
        return lineaccessCur->Rgm();

    if (pPixel)
    {
        if (typ==515 || typ == 315 || typ == 314 || typ == 215 || typ == 214 || typ == 115 || typ == 114)
            return (double)((*(uint32_t*)pPixel) & 0xffffff) / 10. / 1000. + offsetM;
        else if (typ == 14 || typ == 15)
        {
            return (double)(*(uint16_t*)(pPixel + 1)) / 1000. + offsetM;
        }
    }

    return 0.;
}

inline double ZFLinedataStreamAccess::OffsetM() const
{
    return offsetM;
}

inline double ZFLinedataStreamAccess::RefRf() const
{
    if (lineaccessCur)
        return lineaccessCur->RefRf();
    return 0;
}

inline double ZFLinedataStreamAccess::RefRg() const
{
    if (lineaccessCur)
        return lineaccessCur->RefRg();
    return 0;
}


// Get intensity 0..1
inline double ZFLinedataStreamAccess::RfNorm() const
{
    if (lineaccessCur)
        return lineaccessCur->RfLinear1();

    if (pPixel)
    {
        if (typ == 515 || typ == 314 || typ == 315 || typ == 215 || typ == 214 || typ == 115 || typ == 114)
        {
            uint16_t rf = *(uint16_t*)(pPixel + 3);
            if (rf & 1)
                rf = (rf&~1) << 8;
            rf /= 256;
            return (double)rf / 256.;
        }
        else if (typ == 14)
            return (double)((*(uint16_t*)(pPixel + 2)) & 0xffff) / 256.;
    }
    return 0.;
}

// Get intensity (real)
inline int ZFLinedataStreamAccess::Rf() const
{
    if (lineaccessCur)
        return lineaccessCur->Rf();

    if (pPixel)
    {
        if (typ == 515 || typ == 315 || typ == 314 || typ == 215 || typ == 214 || typ == 115 || typ == 114)
        {
            uint16_t rf = *(uint16_t*)(pPixel + 3);
            if (rf & 1)
                rf = (rf&~1) << 8;
            return rf;
        }
        else if (typ == 14)
            return (*(uint16_t*)(pPixel + 3)) & 0xffff;
    }
    return 0;
}

// calculate xyz
inline bool ZFLinedataStreamAccess::XYZ0(double& x, double& y, double& z)
{
    x = y = z = 0;
    if (lineaccessCur && model)
    {
        return model->XYZ0(lineaccessCur, lineaccessCur->Rgm(), x, y, z);
    }
    return false;
}

inline zfs::math::Vec3d ZFLinedataStreamAccess::XYZ0()
{
	double x = 0, y = 0, z = 0;
	if (lineaccessCur && model)
	{
		model->XYZ0(lineaccessCur, lineaccessCur->Rgm(), x, y, z);
	}
	return zfs::math::Vec3d( x,y,z );
}

inline bool ZFLinedataStreamAccess::XYZ(double& x, double& y, double& z)
{
    if (XYZ0(x, y, z))
    {
        matrix.World(x, y, z);
        return true;
    }
    return false;
}

// Pointer to pixel
inline char* ZFLinedataStreamAccess::Ptr()
{
    if (lineaccessCur)
        return lineaccessCur->PPtr();
    return pPixel;
}

inline int16_t ZFLinedataStreamAccess::LEnc() const
{
    if (lineaccessCur)
    {
#ifdef ZFS_5010_SUPPORT
        if (typ == 515 || typ == 315)
            return *(int16_t*)(lineaccessCur->PPtr() + 7);
        else if (typ == 314)
            return *(int16_t*)(lineaccessCur->PPtr() + 5);

        else if (typ == 215)
            return *(int16_t*)(lineaccessCur->PPtr() + 6);
        else if (typ == 214)
            return *(int16_t*)(lineaccessCur->PPtr() + 5);
        else
#endif
        if (typ == 114)
            return *(int16_t*)(lineaccessCur->PPtr() + 5);
        else if (typ == 115)
            return *(int16_t*)(lineaccessCur->PPtr() + 8);
    }
    return 0; // Imager 5003 not supported
}

inline unsigned char ZFLinedataStreamAccess::Add() const
{
    if (lineaccessCur)
    {
#ifdef ZFS_5010_SUPPORT
        if (typ == 515 || typ == 315)
            return *(lineaccessCur->PPtr() + 10); // FIXME
        else if (typ == 314)
            return *(lineaccessCur->PPtr() + 7); // FIXME

        else if (typ == 215)
            return *(lineaccessCur->PPtr() + 10);
        else if (typ == 214)
            return *(lineaccessCur->PPtr() + 7);
        else
#endif

            if (typ == 114)
                return *(lineaccessCur->PPtr() + 6);
            else if (typ == 115)
                return *(lineaccessCur->PPtr() + 9);
    }
    return 0; // Imager 5003 not supported
}


inline void ZFLinedataStreamAccess::FilterResetMask()
{
    if (lineaccessCur)
        lineaccessCur->EnableAll();
}



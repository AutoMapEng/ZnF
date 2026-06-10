#pragma once

#include "zfsapi.h"
#include "config.h"
#include "zfserr.h"
#include "encpixel2angle.h"

#include <string>

#include "zfscan.h"

//
// Implemementation of the supported scanfiletypes
//

#ifdef ZFS_SCENEMODELER_SUPPORT
#define SUPPORT_ZFS3
#endif

#ifdef ZFS_5003_SUPPORT
#define SUPPORT_ZFS15
#define SUPPORT_ZFS16
#define SUPPORT_ZFS17
#define SUPPORT_ZFS18
#define SUPPORT_ZFS19
#define SUPPORT_ZFS20
#define SUPPORT_ZFS200
#define SUPPORT_ZFS201
#endif

#ifdef ZFS_5006_SUPPORT
#define SUPPORT_ZFS114
#define SUPPORT_ZFS115
#endif

#ifdef ZFS_5010_SUPPORT
#define SUPPORT_ZFS214
#define SUPPORT_ZFS215
#endif

#ifdef ZFS_5016_SUPPORT
#ifdef SUPPORT_ZFS215
#define SUPPORT_ZFS315
#endif
#endif

#ifdef ZFS_5018_SUPPORT
#ifndef SUPPORT_ZFS315
#define SUPPORT_ZFS415
#endif
#endif

#ifdef ZFS_5024_SUPPORT
#ifndef SUPPORT_ZFS515
#define SUPPORT_ZFS515
#endif
#endif

#ifdef ZFS_COLOR_SUPPORT
#define SUPPORT_ZFS_COL1000
#endif

class ZFDataBuffer;
class ZFDataAccess;
class ZFTheoModel;
class ZFAngleToPixel;
class ZFScanZFSBasic;
class ZFSHeader;
class ZFLine;

//================================================================================================================

#ifdef SUPPORT_ZFS3

class ZFScan3Basic;

class ZFS_API ZFScanZFS3 : public ZFScan // Scenemodeler
{
    DEFINE_ZFBASE(ZFScan)

        friend class ZFProject;

protected:
    ZFScan3Basic* scan{};
    ZFSHeader* header{};
    int pixelzfs{};	//!< Not maped size.
    int lineszfs{};

    int ench{};
    int encl{};

    int subx{};
    int suby{};

    int bytesPerPixel{};
    int lineHeaderSize{};
    int	lineSize{};
    int highOffset{};
    int lowOffset{};

    int calibTyp{};
    bool bCalibOn{};
    std::string calibinfo;

    char* data{};

public:
    ZFScanZFS3();
    ~ZFScanZFS3();

    virtual const char* FileTyp() const override { return "ZFS"; }
    virtual const char* DataTyp() const override { return "SceneModeler"; }

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const wchar_t* param = NULL, ZFRect* rc = NULL) override;
    virtual void CloseFile() override;
    virtual bool WriteHeader() override;

    virtual bool HasIntens() const override { return true; }
    virtual bool HasRange() const override { return true; }
    virtual bool HasAngle() const override { return true; }
    virtual bool HasXYZ() const override { return true; }

    char* Data() { return data; }
    virtual bool   Set(int pix, int lin) override;
    char* Ptr();
    virtual bool   Angle0(double& henc, double& lenc) const override;
    virtual bool   XYZ0(double& x, double& y, double &z) const override;
    virtual bool   XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;

    virtual bool   SetRgm(double m) override;

    /** Switch calibration on/off.  \return Returns prev mode. */
    bool Calib(bool c);
    const char* CalibInfo() { return calibinfo.c_str(); }
    virtual ZFSHeader* GetZFSHeader__() override { return header; }
    virtual const ZFSHeader* GetZFSHeader__() const override { return header; }
    int     LineHeaderSize() { return 0; }
    virtual int HasCalibration() const override { return calibTyp; }
    virtual bool IsCalibrated() const override { return calibTyp && bCalibOn; }
    virtual void SetCalibTyp(int t) { calibTyp = t; }

    int     BytesPerPixel() { return bytesPerPixel; }
    virtual void    Init();

protected:
    virtual double   _Rgmm() const override;
    virtual int    _Rf() const override;
};

#endif // SUPPORT_ZFS3

//================================================================================================================






//
// ZFS file
//
class ZFS_API ZFScanZFS : public ZFScan
{
    DEFINE_ZFBASE(ZFScan)

    friend class ZFProject;
    friend class zfs::WayCounting;
    friend ZFScan* zfs::ZFSClone(ZFScan*, bool);

public:
    ZFDataBuffer* dataBuffer{};
protected:
    ZFDataAccess* dataAccess{};
    ZFScanZFSBasic* scan{};
    ZFSHeader* header{};
    int pixelzfs{};	//!< Not maped size.
    int lineszfs{};
    int pixelmax{};
    int linesmax{};

    int ench{};
    int encl{};

    double subx{1};
    int suby{1};
    bool clip{};
    int from1{};
    int to1{};
    int from2{};
    int to2{};
    double angleOffset{}; //!< Angle Offset if no calib.

    zfs::VertPixel2Angle pixel2angle;

    int bytesPerPixel{};
    int lineHeaderSize{};
    int	lineSize{};
    static char out[5];
    int highOffset1{};    //!< Offset for 2d view first plane.
    int highOffset2{};    //!< Offset for 2d view second plane.
    int lowOffset{};

    int calibTyp{};
    bool bCalibOn{};
    std::string calibinfo;

public:
    ZFScanZFS();
    ~ZFScanZFS();

    bool allok() override;

    virtual int getEnc();

    virtual const char* FileTyp() const override { return "ZFS"; }
    virtual const char* DataTyp() const override { return "ZFS"; }

    int getLinesFile() const { return lineszfs; } //!< Returns the number of lines in the scanfile.

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const wchar_t* param = nullptr, ZFRect* rc = nullptr) override;
    virtual void CloseFile() override;
    bool OpenHeaderOnly(const wchar_t*) override;

    virtual void    Init(ZFRect*, bool phys = false);
    virtual void    InitCalib(int newtyp = 0) override;
    virtual int     readData(ZFRect*); //!< Read all lines or map file.
    virtual void    InitWithData();
    virtual void    OnPostInit(){}

//    virtual bool extension(double& starth, double& startv, double& h, double& v) override;

    virtual bool HasIntens() const override { return true; }
    virtual bool HasRange() const override { return true; }
    virtual bool HasAngle() const override { return true; }
    virtual bool HasXYZ() const override { return true; }

    int getHoffset1() { return highOffset1; }

    virtual void SetHOffset(double o) override;
    virtual int HasCalibration() const override { return calibTyp; }
    virtual bool IsCalibrated() const override { return calibTyp && bCalibOn; }
    virtual void SetCalibTyp(int t);

    virtual bool Set(int pix, int lin) override; // x,y
    char* Ptr(); //!< Pointer to current pixel.
    const char* Ptr() const; //!< Pointer to current pixel.

    virtual double MOffset() const override;
    virtual bool   isZero() const override;
    virtual bool   Angle0(double& henc, double& lenc) const override;
    virtual bool   XYZ0(double& x, double& y, double &z) const override;
    virtual bool   XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;
    virtual void   PostXYZ0Action(double& x, double& y, double& z) const override;

    virtual bool    SetRgmm(int pix, int lin, int rg) override;
    virtual bool    SetRf(int pix, int lin, int rf) override;

    bool Calib(bool c);
    const char* CalibInfo() const { return calibinfo.c_str(); }
    virtual ZFSHeader* GetZFSHeader__() override { return header; }
    virtual const ZFSHeader* GetZFSHeader__() const override { return header; }
    virtual bool WriteHeader() override;

    virtual bool    Map(int32_t& pixel, int32_t& line) const override;
    virtual bool    UnMap(int32_t& pixel, int32_t& line, short mX = -1, short mY = -1) const override;
    int     BytesPerPixel() { return bytesPerPixel; }
    char*   At_(int pixel = 0, int line = 0);

    char*   AdrLine(int lin) override { return LineHeader(lin); } //!< Returns the Address of the current line.
    const char* AdrLine(int lin) const { return LineHeader(lin); } //!< Returns the Address of the current line.

    char*   LineHeader(int lin);
    const char*   LineHeader(int lin) const;
    int     LineHeaderSize() { return lineHeaderSize; }
    ZFLine* getLine(int linNr);

    int calcPixel(int m) const;
    int calcLines(int m, bool round = false) const;
    virtual ZFMask* AddMask(int x, int y, int w, int h, zfs::math::rgb rgb, const wchar_t* name = NULL, bool set = true) override;
    virtual ZFMask* AddMask(zfs::math::rgb rgb, const char* name = NULL, bool set = true) override;
    virtual ZFMask* AddMask(zfs::math::rgb rgb, const wchar_t* name = NULL, bool set = true) override;
    ZFMask* AddMask(zfs::math::rgb rgb, const std::wstring& name, bool set = true) { return AddMask(rgb, name.c_str(), set); }


    /** Online change calibration*/
    void setCalibParams(ZFVector& _skew, double _mirrorAngle, double _cotanCorrection = 0.,
        double _azimumthEncoderGain = 0., double _azimumthEncoderPhase = 0.,
        double _elevationEncoderGain = 0., double _elevationEncoderPhase = 0.,
        double droop = 0., double *mirrorOffset = NULL);

    void getCalibParams(ZFVector& _skew, double& _mirrorAngle, double& _cotanCorrection,
        double& _azimumthEncoderGain, double& _azimumthEncoderPhase,
        double& _elevationEncoderGain, double& _elevationEncoderPhase,
        double& droop, double *mirrorOffset = NULL);

    // Referenzplatte
    enum { REF_OFF, REF_FIXED, REF_LINEAR, REF_LINEHEADER };
    int refTyp{REF_OFF};
    double ref_a{}, ref_b{}, ref_def{};
    void setReferenzTyp(int t) { refTyp = t; }
    int getReferenzTyp() const { return refTyp; }

    virtual double RefRg(int line) const override; //!< meter
    virtual double RefRf(int line) const override; //!< increments

protected:
    void ImagerCalibRange();
    virtual double _Rgmm() const override;
    virtual int    _Rf() const override;
};

//================================================================================================================


// Imager compressed encoder (default)

class ZFS_API ZFScanZFS14 : public ZFScanZFS
{
    DEFINE_ZFBASE(ZFScanZFS)

public:
    ZFTheoModel* theo{};
    ZFAngleToPixel* a2p{};

public:
    ZFScanZFS14();
    virtual ~ZFScanZFS14();

    bool allok() override;

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const wchar_t* param = NULL, ZFRect* r = NULL) override;

    virtual const char* DataTyp() const override { return "ZFSImager"; }

    virtual bool   Angle0(double& henc, double& lenc) const override;
    virtual bool   XYZ0(double& x, double& y, double &z) const override;
    virtual bool   XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;

    virtual bool XYZ2POS(double x_local, double y_local, double z_local, int& xpos, int& ypos, bool forceScdLayer = false, bool forCal = false) override;
};

//================================================================================================================

/**
* sync by encoder
*/

#ifdef SUPPORT_ZFS114

class ZFS_API ZFScanZFS114 : public ZFScanZFS14
{
    DEFINE_ZFBASE(ZFScanZFS14)

protected:
    bool init_{};
    double lEncGrad{};

public:

    ZFScanZFS114();
    ~ZFScanZFS114();

    virtual const char* DataTyp() const override { return "ZFSImager 114"; }

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const wchar_t* param = NULL, ZFRect* r = NULL) override;

    virtual double	Rgmm10() const;
    virtual bool    SetRgmm10(int pix, int lin, double rg);

    virtual double  MOffset() const override;
    virtual bool	Angle0(double& henc, double& lenc) const override;
    virtual bool	XYZ0(double& x, double& y, double &z) const override;
    virtual bool	XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;
    virtual void	SetHOffset(double o) override;

    virtual bool    SetRgm(double m) override; /// take care about rangecalibration
    virtual bool    SetRgmm(int pix, int lin, int rg) override;
    virtual bool    SetRf(int pix, int lin, int rf) override;

protected:
    virtual double	_Rgmm() const override { return Rgmm10(); }
    virtual int		_Rf() const override;
};

#endif

//================================================================================================================

/**
* sync by lara clock
*/

#ifdef SUPPORT_ZFS115

class ZFS_API ZFScanZFS115 : public ZFScanZFS114
{
    DEFINE_ZFBASE(ZFScanZFS114)

public:

    virtual const char* DataTyp() const override { return "ZFSImager 115"; }

    virtual bool	Angle0(double& henc, double& lenc) const override;
    virtual bool	XYZ0(double& x, double& y, double &z) const override;
    virtual bool	XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;
};

#endif

//================================================================================================================

/**
* sync by encoder
*/

#ifdef SUPPORT_ZFS214

class ZFS_API ZFScanZFS214 : public ZFScanZFS114
{
    DEFINE_ZFBASE(ZFScanZFS114)

public:

    virtual const char* DataTyp() const override { return "ZFSImager 214"; }

    virtual bool Angle0(double& henc, double& lenc) const override;
};

#endif

//================================================================================================================

/**
* sync by lara clock
*/

#ifdef SUPPORT_ZFS215

class ZFS_API ZFScanZFS215 : public ZFScanZFS115
{
    DEFINE_ZFBASE(ZFScanZFS115)

public:

    virtual const char* DataTyp() const override { return "ZFSImager 215"; }

    virtual bool Angle0(double& henc, double& lenc) const override;
};

#endif




//================================================================================================================
/**
*	Imager uncompressed encoder
*/

class ZFS_API ZFScanZFS13 : public ZFScanZFS14
{
    DEFINE_ZFBASE(ZFScanZFS14)

public:

    virtual const char* DataTyp() const override { return "ZFSImager enc"; }

    virtual void OnPostInit() override;

    virtual bool Angle0(double& henc, double& lenc) const override;
    virtual bool XYZ0(double& x, double& y, double &z) const override;
    virtual bool XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;

protected:
    virtual double _Rgmm() const override;
    virtual int _Rf() const override;

};


//================================================================================================================

/**
*	Imager as Profiler uncompressed encoder
*/

#ifdef SUPPORT_ZFS15

class ZFS_API ZFScanZFS15 : public ZFScanZFS13
{
    DEFINE_ZFBASE(ZFScanZFS13)

public:

    virtual const char* DataTyp() const override { return "ZFSImagerProfiler"; };

    virtual bool Angle0(double& henc, double& lenc) const override;
    virtual bool XYZ0(double& x, double& y, double &z) const override;
    virtual bool XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;

protected:
    virtual double _Rgmm() const override;
    virtual int _Rf() const override;
};

#endif // SUPPORT_ZFS15

//================================================================================================================

/**
*	Imager as Profiler compressed encoder
*/

#ifdef SUPPORT_ZFS16

class ZFS_API ZFScanZFS16 : public ZFScanZFS14
{
    DEFINE_ZFBASE(ZFScanZFS14)

public:

    virtual const char* DataTyp() const override { return "ZFSImagerProfiler"; };

    virtual bool Angle0(double& henc, double& lenc) const override;
    virtual bool XYZ0(double& x, double& y, double &z) const override;
    virtual bool XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;
};

#endif // SUPPORT_ZFS16

//================================================================================================================

/**
*	ILAR uncompressed encoder
*/

#ifdef SUPPORT_ZFS17

class ZFS_API ZFScanZFS17 : public ZFScanZFS15
{
    DEFINE_ZFBASE(ZFScanZFS15)

public:

    virtual const char* DataTyp() const override { return "ZFSIlar"; };
};

#endif // SUPPORT_ZFS17

//================================================================================================================



/**
*	Profiler
*/

#ifdef SUPPORT_ZFS19

class ZFS_API ZFScanZFS19 : public ZFScanZFS15
{
    DEFINE_ZFBASE(ZFScanZFS15)

public:

    virtual const char* DataTyp() const override { return "ZFSProfiler"; };
};

#endif // SUPPORT_ZFS19

//================================================================================================================

/**
*	Profiler compressed encoder
*/

#ifdef SUPPORT_ZFS20

class ZFS_API ZFScanZFS20 : public ZFScanZFS16
{
    DEFINE_ZFBASE(ZFScanZFS16)

public:

    virtual const char* DataTyp() const override { return "ZFSProfiler"; };
};

#endif // SUPPORT_ZFS20

//================================================================================================================

#ifdef SUPPORT_ZFS_COL1000

#define ZFScolIdx 1000

class ZFScanZFScolImpl;
class ZFScanZFScolImpl5;
class ZFScanZFScolImpl6;


class ZFS_API ZFScanZFScol : public ZFScan
{
    DEFINE_ZFBASE(ZFScan) // implement dynamic class handling

    friend class ZFScanZFScolImpl;
    friend class ZFScanZFScolImpl5;
    friend class ZFScanZFScolImpl6;


protected:
    ZFScanZFScolImpl* impl;
    bool bHasIntens;
    bool bHasRgb;
    bool bHasRange;
    bool bHasAngle;
    bool bHasXYZ;
    ZFSHeader* header;

public:
    ZFScanZFScol();
    virtual ~ZFScanZFScol();

    virtual const char* FileTyp() const override { return "ZFS"; };
    virtual const char* DataTyp() const override { return "ZFScol"; };

    virtual zfs::zfserr_t OpenFile(const std::wstring&, const wchar_t* param = NULL, ZFRect* rc = NULL) override;
    virtual void CloseFile() override;

    virtual ZFSHeader* GetZFSHeader__() override { return header; }
    virtual const ZFSHeader* GetZFSHeader__() const override { return header; }

    virtual int  HasCalibration() const override { return 1; }
    virtual bool IsCalibrated() const override { return true; }

    virtual bool HasIntens() const override { return bHasIntens; }
    virtual bool HasRGB() const override { return bHasRgb; }
    virtual bool HasRange() const override { return bHasRange; }
    virtual bool HasAngle() const override { return bHasAngle; }
    virtual bool HasXYZ() const override { return bHasXYZ; }

    virtual bool Set(int pix, int lin) override;
    virtual char* AdrLine(int lin) override; // Address of line
    virtual bool    Angle0(double& henc, double& lenc) const override; // Angles
    virtual bool    XYZ0(double& x, double& y, double &z) const override;  // without translation/rotation
    virtual bool    XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;
    virtual zfs::math::rgb   rgb() override;
    virtual zfs::math::rgb   RfCol() { return rgb(); }    // color of pixel corresponding rfMode. Calls Rf() so you have to use Set(x,y) before

    // calculate back from 3D point to 2D pixel
    virtual bool XYZ2POS(double x_local, double y_local, double z_local, int& xpos, int& ypos, bool ignoreThisFlag = false /*forceScdLayer allways false*/, bool forCal = false) override;  // returns true if implemented and pixel valid

    virtual bool SetRGB(int pix, int lin, zfs::math::rgb&) override;

protected:
    virtual int    _Rf() const override; // Intensity/Reflectance
    virtual double _Rgmm() const override; // Range mm
};



class ZFS_API ZFScanZFScol1001 : public ZFScanZFScol
{
    DEFINE_ZFBASE(ZFScanZFScol) // implement dynamic class handling

public:
    virtual bool    Angle0(double& henc, double& lenc) const override; // Angles
    virtual bool    XYZ0(double& x, double& y, double &z) const override;  // without translation/rotation
    virtual bool    XYZ0(double rg, double henc, double lenc, double& x, double& y, double &z) const override;
};


#endif // SUPPORT_ZFS_COL1000


//================================================================================================================
//=== Inline-Implementaion =======================================================================================
//================================================================================================================



#include "zfscanimpl_obsolete.h"

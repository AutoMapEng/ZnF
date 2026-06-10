#pragma once

#include "zfsapi.h"
#include "config.h"

#include "zflineaccess.h"
#include "rf.h"

#ifdef ZFS_PTZ_SUPPORT
#define SUPPORT_PTZ_LBL
#endif

#ifdef ZFS_5003_SUPPORT
#define ZFS_LINEACCESS_5003
#endif

#ifdef ZFS_5010_SUPPORT
#define ZFS_LINEACCESS_5010
#endif

#ifdef ZFS_COLOR_SUPPORT
#define ZFS_LINEACCESS_COLOR
#endif

//=======================================================================================================

#ifdef ZFS_LINEACCESS_5003
class ZFS_API ZFLineAccess14 : public ZFLineAccess
{
public:
    ZFLineAccess14( ZFLineInfo* inf );
    ZFLineAccess14( ZFLine*, ZFLineInfo* inf );

    // access to current pixel

    // Set functions
    virtual void SetRf(int32_t inc ) override;
    virtual void SetRgm( double m, double mOffset ) override;
    virtual void SetRgmm(int32_t m ) override;

protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;

    int32_t __Rf(const char* pixel_ptr) const override { return pixel_ptr ? *(unsigned short*)(pixel_ptr + 3) : 0; } // Intensity
//    double Rgm(double refrgoffset) const override; // Range
    double __Rg_mm(const char* pixel_ptr) const override;
};
#endif // ZFS_LINEACCESS_5003

//=======================================================================================================
//
// sync encoder
//

class ZFS_API ZFLineAccess114 : public ZFLineAccess
{
public:
    ZFLineAccess114( ZFLineInfo* inf );
    ZFLineAccess114( ZFLine*, ZFLineInfo* inf );

    virtual double MOffset() const override;		//!< Returns the globaler offset for range.
    virtual void SetMOffest(double o) override;      //!< Herausschreiben des rage offests

    //virtual double  Rgm(double refrgoffset) const override;


    /// return number of valid pixel in line beginning from start
    /// \param checkall checks encoder values and further values as well
    virtual int32_t GetValidPixel( bool checkall=true ) override; 


    virtual double RefRg() const override; //!< referenz of current line (meter)  return 0 if not available
    virtual double RefRf() const override; //!< referenz intensity of current line (increments)  return 0 if not available

    // Set functions
    virtual void SetRf(int32_t inc) override;
    virtual void SetRgm( double /*m*/, double /*mOffset*/ ) override; //!< Set the range of the current pixel. Unit are meter.
    virtual void SetRgmm(int32_t mm) override;

    virtual bool fill_raw_PData() override;

protected:

    bool __isZero(const char* pixel_ptr) const override
    {
        return pixel_ptr[0] == 0 && pixel_ptr[1] == 0 && pixel_ptr[2] == 0 && pixel_ptr[3] == 0 && pixel_ptr[4] == 0;
    }


    int32_t __EncH(int pixel, const char* pixel_ptr) const override
    {
        // nur pixelindex zurueckgeben
        // durch die pos2angle tabelle werden die ausschnitte behandelt und die umrechnung in rad durchgefuehrt
        return pixel;
    }
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;

    int32_t __Rf(const char* pixel_ptr) const override
    {
        return zfs::rf16to24(*(uint32_t*)(pixel_ptr + 3) & 0xffff);
    }

    double __Rg_mm(const char* pixel_ptr) const override
    {
        return (double)(*(uint32_t*)pixel_ptr & 0xffffff) / 10.;
    }
};

//=======================================================================================================

//
// sync lara
//

class ZFS_API ZFLineAccess115 : public ZFLineAccess114
{
public:
    ZFLineAccess115( ZFLineInfo* inf );
    ZFLineAccess115( ZFLine*, ZFLineInfo* inf );

protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;
};


//=======================================================================================================
//
// sync encoder
//

#ifdef ZFS_LINEACCESS_5010

class ZFS_API ZFLineAccess214 : public ZFLineAccess114
{
public:
    ZFLineAccess214( ZFLineInfo* inf );
    ZFLineAccess214( ZFLine*, ZFLineInfo* inf );

protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;
};

//=======================================================================================================

//
// sync lara
//

class ZFS_API ZFLineAccess215 : public ZFLineAccess115
{
public:
    ZFLineAccess215( ZFLineInfo* inf );
    ZFLineAccess215( ZFLine*, ZFLineInfo* inf );

protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;
};

//=======================================================================================================


#endif // ZFS_LINEACCESS_5010

//=======================================================================================================

#ifdef ZFS_LINEACCESS_5003
class ZFS_API ZFLineAccess201 : public ZFLineAccess14
{
public:
    ZFLineAccess201( ZFLineInfo* inf );
    ZFLineAccess201( ZFLine*, ZFLineInfo* inf );

    char getRgc() const;  // 6.byte im pixel fuer cos/sin restaurierung
};

inline char ZFLineAccess201::getRgc() const
{
    if( ptr )
        return *(iLPtr+5); //current pixel + 5
    else
        return 0;
}
#endif // ZFS_LINEACCESS_5003

//=======================================================================================================

#ifdef ZFS_LINEACCESS_COLOR
class ZFS_API ZFLineAccess1000 : public ZFLineAccess
{
protected:
    virtual zfs::math::rgb rgb_col() override;

public:
    ZFLineAccess1000( ZFLineInfo* inf );
    ZFLineAccess1000( ZFLine*, ZFLineInfo* inf );

    virtual void PreInit() override;

    virtual bool Load( int absLn, int ln, zf::ZFFile* fp, bool skip=false ) override; // loads line into memory


protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;

    int __Rf(const char* pixel_ptr) const override;
    double __Rg_mm(const char* pixel_ptr ) const override;
};
#endif // ZFS_LINEACCESS_COLOR

//=======================================================================================================

#ifdef SUPPORT_PTZ_LBL

class CPtzReader;

class ZFS_API ZFLineAccessPTZ : public ZFLineAccess
{
public:
    ZFLineAccessPTZ( ZFLineInfo* inf );
    ~ZFLineAccessPTZ();

    CPtzReader* ptz{};

    // access to current pixel
    virtual int32_t Rf() const override;
    virtual double Rgm() const override; // Range
    virtual int Rgmm() const override;
    virtual int32_t EncH() const override;
    virtual int32_t EncL() const override;

    // Set functions
    virtual void SetRf(int32_t inc ) override;
    virtual void SetRgm( double m, double mOffset ) override;
    virtual void SetRgmm( int32_t m ) override;
};

#endif // SUPPORT_PTZ_LBL


#pragma once

#include "zfsapi.h"
#include "config.h"

#include <memory>
#include <string>

#include "dataaccess.h"
#include "zfvector.h"
#include "mask.h"
#include "../Common/inttypes_.h"

class ZFSHeader;
class Decompress;
class CUnahe;
class ZFSLinebyline;
namespace zf
{
    class ZFFile;
}
namespace zfs
{
    class AtmosphereCorrection;
    class RawParams;
}



//
// holds all info needed by a line
//
class ZFS_API ZFLineInfo
{
public:
    enum { REF_OFF, REF_FIXED, REF_LINEAR, REF_LINEHEADER };
    enum { REFMOUNT_ALWAYS, REFMOUNT_NOTMOUNTED, REFMOUNT_TEMPORARILY };

                                      // ZFSHeader* header;          // stored pointer from call

    std::wstring fileName;            //!< Name of the file.

    bool ok{};                          //!< True if read ok.

                                      // art und weise wie daten abgelegt
    uint32_t typ{};                     //!< Type number of the file.
    uint32_t comp{};                    //!< Compressions typ of the file. (0 = not compressed 1 = zip)
    uint32_t store{};                   //!< Storage typ. (0 = pixel based, 1 = line based, 2=line based but no linezip, 100 = laier based)
    uint32_t lineHeaderSize{};          //!< Size of the lineheader. is start of first line as well
    uint32_t bytesPerPixel{};           //!< Bytes per pixel in memory.
    uint32_t bytesPerPixel_file{};      //!< Bytes per pixel stored in file.
    double   subsam{};                  //!< Subsample increment of full encoder resolution.
    uint32_t counterbyte{};             //!< position of counterybte if scan has one otherwiae 0

    uint32_t pixel360{};                //!< Number of pixels for whole 360 deg.
    uint32_t pixel{};                   //!< Pixel per line.
    uint32_t enc{};                     //!< encoder vert resolution
    uint32_t encL{};                    //!< encoder horz resolution

    double laraClk{};                   //!< laser clock (datarate) [Hz]
    double rps{};                       //!< vertical motor speed [rps]

    uint32_t lsz{};                     //!< linesize
    uint32_t lszxsub{};
    uint32_t lszxsub_file{};

                                      // Intensity
    uint32_t minRf{};
    uint32_t maxRf{};

                                      // Range
    double minrange{};                  ///< [meter]
    double maxrange{};                  ///< [meter]

                                      // referenzplatte
    int32_t refTyp{ REF_OFF };                   //!< typ how reference is calculated
    int32_t refMount{ REFMOUNT_ALWAYS };                 //!< typ of mounted reference
    double ref_a{};                     //!< Referenz gradient a.
    double ref_b{};                     //!< Referenz gradient b.
    double ref_def{};                   //!< Stored referenz value.
    double offsetM{};                   //!< Globaler offset for range in meter.  (default=0.)
    double scaleM{1.};                    //!< Scaling for range.  (default = 1.)
    double offsetHrad{};                //!< Heigh speed encoder offset.
    double offsetLrad{};                //!< Low speed encoder offset.

    double rfvsnoissA{};
    double rfvsnoiseB{};
    double rfvsnoiseC{};
                                      // h sektions
    uint32_t start1{};                  //!< Clip first start.
    uint32_t stop1{};                   //!< Clip first stop.
    uint32_t start2{};                  //!< Clip second start.
    uint32_t stop2{};                   //!< Clip second stop.
    uint32_t sz1{};                     //!< Length of the first clip.
    uint32_t sz2{};                     //!< Length of the second clip.

    uint32_t firstTime{};               //!< First timestamp value if available (ms)

    CUnahe* unahe{};
                                      //void*   impl;               //!< subclass implementierung

                                      // internal buffers
    Decompress* decom{};                //!< decompressor
    char* buf{};
    char* bufpix{};

    std::shared_ptr<zfs::AtmosphereCorrection> atmosphere{}; ///< atmosphere correction
    std::shared_ptr<zfs::RawParams> straylight{};

                                      // Params for GetValidPixel()
    int32_t validPixel_MaxHEncChange{}; // use 0 to disable check
    int32_t validPixel_MaxLEncChange{}; // use 0 to disable check
    int32_t validPixel_MaxCnt1Change{}; // use 0 to disable check
    int32_t validPixel_MaxCnt2Change{}; // use 0 to disable check

public:
    ZFLineInfo(const ZFSHeader* h);
    ZFLineInfo(const ZFLineInfo& o);
    virtual ~ZFLineInfo();

    void ChangePixel(int32_t newPixel);
};

//#############################################################################################################

//
// holds one scanline in memory and allow access to it
//

class ZFS_API ZFLineAccess : public ZFLine
{
    friend class ZFSLinebyline;

protected:
    uint32_t currentPx{};        //!< Current pixel number.
    uint32_t currentLine{};      //!< Line number at the file.
    uint32_t absLine{};          //!< Absolut line number in scan

    char* iLPtr{};               //!< Pointer to current pixel set by Set()

    ZFLineInfo* info{};          //!< Contains the header information of the file. One global info for all lines in one scan

    virtual zfs::math::rgb rgb_col() { return zfs::math::rgb(); }

public:
    bool skip{};                 //!< Flag is not realy loaded. Only Position is changed
    bool ok{};                   // true if line loaded successfully

#ifdef _DEBUG
    uint32_t tmRd{};
    uint32_t tmDecomp{};
    uint32_t tmResort{};
#endif

    uint32_t validPixelError{};// reason if not all pixel valid
    enum {  VP_OK,      // all pixel ok
            VP_ID,      // id of line bad
            VP_LESS,    // less pixel than line
            VP_HENCERR, // problem henc
            VP_LENCERR, // problem lenc
            VP_CNT1ERR, // problem counterbyte of counter 1
            VP_CNT2ERR, // problem counterbyte of counter 2
            VP_CNT3ERR  // problem counter3 (currently not implemented)
    };

public:
    ZFLineAccess( ZFLine*, ZFLineInfo* inf  );
    ZFLineAccess( ZFLineInfo* inf );
    ~ZFLineAccess();

    virtual void PreInit() {}

    bool isSkipt() const { return skip; }
    bool Ok() const { return ok; }

    // reset ptr entries to zero
    void setNULL();

    int32_t TotalLine() const { return absLine; }
    
    int64_t LoadRawLine( zf::ZFFile* fp, std::vector<char>& buf); // buf must have size to hold whole line. Just for internal usage, Line is in binary format which depends on other settings. On success return number of bytes read otherwise 0
    virtual bool Load(  int32_t absLn, int32_t ln, zf::ZFFile* fp, bool skip=false ); // loads and parse line into internal linebuffer
    virtual bool Load( int32_t ln, const char* fp, int32_t size ); // loads and parse line from memory
    virtual bool Raw2Line( bool zip, int32_t rd, const char* raw );
    virtual bool PostProcessLine();

    ZFLineInfo* Info() { return info; }    //!< Returns the header information.
    const ZFLineInfo* Info() const { return info; }    //!< Returns the header information.

    int32_t BytesPerPixel() const { return info->bytesPerPixel; } //!< Returns the number of Bytes per Pixel.
    int32_t Pixel() const { return info->pixel; }                    //!< Returns the number of Pixels of line.

    void setCurrentLine(int32_t ln ) { currentLine=ln; }

    // get pointer to current pixel or zero if not possible
    virtual bool fill_raw_PData() { return false; }


    // Set current pixel. Thread safe
    char* pixel_ptr(int32_t x) const;
    // following not thread safe
    virtual bool Set( int32_t x );                //!< Set pointer to pixel, afterwards you have access like Rf(),...
    int32_t CurrentPixel() const { return currentPx; }  //!< Returns the number of current pixel.
    int32_t CurrentLine() const { return currentLine; } //!< Returns the number of line.
    int32_t AbsolutLine() const { return absLine; }     //!< Returns the number of line of oll files
    virtual char* PPtr() { return iLPtr; }        //!< Pointer to pixel (use with care).
    virtual const char* PPtr() const { return iLPtr; }  //!< Pointer to pixel

    //!< Move pointer to next pixel
    //! obsolete. Do not use
    //! Not thread safe
    virtual bool SetNext();

    virtual int32_t GetValidPixel( bool UNUSED(checkall)=true ) { return info->pixel; } //!< return number of valid pixel in line beginning from start. checkall=false will only check for missing zero pixel
    int32_t ValidStatus() const { return validPixelError; }       //!< reason if not all pixel valid

    // access to current pixel
    virtual char* AdrLine() {return ptr;} //!< Returns Pointer to lineheader.

    int32_t Rf() const { return __Rf(iLPtr); }             //!< Returns the reflection of the current point. Not thread safe
    int32_t Rf(const char* pixel_ptr) const { return __Rf(pixel_ptr); }             //!< Returns the reflection of the current point. Thread safe
    double RfLinear1() const { return RfLinear1(iLPtr); }           //!< Returns linear reflection 0..1 of the current intensity. Not thread safe
                                          //virtual int32_t RfLUT(); //!< Returns the intensity of the current point.
    double RfLinear1(const char* pixel_ptr) const;

    /// checks rf and rg for zero
    bool isZero() const { return __isZero(iLPtr); }
    bool isZero(const char* pixel_ptr) const { return __isZero(pixel_ptr); }

    // get raw range
    double Rgmm() const { return __Rg_mm(iLPtr); } // get raw range. Not thread safe
    double Rgmm(const char* pixel_ptr) const { return __Rg_mm(pixel_ptr); } // get raw range. Thread safe

    /// Returns the range of the current point in meter.
    double Rgm() const { return Rgm(MOffset()); }
    double Rgm(const char* pixel_ptr) const { return Rgm(pixel_ptr,MOffset()); }

    double Rgm(double refrgoffset) const { return Rgm(iLPtr,refrgoffset); }
    double Rgm(const char* pixel_ptr, double refrgoffset) const;


    // High speed encoder value. Typically vertical
    int32_t EncH( int pixel, const char* pixel_ptr ) const { return __EncH( pixel,pixel_ptr); }
    int32_t EncH() const { return __EncH(currentPx, iLPtr); }
    // Low speed encoder value. Typical horizontal
    int32_t EncL(int pixel, const char* pixel_ptr) const { return __EncL(pixel,pixel_ptr); }
    int32_t EncL() const { return __EncL(currentPx, iLPtr); }


    virtual double MOffset() const;        //!< Returns the globaler offset for range.
    virtual void SetMOffest(double o) {}

    virtual double RefRg() const { return 0.; } //!< referenz of current line (meter)  return 0 if not available
    virtual double RefRf() const { return 0.; } //!< referenz intensity of current line (increments)  return 0 if not available

    // access to raw value
    uint32_t intd( int32_t o ) const { return *(uint32_t*)(iLPtr+o); }
    int8_t chard( int32_t o=0 ) const { return *(int8_t*)(iLPtr+o); }
    uint8_t uchard( int32_t o=0 ) const { return *(uint8_t*)(iLPtr+o); }
    int16_t shortd( int32_t o=0 ) const { return *(int16_t*)(iLPtr+o); }
    uint16_t ushortd( int32_t o=0 ) const { return *(uint16_t*)(iLPtr+o); }
    // access to raw value with a already set pointer
    uint32_t intd(const char* pp, int32_t o) const { return *(uint32_t*)(pp + o); }
    int8_t chard(const char* pp, int32_t o = 0) const { return *(int8_t*)(pp + o); }
    uint8_t uchard(const char* pp, int32_t o = 0) const { return *(uint8_t*)(pp + o); }
    int16_t shortd(const char* pp, int32_t o = 0) const { return *(int16_t*)(pp + o); }
    uint16_t ushortd(const char* pp, int32_t o = 0) const { return *(uint16_t*)(pp + o); }

    // Set functions //7.0.0.10 PG warnung beseitigt
    virtual void SetRf( int32_t /*inc*/ ) {} //!< Set reflection of the current pixel.
    virtual void SetRgm( double /*m*/, double /*mOffset*/ ) {} //!< Set the range of the current pixel. Unit ar meter.
    virtual void SetRgmm( int32_t /*m*/ ) {}


    /// set filtered out pixel to zero. \return number of pixel set to zero
    int zeroFilteredOutPixel();
    /// return the number of filtered out points of this line
    int countFilteredOut() const;

    /// set all pixel to zero
    void zeroAllPixel();

    //virtual bool WriteLine( std::fstream* zfsFile, std::string& err );    //!< Writes the current line to the file.
    bool WriteLine(zf::ZFFile* fp, std::string& err ) const;
    bool WriteLineHeader(zf::ZFFile* fp );

protected:
    virtual bool __isZero(const char* pixel_ptr) const { return false; }

    virtual int32_t __EncH( int pixel, const char* pixel_ptr ) const = 0;           // High speed encoder value
    virtual int32_t __EncL( int pixel, const char* pixel_ptr ) const = 0;           // Low speed encoder value

    virtual int32_t __Rf(const char* pixel_ptr) const = 0;             //!< Returns the reflection of the current point.
    virtual double __Rg_mm(const char* pixel_ptr) const = 0;

};

/**
 * @brief Construct a concreate ZFLineAccess from ZFLineInfo
 *
 * The concrete subtype of ZFLineAccess needed to decode RAW data is encoded in
 * ZFSHeader.
 * If no matching implementation exists, nullptr is returned.
 *
 * @param header ZFS-Header
 * @return Concrete ZFLineAccess class
 */
std::unique_ptr<ZFLineAccess> ZFS_API makeLineAccess(ZFLineInfo& info);

//=======================================================================================================



inline double ZFLineAccess::RfLinear1(const char* pixel_ptr) const
{
    const double rf = (double)Rf(pixel_ptr) / info->maxRf;
    if(rf>1.) return 1.;
    return rf;
}


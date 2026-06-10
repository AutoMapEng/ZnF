#pragma once
#include <stdint.h>
#include "zfsapi.h"
#include <string.h>
#include <vector>

#include "pixel.h"

#define DATAACCESS




class ZFSHeader;
class ZFRect;

//###############################################################################################################

/**
 \class ZFLine
 \brief Holds a raw scan line in memory
 \author    Haertl
 */
class ZFS_API ZFLine
{
protected:
    char* ench{ nullptr };      //!< holds expanded encoder if datatyp 215
    int sizeEnch{ 0 };

public:
    char* ptr{ nullptr };		///< Pointer to the line.
    int size{ 0 };		///< Size of the line.
    bool dirty{ false };     ///< flag for data changed
    zfs::PDatas<zfs::PDataDetail> pdatas;


    ZFLine() {}
    ZFLine(int sz) { alloc(sz); }
    virtual ~ZFLine() {} // !!! free darf hier nicht aufgerufen werden, muss von der application selber durchgefuehrt werden

    void alloc(int sz);
    void allocEnc(int pixel);
    void free();
    bool IsDirty() const { return dirty; } /// return true if line has been modified

    // return pointer to line begin
    char* LPtr() { return ptr; }
    const char* LPtr() const { return ptr; }

    /// copy raw line and expanded encoder but not buf
    bool copyFrom(const ZFLine* line);



    //
    // mask
    //

    /// add pdata/mask to the line or resize it if neccessary
    void allocPDatas(int pixel);
    void AddMask(int pixel) { allocPDatas(pixel); }

    /// check wheather the scan has a mask for the line
    bool hasPDatas() const;
    bool HasMask() const { return hasPDatas(); }

    /// check if pixel is enabled (default) or disabled maybe from any filter
    bool IsSet(int32_t pixel) const;

    /// return the typ/reason of the mask
    uint8_t getFilterTyp(int32_t pixel) const;

    /// enable or disable pixel via mask
//    void SetMask(int32_t x, bool value = true) { value ? Enable(x) : Disable(x); }

//    void Enable(int32_t pixel);
//    void Disable(int32_t pixel, uint8_t typ = 1);

    /// disable the whole line (all pixel)
    void DisableAll( uint8_t typ = 1);

    /// enable all pixel in this line
    void EnableAll();


    /// return true if there is a encoder table
    bool EnchAv() const { return ench ? true : false; }
    virtual int EnchVal(int px) const { return ench ? (*(int*)(ench + 3 * px)) & 0x00ffffff : 0; }
    void EnchSet(int px, int val);



    // new interface 
    // support multithreaded access

    zfs::PDataDetail* PData(int pixel)
    {
        if (pixel >= 0 && pixel < static_cast<int>(pdatas.size()))
            return &pdatas.points[pixel];
        return {};
    }
    const zfs::PDataDetail* PData(int pixel) const
    {
        if (pixel >= 0 && pixel < static_cast<int>(pdatas.size()))
            return &pdatas.points[pixel];
        return {};
    }
};

/*
inline void ZFLine::Disable(int32_t pixel, uint8_t typ)
{
    if (pixel < static_cast<int>(maskTyp.size())) maskTyp[pixel] = typ;
}

inline void ZFLine::Enable(int32_t pixel) { if (pixel < static_cast<int>(maskTyp.size()))  maskTyp[pixel] = 0; }
*/


inline bool ZFLine::IsSet(int32_t pixel) const
{
    if (pdatas.points.empty()) return true;
    if (pixel >= static_cast<int32_t>(pdatas.points.size())) return false;
    return pdatas.points[pixel].mask == 0 ? true : false;
//    return pixel < static_cast<int>(maskTyp.size()) ? maskTyp[pixel]==0 : true;
}

inline uint8_t ZFLine::getFilterTyp(int32_t pixel) const
{
    if (pdatas.points.empty()) return 0;
    if (pixel >= static_cast<int32_t>(pdatas.points.size())) return 0;
    return pdatas.points[pixel].filterID;
}

//###############################################################################################################

class ZFDataAccess;
class Decompress;
class BufferReadParam;
namespace zf
{
    class ZFFile;
}

/**
 \class ZFDataBuffer
 \brief Holds all lines of ZFLine of a scan in memory
 \author    Haertl
 */
class ZFDataBuffer
{
    friend class ZFDataAccess;

    ZFSHeader* header{ nullptr };

    int lineHeaderSize{ 0 };
    int bytesPerPixel{ 0 };
    int pixel{ 0 };          ///< not rotated value
    int lines{ 0 };          ///< not rotated value
    int subsample{ 1 };
    int encH{ 0 };

    /**
     \class Buffers
     \brief Used while reading line from file
     \author    Haertl
     */
    class Buffers
    {
    public:
        char* buf{ nullptr };
        char* bufpix{ nullptr };
        char* uncomp{ nullptr };

        Buffers() {}
        virtual ~Buffers()
            { Free(); }

        void Alloc(int sz);
        void Free();

        /// return true if memory allocated
        bool Ok() const
            { return buf && bufpix && uncomp; }
    };

public:
    char info[256]{0};

public:
    ZFDataBuffer() {}
    virtual ~ZFDataBuffer() { free(); }

    std::vector<ZFLine> lin;

//    void setSubsample(int s) { subsample = s; }

    /// read all lines
    enum rd_t
    {
        RD_OK = 1,

        RD_UNDEF = -1,
        RD_READ_HEADER_FAILED = -2,
        RD_START_STOP_WRONG = -3,
        RD_NO_PIXEL_OR_LINES = -4,
        RD_OPEN_FILE = -5,
        RD_ZIP = -6,
        RD_ALLOC = -7,
        RD_USER_ABORT = -8,
        RD_READ_LINE = -9,
        RD_NOT_IMPLEMENTED = -10,   ///< type not supported
        RD_EXPAND = -11,
        RD_315_STARTANGLE = -12
    };
    rd_t readData(const std::wstring& fileName, ZFRect*, bool callCallbacks);

    /// read just a single line
    rd_t readLineFromZFS(int l, ZFLine&, int& sub, zf::ZFFile& file, const BufferReadParam& pr, ZFDataBuffer::Buffers& b, Decompress* d);
    rd_t readLineFromZFS__(int l, ZFLine&, ZFLine&, int& sub, zf::ZFFile& file, const BufferReadParam& pr, ZFDataBuffer::Buffers& b, Decompress* d);

    void free();

    /**
    \brief access a line
    */
    ZFLine& operator[](int l) { return lin[l]; }

    const char* Info() const { return info; }
};

/**
 \class ZFDataAccess
 \brief allows access to individual pixels of the lines in memory
 \author    Haertl
 */
class ZFDataAccess
{
    char* ptr{};	        ///< pointer to pixel in line
    ZFDataBuffer* buf{};    ///< the line

public:
    ZFDataAccess(ZFDataBuffer*);
    virtual ~ZFDataAccess();

//    void setSubsample(int s) { buf->setSubsample(s); }
    int readData(const std::wstring& fileName, ZFRect* rc) { int rd = buf->readData(fileName, rc, true); set(0, 0); return rd; }
    void free() { buf->free(); }

    bool isValid(int ln) const
    {
        return buf && ln>=0 && ln<(int)buf->lin.size() && buf->lin[ln].ptr;
    }

    /**
     \fn    char* ZFDataAccess::set(int p, int l)
     \brief set pointer to a defined pixel
     \author    Haertl
     \param p   pixel
     \param l   line
     \return    null if it fails, else a pointer to the pixel
     */
    char* set(int p, int l)
        { if (isValid(l)) return ptr = buf->lin[l].ptr + buf->lineHeaderSize + p*buf->bytesPerPixel; return ptr = nullptr; }

    unsigned short shortd(int o) const
        { return ptr ? *(unsigned short*)(ptr + o) : 0; }

    unsigned int intd(int o) const
        { return ptr ? *(unsigned int*)(ptr + o) : 0; }

    unsigned short* shortdp(int o) const
        { return ptr ? (unsigned short*)(ptr + o) : 0; }

    char chard(int o = 0) const
        { return ptr ? *(char*)(ptr + o) : 0; }

    char* chardp(int o = 0)
        { return ptr ? (char*)(ptr + o) : 0; }

    char* lineHeader(int l)
        { return ptr ? buf->lin[l].ptr : 0; }
    const char* lineHeader(int l) const
        { return ptr ? buf->lin[l].ptr : 0; }

    ZFLine& Lin   (int l) { return (*buf)[l]; }
    ZFLine* LinPtr(int l) { return &((*buf)[l]); }
    const char* Info() const { return buf->info; }
};


//###############################################################################################################

int detectLines(ZFSHeader* header, const wchar_t* fileName);

//###############################################################################################################


inline char* blk2pix_lheader(int lhs, char* ptr, char* dest)
{
    // lineheader
    memcpy(dest, ptr, lhs);
    return dest + lhs;
}

//===============================================================================================================

// umsortieren: block -> pixel orientated
inline void blk2pix_4bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 2;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = 0;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        if (sub)
        {
            prg += 2 * sub;
            prf += 2 * sub;
        }
    }
}

//===============================================================================================================

__forceinline void blk2pix_5bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    short* prg = (short*)(ptr);
    short* prf = (short*)((char*)prg + pixel*sub * 2);
    char* penc = (char*)prf + pixel*sub * 2;

    if (sub >= 1)
    {
        sub--;
        while (pixel)
        {
            *dest++ = *penc++;
            *(short*)dest = *prg++; dest += 2;
            *(short*)dest = *prf++; dest += 2;

            penc += sub;
            prg += sub;
            prf += sub;

            pixel--;
        }
    }
    else
    {
        while (pixel)
        {
            *dest++ = *penc++;
            *(short*)dest = *prg++; dest += 2;
            *(short*)dest = *prf++; dest += 2;

            pixel--;
        }
    }
}

//===============================================================================================================

inline void blk2pix_6bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    int* prg = (int*)(ptr);  // range
    short* prf = (short*)((char*)prg + pixel*sub * 3); // intensity
    char* penc = (char*)prf + pixel*sub * 2; // encoder

    if (sub > 1)
    {
        int subb = sub;
        sub--;
        while (pixel)
        {
            *dest++ = *penc++;
            *(int*)dest = (*prg) & 0xffffff; prg = (int*)((char*)prg + 3 * subb); dest += 3;
            *(short*)dest = *prf++; dest += 2;

            penc += sub;
            //            prg += sub;
            prf += sub;

            pixel--;
        }
    }
    else
    {
        while (pixel)
        {
            *dest++ = *penc++;
            *(int*)dest = (*prg) & 0xffffff; prg = (int*)((char*)prg + 3); dest += 3;
            *(short*)dest = *prf++; dest += 2;

            pixel--;
        }
    }
}

//===============================================================================================================
/*
    ptr   = blockweiser inputbuffer
    dest  = pixel ousgabebuffer
    pixel = anzahl
    sub   = subsample beim einlesen
*/
inline void blk2pix_7bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    int* prg = (int*)(ptr);  // range
    short* prf = (short*)((char*)prg + pixel*sub * 3); // intensity
    char* penc = (char*)prf + pixel*sub * 2; // encoder

    if (sub > 1)
    {
        int subb = sub;
        sub--;
        while (pixel)
        {
            *(int*)dest = (*prg) & 0xffffff; prg = (int*)((char*)prg + 3 * subb); dest += 3;
            *(short*)dest = *prf++; dest += 2;
            *dest++ = *penc++;
            dest++;  penc++;

            penc += sub;
            //            prg += sub;
            prf += sub;

            pixel--;
        }
    }
    else
    {
        while (pixel)
        {
            *(int*)dest = (*prg) & 0xffffff; prg = (int*)((char*)prg + 3); dest += 3;
            *(short*)dest = *prf++; dest += 2;
            *dest++ = *penc++;
            *dest++ = *penc++;

            pixel--;
        }
    }
}

//===============================================================================================================

inline void blk2pix_8bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 2;
    char* penc = prf + pixel*sub * 2;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        if (sub)
        {
            penc += 4 * sub;
            prg += 2 * sub;
            prf += 2 * sub;
        }
    }
}

//===============================================================================================================

inline void blk2pix_11bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 3;
    char* penc = prf + pixel*sub * 2;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        if (sub)
        {
            penc += 6 * sub;
            prg += 3 * sub;
            prf += 2 * sub;
        }
    }
}

inline void blk2pix_11bbytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 3;
    char* penc = prf + pixel*sub * 3;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        if (sub)
        {
            penc += 6 * sub;
            prg += 3 * sub;
            prf += 3 * sub;
        }
    }
}

/*
inline void blk2pix_7bytes9( char* head, char* ptr, char* dest, int pixel, int sub )
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub*3;
    char* penc = prf + pixel*sub*2;

    sub --;

    int enc = *(int*)(head+4);
    for( int p=0; p<pixel; p++ )
    {
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        enc += *(unsigned char*)penc++; // high encoder
        *dest++ = enc & 0xff;
        *dest++ = (enc>>8) & 0xff;
        *dest++ = (enc>>16) & 0xff;
        *dest++ = *penc++; // low encoder
        if( sub )
        {
            prg += 3*sub;
            prf += 2*sub;
            penc += 2*sub;
        }
    }
}*/

inline void blk2pix_7bytes9(char* /*head*/, char* ptr, char* dest, int pixel, int sub, int enc)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 3;
    char* penc = prf + pixel*sub * 2;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        enc += *(unsigned char*)penc++; // high encoder
        *dest++ = enc & 0xff;
        *dest++ = (enc >> 8) & 0xff;
        *dest++ = (enc >> 16) & 0xff;
        *dest++ = *penc++; // low encoder
        if (sub)
        {
            prg += 3 * sub;
            prf += 2 * sub;
            penc += 2 * sub;
        }
    }
}

//===============================================================================================================

inline void blk2pix_115_7bytes9(char* /*head*/, char* ptr, char* dest, int pixel, int sub, int enc)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 3;
    char* pench = prf + pixel*sub * 2;
    char* pencl = pench + pixel*sub;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prf++;
        *dest++ = *prf++;
        enc += *(unsigned char*)pench++; // high encoder
        *dest++ = enc & 0xff;
        *dest++ = (enc >> 8) & 0xff;
        *dest++ = (enc >> 16) & 0xff;
        *dest++ = *pencl++; // low encoder
        if (sub)
        {
            prg += 3 * sub;
            prf += 2 * sub;
            pench += sub;
            pencl += sub;
        }
    }
}

//===============================================================================================================

inline void blk2pix_115_8bytes10(char* /*head*/, char* ptr, char* dest, int pixel, int sub, int enc)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 3;
    char* pench = prf + pixel*sub * 2;
    char* pencl = pench + pixel*sub;
    char* padd = pencl + pixel*sub;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        // rg
        *dest++ = *prg++;
        *dest++ = *prg++;
        *dest++ = *prg++;
        // rf
        *dest++ = *prf++;
        *dest++ = *prf++;
        // ench,encl,add
        enc += *(unsigned char*)pench++; // high encoder
        *dest++ = enc & 0xff;
        *dest++ = (enc >> 8) & 0xff;
        *dest++ = (enc >> 16) & 0xff;
        *dest++ = *pencl++; // low encoder
        *dest++ = *padd++; // additional
        if (sub)
        {
            prg += 3 * sub;
            prf += 2 * sub;
            pench += sub;
            pencl += sub;
            padd += sub;
        }
    }
}

//===============================================================================================================

inline void blk2pix_12bytes(char* ptr, char* dest, int pixel, int sub)
{
    // linedata
    char* prg = ptr;
    char* prf = prg + pixel*sub * 4;
    char* penc = prf + pixel*sub * 4;

    sub--;

    for (int p = 0; p < pixel; p++)
    {
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *penc++;
        *dest++ = *prg++; //rgc
        *dest++ = *prg++;
        *dest++ = *prg++; //rfc
        *dest++ = *prg++;
        *dest++ = *prf++; //rgf
        *dest++ = *prf++;
        *dest++ = *prf++; //rff
        *dest++ = *prf++;
        if (sub)
        {
            penc += 4 * sub;
            prg += 4 * sub;
            prf += 4 * sub;
        }
    }
}

//===============================================================================================================


inline void blk2pix_2xx(char* ptr, char* dest, int pixel, int bpp)
{
    const int mxbpp = 32;
    if (bpp < mxbpp)
    {
        char* p[mxbpp];
        for (int i = 0; i < bpp; i++)
            p[i] = ptr + i*pixel;

        for (int j = 0; j < pixel; j++)
        {
            for (int k = 0; k < bpp; k++)
                *dest++ = *(p[k])++;
        }
    }
}

//===============================================================================================================

inline void ZFLine::EnchSet(int px, int val)
{
    char* l = ench + 3 * px;
    *l++    = val         & 0xff;
    *l++    = (val >> 8)  & 0xff;
    *l      = (val >> 16) & 0xff;
}

//===============================================================================================================


bool ZFS_API ExpandHEncoder215(int pixel, ZFLine& ln, int lineHeaderSize, int bytesPerPixel, int sz1, int sz2, int enc1, int enc2);

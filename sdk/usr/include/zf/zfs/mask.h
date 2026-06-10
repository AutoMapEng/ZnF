#pragma once

#include "zfsapi.h"
#include "zfbase.h"
#include "zfrgb.h"
#include <zfcore/filename.h>

#include <list>
#include <utility>
#include <string>
#include <stdint.h>

class ZFScan;
class ZFScanZFS;




/**
*	Filter definition name and color
*/
class FilterMaskInfo
{
public:
    std::wstring name;
    zfs::math::rgb color;

    FilterMaskInfo(const std::wstring& n, const zfs::math::rgb& c) : name(n), color(c) {}
};

extern ZFS_API FilterMaskInfo MASK_INFO_MIXPIXEL;
extern ZFS_API FilterMaskInfo MASK_INFO_QUALITY;
extern ZFS_API FilterMaskInfo MASK_INFO_INVALIDATE;
extern ZFS_API FilterMaskInfo MASK_INFO_COMPENSATOR;


/**
 *@brief mask for whole scan or a single part 
 */
class ZFS_API ZFMask : public ZFBase
{
    DEFINE_ZFBASE(ZFBase)

    friend class ZFScan;
    friend class ZFSLinebyline;
    friend class LineByLineMasksOfPart;

public:
    enum flag_t : uint32_t
    {
        FIXED = (1<<0),
    };

protected:
    uint32_t w{ 0 };     ///< width
    uint32_t w32{ 0 };   ///< width int32 memory
    uint32_t h{ 0 };     ///< height
    zfs::math::rgb rgb;      ///< color of mask
    std::wstring name;  ///< name of the mask
    uint32_t flags{0}; ///< see flag_t
    uint32_t* mask{ nullptr };
    int8_t filter_id{}; ///< optional the filter mask id

    ZFScanZFS* zfs{ nullptr }; ///< the scan which belongs to the mask
    uint32_t normalScanAccessMode{};

    bool useGlobal{ true };				//PG//!< Use this mask for calculatin global mask the default is true

public:
    ZFMask();
    virtual ~ZFMask();
    ZFMask(const ZFMask& other);    ///< Makes a deep copy of the mask array, but not the zfs.

    ZFMask& operator=(ZFMask other);

    void Alloc(int w, int h, zfs::math::rgb rgb, const std::wstring& name = L"", bool set = true, ZFScanZFS* z = NULL);
    void Free();
    virtual void Erase(bool set = true, bool sync = true);
    bool HasMask() const { return mask ? true : false; }

    void setFlags(uint32_t fx) { flags = fx; }
    void setFlagBit(uint32_t fx) { flags |= fx; }
    void clrFlagBit(uint32_t fx) { flags &= ~fx; }
    uint32_t getFlags() const { return flags; }

    void setFilterID( uint8_t i) { filter_id =i; }
    uint8_t getFilterID() const { return filter_id; }

    const std::string NameC() const;
    const std::string NameUTF8() const;
    const std::wstring NameW() const;
    void SetNameW(const std::wstring& n) { name = n; }

    int SizeX() const { return w; }
    int SizeY() const { return h; }
    int Size() const { return SizeX() * SizeY(); }

    virtual void Set(int x_, int y_, bool map = true) { if (mask && x_ >= 0 && y_ >= 0 && x_ < (int)w && y_ < (int)h) *(mask + (x_ >> 5) + y_*w32) |= (1 << (x_ & 0x1f)); }
    virtual void Clr(int x_, int y_, bool map = true) { if (mask && x_ >= 0 && y_ >= 0 && x_ < (int)w && y_ < (int)h) *(mask + (x_ >> 5) + y_*w32) &= ~(1 << (x_ & 0x1f)); }
    virtual bool IsSet(int x_, int y_, bool map = true) const;
    bool IsVis(int x, int y) const { return IsSet(x, y); }
    zfs::math::rgb Rgb() const { return rgb; }

    //lowlevel
    unsigned char* mem() const { return (unsigned char*)mask; }                  // pointer to memory
    uint32_t memsize() const { return w32*h * sizeof(uint32_t); } // size of mask in bytes
    size_t maskSize() const { return (size_t)w32*h; }   // number of elements in the mask array
    int memsizeX() const { return w32 * sizeof(uint32_t); } // bytes

    /** if use Global is set to false the mask is not used for calculating global mask*/
    void useMaskGlobal(bool bo) { useGlobal = bo; }

    /** returns true if use to calculate global mask */
    bool useForGlobal() const { return useGlobal; }

    void setScan(ZFScanZFS* scan) { zfs = scan; }

    /// count the cleared/masked pixels
    int numberOfMaskedPixel() const;

    // find the first cleared pixel
    std::pair<int, int> findFirst() const;

private:
    void swap(ZFMask& other) throw();
};


/**
 * mask for only a partial of a scan
 */
class ZFS_API ZFMaskPartial : public ZFMask
{
protected:
    int x{};
    int y{};
    ZFScan* parent{};
    ZFMask* global{};

public:
    ZFMaskPartial(ZFScan* p, ZFMask* glb) : ZFMask() { x = y = 0; parent = p; global = glb; }

    void Alloc(int x_, int y_, int wd, int hg, zfs::math::rgb rgb_, const std::wstring& name_=L"", bool set = true, ZFScanZFS* z = NULL) { x = x_; y = y_; ZFMask::Alloc(wd, hg, rgb_, name_, set, z); }
    virtual void Erase(bool set = true, bool sync = true) override;

    int StartX() const { return x; }
    int StartY() const { return y; }

    virtual void Set(int x_, int y_, bool map = true) override;
    virtual void Clr(int x_, int y_, bool map = true) override;
    virtual void SetNoSync(int x_, int y_);
    virtual void ClrNoSync(int x_, int y_);
    virtual bool IsSet(int x_, int y_, bool map = true) const override;
};

/**
*   This class contains information about a mask. For example number of masked pixel.
*/
class ZFS_API ZFMaskInfo
{
public:
    std::wstring scanName;      //< Name of the scan which contains the current mask.
    std::wstring maskName;      //< Name of the mask
    size_t nrSelectedPixel;        //< Number of selected pixel
    size_t nrPixelMask;            //< Number of pixels in the mask
    size_t nrPixelScan;            //< Number ob pixels in the scan

public:

    /**
    *   Create Info object of the mask.
    *   \param scanName_ => Name of the scanfile
    *   \param nrPixelScan_ => Number of pixels in the scan
    *   \param mask => Mask
    */
    ZFMaskInfo( const std::wstring& scanName_, size_t nrPixelScan_, const ZFMask& mask);

    /**
    \brief   Returns percent of masked pixel
    */
    double getPercentOfScan() const;

    /**
    \brief return valid pixel (all - masked)
    */
    uint64_t validPixel() const;
};


typedef std::list <ZFMaskPartial*> mask_lst;
typedef std::list <FilterMaskInfo> FilterMaskInfoList;

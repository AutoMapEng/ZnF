#pragma once

#include <vector>
#include "zflineaccessimpl.h"
//#include "pixel.h"

#ifdef ZFS_5010_SUPPORT
#define ZFS_LINEACCESS_5016
#endif

#ifdef ZFS_LINEACCESS_5016

class ZFModel;


namespace zfs
{
    class SubEncoderFractionalValMappingTables;
}

//
// 314 enc
//
class ZFS_API ZFLineAccess314 : public ZFLineAccess214
{
private:

public:

    ZFLineAccess314(ZFLineInfo* inf);
    ZFLineAccess314(ZFLine*, ZFLineInfo* inf);

    double MOffset() const override;		//!< Returns the globaler offset for range.

protected:
    /// vertical encoder
    virtual int32_t __EncH(int pixel, const char* pixel_ptr) const override
    {
        /*    if (EnchAv())
            {
                return (*(int32_t*)(ench + 3 * currentPx)) & 0x00ffffff;
            }*/
        return pixel;
    }
    /// horz encoder
    virtual int32_t __EncL(int pixel, const char* pixel_ptr) const override
    {
        const int32_t linc = *((int32_t*)&ptr[8]); // l-encoder wert im lineheader
        const short dl = shortd(pixel_ptr,5);
        if (dl < -20000)
        {
            const int32_t v = (int32_t)(0xff000000 | (-(int32_t)info->encL & 0xff0000) | (uint16_t)dl);
            return v + linc;
        }
        return linc + dl;
    }

    int32_t __Rf(const char* pixel_ptr) const override;
    double __Rg_mm(const char* pixel_ptr) const override;

};



//
// 315 lara
//
class ZFS_API ZFLineAccess315 : public ZFLineAccess215
{
public:

    ZFLineAccess315(ZFLineInfo* inf);
    ZFLineAccess315(ZFLine*, ZFLineInfo* inf);


protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override
    {
        if (EnchAv())
        {
            return (*(int32_t*)(ench + 3 * pixel)) & 0x00ffffff;
        }
        return pixel;
    }

    int32_t __EncL(int pixel, const char* pixel_ptr) const override
    {
        const int32_t linc = *((int32_t*)&ptr[8]); // l-encoder wert im lineheader
        const short dl = shortd( pixel_ptr, 7);
        if (dl < -20000)
        {
            const int32_t v = (int32_t)(0xff000000 | (-(int32_t)info->encL & 0xff0000) | (uint16_t)dl);
            return v + linc;
        }
        return linc + dl;
    }

    int32_t __Rf(const char* pixel_ptr) const override;
    double __Rg_mm(const char* pixel_ptr) const override;
};




ZFS_API bool ExpandHEncoder315(int pixel, ZFLine& ln, int lineHeaderSize, int bytesPerPixel, int sz1, int sz2, int enc1, int enc2, int encH= 1/*, const double& rps = 1, const double& laraClk = 1*/);


//##################################################################################################################################################################################################################


// ======================================================================================

#endif // ZFS_LINEACCESS_5016

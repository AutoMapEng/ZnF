#pragma once

#include <vector>
#include "zflineaccessimpl315.h"

#ifdef ZFS_5018_SUPPORT
#define ZFS_LINEACCESS_5018
#endif

#ifdef ZFS_LINEACCESS_5018

namespace zfs
{
    class SubEncoderFractionalValMappingTables;
}

//
// 315 lara
//
class ZFS_API ZFLineAccess415 : public ZFLineAccess315
{
public:

    ZFLineAccess415(ZFLineInfo* inf);
    ZFLineAccess415(ZFLine*, ZFLineInfo* inf);



    virtual double RefRg() const override;
    virtual double RefRf() const override;


    /// number of supported intensity channels
    int channelsSupported() const { return 5; }

    /// return intensity of channel ch (0..4)  !no range check
    uint32_t rfch(const char* pixel_ptr, int ch) const;

    /// set new intensity for selected channel
    void setRfCh(uint32_t ch, uint32_t rf);


    /// select the channel to view
    void setRfChannel(int ch) { if (ch >= 0 && ch < channelsSupported()) channel = ch; }

protected:
    int32_t __EncH(int pixel, const char* pixel_ptr) const override;
    int32_t __EncL(int pixel, const char* pixel_ptr) const override;

    int32_t	__Rf(const char* pixel_ptr) const override { return static_cast<int32_t>(rfch(pixel_ptr,channel)); }

private:
    int channel = 0;
};

ZFS_API bool ExpandHEncoder415(int pixel, ZFLine& ln, int lineHeaderSize, int bytesPerPixel, int sz1, int sz2, int enc1, int enc2, int encH= 1, const double& rps= 1, const double& laraClk= 1 );

#endif // ZFS_LINEACCESS_5018

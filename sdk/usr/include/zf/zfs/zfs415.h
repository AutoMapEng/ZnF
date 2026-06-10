#pragma once

#include "zfsapi.h"
#include "config.h"

#include "zfscanimpl.h"
#include "zfs315.h"



#ifdef SUPPORT_ZFS415


/**
\brief  a scan with 5 intensity channels
        sync by lara clock
*/
class ZFS_API ZFScanZFS415 : public ZFScanZFS315
{
    DEFINE_ZFBASE(ZFScanZFS315)

public:
    ZFScanZFS415();

    virtual const char* DataTyp() const override { return "ZFSImager 415"; };

    /// number of supported intensity channels
    int channelsSupported() const { return 5;  }

    /// some further init
    virtual zfs::zfserr_t OnOpen() override;
    
    /// return intensity of selected channel

    virtual bool Angle0(double& henc, double& lenc) const override;

    /// select the channel to view
    void setRfChannel(int ch) { if (ch >= 0 && ch < channelsSupported()) channel = ch; }

    /// return selected channel
    int rfChannel() const { return channel; }

    /// return intensity of channel ch (0..4)  !no range check
    uint32_t rfch(int ch) const;

    /// set new intensity for selected channel
    void setRfCh(uint32_t ch, uint32_t rf);

    virtual double RefRg(int line) const override; //!< meter
    virtual double RefRf(int line) const override; //!< increments

protected:
    int channel = 0;
    virtual int	_Rf() const override { return static_cast<int>(rfch(channel)); }
};





#endif // SUPPORT_ZFS315

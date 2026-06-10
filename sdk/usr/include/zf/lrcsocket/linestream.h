#pragma once


#include <string>
#include <memory>

#include "lrcsocketapi.h"

#include "udplinestream.h"
#include "../zfs//Math/zfvec.h"
#include "../zfs/pixel.h"




//####################################################################################################

/*
    class reciveing udp linestream

    !!! There is a newer class Stream which is recommended to use !!!

        scan command must have additional param "-linestream"
            or "-linestream all" for receiving all lines in realtime
            and "-ip <addr>" to define the ip-adrress where to send data


What the scanner sends is the same as the scanner stores in the zfs file.
So you get a zfs file structure over udp/ip.
Every line is send as a single udp package.
If line does not fit into one udp, line is splitted into severel parts.
The head structure is a structure around the line which has information about the transport.
If line is splitted the head describes which part of the line it is.
You normaly do not need the information in the head description.
If a line is very smale, two lines are will be transfered in one udp package,
but you will get two calls of OnData, for every real line one.
Independent of the spliting or combining you will get one call of OnData for a whole single line

You may also lose some lines because the operating system miss some UDP packages.

The data you get over UDP are not calibrated.
So you need a valid zfs-header which is send once at the beginning of the scan.
You have to test if you have a zfsheader received.
With this header you can calibrate with the zfs.dll the current line and get accurate xyz values.
The calibration works only in 3d space.

!!! Library did not implement all kind of scans/data-types
    If your datatyp is missed, please ask for update
*/

class ZFLinedataStreamAccess;
class ZFSHeader;
class ZFSFilter;
namespace lrcsocket
{
    class ZFStreamDeviceInfo;
}

#define MAX_LINDATA_STREAMS		MAX_DEVS	// number of maximum received streams
typedef unsigned int id_t;



/**
\brief handles the stream commng from a socket interface
        !!! Please try to use class Stream instead !!!
        Just for intarnal usage only
*/
class LRCSOCKET_API ZFLinedataStream : public lrcsocket::UDPLinedataStream
{
    ZFLinedataStreamAccess* access[MAX_LINDATA_STREAMS]{};

protected:

    int currentPixel = 0; // set by Set()

    unsigned int m_dwLastTickCount = 0;// mig [10/11/2006]


public:
    ZFLinedataStream();
    virtual ~ZFLinedataStream(); // disconnects as well

	/// return the max number of supported streams
	int maxStreamsSupported() const { return MAX_LINDATA_STREAMS; }

    /// will be called before bind
//    virtual void configSocket(lrcsocket::zfsocket sock_) { /*setSockOptIncreaseBuffer(sock_);*/ }


    /// check if zfs-header available
    bool HasZFSHeader(id_t id) const;

    /// get pointer to zfsheader otherwise nullptr
    ZFSHeader* GetZFSHeader(id_t id);

    bool SetZFSHeader(id_t id, ZFSHeader*);

    /// return the filtered datastream. Need a valid zfs-header received. Called after OnData from OnRAWData
    virtual void OnFilteredLine(const zfs::PDatas<zfs::PDataDetail>& line) {}

    /// highlevel called by OnRAWData, filtering is called
    virtual void OnData( // receive full raw line. converts raw data to xyz and calls OnXYZ()
        const lrcsocket::ZFStreamDeviceInfo&,     // information about the device, libary can receive streams from several devices
        const lrcsocket::LineRawInfo&,    // see description of struct
        const char* /*head*/,  // head of the received buffer
        const char* /*line*/,  // line with lineheader
        int /*pixel*/)
    {}    // number of pixel in line

    /// received command
	virtual void OnCmd(const lrcsocket::ZFStreamDeviceInfo&, const char* commandString ) {}; 


    // misc
    bool CalibAngle(id_t, int hidx, double rg, double& h, double& l);
    bool hasCalibration(id_t) const; // mig [9/27/2006]

	// get info just from the last received package. do not use if there different kind of scanners connected at the same time
    int getTyp() const { return typ; }
    int getPixel() const { return pixel; } ///< return pixel count per line
    int getVersion() const { return ver; }
    int getLineheaderSize() const { return lineHeaderSize; }
    int getBytesPerPixel() const { return bytesPerPixel; }


    ZFLinedataStreamAccess& Get(int id);
    const ZFLinedataStreamAccess& Get(int id) const;


    /// only override if you want to implement everything by your own like filtering
    virtual void OnRAWData(const lrcsocket::ZFStreamDeviceInfo& dev, const char* in, const lrcsocket::LineRawInfo& lri, const char* head, const char* line, int pixel, int lsz) override;
    virtual void OnRAWFileHeader(const lrcsocket::ZFStreamDeviceInfo&, const char* in, const ZFSHeader&) override; // receive a fileheader at start of scan, base implementation must be called to have access to calibrated values
    virtual void OnRAWCmd(const lrcsocket::ZFStreamDeviceInfo&, const char*) override; // receive command

public:
    // see zfs.dll for more informations

    // call from your local OnData implementation
    // id_t id must be 0 if only one stream is used

    /// check if data has a mechanical calibration 0=no-calib otherwise return calibtyp
    int IsCalib(id_t id = 0) const;

    /// Datatyp (zfs-header index 1)
    int Typ(id_t id = 0) const;

    /// used subsample
    int HSubsample(id_t id = 0);

    /// set current pixel you will access
    void Set(id_t, int pixel);

    /// check if mask bit for pixel is set or not
    bool IsSet(id_t, int pixel) const;

    // check if Filter fuction filtered out that point (nonzero)
    int isFilteredOut(id_t, int pixel) const;

    /// get range of pixel [meter]
    double Rgm(id_t id = 0) const;

    /// get intensity depending on system [increment]
    int Rf(id_t id = 0) const;

    /// get normalized intensity 0..1 of pixel
    double RfNorm(id_t id = 0) const;

    /// calculates xyz if it is a standard zfs scan (zfs scan header must be available) returns local coordinates
    bool XYZ0(id_t, double& x, double& y, double& z);
    zfs::math::Vec3f XYZ0(id_t);

    /// return world coordinates
    bool XYZ(id_t, double& x, double& y, double& z);

    // rangeoffset used from referenz
    double OffsetM(id_t id = 0) const;

    /// linenumber stored in the lineheader
    uint32_t lineNumber(id_t = 0) const;

    //
    // lowlevel
    //

    /// Address of pixel set by Set(). Allows access to all raw bytes. Return NULL if not possible
    char* Ptr(id_t id = 0);

    /// Filter the whole line, check for filtered out pixel with IsSet
    /// !!! obsolete
    void Filter(id_t id = 0);

	const ZFSFilter* GetFilterParam(id_t id = 0) const;

    void SetFilterParam(const ZFSFilter& param, id_t id = 0);

    /// reset filter mask
    void ResetFilterMask(id_t id = 0);


    static void XYZ2polar(id_t, const double x, const double y, const double z, double &henc_rad, double &lenc_rad, double* rg);
};


//####################################################################################################


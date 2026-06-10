#pragma once

#include <memory>

#include "lrcsocketapi.h"
#include "lrc_def.h"
#include "udpstream.h"
#include "ports.h"


namespace std
{
	class thread;
}


//####################################################################################################

/*
    class reciveing udp statusstream

        scan command must have additional param "-statusstream"
            and "-ip <addr>" to define the ip-adrress where to send data
*/

namespace lrcsocket
{
    class ZFStreamDeviceInfo;
}


// !! Do not use for new implementations !!
class LRCSOCKET_API ZFInfoStream : public zf::UdpStream
{
public:
    ZFInfoStream();
    virtual ~ZFInfoStream(); // disconnects as well

    // connection
    bool Connect(uint16_t port = PORT_INFOSTREAM_DEFAULT);
    bool Disconnect(bool waitThread = true);
    bool IsConnected() const { return bConnected; }
    bool IsThreadRunning() const { return !bTreadStopped; }

    virtual void OnConnected() {}

    // lowlevel
    virtual void OnData(const lrcsocket::ZFStreamDeviceInfo&, const char* data, int size); // receive raw data.

    // highlevel

    /// new package
    virtual void OnInfoStart() {}
    virtual void OnInfo(int nr, int idx, const char* info, const char* val) = 0;
    virtual void OnProgress(int percent) { }

    void Freeze(bool f) { bFreeze = f; }

protected:
    int port{};
    bool bKill{};
    bool bConnected{};
    bool bTreadStopped{};
    bool bFreeze{};

    std::shared_ptr<std::thread> th;
    long nrd{}; // anzahl empfangener buffer

    void ThreadFunc();
};


//####################################################################################################


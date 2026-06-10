#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "lrcsocketapi.h"
#include "../Common/typ.h"

#define MAX_DEVS	3

#ifdef _WINDOWS
#pragma warning(disable:4251)
#endif

//================================================================================0
class ZFLinedataStream;

namespace lrcsocket
{
#pragma pack(1)
    struct LineRawInfo
    {
        uint32_t dwLineServerId{};
        uint32_t scannerserie{};		///< 5003,5006,5010,5016,...
    };
#pragma pack()

    //================================================================================0

    class UDPLinedataStream;

    /**
    \brief  ZFStreamDeviceInfo information about  the datastram and the connection
            The library can handle more than one stream comming in on the same port. So each stream gets its own information
    */
    class LRCSOCKET_API ZFStreamDeviceInfo
    {
    private:
        friend class ZFLinedataStream;
        friend class lrcsocket::UDPLinedataStream;


    public:
        ZFStreamDeviceInfo() = default;
        virtual ~ZFStreamDeviceInfo();

        void add_to_buffer(const char* data, size_t size);
        void clear_buffer();

        // lines per second
        double getLPS() const { return lps; }

    public:
        std::vector<char> bufOut;					///< the buffer of the collected line for further processing

        char ip[256]{0};	        ///< ip of current stream
        int idx{};		        ///< index from 0..n-1  depending on ip-addres
        uint64_t tm{};       ///< last updated time

#define LOG_BUF	1024
        std::string log;      ///< some additional infos

        int64_t frames{};             ///< anzahl empfangener frames
        int64_t curLine{};           ///< current scanline
        int64_t linesMixedCount{};   ///< lines mixed because of udp
        int64_t linesMissedReal{};   ///< lines missed in datastream
        double lps{};		        ///< lines per second approximate

        int64_t trashed{};           ///< lines trashed away or unknown frame
        int64_t linesTotal{};        ///< lines total received
        uint64_t countCmd{};        ///< number of commands got
        uint64_t countZFSHeader{};  ///< number of zfs-header got
        uint64_t countNewScan{};    ///< number of new scans detected

    public:
        void Reset() // statusvariablen reseten
        {
            log[0] = 0; curLine = linesTotal = trashed = linesMixedCount = linesMissedReal = 0; lps = 0.; frames = 0;
        }

        const char* buf() const { return &bufOut[0]; }
        char* buf() { return &bufOut[0]; }

        void SetIP(const char*);	///< set the ip

        void Alloc();	///< alloc memory for inputstream
        void Free();	///< free memory

    public:
        int lncnt{};				///< needed for calculating lps
        uint64_t lncntTm{};	///< needed for calculating lps
    };

    //======================================================================================

    // return index 0..n-1 or -1 for failed
    LRCSOCKET_API int ParseDev(ZFStreamDeviceInfo& d);

    // idx 0..n-1
    LRCSOCKET_API ZFStreamDeviceInfo* GetDevInfo(int idx);

    /// return number of devices already used
    LRCSOCKET_API int GetDevs();

    /// max number of devices supported
    LRCSOCKET_API int MaxDevs();

    /// call on exit program to prevent memory leaks
    LRCSOCKET_API void DevsCleanup();

    //======================================================================================

}
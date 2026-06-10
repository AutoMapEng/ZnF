#pragma once

#include <string>
#include <vector>

#include "../pps_sync.h"
#include "timeapi.h"

#ifdef _WINDOWS
namespace zfs
{

#if 0
    /**
    * struct for saving to file
    */
#pragma pack(1)
    class LineCounterInfo
    {
    public:
        // raw from file read
        int32_t part{};
        uint32_t line{}; ///< the line in the zfs file  0..lines-1
        uint32_t firmLine{};
        uint32_t vertstart{};
        uint32_t cnt1{};
        uint32_t cnt2{};
        int32_t pixelChangedAt{ -1 };
        uint32_t timeLH{}; ///< from lineheader
        uint32_t flagsLH{}; ///< from lineheader

        uint32_t timeRelStart{};
        uint32_t nValid{};
        bool pause{};			///< if line is first line after a pause

        double deltaTime{}; ///< delta to pre sync pulse (just for lines with pixelChangedAt)
        int deltaPixel{};

        // calculated entries
        int32_t deltaCnt2Pre{};
        int32_t deltaTime2Pre{};
        int32_t deltaLineNr{};

        uint32_t chk{}; ///< check status bits

        double tmLineStart{};
    };
#pragma pack()


    class TimeApi
    {
    public:
        int type{};
        double speed{};
        int hz{};
        int subsample{};
        int encoderVert{};
        bool laraMode{};

        uint32_t lines{};
        uint32_t pixel{};
        uint32_t lhsz{};
        uint32_t psz{};
        uint32_t addByte{};


        std::string syncMessage; ///< GPS Sync message
        uint32_t tmPPS32{};				///< pc-time of first pps got because of gps send time message [ms]
        uint64_t tmPPS64{};				///< pc-time of first pps [ms]
        uint32_t tmPPSPuls32{};			///< pc-time of first pps pulse after gps message received
        uint64_t tmPPSPuls64{};			///<
        zfs::ZFTime tglob;                   ///< global time for counter zero

        double scantimeSec{}; ///< diff between first and last line
        int scanDeltaCnt1{};
        int scanDeltaCnt2{};

        std::vector<LineCounterInfo> rawCounterList;
        std::vector<LineCounterInfo> modifiedCounterList;

        double pixelTimeSec() const { return hz ? 1. / hz : 1; }
    };



    std::vector<LineCounterInfo> collect(ZFSLinebyline* lbl, const TimeApiParams& );

    bool writeTime(const std::vector<LineCounterInfo>& lst, const std::wstring& fn);
    std::vector<LineCounterInfo> readTime(const std::wstring& fn);

    void collectTimeApiFromHeader(const ZFSHeader& zhead, TimeApi& api);
    void collectTimeApiFromScan(ZFSLinebyline* lbl, TimeApi& api);

#endif
}
#endif // _WINDOWS

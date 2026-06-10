#pragma once

/**
    New API since V8.5 for gettings time of each pixel synced with a pps signal from a gps

*/

#include "../zfsapi.h"
#include "../zfserr.h"

#include <vector>
#include <list>
#include <string>

#include "../../Common/inttypes_.h"

#include "../file_extensions.h"
#include "../Tools/progress.h"


class ZFSLinebyline;
class ZFSHeader;

namespace zfs
{
    typedef uint32_t  ms_t;       ///< time [ms]
    typedef int64_t   ms64_t;     ///< time 64bit [ms] signed to allow difference calculation
    typedef uint32_t  cnt_t;      ///< counter
    typedef uint32_t  linenr_t;   ///< linenumber
    typedef uint16_t  flags_t;    ///< flags
    typedef uint32_t  enc_t;      ///< encoder
    typedef double    tmsec_t;    ///< the time as seconds

    class TimeAPI;


    //#define ext_timeapi ".time"
#define TIMEAPI_EXT ext_zfs_time ///< extension used to store time sync information

#define ext_timeapi_report ".time.report"
#define wext_timeapi_report L".time.report"
#define folder_timeapi_report "report"
#define wfolder_timeapi_report L"report"

#define ext_timeapi_json     ".timeapi.json"
#define wext_timeapi_json   L".timeapi.json"


    //####################################################################################################################

    /**
    \brief  Time for the pixel.
            Returns the time information for a pixel
    */
    class ZFS_API ZFTime
    {
    public:
        enum zftime_t
        {
            T_UNKNOWN,
            T_ERROR,    ///< not possible to get time

            T_PC,       ///< time based on pc time
            T_REL,      ///< time relative
            T_ABS_UTC,  ///< time absolute to utc
            T_ABS_WEEK, ///< time absolute to week
        };
        zftime_t tt{ T_UNKNOWN };    ///< time type

        // relative time to pps zero stored in week or utc
        tmsec_t seconds{}; ///< resolution nanoseconds (includes the secondsFromLinestart)
        tmsec_t secondsFromLinestart{};    ///< just the part from linestart

        // absolute time at pps zero
        int week = 0;       ///< for T_ABS_WEEK
        int weeksec = 0;    ///< for T_ABS_WEEK
        int hour = 0;       ///< for T_ABS_UTC
        int min = 0;        ///< for T_ABS_UTC
        int sec = 0;        ///< for T_ABS_UTC
        int secOffsetAbs = 0; ///< a additional offset for Abs time

        ZFTime() = default;

        bool isValid() const { return tt >= T_PC; }

        /// relative time to first pps puls. Not start of scan or data.
        tmsec_t SecondsRel() const { return seconds; }

        tmsec_t absOffsetSec() const
        {
            if (tt == T_ABS_UTC) return sec + 60 * min + 3600 * hour + secOffsetAbs;
            if (tt == T_ABS_WEEK) return weeksec + secOffsetAbs;
            return secOffsetAbs;
        }

        tmsec_t SecondsAbs() const
        {
            return seconds + absOffsetSec();
        }

        std::string GetTimeTypeString() const;                    ///< get a string for the time typ
        std::string Dump(bool abs, bool dec, bool verbose = false) const;    ///< print out for debugging abs=seconds as absolute or relative
    };

    //####################################################################################################################

    /**
    \brief  Information about every line
            Gives some information about the line.

            This struct will be written and read back
    */
#pragma pack(1)
    struct ZFLineSyncInfo
    {
        // first read from zfs
        int32_t part{};			///< part number 0..n-1
        int32_t nValid{};         ///< number of valid pixel in this line
        int32_t nMiss{};          ///< too much pixel which are missed
        ms64_t ms{};            ///< pc-time of first pixel in the line [ms]  !!! not accurate !!!
        ms64_t relTime{};       ///< relative time to first line. first line starts with zero
        cnt_t cnt{};          ///< counter depending on param, it is cnt1 or cnt2
        linenr_t lnr{};       ///< zeilennummer aus lineheader
        flags_t lineflags{};  ///< flags from lineheader

        uint8_t ch{};            ///< true if counter change in this line was detected
        int32_t pxCh{ -1 };           ///< position on that counter has changed (-1 if not)

        uint8_t pause{};			///< if line is first line after a pause
        int32_t missedLines{};	///< number of missed lines or 0 for no
        double tmLineStart{}; ///< the relative time of first pixel in the line [sec]
        double tmPerPixel{};  ///< the time distance of each pixel in this line [sec]
        int32_t cntOffset{};		///< offset which has to be added to cnt

        int32_t pxSinceLastCh{};  ///< number of pixel since last counter change
        ms64_t tmSinceLastCh{}; ///< pc-time since last counter change [ms] !!! not accurate !!!

        double tmLinePre{};   ///< time to previous line [sec]

        ZFLineSyncInfo() = default;

        cnt_t CntOrg() const { return cnt; } ///< orginal counter value
        cnt_t Cnt() const { return cnt + cntOffset; } ///< corrected counter value
        cnt_t CntOffset() const { return cntOffset; } ///< current offset used
    };
#pragma pack()

    ZFS_API std::string Dump(const ZFLineSyncInfo& i, const TimeAPI&, bool verbose = false); ///< print out for debugging
    ZFS_API std::string DumpVar(const ZFLineSyncInfo& i, const std::string& cr="\n"); ///< print out for debugging


    typedef std::vector<ZFLineSyncInfo> linesyncinfo_vec_t;

    //####################################################################################################################

    class PPS_Sync_WR;

    /**
    \brief Params used for sync with pps for function Create
    */
    struct TimeAPI_Params
    {
        enum sync_t
        {
            SYNC_AUTODETECT,    ///< Auto detect or no counterinput
            SYNC_PPS_CNT1,      ///< PPS connected to counter1 (default)
            SYNC_PPS_CNT2,      ///< PPS connected to counter2
            SYNC_NO_PPS,        ///< there is no pps counter available, so try to use the time from the lineheader which is inaccurate
        };

        enum {
            PRM_NOSTARTOFFSET = 1,		///< do not try to recalculate a startoffset.
            PRM_VERIFY = 2,

            PRM_DEFAULT = PRM_VERIFY
        };

        enum report_t
        {
            REPORT_STORE=1,
            REPORT_PPS=2,

            REPORT_EVERYTHIN = 0xffff
        };

        TimeAPI_Params() = default;

        /// see PRM_...
        uint32_t prm{ PRM_DEFAULT };

        /// see SYNC_...
        sync_t sync{ SYNC_AUTODETECT };

        /// the pps pulse wide (currently not implemented) default: 1 second
        double pulse_hz{ 1 };

        /** \brief The maximal allowed offset between pc-time and pps counter (default 3000) [seconds].
        */
        uint32_t maxOffset{ 3000 };

        /** \brief The time from begining of the scan data to first pps pulse must be less than this value [seconds].
                   Typical in the first seconds there must be a pps pulse, otherwise your gps/imu in not cofigured correct */
        uint32_t firstPPSCheck{ 60 };

        bool tryRepair{ true }; ///< try to repair wrong counter pulses or ignor it and sync to next correct puls

        report_t report{ REPORT_EVERYTHIN };

        bool verbose{}; ///< more outputs

        bool cacheList{ false }; ///< helps while debugging or multiple calls
    };



/**
\brief  Main class which handles sync based on pps signal connected to scanner
        The ReadIn function will read the whole scan and create for every line a timestamp.
        It calculates the pixels in between the pps pulses and creates time for all the pixel and lines before the first pps pulse as well.
        So later on you can get for every pixel in the line the exact relative and absolute time.
        The absolute time is the time from the gps like utc or week plus the relative time from that point.
        If there is no counter or pps it uses a fallback and sync with the pc-time which is not accurate. ZFTime returnvalue will tell how it is synced
*/
    class ZFS_API TimeAPI
    {
        friend class PPS_Sync_WR;

    public:

        struct ZFS_API Result
        {
            // source zfs file
            int m_lines{};		    ///< lines in the whole record processed
            int m_parts{};		    ///< number of parts processed
            ms64_t m_tmFirstLine{};	///< pc-time first line [ms]
            ms64_t m_tmLastLine{};	///< pc-time last line [ms]
            // counter
            uint64_t m_cnt1FirstLine{};
            uint64_t m_cnt1LastLine{};
            uint64_t m_cnt2FirstLine{};
            uint64_t m_cnt2LastLine{};

            // the correction
            int64_t m_offsetLastLine{}; ///< the last correction offset in the dataset
            int64_t m_offsetMax{}; ///< the maximal correction offset in the dataset

            // final
            double m_timeAbsoluteOffset{}; ///< the offset used to get absolute time
            double m_timeFirstLine{}; ///< the final time [sec]
            double m_timeLastLine{}; ///< the final time [sec]

            int64_t m_lineFirstCntChange{};		///< line number on first counter change
            int64_t m_lineLastCntChange{};		///< line number of last counter change
            ms64_t m_pctimeFirstCntChange{};   ///< pc-time on first counter change [ms]
            ms64_t m_pctimeLastCntChange{};    ///< pc-time on last counter change [ms]
            cnt_t m_firstCnt_OnFirstCntChange{};	///< counter value in scan data at first counter change
            cnt_t m_lastCnt_OnLastCntChange{};	///< counter value in scan data at last counter change

            // repair statistic
            int m_nMissed{};        ///< counts the number of missen counters. pps have to come every second.
            int m_nOver{};          ///< counts of too much signals
            int m_nJumps{};       ///< number of jumps detected
            int m_nJumpsLines{};  ///< lines missed
            int m_nMissedLines{}; ///< missed lines because of transfer failure


            std::string toJson(int indent=0) const;
            bool fromJson(const std::string& );
            bool isValid() const { return m_lines && m_parts; } ///< is result valid
            bool isPerfect() const { return isValid() && m_offsetMax == 0; } ///< nothing strange happend
        };

        enum syncmode_t : int64_t
        {
            SYNCMODE_NOTSYNCED,
            SYNCMODE_PCTIME,
            SYNCMODE_CNT1,
            SYNCMODE_CNT2
        };
        enum err_t : int64_t
        {
            ERR_OK,                 ///< no error
            ERR_SCANNOTSUPPORTED,   ///< this scan typ is not supported
            ERR_UNDEF,              ///< undefined
            ERR_SCAN,               ///< err use scan
            ERR_NOLINES,            ///< no lines found
            ERR_LHSZ,               ///< lineheadersize too smale
            ERR_NOCNTBYTE,          ///< scan has no counterbyte
            ERR_NOCNTCH,            ///< no counterchange found at first 3 seconds
            ERR_WR_TIMEFILE,        ///< error on write .time file
            ERR_RD_TIMEFILE,        ///< error of read .time file
            ERR_STARTWITHILLEGALPULS,
            ERR_READFILE,           ///< file gone while reading
            ERR_MISSZFSHEADERITEM,  ///< miss a header item
            ERR_EXCEPTION,          ///< a exception

            _ERR_
        };

        enum warn_t : int64_t
        {
            WARN_MSG2PPS = _ERR_,	///< time gps message to next pps puls less than 500ms
        };

        TimeAPI();

        /// main function to do time synchronisation. new style
        err_t Create(ZFSLinebyline*, const TimeAPI_Params& prm, bool force = false, zf::cancel_progress_f prog = {});

        /// read in and calculate (old style)
        err_t Create(ZFSLinebyline*, int prm = TimeAPI_Params::PRM_DEFAULT, TimeAPI_Params::sync_t = TimeAPI_Params::SYNC_AUTODETECT, double pulse_hz = 1., bool storeReport = true, bool force = false);

        /// create data from memory based data
        err_t Create(ZFSHeader&, const linesyncinfo_vec_t& lines, const TimeAPI_Params&);

        err_t ReadIn(ZFSHeader*, ZFSLinebyline*); ///< read in and calculate
        err_t Err() const { return err; }

        /// The final timeapi report
        const Result& result() const { return m_result; }

        void Verify();

        /// return time information for selected pixel.  tt(time type) ist set to T_ERROR or T_UNDEF if time is invalidate
        ZFTime GetTime4Pixel(int line, int pixel) const;
        ZFLineSyncInfo* GetLineSyncInfo(int line);    ///< return information about the current line
        const ZFLineSyncInfo* GetLineSyncInfo(int line) const;    ///< return information about the current line

        static const char* ErrString(err_t); ///< get a string for the errorcode
        std::string Dump(bool verbose = false) const;             ///< print out for debugging

        /// return the time object when a message vom the gps was received and the relative time starts
        const ZFTime& TGlob() const { return tglob; }

        double getRps() const { return speed; }   ///< motorspeed
        double getHz() const { return hz; }           ///< pixelclock

        /// return the way the data are synced
        syncmode_t GetSyncedWay() const { return m_syncmode; }

        cnt_t FirstCntChangeValue() const { return m_result.m_firstCnt_OnFirstCntChange; }
        cnt_t LastCntChangeValue() const { return m_result.m_lastCnt_OnLastCntChange; }

        int FirstOffset() const; ///< offset of first line
        int LastOffset() const; ///< offset of last line
        double ScanDuration() const; ///< return seconds

        /// get and set additional offset for GetTime4Pixel() function
        double offset() const { return tglob.secOffsetAbs; }
        void setOffset(int o) { tglob.secOffsetAbs = o; }

    protected:
        int m_prm{};
        err_t err{ ERR_UNDEF };                      ///< last error
        syncmode_t m_syncmode{ SYNCMODE_NOTSYNCED };          ///< the way the data are synced
        linesyncinfo_vec_t lin_inf;     ///< info array for each line
        ZFTime tglob;                   ///< global time for counter zero
        std::wstring logFileName;		///< filename

        int m_typ{};          ///< scandata typ
        bool m_lara{ true };        ///< scanmode
        enc_t m_enc_v{};      ///< vertical encoder resolution
        int m_subsample{ 1 };    ///< used subsample
        double m_puls_hz{ 1 };   ///< pps pulse frequency
        int m_timeBetweenLinesMS{};

        std::string m_msg;      ///< gps receiver message for get time
        ms_t m_puls_ms{ 1000 };         ///< from m_puls_hz
        ms_t m_puls_limit_ms{ 25 };   ///< limits for puls detection
        TimeAPI_Params::sync_t m_st{};            ///< sync mode
        int m_addByte{};          ///< additional byte
        int m_nChFound{};         ///< number of conter changes found

        /// some static informations
        double speed{};       ///< rps of motor used
        double hz{};          ///< datarate used
        bool bCounterbyte{};  ///< true if additional counterbyte is used

        ZFLineSyncInfo& getLine(const int idx) { return lin_inf[idx]; }	///< access list
        const ZFLineSyncInfo& getLine(const int idx) const { return lin_inf[idx]; }	///< access list

        void Reset();
        void captureBasicScanInfo(ZFSLinebyline* lbl);
        err_t ParseZFSHeader(ZFSHeader* zheadptr);
        err_t ReadIn_(ZFSHeader*, ZFSLinebyline*); ///< read in and calculate
        err_t ReadZFSLines(ZFSLinebyline*, zf::cancel_progress_f prog = {}); ///< read in to lin_inf
        err_t Create_(ZFSHeader*, ZFSLinebyline*, const linesyncinfo_vec_t* lines, const TimeAPI_Params&, bool force, zf::cancel_progress_f prog = {});    ///< Read in the scan and calculate time for every line
        err_t rebuild(ZFSHeader*, ZFSLinebyline*, const linesyncinfo_vec_t* lines, const TimeAPI_Params&, bool force, zf::cancel_progress_f prog = {});
        err_t WriteTemp(const std::wstring&); // write result for next usage
        err_t ReadTemp(const std::wstring&);  // read already processed file

        /// reset logging
        void ReportReset();
        // print global information like app version
        void ReportGlobal();
        /// print params used
        void ReportParams(const TimeAPI_Params&, bool force);
        /// print infos got from zfs-header
        void ReportZFSHeaderInfos(ZFSHeader&);
        void ReportLineResult();
        void ReportResult();
        /// report a string
        void Report(const std::string&);
        /// set the filename for the reportfile
        void SetReportFile(ZFSLinebyline*, bool store);

        void GetPCTimeOfLine(const char* lp, int cl, ZFLineSyncInfo& inf);
        int DetermineValidPixelInLine(ZFSLinebyline* lbl, int pixelMax, ZFLineSyncInfo& inf);
        void Get_Cnt_Nr_Flags_FromLineheader(const char* lp, linenr_t&, cnt_t&, flags_t&);
        int DetectCntCh(ZFSLinebyline* lbl, ZFLineSyncInfo& inf);
        int CalcBack(int cl, ZFLineSyncInfo& inf);
        bool CalcBetweenCounter(int ln_from, int ln_to);
        void Calc2End(int ln_from);
        void AdjCounter(int from, int add);
        int CalcPixelSinceLastCounterChange(int line);
        int CalcPixel2NextCounterChange(int lnine);
        void find_counter_changes();

    public:
        Result m_result;

        ms_t   m_tmPPS32{};				///< pc-time of first pps got because of gps send time message [ms]
        ms64_t m_tmPPS64{};				///< pc-time of first pps [ms]
        ms_t   m_tmPPSPuls32{};			///< pc-time of first pps pulse after gps message received
        ms64_t m_tmPPSPuls64{};			///<
        ms_t   m_tmMSG2PPS{};				///< time from gps msg to next pps pulse
        double m_startDiff{};				///< missmatch between counter and pc-time [sec]

        int m_nVerifyErrors{};			///< set if PRM_VERIFY is used
        int m_nVerifyWarnings{};			///< set if PRM_VERIFY is used

        std::vector<err_t> m_errorList;
        std::vector<warn_t> m_warningList;

        std::string report; ///< local report
    };


/// return the timeapi filename for a zfs scan
    ZFS_API std::wstring timeAPI_File(const std::wstring& zfs);
    ZFS_API std::wstring timeAPI_ReportFile(const std::wstring& zfs);

    //####################################################################################################################
} // end namespace zfs


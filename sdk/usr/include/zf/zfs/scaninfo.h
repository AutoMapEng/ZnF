#pragma once

#include "zfsapi.h"
#include "flags.h"
#include "../Common/inttypes_.h"

#include <string>
#include <vector>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

class ZFSHeader;
class ZFScan;
class ZFSLinebyline;

namespace zfs
{
    enum zfs_feature_t
    {
        FEATURE_COMPENSATOR = 1,
    };
    ZFS_API int ZfsEnableFeature(int feature, bool en); // todo verschieben in zfs.h
    ZFS_API int ZfsGetFeature(); // todo verschieben in zfs.h

    /**
    \brief Holds Scanner serie and typinfo
    */
    class ScannerInfo
    {
    public:
        scannerserie_t		mSerie;     ///< serie of scanner
        scannertyp_t		mTyp;       ///< typ of the serie
        std::string			mModel;     ///< model name

    public:
        ScannerInfo() : mSerie(SERIE_UNKNOWN), mTyp(TYP_UNDEF), mModel("") {}

        void set(scannerserie_t serie, scannertyp_t typ) { mSerie = serie; mTyp = typ; }

        bool empty() const { return mTyp == TYP_UNDEF || mSerie == SERIE_UNKNOWN; }

        bool I5016_or_newer() const { return mSerie >= SERIE_5016; }
        bool I5024_or_newer() const { return mSerie >= SERIE_5024; }
    };


    /// detect sync typ
    ZFS_API sync_t DetectSyncTyp(int datatyp);
    ZFS_API sync_t DetectSyncTyp(const ZFSHeader*);
    inline bool isSyncLara(int datatyp) { return DetectSyncTyp(datatyp) == SYNC_LARA; }
    inline bool isSyncEnc(int datatyp) { return DetectSyncTyp(datatyp) == SYNC_ENC; }

    /// is it a default scanner type not a other format like a color format
    ZFS_API bool isScannerType(int datatyp);

    /// scanner typ
    ZFS_API ScannerInfo DetectScannerTyp(const ZFSHeader*);
    ZFS_API ScannerInfo DetectScannerTyp(const std::string& modelName);
    ZFS_API ScannerInfo DetectScannerTyp(int dt, bool profiler=false);

    /// return a number like 5003,5006,5010,5016,9012,...
    ZFS_API int scannerserieAsInt(scannerserie_t);

    /// quality setting of scan
    ZFS_API quality_t DetectQuality(const ZFSHeader*);
    ZFS_API quality_t DetectQuality(const std::string &s);

    ZFS_API std::string QualityString(quality_t q,
                                      std::map<zfs::quality_t, std::string > qualitySettings = {
                                          {zfs::QUALITY_SUPERLOW, "Speed++"},
                                          {zfs::QUALITY_LESS, "Speed+"},
                                          {zfs::QUALITY_NORMAL, "Balanced"},
                                          {zfs::QUALITY_HIGH, "Quality+"},
                                          {zfs::QUALITY_SUPERHIGH, "Quality++"}});

    ZFS_API const char* QualityStringShort(quality_t q);

    ZFS_API quality_levels ReadQualityLevels(const ZFSHeader* h);
    ZFS_API resolution_levels ReadResolutionLevels(const ZFSHeader* h);

    /// resolution of scan
    ZFS_API resolution_t DetectResolution(const char* aResolutionString);
    ZFS_API resolution_t DetectResolution(uint32_t vertPixel360deg);
    ZFS_API resolution_t DetectResolution(const ZFSHeader*);

    ZFS_API std::string ResolutionString(resolution_t r, std::map<resolution_t,
                                         std::string> resolutionSettings =
                                               {{zfs::RES_P, "Preview"},
                                                {zfs::RES_L, "Low"},
                                                {zfs::RES_M, "Middle"},
                                                {zfs::RES_H, "High"},
                                                {zfs::RES_SH,"Superhigh"},
                                                {zfs::RES_UH,"Ultrahigh"},
                                                {zfs::RES_EH,"Extremlyhigh"}});

    ZFS_API const char* ResolutionStringShort(resolution_t r); // scan command resolution string (empty if undefined)

    /// Return the scan duration in seconds
    ZFS_API double ScanDuration_sec( const ZFSHeader* );

    /// The scantime a full 180deg scan would take
    ZFS_API double ScanDuration_fullscan_sec(const ZFSHeader*);

    /// calculate the 3d-distance to a neighbour pixel perpendicular to the laser direction if the pixel is distance meter away from the scancenter.
    ZFS_API double getPixelpitch(const ZFSHeader*, double distance);

    ZFS_API bool scannedAsProfiler(const ZFSHeader*); ///! return true is scan is taken in profilermode. Profiler return true allways
    ZFS_API bool scannedAsImager(const ZFSHeader*); ///! return true is scan is taken in imagermode. Not imager are profiler, static or userdefined

    /**
    \brief device info which are not stored in the zfs-header
    */
    class ZFS_API DeviceInfo
    {
    public:
        ScannerInfo iTyp{};	///< scanner typ from DetectScannerTyp

        // intensity default if zfs header has no values
        int minRf{3000};  ///< [inc]
        int maxRf{ 5000000 };  ///< [inc]

        // lara system channels
        double dMaxRange{};	///< ambigute interval  [meter]
        int iChannels{};		///< number of channels 2 or 3
        double dNonius1{};	///< [meter]
        double dNonius2{};	///< [meter] zero if not used
        double dNonius3{};	///< [meter] zero if not used for future use

        // for systemcheck, mechanical accuracy
        double dlin{};        ///< linearity of system in mm
        double dlenc{};       ///< thresh. lowspeedenc in mrad
        double dhenc{};        ///< thresh. highspeedhenc in mrad

        //double dAccL1L2;	///< layer1 to layer2
        //double dAccLT;		///< layer to target
        //double dAccTT;		///< measurement target to target

        // range noise => [std(range)=a*int^(b)+c; std(range) in mm]
        double lara_rfvsnoise_a{};///< noise parameter a
        double lara_rfvsnoise_b{};///< noise parameter b
        double lara_rfvsnoise_c{};///< noise parameter c

        //
        double dist_kos_tripod{};	///< distance between mirrorcenter and level-rotations center [meter]
        double th_level_mrange{}; ///< threshold => operating range [deg]
        double th_level_crange{}; ///< threshold => measurement range [deg]
        double level_resolution{};///< resolution of level [deg]

        //Range noise
        double RgN_05_b_Max{};
        double RgN_05_g_Max{};
        double RgN_05_w_Max{};

        double RgN_1_b_Max{};
        double RgN_1_g_Max{};
        double RgN_1_w_Max{};

        double RgN_2_b_Max{};
        double RgN_2_g_Max{};
        double RgN_2_w_Max{};

        double RgN_5_b_Max{};
        double RgN_5_g_Max{};
        double RgN_5_w_Max{};

        double RgN_10_b_Max{};
        double RgN_10_g_Max{};
        double RgN_10_w_Max{};

        double RgN_25_b_Max{};
        double RgN_25_g_Max{};
        double RgN_25_w_Max{};

        double RgN_50_b_Max{};
        double RgN_50_g_Max{};
        double RgN_50_w_Max{};

        double RgN_100_b_Max{};
        double RgN_100_g_Max{};
        double RgN_100_w_Max{};

        double RgN_200_b_Max{};
        double RgN_200_g_Max{};
        double RgN_200_w_Max{};

        //Compensator thresholds
        double min_hori_scan_field{};	// [deg]
        double max_tilt_changes{};	// [deg]

        DeviceInfo() = default;
    };

    ZFS_API DeviceInfo GetDeviceInfo(const ZFSHeader*);


    //#################################################################################
    /// Scanned region returns the angle of the scaned area
    class ZFS_API ScannedArea
    {
    public:
        bool vertValid, horzValid;	///< indicates if detection was succesfull
        double horzFrom, horzTo;	///< horizontal start and end angle 0..360 [deg]
        double vertFrom, vertTo;	///< vertical start and end angle. zero shows to bottom / reference modul 0..360 [deg]
        double vert;				///< the vertical scanned area. 0..360 [deg]
        double horz;				///< the horizontal scanned area. Can be more than 360deg [deg]

        ScannedArea() : vertValid(false), horzValid(false), horzFrom(0.), horzTo(0.), vertFrom(0.), vertTo(0.), vert(0.), horz(0.) {}

        double horzRad() const { return horz * M_PI / 180.; };
        double horzFromRad() const { return horzFrom*M_PI/180.; };
        double horzToRad() const { return horzTo * M_PI / 180.; };
        double vertRad() const { return vert * M_PI / 180.; };
        double vertFromRad() const { return vertFrom * M_PI / 180.; };
        double vertToRad() const { return vertTo * M_PI / 180.; };

        ScannedArea toRadians() const;    ///< Return a scanned area with all angles converted to radians.
    };

    /// return covered scan area. ZSFHeader version will not return valid and accurate values for all possible types.
    ZFS_API ScannedArea GetScannedArea(const ZFSHeader*);

    /// return covered scan area from zfs header or if accurate is defined, check in scanfile for real covered area (slow)
    ZFS_API ScannedArea GetScannedArea(ZFScan*, bool accurate);
    ZFS_API ScannedArea GetScannedArea(ZFSLinebyline*, bool accurate);

    /// return the vertical hardware workspace like 340deg for an IMAGER5016 [deg]
    ZFS_API double GetVerticalWorkspace(const ZFSHeader&);

    ZFS_API int GetOverloadRfValue(const ZFSHeader& h);
    ZFS_API int GetOverRfPixelMargin(const ZFSHeader& h);

    /// test if scan is a selection
    class ZFS_API ScanSelection
    {
    public:
        bool isSelection;	///< if is a selection scan
        bool isTargetScan;	///< if it is a target scan. a target scan ia a selection as well
        bool isChild;		///< if it is marked as a chld scan than it is a selection scan

        ScanSelection() : isSelection(false), isTargetScan(false), isChild(false) {}

        /// detect if it is a full scan or just a selection scans
        bool isFull() const;
    };
    ZFS_API ScanSelection isScanAnSelectionScan(const ZFSHeader*);
    ZFS_API ScanSelection isScanAnSelectionScan(const ZFSHeader&);

    /**
    \brief detect if a counterbyte is used
    \return 0 for not used or the position of the counter byte
    */
    ZFS_API_F int32_t ZfsDetectCounterbyte(const ZFSHeader* h);



} // endif namespace

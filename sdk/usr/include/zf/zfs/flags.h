#pragma once

#include <map>



namespace zfs
{
    // sync typ
    enum sync_t { SYNC_UNDEF, SYNC_ENC, SYNC_LARA };

    // scanner typ
    enum scannerserie_t { SERIE_UNKNOWN, SERIE_SCENEMODELLER, SERIE_I5003, SERIE_I5006, SERIE_I5010, SERIE_P9000, SERIE_5016, SERIE_5024, _SERIE_MAX_ };

     // scanner typ
    enum scannertyp_t { TYP_UNDEF, TYP_SCENEMODELLER, 
        TYP_5003, 
        TYP_5006, TYP_5006i, TYP_5006im, TYP_5006h, TYP_5006hm, TYP_5006ex, 
        TYP_5010, TYP_5010C, TYP_5010X, TYP_5010N, TYP_9012, TYP_9020,
        TYP_5016, 
        TYP_5024E, TYP_5024A, TYP_5024S,
    };

    // quality setting of scan
    enum quality_t {
        QUALITY_UNDEF = -1,
        QUALITY_SUPERLOW = 0,	///< superlow / speed++
        QUALITY_LESS,	        ///< low / speed+
        QUALITY_NORMAL,		    ///< standard
        QUALITY_HIGH,		    ///< high
        QUALITY_SUPERHIGH,	    ///< premium
        _QUALITY_,			    ///< array size

        QUALITY_PREMIUM = QUALITY_SUPERHIGH,
        QUALITY_MIN = QUALITY_SUPERLOW,
        QUALITY_MAX = QUALITY_SUPERHIGH,
        QUALITY_DEFAULT = QUALITY_NORMAL,
    };

    typedef std::map<int, quality_t> quality_levels;

    // resolution of scan
    enum resolution_t {
        RES_UNDEF=-1,
        RES_P=0,		///< preview
        RES_L,			///< low
        RES_M,			///< midle
        RES_H,			///< high
        RES_SH,			///< superhigh
        RES_UH,			///< ultrahigh
        RES_EH,			///< extrem high
        _RES_,			///< array size

        RES_MIN = RES_P,
        RES_MAX = RES_EH,
        RES_DEFAULT = RES_H,
    };

    typedef std::map<int, resolution_t> resolution_levels;

    /// Lineheader Flags. One or more bits can be set
    enum lh_flag_t {
        LH_FLAG_RECINPUT=1,     ///< signal from external input if hardare supports it
        LH_FLAG_TRIGGERED=2,    ///< flag for start capturing
        LH_FLAG_LASEROFF=4,     ///< laser off
        LH_FLAG_CNT1_CHANGED=8, ///< counter has changed
        LH_FLAG_MARKERON=16,    ///< marker active
        LH_FLAG_BADLINE=32,     ///< line marked as bad, values are not correkt, skip this line
        LH_FLAG_NOREF=64,		///< no referenze module present
        LH_FLAG_NEXTC=128		///< new temperature calibration, offsets changed
    };


    /// Flags Info about current scan
    enum
    {
        ZFSFLAG_NO				    = 0,
        ZFSFLAG_LEVEL			    = 1,		///< Scan level information available
        ZFSFLAG_LEVELED             = 2,        ///< Level was used als registrationmatrix
        ZFSFLAG_COMPENSATED		    = 4,		///< Scan compensator information available
        ZFSFLAG_IGNOR_COMPENSATOR   = 8,        ///< ignor compensator
        ZFSFLAG_PROFILER		    = 0x10,		///< Scan is a profiler scan
        ZFSFLAG_ADDBYTE			    = 0x20,		///< Scan has additional byte
        ZFSFLAG_COLOR			    = 0x40,		///< Scan has colorinformation
        ZFSFLAG_IRED                = 0x80,     ///< Thermaldata
    };
    typedef unsigned int zfs_flags_t;


} // endif namespace zfs

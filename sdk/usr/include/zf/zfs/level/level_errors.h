#pragma once
#include "../zfsapi.h"

#include <string>



namespace zfs
{
    namespace level
    {

        /**
        *   Error Werte: 0 = Ok 1-99 Warnungen ab 100 Fehler
        */
        enum class eErrorCode {
            ERROR_OK = 0,               //!< No error, matrix available

            ERROR_UNDEF = 1,
            
            FLAG_USE_COMPENSATOR = 2, ///< das ganze ist veraltet und funktioniert wahrscheinlich nicht mehr. TODO entfernen

            _WARNING_ = 10,
            WARNING_RANGE = 10,         //!< Warning: Level at the end of the valid measurement limit
            WARNING_SETTING = 11,		//!< Warning: Unkown Scanner settings
            WARNING_COMPENSATOR_DISABLED = 12, //!< Waring: Compensator disabeld for this scan
            WARNING_TOOFASTTILTCHANGES = 13,//!< Warning: Calculation OK, but: Tilt changes during scanning to fast for some LencPositions! Max allowed tilt changes between two LencPositios 0.007grad.

            _ERROR_ = 100,
            ERROR_UNKNOWNERROR = 100,   //!< Unknown error
            ERROR_SCANERROR = 101,      //!< No ZFSHeader in scan
            ERROR_OUTOFRANGE = 102,     //!< Level out of valid measurement limits. Fehler Libelle ausserhalb des definierten Bereiches
            ERROR_NOLEVELINFO = 103,    //!< No level information in zfsheader. Keine Libelleninformation im Scanheader
            ERROR_HARDWARESTATUS = 104, //!< Scanner < IMAGER 5010
            ERROR_SCAN = 105,			//!< No scan
            ERROR_HARDWARE = 106,		//!< Scanner < IMAGER 5003, 5006
            ERROR_CALCCORR = 107,		//!< Error while calculating corrected level values.
            ERROR_LEVELOFFSET = 108,	//!< Level offsets not determinable from line level values (scanned lenc-region is too small) => and no useful parent scan is available
            ERROR_NODYNLEVELINFO = 109,  //!< No level information saved during scan. Keine Libelleninformationen waehrend des Scans erfasst.
            ERROR_PARANTSCAN_ISOPEN = 110,  //!<Parant Scan is already open. Der Parant-Scan ist bereits geoeffnet.
            ERROR_NOLEVELINFO_LINEHEADER,
            ERROR_TO_SMALE_SEL,
            ERROR_EXCEPTION,
            ERROR_Movement,		//< Movement during scan
            ERROR_FILTER, // Filtering failed
        };
    
        ZFS_API
        std::string error2string(eErrorCode);
    
    }
} // end namespace zfs

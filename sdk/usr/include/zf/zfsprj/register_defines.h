#pragma once

/// flags for register info. on or more flags
/// flags duerfen nicht veraendert werden, nur neue hinzugefuegt
#define REGISTER_FLAG_FLAGSUSED         1        ///< flags field used
#define REGISTER_FLAG_REGISTERED        2        ///< scan registered. A scan with no matrix can be registered as well if it is the first scan.
#define REGISTER_FLAG_EXTERN            4        ///< register information from external source, registration should not be changed
#define REGISTER_FLAG_BUNDLE            8        ///< registration optimized over project
#define REGISTER_FLAG_KOMPENSATOR       0x10     ///< compensator used or scan only leveled
#define REGISTER_FLAG_INACCURATE        0x20     ///< registration not accurate, for example preregistration
#define REGISTER_FLAG_TOTALSTATION      0x40     ///< registered by using total station info
#define REGISTER_FLAG_GPS               0x80     ///< registered by using gps coordinates
#define REGISTER_FLAG_ICP               0x100    ///< registered using ipc algorithm
#define REGISTER_FLAG_SUPIROUR          0x200    ///< superior matrix used
#define REGISTER_FLAG_SCANPOS           0x400    ///< scan position + level
#define REGISTER_FLAG_SCANTOSCAN        0x800    ///< registered using scan to scan
#define REGISTER_FLAG_1TO               0x1000   ///< registered using one target registration
#define REGISTER_FLAG_2TO               0x2000   ///< registered using traverse
#define REGISTER_FLAG_SCANTRA           0x4000   ///< registered program Scantra
#define REGISTER_FLAG_IMU               0x8000   ///< registered by using IMU data
#define REGISTER_FLAG_MANUAL            0x10000  ///< registered by hand
#define REGISTER_FLAG_MANUAL_FIX        0x20000  ///< registered by hand using as preregistration
#define REGISTER_FLAG_SLAM			    0x40000  ///< offslam 

/// flags for free pos register
#define REGSCAN_FREEPOS_USED        1           // Freie positionierung daten sind vorhanden
#define REGSCAN_FREEPOS_HEIGHT      2           // Hoehe vorhanden
#define REGSCAN_FREEPOS_XYZ         4           // XYZ Wert vorhanden
#define REGSCAN_FREEPOS_CONPOINTS   8           // Anschusspunkte vorhanden
#define REGSCAN_FREEPOS_TARGETEXIST 0x10        // Target im Scan vorhanden


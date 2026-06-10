#pragma once

/**
 * \file
 * Public defines for zfs.dll
 */

/**
 * Flags for opening ZFS scans
 */
enum
{
    /// only ZFScan
    OPEN_NOMASKS = 1,  ///< do not load masks
    OPEN_MULTIOPEN = 4,  ///< allow multiple open of a scan (use with care)
    OPEN_PHYSICAL = 8,  ///< open in physical taken mode, no wrap

    // common flags
    OPEN_NOIMAGE = 2,  ///< open scan without color information
    OPEN_IGNORCALTYP = 0x0010,  ///< force open scan if there is no or wrong calib
    OPEN_XYZ2POS = 0x0020,  ///< enable support for back transformation from xyz to pixel and line. Need more time for loading scan
    OPEN_IGNOR_LEVEL = 0x0040,  ///< do not use leveling system, do not create levelmatrix
    OPEN_NEW_MASK = 0x0080,  ///< use new masks older SDK maybe can not read this masks
    OPEN_IGNOR_COMPENSATOR = 0x8000,  ///< do not use compensator
    OPEN_IGNOR_RFCOMP = 0x10000,  ///< do not use compensated intensity values RfNorm will not work
    OPEN_NOCALLBACKS = 0x20000,  ///< do not call callbacks for this scan even if there is a global handler installed
    OPEN_NOSCANLIST = 0x40000,  ///< do not put scan into scanlist. Just for internal usage
    _OPEN_ENCOPT = 0x100000,  ///< not used
    OPEN_IGNOR_STRAYLIGHT = 0x200000,
    OPEN_USEMATRIX_320 = 0x400000, ///< use registration matrix 320 (final with coordinate system) instead of 280 (without coordinate system) fallback to 320 if 280 does not exist

    // Linebyline flags
    OPEN_SINGLE = 0x0100,   ///< open only single file if scan has more scans
    OPEN_NOPRENEXT = 0x0200,   ///< do not load preview and next line, is much faster but filter will not work (only linebyline)
    OPEN_CREATE_TEMP_POS = 0x0400,   ///< create a linepos file in temp folder, usefull for faster read is scan is opend several times
    OPEN_READ_MASKS = 0x0800,   ///< read masks created by viewer as well
    OPEN_RW = 0x1000,   ///< allow write to file
    OPEN_UNAHE = 0x2000,   ///< calculate unahe for viewing, takes time for open scan
    OPEN_UNTIL_LAST = 0x4000,   ///< open profiler scans until last availabel scan. Can open scans if rest of scanparts are missing
    OPEN_EXTERNAL = 0x8000,
    OPEN_LOGGING = 0x80000,    ///< create logging files
};


/// default open flags for Viewer
#define OPEN_DEFAULTFLAGS_ZFSCAN     (OPEN_XYZ2POS)

/// faster open flags for Viewer
#define OPEN_FASTFLAGS_ZFSCAN        (OPEN_XYZ2POS | OPEN_IGNOR_COMPENSATOR | OPEN_IGNOR_RFCOMP)

/// default flags for opening ZFS files via LineByLine
#define OPEN_DEFAULTFLAGS_LBL        (OPEN_CREATE_TEMP_POS)

/// extra fast flags for opening ZFS files
#define OPEN_FASTFLAGS_LBL           (OPEN_CREATE_TEMP_POS | OPEN_IGNOR_RFCOMP | OPEN_IGNOR_LEVEL | OPEN_IGNOR_COMPENSATOR)


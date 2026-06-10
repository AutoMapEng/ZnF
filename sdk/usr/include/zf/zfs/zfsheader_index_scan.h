#pragma once


namespace zfs
{
    enum
    {
        ZFSIDX_HORZ_FIRST = 67, ///< first horizontal angle in scan line (double) [deg] 
        ZFSIDX_HORZ_LAST = 68, ///< last horz angle in scan line (double) [deg]
        ZFSIDX_HORZ = 69, ///< the horizontal scanned area (double [deg]

        ZFSIDX_MIN_RANGE = 11010, ///< minimum range (int) [mm]
        ZFSIDX_MAX_RANGE = 50, ///< maximum system range (double) [mm]

        ZFSIDX_VERT_SPEED = 70, ///< the vertical rotation speed (double) [rps]

        ZFSIDX_MIN_RF = 100, ///< minimum intensity (int) [inc]
        ZFSIDX_MAX_RF = 112, ///< maximum intensity (int) [inc]

        ZFSIDX_REF = 413, ///< typ (int) 
        ZFSIDX_REF_RG = 410, ///< range (double) [mm]
        ZFSIDX_REF_RF = 411, ///< intensity (double) [inc]
        ZFSIDX_REF_RG_DESIRED = 425, ///< range (double) [mm]
        ZFSIDX_REF_RF_DESIRED = 429, ///< intensity (double) [inc]


        ZFSIDX_SCAN_POSITION = 590, ///< scan position (utf-8 string)
        ZFSIDX_PROJECT = 591, ///< project name scan belong to (utf-8 string)
        ZFSIDX_NEXT_PART = 592, ///< next part of a larger scan chain (utf-8 string)
        ZFSIDX_PREV_PART = 593, ///< prev part of a larger scan chain (utf-8 string)

        ZFSIDX_OPERATOR = 596, ///< operator (utf-8 string)
        ZFSIDX_COMMENT = 597, ///<  (utf-8 string)
    };

} // end namespace zfs



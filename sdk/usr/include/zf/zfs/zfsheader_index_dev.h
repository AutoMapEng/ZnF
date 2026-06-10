#pragma once


namespace zfs
{
    enum
    {
        ZFSIDX_VERT_ENCODER = 73, ///< vertical encoder resolution (int) [inc]
        ZFSIDX_HORZ_ENCODER = 74, ///< horizontal encoder resolution (int) [inc]

        ZFSIDX_LARA_TYP = 11000, // int
        ZFSIDX_SERIALNO = 11014, // string
        ZFSIDX_LARA_SERIALNO = 11003, // int
        ZFSIDX_OS = 11210, // string
        ZFSIDX_FIRMWARE = 11211, // string
        ZFSIDX_HARDWARE = 11213, // string
        ZFSIDX_MODEL = 11214, // string
        ZFSIDX_FIRMWARE_AS = 11215, // string
        ZFSIDX_FIRMWARE_FPGA = 11216, // string
        ZFSIDX_NAV_HARDWARE= 11218, //string
        ZFSIDX_NAV_FIRMWARE = 11219, // string
        ZFSIDX_LARA_HARDWARE= 11220, // string
    };

} // end namespace zfs



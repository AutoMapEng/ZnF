#pragma once


namespace zfs
{
    enum
    {
        ZFSHEADIDX_DATATYP = 1, ///< data type
        ZFSHEADIDX_PIXEL = 2,   ///< pixel per line
        ZFSHEADIDX_BYTESPERPIXEL = 3,   ///< bytes per pixel used for pixel
        ZFSHEADIDX_LINES = 4,   ///< number of lines

        ZFSIDX_ELEOFFSET = 45,  ///< elevation offset [deg]

        ZFSHEADIDX_REGISTER_EXTERN = 320,  ///< start 4x4 matrix for external registration
        ZFSHEADIDX_REGISTER_STEHACHSE = 360,  ///< 4x4 matrix 
        ZFSHEADIDX_REGISTER_INTERN = 280,  ///< start 4x4 matrix for internal registration

        ZFSHEADIDX_TEMP1 = 10000,   ///< sensors first measurement
        ZFSHEADIDX_TEMP2 = 10050,   ///< sensors second measurement

        ZFSHEADIDX_ATTACHEDFILES = 51000,

        ZFSIDX_MINVER_MAJOR = 210, ///< minimum software version required (int)
        ZFSIDX_MINVER_MINOR, ///< minimum software version required (int)
        ZFSIDX_MINVER_PATCH, ///< minimum software version required (int)
        ZFSIDX_MINVER_BUILD, ///< minimum software version required (int)
    };

} // end namespace zfs



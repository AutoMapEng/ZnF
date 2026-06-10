#pragma once


namespace zfs
{
    enum
    {
        // Layer / Masks stored in zfs file
        ZFSIDX_LAYER_N = 30000,

        ZFSIDX_LAYER_STEP  = 100,   ///< step for each layer, first layer stored with this offset
        ZFSIDX_MAX_ZFSLAYER = 20,

        /// just the offsets for each layer
        ZFSIDX_LAYER_ID = 0,
        ZFSIDX_LAYER_SIZEX,
        ZFSIDX_LAYER_SIZEY,
        ZFSIDX_LAYER_BITS,
        ZFSIDX_LAYER_STARTX,
        ZFSIDX_LAYER_STARTY,
        ZFSIDX_LAYER_NAME,
        ZFSIDX_LAYER_COMPRESSION,
        ZFSIDX_LAYER_ADDRESS_LOW,
        ZFSIDX_LAYER_PARAM1, // color
        ZFSIDX_LAYER_PARAM2, // 10
        ZFSIDX_LAYER_BUFSIZE,
        ZFSIDX_LAYER_SCALEX,
        ZFSIDX_LAYER_SCALEY, 
        ZFSIDX_LAYER_FILTERID,      ///< if supported. Since 9.3. see filter_id.h
        ZFSIDX_LAYER_FLAGS,         ///< like fixed filter
        ZFSIDX_LAYER_ADDRESS_HIGH,

        ZFSIDX_LAYER_COLOR = ZFSIDX_LAYER_PARAM1,
    };



} // end namespace zfs



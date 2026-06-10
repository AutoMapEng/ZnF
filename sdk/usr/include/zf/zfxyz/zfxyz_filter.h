#pragma once
#include "zfxyzapi.h"

/**
    Modul for implementing filtering of stream
*/
#include <stdint.h>


namespace zfxyz
{
    class ZFSHeaderStr;
    struct XYZPixelIn;


    /**
    \brief FilterParams for received datastream.
    */
    class ZFXYZ_API FilterParams
    {
    public:
        uint32_t m_minRf;   ///< minimum intensity
        double m_minRg;     ///< minimum range [meter]
        double m_maxRg;     ///< maximum range [meter]

        FilterParams();

        /// set default filter values
        void SetDefault();

        // set filter params depending on detected scanner device
        void SetDefault(const ZFSHeaderStr &);
    };



    /**
        Filter the line and set mask bit if pixel is filtered out
        Just mark pixels as filtered out. Does not remove the pixel itself.
    */
    ZFXYZ_API int FilterLine(const FilterParams& param,
        const char* lineheader,
        int32_t sizeLineheader,
        XYZPixelIn* pixelData,
        int nPixel);
}

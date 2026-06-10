#pragma once

//#define STRAYLIGHT_HISTO

#include <zfsfilter/filterapi.h>

#include <zfs/2dselection.h>
#include <zfs/corr/straylight.h>


class ZFScan;
class ZFSHeader;

namespace zfsfilter
{
    namespace repair
    {

        /**
        \brief The result of the detection / params for repair
        */
        struct FILTERDLL_API RawResult
        {
            ZFRect MeasurementRegion;
        };

        struct FILTERDLL_API CorrectedDistanceResult
        {
            ZFRect CorrectedRegion;
            bool succ{ false };
            double avr{}; ///< average of range change

            CorrectedDistanceResult() {}
        };


        /**
        \brief offset ermitteln
        \param offset cos and sin
        \param offsetRep offsets for repair scan
        \param ossetView offset for showing in the dialog. Same value as 5016-firm9.2 will return.
        */
        FILTERDLL_API bool RawOffset_Ermitteln( ZFScan* zfs, zfs::RawParams::cossin_t& offsetRep, zfs::RawParams::cossin_t& offsetView, zfs::RawParams&, RawResult& result, unsigned rawLimit = 4095);

        /**
        \brief Apply raw offset to scan
        */
        FILTERDLL_API CorrectedDistanceResult RawOffset_Anwenden( ZFScan* zfs, const zfs::RawParams::cossin_t& offset, unsigned rawLimit = 4095, bool debug=false );
    }
}

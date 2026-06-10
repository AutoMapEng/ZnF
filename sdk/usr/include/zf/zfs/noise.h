#pragma once

#include <math.h>

#include "zfsapi.h"
#include "../Common/inttypes_.h"



class ZFSHeader;

namespace zfs
{

    //#################################################################################

    /**
    \brief range noise depending on intensity
    */
    class ZFS_API RangeNoiseBase
    {
    public:

        RangeNoiseBase() = default;
        RangeNoiseBase(const ZFSHeader* h) { Init(h); }	/// construct and init

        // setup
        bool Init(const ZFSHeader*);

        /// check if scan support calculation of RangeNoise
        bool isAvailable() const { return m_bAvailable; }		

        /// brief return noise [mm] depending on intensity inc
        double calcMM(const int32_t rf) const;
        /// same but as meter
        double calcM(const int32_t rf) const { return calcMM(rf) * 0.001; }

    protected:
        bool m_bAvailable{};

        /** factors in noise calculation formula */
        double m_dA{}, m_dB{}, m_dC{};
        double m_dD{}; ///< Factor that considers the data rate.

    };

    
    /**
    \brief range noise depending on intensity
    */
    template<typename T> 
    class RangeNoiseT: public RangeNoiseBase
    {
    public:

        RangeNoiseT() = default;
        RangeNoiseT(const ZFSHeader* h) : RangeNoiseBase(h) {}	/// construct and init

        T calcMM(const int32_t rf_inc) const { return static_cast<T>(RangeNoiseBase::calcMM(rf_inc)); }
        T calcMM(const uint32_t rf_inc) const { return calcMM(static_cast<int32_t>(rf_inc)); }
        T calcMM(const double rf_inc) const { return calcMM(static_cast<int32_t>(rf_inc)); }

        T calcM(const int32_t rf_inc) const { return static_cast<T>(calcMM(rf_inc) * 0.001); }
        T calcM(const uint32_t rf_inc) const { return calcM(static_cast<int32_t>(rf_inc)); }
        T calcM(const double rf_inc) const { return calcM(static_cast<int32_t>(rf_inc)); }
    };


    typedef RangeNoiseT<float> RangeNoiseF;
    typedef RangeNoiseT<double> RangeNoise;

    //======================================================================================================================================

    inline double RangeNoiseBase::RangeNoiseBase::calcMM(const int32_t rf) const
    {
        if (m_bAvailable && rf > 0)
        {
            return (m_dA * pow(rf, m_dB) + m_dC) * m_dD;
        }
        return 0.;
    }

} // endif namespace

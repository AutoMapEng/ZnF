#pragma once

#include "zfsapi.h"
#include "flags.h"

#include <cstddef>
#include <vector>

class ZFSHeader;

namespace zfs
{
    //================================================================================================================
	/**
	 * @class LaserSpotDiameter
	 * @brief Class for providing Laser Spot Sizes as a precalculated LUT.
	 *
	 * @author Juergen Holzner
	 *
	 * Class for providing Laser Spot Sizes as a precalculated LUT.
	 *
	 */
    //================================================================================================================
    class ZFS_API LaserSpotDiameter
    {
    public:

        LaserSpotDiameter();
        LaserSpotDiameter( const scannerserie_t aScannerSeries );
        LaserSpotDiameter( const ZFSHeader& aHeader );

        // init
        void init( const scannerserie_t aScannerSeries, const double& aMinDist, const double& aMaxDist);
        void init( const scannerserie_t aScannerSeries);
        void init( const ZFSHeader& aHeader);

        // values
        double operator()(const double& aDistance) const;

        double Exact( const double& aDistance ) const;

        double DistanceForGivenLaserSpotDiameter(const double& LaserSpotDiameter) const;
        double DistanceOfPixelSamplingEquivalence(const int& PixelPer360Deg, bool& DistanceExists) const;

        // getter
        bool           available()           const { return m_bAvailable;}
        double         getMaxDistance()      const { return m_MaximumDistance;}
        double         getMinDistance()      const { return m_MinimumDistance;}
        double         getSamplingDistance() const { return m_SamplingDistance;}

    private:

        bool                m_bAvailable{};

        double              m_SamplingDistance{ 0.001 };
        double              m_MaximumDistance{};
        double              m_MinimumDistance{};

        double              m_LaserSpotRadius0{};
        double              m_LaserSpotFarFieldDivergence{};

        double              m_LaserSpotDiameter0{};
        double              m_DistanceMultiplierSqr{};


        std::vector<double> m_LaserSpotDiameterLUT;

        void setupLUT();

        size_t getLUTIdx( const double& aDistMeter ) const;

        double SearchFunction(const double& InterPixelAngle, const double& Distance) const;
        double DerivSearchFunction(const double& InterPixelAngle, const double& Distance) const;

    };


    //================================================================================================================
    //================================================================================================================
    //================================================================================================================

    inline double LaserSpotDiameter::operator()(const double& aDistance) const
    {
        if (m_bAvailable)
            return m_LaserSpotDiameterLUT[getLUTIdx(aDistance)];
        else
            return 0;
    }
    // ===================================================================================================

    inline size_t LaserSpotDiameter::getLUTIdx(const double& aDistMeter) const
    {
        if (m_bAvailable)
        {
            if (aDistMeter >= m_MinimumDistance && aDistMeter < (m_MaximumDistance - m_SamplingDistance))
                return static_cast<size_t>((aDistMeter - m_MinimumDistance) / m_SamplingDistance + 0.5);
            else if (aDistMeter >= (m_MaximumDistance - m_SamplingDistance)) return (m_LaserSpotDiameterLUT.size() - 1);
            else return 0;
        }
        return 0;
    }
    // ===================================================================================================
}


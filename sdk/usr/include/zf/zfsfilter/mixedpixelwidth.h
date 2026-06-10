#pragma once

#include "filterapi.h"

#include <cstddef>
#include <cmath>

#include <limits>
#include <vector>

#include <zfs/laserspotsize.h>

#include <zfs/Math/math/range.h>

typedef unsigned char byte;

//================================================================================================================
// forward declarations
//================================================================================================================
namespace zfs
{
    class DatasetParameter;
}
//================================================================================================================
namespace zfsfilter
{
    //================================================================================================================
    /**
     * @class zfsfilter::MixedPixelWidth
     * @brief Class for providing mixed pixel widths (as exact values - no interpolation or LUT)
     *
     * @author Juergen Holzner
     *
     * Class for providing mixed pixel widths (as exact values - no interpolation or LUT)
     *
     */
    //================================================================================================================
    class FILTERDLL_API MixedPixelWidth
    {
        public:
            MixedPixelWidth();
            MixedPixelWidth( const zfs::DatasetParameter&  aDatasetParameter,
                             const zfs::LaserSpotDiameter& aLaserSpotDiameter,
                             const double&                 aMPWFactor = std::sqrt( -0.5f* log( 1.0e-3f ) ),
                             const double&                 aProfileStepping= 0.);// in case of profiler, in [m]

            void init( const zfs::DatasetParameter&  aDatasetParameter,
                       const zfs::LaserSpotDiameter& aLaserSpotDiameter,
                       const double&                 aMPWFactor = std::sqrt( -0.5f* log( 1.0e-3f ) ),
                       const double&                 aProfileStepping= 0.);// in case of profiler, in [m]

            /*Values*/
            void setMPWFactor(const double& aMPWFactor= std::sqrt( -0.5f* log( 1.0e-3f ) ) );
            void setProfileFeed(const double& aProfileStepping);

            double MPW_Hor(const double& aDistance, const double& aVertAngle) const;
            double MPW_Vert(const double& aDistance) const;

            bool available() const;

            double getMPWFactor() const;

        private:

            bool m_bAvailable;
            bool m_bIsProfilerOrInProfilerMode;

            double m_MPWFactor;
            double m_ProfileFeed; // in case of profiler
            double m_dAngleHorizontal; // horizontal profile distance der. from header param. at 0° Latitude 
            double m_dAngleVertical; // vertical profile distance der. from header param.

            zfs::LaserSpotDiameter m_LaserSpotDiameter;

            void basic_init();

    };

    //================================================================================================================
    /**
     * @class zfsfilter::MixedPixelWidthLUT
     *
     * @brief Class for providing mixed pixel widths as a precalculated LUT.
     *
     * @author Juergen Holzner
     *
     * Class for providing Mixed Pixel Widths as a precalculated LUT. 
     *
     * Typename T determines the type of the values in the look up table and can be uint16, uint32, double, or double
     * and also the possible maximal values.
     */
    //================================================================================================================
    template<typename T> class FILTERDLL_API MixedPixelWidthLUT: public MixedPixelWidth
    {
        public:
            MixedPixelWidthLUT();
            MixedPixelWidthLUT( const zfs::DatasetParameter&  aDatasetParameter,
                                const zfs::LaserSpotDiameter& aLaserSpotDiameter,
                                const double&                 aMPWFactor = std::sqrt( -0.5f* log( 1.0e-3f ) ),
                                const double&                 aSamplingDistance = 1e-3,
                                const double&                 aMaxMPWHor = std::numeric_limits<T>::max(),
                                const bool                    aEnableRounding= true,
                                const double&                 aProfileStepping= 0);

            void init( const zfs::DatasetParameter&  aDatasetParameter,
                       const zfs::LaserSpotDiameter& aLaserSpotDiameter,
                       const double&                 aMPWFactor= std::sqrt( -0.5f* log( 1.0e-3f ) ),
                       const double&                 aSamplingDistance= 1e-3,
                       const double&                 aMaxMPWHor = std::numeric_limits<T>::max(),
                       const bool                    aEnableRounding= true,
                       const double&                 aProfileStepping= 0);

            /*Sampling for LUTs*/
            bool setSamplingDistance(const double& aSamplingDistance);

            /*Maxima*/
            bool setMaxMPWHor(const double& MaxMPWHor= std::numeric_limits<T>::max()); // useful when image tiling is used for overlap calculations

            /*Values*/
            bool setMPWFactor(const double& aMPWFactor= std::sqrt( -0.5f* log( 1.0e-3f ) ) );

            /*Enquiry*/
            T MPW_Hor0( const double& aDistance ) const;
            T MPW_Hor(const double& aDistance, const double& aVertAngle) const;
            T MPW_Vert(const double& aDistance) const;

            bool available() const;

        private:

            bool m_bAvailable;
            bool m_bRoundingEnabled;
            bool m_bIsProfilerOrInProfilerMode;

            double m_MaxDistance; // m

            double m_SamplingDistance;  // m

            double m_MultSamplingDistance;  // m

            double m_MaxMPWHor;
            double m_MaxMPWVert;

            zfs::math::Range<double> m_AngleLUTRange;

            // LUTs
            size_t m_LUTSizeMPW;

            std::vector<double> m_MPW_Vert_LUT;
            std::vector<double> m_MPW_Hor_LUT;

        private:
            void basic_init();

            bool setupMPWLUTs();
            
            double Interp( const std::vector<double>& LUT, const double& aDist ) const;

    };
    //================================================================================================================
}

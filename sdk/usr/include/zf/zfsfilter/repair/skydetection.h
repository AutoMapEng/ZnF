/**
 * \file SkyDetection.h
 * @brief Definition of SkyDetection
 *
 * This file contains all definitions for the SkyDetection class
  *
 * @author  Jürgen Holzner (Zoller + Fröhlich GmbH)
 * @date    $Date: 2014-08-08 12:59:59 +0200 (Fr, 08 Aug 2014) $
 * @version $Revision: 80 $
 */

#pragma once

#undef min
#undef max

#include <zfsfilter/filterapi.h>

#include <cstdint>

#include <utility>
#include <vector>

#include <zfs/Math/statistics/histogram.h>

using namespace zfs::math;

namespace zfsfilter
{
    namespace repair
    {
        typedef uint32_t                                                              sky_det_hist_count_t;
        typedef double                                                                sky_det_param_t;
        typedef uint32_t                                                              distance_bin_count_t;
        typedef double                                                                distance_t;

        typedef std::vector<distance_t>                                               distances_t;

        typedef std::pair< 
            zfs::math::Histogram<distance_t, distance_bin_count_t>, 
            zfs::math::Histogram<distance_t, distance_bin_count_t> >  
                distance_histograms_t;

        // =====================================================================================
        enum class SkyDetectionTestResultSet_t: uint8_t
        {
            SKY_DET_RESULT_UNDEF, SKY_DET_SUCCEEDED, SKY_DET_FAILED
        } ;
        // =====================================================================================
        // Sky Detection
        // =====================================================================================
        /**
         * @class SkyDetection
         * @brief  Class for testing a histogram from a TLS data portion on sky.
         *
         * @author Juergen Holzner
         *
         * Class for testing a histogram from a TLS data portion on sky.
         *
         * aPDFParameter: can be provided otherwise the parameters are estimated from the histogram
         */
        template<class param_t, class hist_count_t> class FILTERDLL_API SkyDetection
        {
        public:
            typedef std::pair<param_t, param_t> channel_parameters_t;

            typedef std::pair< zfs::math::Histogram<param_t, hist_count_t>, zfs::math::Histogram<param_t, hist_count_t> > channel_histograms_t;

            typedef std::pair< std::vector<param_t>, std::vector<param_t> > channel_increments_t;

            typedef std::vector<param_t>      channel_bin_ranges_t;
            typedef std::vector<hist_count_t> channel_frequencies_t;

            SkyDetection();

            SkyDetection( const Histogram<param_t, hist_count_t>& aCosChannelDataHistogram,
                          const Histogram<param_t, hist_count_t>& aSinChannelDataHistogram,
                          const param_t& aSqrPDFDeviationThreshold,
                          const param_t& aParameterRangeLimitInMultiplesOfSigma );

            double                      CalculateChiSquare( const Histogram<param_t, hist_count_t>& aChannelHistogram, const param_t& aParamLimitMultipleSigma );
            double                      CalculateChiSquare( const Histogram<param_t, hist_count_t>& aChannelHistogram, const channel_parameters_t& aChannelParameter, const param_t& aParamLimitMultipleSigma );

            static channel_parameters_t EstimateGaussianParameters( const Histogram<param_t, hist_count_t>& aChannelHistogram, const param_t& aParamLimitMultipleSigma );

            double                      Gaussian( const channel_parameters_t& aChannelParameter, const param_t& aValue );
            double                      GaussianMultiplier( const channel_parameters_t& aChannelParameter);

            SkyDetectionTestResultSet_t TestResult() const
            {
                return m_TestResult;
            }

        protected:

            const param_t                                 m_ParameterRangeLimitInMultiplesOfSigma; // < provided parameter range limit as multiple of the std.dev. of the channel,
                                                                                                   //   interpreted with respect to mean of pdf (input)
                                                                                                   
            const Histogram<param_t, hist_count_t>* const m_CosineChannelHistogram;                // < cosine channel histogram (input)
            const Histogram<param_t, hist_count_t>* const m_SineChannelHistogram;                  // < sine channel histogram (input)
                                                                                                   
            const param_t                                 m_TestThreshold;                         // < aka confidence intervall upper limit for that the test is conducted,
                                                                                                   //   if not given it is derived using quantile and information about the data,
                                                                                                   //   i.e. number of used bins (can be input)
                                                                                                   
            channel_parameters_t                          m_ChiSqr;                                // < determined chi sqr value for each channel (Cosine Channel,Sine Channel) (derived)
                                                                                                   
            SkyDetectionTestResultSet_t                   m_TestResult;                            // < contains the result of the test (output)

            // -- protected functions
            void DoTest();

        private:

            const bool m_cbUseNormalizationToHistogramMaximum;
        };
    }
}
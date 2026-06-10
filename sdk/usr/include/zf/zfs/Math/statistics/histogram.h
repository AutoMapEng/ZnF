/**
 * \file histogram.h
 * @brief Definition of Histogram
 *
 * This file contains all definitions for the Histogram class
  *
 * @author  Jürgen Holzner (Zoller + Fröhlich GmbH)
 * @date    $Date: 2014-08-08 12:59:59 +0200 (Fr, 08 Aug 2014) $
 * @version $Revision: 80 $
 */

#pragma once

#undef min
#undef max

#include <zfs/zfsapi.h>

#include <vector>

#include <zfs/Math/math/range.h>

namespace zfs {
    namespace math {
        // ==================================================================================================
        // histogram data for arbitrarily tiling of the bins
        // ==================================================================================================
        template<typename param_t, typename bin_count_t> class ZFS_API Histogram
        {
        public:
            Histogram();

            Histogram(const std::vector<param_t>& a_BinRanges);
            Histogram(const std::vector<param_t>& a_BinRanges, const std::vector<bin_count_t>& a_Frequencies);

            Histogram(const std::vector<param_t>& a_BinRanges, const std::vector<param_t>& a_Samples, bool bSamples);

            Histogram(const param_t& aLowerBorder, const param_t& aBinSize, const size_t& aNoOfBins);                                             //< UpperBorder and LowerBorder are considerted to refer to bin upper and lower borders
            Histogram(const param_t& aLowerBorder, const param_t& aBinSize, const size_t& aNoOfBins, const std::vector<param_t>& a_Samples);      //< UpperBorder and LowerBorder are considerted to refer to bin upper and lower borders

            void InitHistogram(const std::vector<param_t>& a_BinRanges);
            void InitHistogram(const std::vector<param_t>& a_BinRanges, const std::vector<bin_count_t>& a_Frequencies);

            void InitHistogram(const std::vector<param_t>& a_BinRanges, const std::vector<param_t>& a_Samples, bool bSamples);

            void InitHistogram(const param_t& aLowerBorder, const param_t& aBinSize, const size_t& aNoOfBins);
            void InitHistogram(const param_t& aLowerBorder, const param_t& aBinSize, const size_t& aNoOfBins, const std::vector<param_t>& a_Samples);

            // operators
            bool   addSample(const param_t& aSampleValue, bool bSaveSample = false);
            size_t getBinIndex(const param_t& aSampleValue) const;
            void   clear();

            // threshold the lower frequency values
            void                            thresholdHistogram(const double& aFractionOfMaximum);
            std::vector<bin_count_t>        thresholdedFrequencies(const double& aFractionOfMaximum) const;

            // getter
            size_t                          getNoOfBins()                                     const { return m_BinCenters.size(); }
            param_t                         getBinSize()                                      const { return m_BinSize; }

            const Range<param_t>& getHistogramRange()                               const { return m_HistogramRange; }
            const std::vector<bin_count_t>& getHistogramFrequencies()                         const { return m_Frequencies; }
            const std::vector<param_t>& getHistogramBinCenters()                          const { return m_BinCenters; }
            const std::vector<param_t>& getHistogramBinRanges()                           const { return m_BinRanges; }

            param_t                         getMinimumOccuringValue()                         const { return m_MinimumOccuringValue; }
            param_t                         getMaximumOccuringValue()                         const { return m_MaximumOccuringValue; }

            uint64_t                        getSumOfFrequencies()                             const { return m_SumOfFrequencies; }
            uint64_t                        getSumOfFrequencies(const Range<param_t>& aRange) const;

            std::vector<param_t>            getBinSizes()                                     const;

            std::vector<param_t>            getHistogramRelativeFrequencies()                 const;

            bin_count_t                     getMaximumHistogramFrequency() const;
            size_t                          getBinIndexOfMaximumHistogramFrequency() const;
            param_t                         getBinCenterValueOfMaximumHistogramFrequency() const;

            Range<param_t>                  getMainParameterRangeOfHistogram(const double& aFractionOfMaximum) const; // < aFractionOfMaximum determines the limit used to reject the lower histogram count entries 

            param_t                         getApproximateMeanFromHistogram() const;
            param_t                         getApproximateStdDevFromHistogram() const;
            param_t                         getApproximateVarianceFromHistogram() const;
            std::pair<param_t, param_t>     getApproximateMeanAndStdDevFromHistogram() const;

        protected:

            bool                            m_bSamples;

            uint64_t                        m_SumOfFrequencies;

            param_t                         m_BinSize;

            param_t                         m_MinimumOccuringValue;
            param_t                         m_MaximumOccuringValue;

            Range<param_t>                  m_HistogramRange;

            std::vector<bin_count_t>        m_Frequencies;

            std::vector<param_t>            m_SampleValues; // < maybe pack together into a struct (for advantages, saver, value check, ...), e.g. hist_bin_data ... ?
            std::vector<param_t>            m_BinCenters;   // < maybe pack together into a struct (for advantages, saver, value check, ...)
            std::vector<param_t>            m_BinRanges;    // < maybe pack together into a struct (for advantages, saver, value check, ...)

            void setBinSize();
            void setBinCenters();
            void setHistogramRange();
            void setFrequencies();
            void setMinMaxOccuringValue();
            void setHistogramBins(const param_t& aLowerBorder, const param_t& aBinSize, const size_t& aNoOfBins);

            bool allBinsHaveSameBinSize();

            void basic_init();

        };
        // ==================================================================================================
    } // end namespace math
} // end namespace zfs
#pragma once

/**
 * \file ChiSqrGOFTest.h
 * @brief Definition of ChiSqrGOFTest
 *
 * This file contains all definitions for the ChiSqrGOFTest class
  *
 * @author  Jürgen Holzner (Zoller + Fröhlich GmbH)
 * @date    $Date: 2014-08-08 12:59:59 +0200 (Fr, 08 Aug 2014) $
 * @version $Revision: 80 $
 */

#undef min
#undef max

#include <zfs/zfsapi.h>

#include <cstdint> // uint64_t
#include <utility> // std::pair
#include <vector>  // std::vector

#include <zfs/Math/statistics/statistics.h>

namespace zfs {
    namespace math {
        // =====================================================================================
        template<class param_t> class Range;
        template<class param_t, class bin_count_t> class Histogram;
        // =====================================================================================
        typedef std::pair<double, double> pdf_parameter_t;
        // =====================================================================================
        // Chi Square Test
        // =====================================================================================
        /**
         * @class ChiSqrGOFTest
         * @brief  Class for conducting a Chi square (goodness-of-fit) test.
         *
         * @author Juergen Holzner
         *
         * Class for conducting a Chi square (goodness-of-fit) test.
         *
         * Example:
         *
         * 1) ChiSqrGOFTest( PDF_UNIFORM, SampleHistogram1, 0.99, pdf_parameter_t(0,1)); // given a Histogram SampleHistogram1 with some sample frequencies, the test is conducted for a uniform distribution (with support [0,1]) for a confidence number of 0.99
         * 2) ChiSqrGOFTest( PDF_GAUSSIAN , SampleHistogram2, 0.99, pdf_parameter_t(0,1)); // given a Histogram SampleHistogram2 with some sample frequencies, the test is conducted for the standard normal distribution for a confidence number of 0.99
         *
         */
        enum class ChiSqrTestResultSet_t
        {
            CSQR_RESULT_UNDEF, CSQR_RESULT_SUCCEEDED, CSQR_RESULT_FAILED
        };

        template<class param_t, class bin_count_t> class ZFS_API ChiSqrGOFTest // das ist eigentlich ein Chi-Quadrat-Anpassungstest (... passender Klassenname auf Englisch)
        {
        public:

            ChiSqrGOFTest();

            ChiSqrGOFTest(const PDFSet_t aPDFType,
                const Histogram<param_t, bin_count_t>& aHistogram,
                const double& aConfidenceNumber,
                const param_t& aParameterRangeLimitInMultiplesOfSigma = 0);

            ChiSqrGOFTest(const PDFSet_t aPDFType,
                const Histogram<param_t, bin_count_t>& aHistogram,
                const double& aConfidenceNumber,
                const pdf_parameter_t& pdf_parameter_t,
                const param_t& aParameterRangeLimitInMultiplesOfSigma = 0);

            void init(const PDFSet_t aPDFType,
                const Histogram<param_t, bin_count_t>& aHistogram,
                const double& aConfidenceNumber,
                const param_t& aParameterRangeLimitInMultiplesOfSigma = 0);

            void init(const PDFSet_t aPDFType,
                const Histogram<param_t, bin_count_t>& aHistogram,
                const double& aConfidenceNumber,
                const pdf_parameter_t& pdf_parameter_t,
                const param_t& aParameterRangeLimitInMultiplesOfSigma = 0);

            ChiSqrTestResultSet_t TestResult() const
            {
                return m_TestResult;
            }

            // additional/auxilliary functions ================================================================================================================
            // in this context with respect to X^2 (Chi-Square) Distribution
            static double CalcConfidenceNumberFromUpperIntervallBorder(const int& aDegOfFreedom, const param_t& aUpperBorder);
            // confidence Intervall from condfidence number
            static bool   ConfidenceIntervallFromConfidenceNumber(const double& aDegreeOfFreedom, const double& aConfidenceNumber, Range<double>& aIntervall);
            // < determine if all provided bins have same bin size
            static bool   AllBinsHaveSameBinSize(const std::vector<param_t>& aBinSizes);

        protected:

            double                    m_ConfidenceNumber;                      // < specifies the confidence number and finally the confidence intervall for checking the result (input)

            PDFSet_t                  m_PdfType;                               // < pdf type to be tested (input)

            pdf_parameter_t           m_PDFParameter;                          // < (Mean, Std.Dev.) if m_PdfType==PDF_GAUSSIAN and (Min, Max) if m_PdfType= PDF_UNIFORM, this (can) be input for test (input)
            double                    m_ParameterRangeLimitInMultiplesOfSigma; // < provided parameter range limit in terms of the std. dev. of the data in the histogram (input)

            std::vector<param_t>      m_BinRanges;                             // < bin ranges of used histogram
            std::vector<bin_count_t>  m_Frequencies;                           // < frequencies of the histogram to be tested (input)
            uint64_t                  m_SumOfFrequencies;                      // < sum of frequencies of the histogram to be tested (input/derived)

            double                    m_DegOfFreedom;                          // < derived from the given histogram (derived)
            double                    m_ChiSqr;                                // < determined chi sqr value (derived)

            ChiSqrTestResultSet_t     m_TestResult;                            // < contains the result of the test (output)

            // -- protected functions
            void DoTest();

            bool CalculateChiSquare();

            void base_init();
        };
    } // end namespace math
} // end namespace zfs
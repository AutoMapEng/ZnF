#pragma once

#include <cstdlib>
#include <vector>

#include <zfs/zfsapi.h>

#include <zfs/Math/math/def.h>
#include <zfs/Math/math/range.h>

#ifdef _CPP11
#include <random> // random engines
#endif

namespace zfs {
    namespace math {
        // ==============================================================================================
        template<class T> class Range;

        // ==============================================================================================
        // pdf type set for calculations of stddev, skewness and kurtosis
        // ==============================================================================================
        typedef enum PDF_SET
        {
            PDF_UNDEF, PDF_NORMAL, PDF_UNIFORM, PDF_EXPONENTIAL
        } PDFSet_t;
        // ==============================================================================================
        // std. dev. estimation type set (for determination of variance)
        // ==============================================================================================
        typedef enum ESTIMATION_TYPE_SET  // see : https://en.wikipedia.org/wiki/Unbiased_estimation_of_standard_deviation
                                          // for a proof on the equation for c4(n) see : 
                                          // https://stats.stackexchange.com/questions/11707/why-is-sample-standard-deviation-a-biased-estimator-of-sigma
        {
            EST_TYPE_COMMON,         // usual way of estimating variance: https://en.wikipedia.org/wiki/Variance#Sample_variance : 1/(n-1)*sum(i=0...n-1, (X-sample_mean)*(X-sample_mean))
            EST_TYPE_RULE_OF_THUMB,  // refers to equation for normal pdf: for n= 3, the error is 1.3%, for n= 9, the error is 0.1% as given on https://en.wikipedia.org/wiki/Unbiased_estimation_of_standard_deviation
            EST_TYPE_TO_FIRST_ORDER, // refers to equation for c4(n) for the normal pdf where the series expansion is stopped after the first order term, according to https://stats.stackexchange.com/questions/11707/why-is-sample-standard-deviation-a-biased-estimator-of-sigma
                                     // when the pdf is of any other type (including PDF_UNDEF) the equation for the O(n^-1) terms is used as also provided on https://en.wikipedia.org/wiki/Unbiased_estimation_of_standard_deviation
                                     EST_TYPE_EXACT           // refers to the equation for the normal pdf, where c4(n) is exactly calculated and the value for the variance is corrected accordingly (only for relatively low sample numbers (maximum 24), otherwise EST_TYPE_TO_FIRST_ORDER is used)
        } EstimationTypeSet_t;
        // ==============================================================================================
        // auxiliary functions
        // ==============================================================================================
        /**
         * function to generate normally distributed random numbers.
         * (Limitation: Due to use of rand(), the maximum number range is ~ -4.56(009) ... +4.56(009))
         * Use srand in a calling function!
         * \author Juergen Holzner
         */
#ifdef _CPP11
        double ZFS_API randn();
#else
        double ZFS_API randn();
#endif
        // ==============================================================================================
        // add normal distributed (nd) noise to given data in vector
        // ==============================================================================================
        template <class T> void ZFS_API add_nd_noise(std::vector<T>& avector, const T& aMean, const T& aStdDev);
        template <class T> void ZFS_API add_nd_noise(T* const apVector, const size_t aLength, const T& aMean, const T& aStdDev);
        // ==============================================================================================
        template <class T> void ZFS_API add_ed_noise(std::vector<T>& avector, const T& aLower, const T& aUpper);
        template <class T> void ZFS_API add_ed_noise(std::vector<T>& avector, const Range<T>& aParRange);
        template <class T> void ZFS_API add_ed_noise(T* const apVector, const size_t aLength, const Range<T>& aParRange);
        template <class T> void ZFS_API add_ed_noise(T* const apVector, const size_t aLength, const T& aLower, const T& aUpper);
        // ==============================================================================================
        // statistical numbers from data in vectors
        // ==============================================================================================
        // maximum of the numbers in a vector
        template< class T> T ZFS_API max_vector(const std::vector<T>& avector, int& max_pos);
        // minimum of the numbers in a vector
        template< class T> T ZFS_API min_vector(const std::vector<T>& avector, int& min_pos);
        // median of the numbers in a vector
        template <class T> T ZFS_API median_vector(std::vector<T> avector);
        // mean of the numbers in a vector
        template <class T> double ZFS_API mean_vector(const std::vector<T>& avector);
        // standard deviation of the numbers in a vector
        // sigma= 1/n* sum(i= 0...n-1; (x-sample_mean)^2) for bUnbiased= false, sigma= 1/(n-1)* sum(i= 0...n-1; (x-sample_mean)^2) otherwise
        template <class T> double ZFS_API stddev_vector(const std::vector<T>& aSampleVector, const bool bUnbiased = true);
        // variance of the numbers in a vector
        template <class T> double ZFS_API variance_vector(const std::vector<T>& avector, const bool bUnbiased = true);
        // unbiased std. dev. factor, also used for calculation of the subsequent function for the following input options:
        // double sigma= stddev_vector( aSampleVector, PDF_NORMAL, EST_TYPE_EXACT);
        double ZFS_API c4(const size_t n);
        // appart from the options given with the choosen PDF and estimation type, choosing bUnbiased as false delivers 1/n*sum(n= 0 ...n-1, (x-sample_mean)^2) 
        template <class T> double ZFS_API stddev_vector(const std::vector<T>& aSampleVector, PDFSet_t aPDFType, EstimationTypeSet_t aEstimationType = EST_TYPE_COMMON, const bool bUnbiased = true);
        // sample skewness of the numbers in a vector (for bUnbiasedForNormalDist= true, exact variant (PDF_NORMAL,EST_TYPE_EXACT) of for the sample variance value is used, for bUnbiasedForNormalDist= false, first order (PDF_UNDEF,EST_TYPE_TO_FIRST_ORDER) calculation for used variance value is used)
        template <class T> double ZFS_API skewness_vector(const std::vector<T>& avector, bool bUnbiasedForNormalDist = true);
        // sample kurtosis of the numbers in a vector (bUnbiasedForNormalDist: calculates an unbiased estimate for normally distributed sample populations)
        // (for bUnbiasedForNormalDist= true, exact variant (PDF_NORMAL,EST_TYPE_EXACT) of for the sample variance value is used, for bUnbiasedForNormalDist= false, first order (PDF_UNDEF,EST_TYPE_TO_FIRST_ORDER) calculation for used variance value is used)
        template <class T> double ZFS_API kurtosis_vector(const std::vector<T>& avector, bool bUnbiasedForNormalDist = true);
        // histogram of the numbers in a vector
        std::vector<int> ZFS_API histogram(const std::vector<double>& data_vector, const int& no_of_bins, const double& hist_min, const double& hist_max, double& bin_size);
        template<class T> std::vector<int> ZFS_API histogram(const std::vector<T>& data_vector, T bin_size, const Range<T>& hist_range);
        template<class T> std::vector<int> ZFS_API histogram(const std::vector<T>& data_vector, T bin_size, const Range<T>& hist_range, std::vector<int>& data_in_bin);
        template<class T> std::vector<int> ZFS_API histogram_off_zero(const std::vector<T>& data_vector, T bin_size, const Range<T>& hist_range);
        // normalizes the entries of a vector by the sum of all elements in the vector
        template <class T> std::vector<double> ZFS_API normalize(const std::vector<T>& vector_in);
        // accumulated squared difference up to and including index vector_length
        template <class T> T ZFS_API quadratic_error(const std::vector<T>& vector1, const std::vector<T>& vector2, int vector_length);
        // accumulated squared difference of two vectors
        template <class T> T ZFS_API quadratic_error(const std::vector<T>& vector1, const std::vector<T>& vector2);
        // get the elements from a given data vector that for the given quantile with respect to the given data vector
        // the quantile is provided as an integer number, giving the percentage of data located between -oo and the
        // number for which <quantile> % of data lies below that number.
        template <class T> std::vector<int> ZFS_API get_indices_of_data_having_quantile(const std::vector<T>& data_vector, int quantile);
        // ==============================================================================================

    } // end namespace math
} // end namespace zfs
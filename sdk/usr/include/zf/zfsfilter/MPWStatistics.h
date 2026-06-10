#pragma once

#include <vector>
#include <set>

#include "filterapi.h"
#include "multithreading_base.h"

namespace zfsfilter {

    template<class T> class Matrix;

    // =============================================================================================================
    /** \class MPWStatNumbers
     * \brief  Class for keeping statistical numbers
     *
     * \author Juergen Holzner
     *
     */
    template< class T>
    struct FILTERDLL_API MPWStatNumbers
    {
        double       Mean, stdDev;
        int          nSamples;
        T            Min, Max, ExpectedMax;
        std::set<T>  UniqueValuesSet;

        MPWStatNumbers()
            : Mean(0.)
            , stdDev(0.)
            , nSamples(0)
            , Min(0)
            , Max(0)
            , ExpectedMax(0)
        { }
    };

    // =============================================================================================================
    /** \class MPWStatistics
     * \brief  Class for deriving the mixed pixel width statistics given pointers matrices
     *
     * \author Juergen Holzner
     *
     */
    template< class T >
    class FILTERDLL_API MPWStatistics : public MultithreadingBase
    {
    public:

        // constructors
        MPWStatistics();
        MPWStatistics(const Matrix<T>& a_MPW_a, const Matrix<T>& a_MPW_r, const bool a_bGetUniqueValuesSet = false, const bool a_bProcessMultithreaded = true);

        // operators
        void calculate(const Matrix<T>& a_MPW_a, const Matrix<T>& a_MPW_r);

        // getter
        const MPWStatNumbers<T> getStatNumbersMPW_r() const;
        const MPWStatNumbers<T> getStatNumbersMPW_a() const;
        const MPWStatNumbers<T> getStatNumbersMPW_ar_Overall() const;

        // setter
        void setStatNumbersMPW_r(const MPWStatNumbers<T>& a_StatNumbersMPW_r);
        void setStatNumbersMPW_a(const MPWStatNumbers<T>& a_StatNumbersMPW_a);

        // helper
        void calculateStatNumbersOverall();
        void setMPWStatisticsFromVector(const std::vector<MPWStatistics>& aMPWStatisticsVec);

    private:

        static const double m_c_UseProcessorPortion;

        // member variables
        bool               m_bGetUniqueValues;

        MPWStatNumbers<T>  m_StatNumbersForMPW_a;
        MPWStatNumbers<T>  m_StatNumbersForMPW_r;
        MPWStatNumbers<T>  m_StatNumbersMPW_ar_Overall;


        // private member functions
        const MPWStatNumbers<T> calculateStatNumbers(const Matrix<T>& a_MPW) const;
        const MPWStatNumbers<T> calculateStatNumbersOverall(const MPWStatNumbers<T>& aMPWaStatNumbers, const MPWStatNumbers<T>& aMPWrStatNumbers) const;
        const MPWStatNumbers<T> calculateStatNumbersOverallFromVector(const std::vector<MPWStatNumbers<T>>& aMPWStatNumbersVec) const;
        const MPWStatistics     calculateMPWStatisticsFromVector(const std::vector<MPWStatistics>& aMPWStatisticsVec) const;

        void setStatNumbersMPW_ar_Overall(const MPWStatNumbers<T>& a_StatNumbersMPW_ar_Overall);

        T    ExpectedMPWMax(const MPWStatNumbers<T>& aMPWStatNumbers) const;

        // operators
        void        calculateSection(const Matrix<T>& a_MPW_a, const Matrix<T>& a_MPW_r, const int line_start, const int line_stop);
        void        calculateMT(const Matrix<T>& a_MPW_a, const Matrix<T>& a_MPW_r);
        static void ThreadWorkerCalcMT(MPWStatistics<T>* const pMPWStatistics, const Matrix<T>* const MPW_a, const Matrix<T>* const MPW_r, const int line_start, const int line_end);
    };
} // zfsfilter

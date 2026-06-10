#pragma once

#include <zfs/zfsapi.h>

#include <vector>

namespace zfs {
    namespace math {

        // ====================================================================================================================================================
        /** \class BasicStatisticalData
          * \brief This class holds the statistics for a specific data base
          * \author juergen holzner
          */
        template<class T> class ZFS_API BasicStatisticalData
        {
        protected:
            unsigned int    NoOfDataSamples;
            T               MinVal, MaxVal;
            double          MeanVal, StdDevVal;

        public:
            BasicStatisticalData();
            BasicStatisticalData(const unsigned int N_, const T& min_, const T& max_, const double& mean_, const double& stdev_);
            BasicStatisticalData(const std::vector<T>& data_base);
            virtual ~BasicStatisticalData();

            void setStatistics(const unsigned int N_, const T& min_, const T& max_, const double& mean_, const double& stdev_);
            virtual void calcStatistics(const std::vector<T>& data_base);

            unsigned int    getN()      const;
            T               getMin()    const;
            T               getMax()    const;
            double          getMean()   const;
            double          getStdDev() const;
        };

        // ====================================================================================================================================================
        /** \class ExtendedStatisticalData
        * \brief This class holds the statistics for a specific data base and no. of points within specified sigma ranges
        * \author juergen holzner
        */
        typedef enum SIGMA_OPTION_SET
        {
            ESD_UNDEF = 0, ESD_DATA_SIGMA = 1, ESD_GIVEN_SIGMA = 2
        } SigmaOptionType;

        template<class T> class ZFS_API ExtendedStatisticalData : public BasicStatisticalData<T>
        {

        public:

            ExtendedStatisticalData();
            ExtendedStatisticalData(const std::vector<T>& data_base, const double& aSigma = -1., const unsigned int aNSigma = 0, const unsigned int aNSigmaStep = 0); //!< if -1.0 then data sigma is used
            ~ExtendedStatisticalData();

            void calcExtendedStatistics(const std::vector<T>& data_base, const double& aSigma = -1., const unsigned int aNSigma = 0, const unsigned int aNSigmaStep = 0); //!< if -1.0 then data sigma is used
            void calcStatistics(const std::vector<T>& data_base);

            // -- getter and setter
            void setSigma(const double& aSigma, const unsigned int aNSigma = 0, const unsigned int aNSigmaStep = 0);

            SigmaOptionType           getSigmaOption() const;
            double                    getSigma() const;
            unsigned int              getNSigma() const;
            bool                      getStatisticsCurrent() const;
            std::vector<unsigned int> getNoOfSamplesWithInMultiplesOfSigma() const;
            std::vector<unsigned int> getNoOfSamplesWithInMultiplesOfSigmaCum() const;

        protected:

            std::vector<unsigned int>    m_NoOfSamplesWithInMultiplesOfSigma;
            std::vector<unsigned int>    m_NoOfSamplesWithInMultiplesOfSigmaCum;
            bool                         m_bExtendedStatisticsCalculated;
            unsigned int                 m_NSigma; //< Maximum Sigma Value for which the no. of points shall be calculated
            unsigned int                 m_NSigmaStep;
            double                       m_Sigma;
            SigmaOptionType              m_SigmaOption;

            void clear();
        };

    } // end namespace math
} // end namespace zfs
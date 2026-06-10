#pragma once

#include "filterapi.h"

#include "Matrix.h"

namespace zfsfilter {
    // ===========================================================================================
    // Kalman filter sample
    // ===========================================================================================
    template<class T> class FILTERDLL_API KalmanFilterDataSample
    {

    public:

        T sample_value;
        T sample_noise_var;

        KalmanFilterDataSample();

        KalmanFilterDataSample( const T& a_sample_value,
                                const T& a_sample_noise_var );
    };
    // ===========================================================================================
    // Kalman filter line
    // ===========================================================================================
    template<class T> class FILTERDLL_API KalmanFilterDataLine
    {

    public:

        std::vector< KalmanFilterDataSample<T> > m_LineData;

        KalmanFilterDataLine();
        KalmanFilterDataLine( const std::vector<T>& measurements, const std::vector<T>& noise_var );

        KalmanFilterDataSample<T>& operator[]( const uint32_t idx );
        KalmanFilterDataSample<T> operator[]( const uint32_t idx ) const;

        size_t getNumberOfSamples() const;

    };
    // ===========================================================================================
    // KalmanFilter
    // ===========================================================================================
    template <class T> class FILTERDLL_API KalmanFilter
    {
        protected:
            T            y_k;  ///<innovation
            Matrix<T>      H;  ///<observation matrix
            Matrix<T>    x_k;  ///<current state vector
            Matrix<T>    P_k;  ///<current state error co-variance matrix
            Matrix<T>      Q;  ///<process co-variance matrix, process noise variance
            Matrix<T>      A;  ///<system matrix, is updated during the process
            Matrix<T> xp_kp1;  ///<predicted measurement state vector
            Matrix<T> Pp_kp1;  ///<predicted posterior error co-variance matrix
            Matrix<T>    S_k;  ///<innovation co-variance
            Matrix<T>  K_kp1;  ///<kalman gain
            Matrix<T>  x_kp1;  ///<posteriori state vector
            Matrix<T>  P_kp1;  ///<posterior state error co-variance

            KalmanFilterDataLine<T> m_KalmanFilterDataLine;

        public:
            KalmanFilter();
            KalmanFilter(const int number_of_states);
            virtual ~KalmanFilter() {}

            virtual void filter() = 0;             ///< filter function

            virtual void initialisation() = 0;     ///< initialisation ( kalman filter step 1 )
            virtual void time_update() = 0;        ///< prediction     ( kalman filter step 2 )
            virtual void measurement_update() = 0; ///< correction     ( kalman filter step 3 )

            KalmanFilterDataLine<T>& KalmanSampleDataAccess();

            size_t getNumberOfSamples() const;
    };
    // ===========================================================================================
} // namespace zfsfilter

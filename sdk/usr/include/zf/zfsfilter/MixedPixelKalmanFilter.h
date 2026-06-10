#pragma once

#include <vector>
#include <cstdint>

#include "filterapi.h"

#include <zfs/Math/math/polynomial.h>

#include "ExtendedKalmanFilter.h"

namespace zfsfilter {

    /** \class MixedPixelKalmanFilterParameter
     * \brief  Parameter for MixedPixelKalmanFilter.
     */

    template<typename T> class FILTERDLL_API MixedPixelKalmanFilterParameter
    {
    public:
        uint32_t m_DataSize;
        T        m_AngleStep;
        T        m_ProcessVariance;
        T        m_ScannerTypeNoiseOffsetVar;

        MixedPixelKalmanFilterParameter( );
        MixedPixelKalmanFilterParameter( const T&       a_AngleStep,
                                         const T&       a_ProcessVariance,
                                         const T&       a_ScannerTypeNoiseOffsetVar,
                                         const uint32_t a_DataSize );
    };

    /** \class MixedPixelKalmanFilter
     * \brief  Class implementing a kalman filter for mixed pixel filtering.
     *
     * \author Juergen Holzner
     *
     * Class implementing a kalman filter for mixed pixel filtering
     *
     */

    template<class T> class FILTERDLL_API MixedPixelKalmanFilter : public ExtendedKalmanFilter<T>
    {
        // -- friend class
        template<typename S> friend class MixedPixelKalmanProcessing;

        static const T ZETA_EPS;

        // -- private member variables

        // parameters (input)
        MixedPixelKalmanFilterParameter<T> m_MixedPixelKalmanFilterParameter;

        // filter variables, time counters
        bool                       m_bvalid;
        uint32_t                   m_k;
        size_t                     m_L;

        // parameters (internal)
        T                          m_2xcosAngleStep;

        // filter signal variables
        T                          zeta_k;

        // filter input data
        std::vector<bool> maskI;

        // filter (output/intermediate) data
        std::vector<bool> invalids;
        std::vector<bool> linear_sections;

        // private member
        void   basic_initialization();
        void   initialisation();                                ///<initialisation
        void   find_valid_sequence();                           ///<find a valid sequence starting at m_k
        void   time_update();                                   ///<prediction
        void   measurement_update();                            ///<correction
        void   update_system_matrix();                          ///<new system matrix for time step
        void   calculate_state_vector_time_update();            ///<new state vector for time step
        void   determine_current_process_variance();            ///<update process variance
        void   reinitialization();                              ///<after process change

    public:
        // -- private constants
        static const T m_c_QStddevValues[3];
        static const T m_c_SharpnessFactors[3];

        // constructors
        MixedPixelKalmanFilter();
        MixedPixelKalmanFilter( const MixedPixelKalmanFilterParameter<T>& a_MixedPixelKalmanFilterParameter );

        // public member functions
        void init_parameter( const MixedPixelKalmanFilterParameter<T>& a_MixedPixelKalmanFilterParameter );

        // operators
        void filter();                                   
    };
} // namespace zfsfilter

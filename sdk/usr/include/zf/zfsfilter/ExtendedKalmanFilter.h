#pragma once

#include "filterapi.h"

#include "KalmanFilter.h"

namespace zfsfilter {
    /** \class ExtendedKalmanFilter
     * \brief  Base class for providing extended kalman filtering functionality.
     *
     * \author Juergen Holzner
     *
     * Base class for providing extended kalman filtering functionality.
     *
     */

    template<class T> class FILTERDLL_API ExtendedKalmanFilter : public KalmanFilter<T>
    {
    public:
        ExtendedKalmanFilter() : KalmanFilter<T>() {}
        ExtendedKalmanFilter(const int number_of_states) : KalmanFilter<T>(number_of_states) {}

        void filter(){}              ///< filter function 

        void initialisation() {}     ///< filter step: initialisation
        void time_update() {}        ///< filter step: prediction
        void measurement_update() {} ///< filter step: correction

        virtual void update_system_matrix() = 0;               ///<new system matrix for time step
        virtual void calculate_state_vector_time_update() = 0; ///<new state vector for time step
    };
} // namespace zfsfilter

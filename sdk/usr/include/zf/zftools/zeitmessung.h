#pragma once

#include "../Common/inttypes_.h"

#include <zfcore/timemeasurement.h>

namespace zftools
{
    typedef zf::TimeMeasurement_<uint32_t, std::milli> Zeitmessung;

    using TimeMeasurement = zf::TimeMeasurement_<double,zf::standard_period>;

#if 0


    typedef std::ratio<1, 1> standard_period; // [seconds]


    /**
    @brief class for (programm execution) time measurement
    */
    template <typename return_t, typename _Period>
    class TimeMeasurement
    {
        std::chrono::steady_clock::time_point m_tmStart;
        std::chrono::steady_clock::time_point m_tmLast;		///< last time duration() called

    public:
        TimeMeasurement()
        {
            restart();
        }

        /// restart time
        void restart()
        {
            m_tmStart = m_tmLast = std::chrono::steady_clock::now();
        }

        ///  set new point for duration2last
        void capture()
        {
            m_tmLast = std::chrono::steady_clock::now();
        }

        /// return the duration to start [ms]
        return_t duration()
        {
            const std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
            const return_t duration = std::chrono::duration_cast<std::chrono::duration<return_t, _Period>>(cur - m_tmStart).count();
            return duration;
        }

        double durationSeconds() { return double(duration()) / 1000.; }

        /// more precise return value [ms]
        double durationAccurate()
        {
            const std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::duration<double, standard_period>>(cur - m_tmStart).count() * 1000.;
        }

        /// the duration to the last set point. If no last set, it is the time to the start
        return_t duration2last()
        {
            std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
            return_t duration = std::chrono::duration_cast<std::chrono::duration<return_t, _Period>>(cur - m_tmLast).count();
            m_tmLast = cur;
            return duration;
        }

    };

    typedef TimeMeasurement<uint32_t,std::milli> Zeitmessung;

#endif
} // end namespace zftools

#pragma once


#include <chrono>
#include <ratio>

namespace zf
{

    typedef std::ratio<1, 1> standard_period; // [seconds]


    /**
    @brief class for (programm execution) time measurement
    */
    template <typename return_t, typename _Period>
    class TimeMeasurement_
    {
        std::chrono::steady_clock::time_point m_tmStart;
        std::chrono::steady_clock::time_point m_tmLast;		///< last time duration() called

    public:
        TimeMeasurement_()
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
        return_t durationMS() const
        {
            const std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
            const return_t duration = std::chrono::duration_cast<std::chrono::duration<return_t, _Period>>(cur - m_tmStart).count();
            return duration;
        }

        return_t duration() const { return durationMS(); }

        double durationSeconds() const { return double(durationMS()) / 1000.; }

        /// more precise return value [ms]
        double durationAccurate() const
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

    typedef TimeMeasurement_<uint32_t,std::milli> TimeMeasurement;


} // end namespace zftools

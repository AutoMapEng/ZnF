#pragma once

#include <zfs/zfsapi.h>

#include "dft.h"

namespace zfs {
    namespace math {
        // ====================================================================================
        /* @class Upsample
        *  @brief Class for upsampling of signals by a constant factor
        *  @author Juergen Holzner
        *  This class gives the possibility to upsample signals by a constant factor
        *  using fft (this is merely for periodic signals, or signals which were "made"
        *  periodic, e.g. using a suitable approach like appropriate windowing of the time domain signal)
        *  Class Interpolation can be also used by suitable choice of the input resampling position vector.
        *  Class Interpolation uses FIR (resampling) kernels that "include"/act like a low-pass (wheighted average).
        *  With FFTs, the operation could possibly be made faster (blocking, fixed FFT - length, sample selection, windowing ...).
        */
        template<class T> class ZFS_API Upsample
        {
        public:
            Upsample();
            Upsample(const std::vector<T>& aInputSignal, const int aUpsamplingFactor);
            ~Upsample();

            int                  getUpsamplingFactor() const;
            const std::vector<T> getInputSignal()      const;
            const std::vector<T> getUpsampledSignal()  const;

            void setInputSignal(const std::vector<T>& aInputSignal);
            void setUpsamplingFactor(const int aFactor);

            void upsample();

        private:

            int            m_UpsamplingFactor;
            std::vector<T> m_InputSignal;
            std::vector<T> m_UpsampledSignal;

            const std::vector<T> doUpsampling();
        };
    } // end namespace math
} // end namespace zfs
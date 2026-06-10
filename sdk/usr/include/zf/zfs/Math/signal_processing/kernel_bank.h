#pragma once

#include <vector>

#include <zfs/zfsapi.h>

#include "kernels.h"

namespace zfs {
    namespace math {
        /*
        * @class for keeping a "bank" of precalculated kernels for resampling
        * @author J. Holzner
        */
        template<class T> class ZFS_API KernelBank
        {
        public:
            KernelBank();
            KernelBank(const int aPixelDivisor,
                const KernelType aKernelType,
                const int aKernelLength,
                const SignalWindowNameType aSignalWindowType);
            ~KernelBank();

            void initialize(const int aPixelDivisor,
                const KernelType aKernelType,
                const int aKernelLength,
                const SignalWindowNameType aSignalWindowType);

            const Kernel<T>* const operator[](const T& aPixelShift) const;
            const std::vector<T> operator()(const T& aPixelShift) const;

            void setPixelResolution(const int aPixelDivisor = 1); // set a division factor

            void setResamplingKernel(const KernelType aKernelType = KernelType::KS_UNDEF,
                const int aKernelLength = 0,
                const SignalWindowNameType aSignalWindowType = SignalWindowNameType::SWN_NONE);

            T      getPixelResolution()  const;
            size_t getNoOfKernels() const;

            int        getKernelLength() const;
            int        getKernelWindow() const;
            KernelType getKernelType() const;

            void   clear();

        private:
            // -- class constants
            static const int                          m_cDefaultResKernelTruncSincLength;
            static const SignalWindowNameType         m_cDefaultResKernelWindow4TruncSinc;

            // -- member variables
            bool                                      m_bRecalculate;
            int                                       m_PosKernelIdxOffset; /// < Offset for Kernel indexing for pixel shifts gt 0
            int                                       m_NoOfKernelsD2;

            T                                         m_PixelResolution;

            KernelType                                m_KernelType;
            int                                       m_TruncSincLength;
            SignalWindowNameType                      m_TruncSincWindow;

            std::vector<Kernel<T>*>                   m_KernelBank;

            // -- kernel bank generator
            void generateKernelBank();
            int  pixelShiftToKernelBankIdx(const T& aPixelShift) const;
        };
    } // end namespace math
} // end namespace zfs
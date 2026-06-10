#pragma once

#include <zfs/zfsapi.h>

#include "kernels.h"

namespace zfs {
    namespace math {
        // ====================================================================================
        /* @class Interpolation
        *  @brief Class for interpolation of signals
        *  @author Juergen Holzner
        *  Class for interpolation of a 1-D signal at a certain position
        */
        template<class T> class ZFS_API Interpolation
        {
        public:

            Interpolation();
            Interpolation(const KernelType aKernelType);
            Interpolation(const KernelType aKernelType, const std::vector<T>& aInputSignal);

            Interpolation(const T& aPosition, const KernelType aKernelType);
            Interpolation(const T& aPosition, const std::vector<T>& aInputSignal, const KernelType aKernelType);

            Interpolation(const std::vector<T>& aPositionVector, const KernelType aKernelType);
            Interpolation(const std::vector<T>& aPositionVector, const std::vector<T>& aInputSignal, const KernelType aKernelType);

            Interpolation(Kernel<T>* const aKernel);
            Interpolation(Kernel<T>* const aKernel, const std::vector<T>& aInputSignal);

            ~Interpolation();

            // -- initializer
            void initialization(const T& aPosition, const KernelType aKernelType);
            void initialization(const std::vector<T>& aPositionVector, const KernelType aKernelType);

            void initialization(const T& aPosition, const std::vector<T>& aInputSignal, const KernelType aKernelType);
            void initialization(const std::vector<T>& aPositionVector, const std::vector<T>& aInputSignal, const KernelType aKernelType);

            void initialization(Kernel<T>* const aKernel, const std::vector<T>& aInputSignal);

            // -- setter

            // - input
            void setInputSignal(const std::vector<T>& aInputSignal);

            // - kernel
            void setKernel(Kernel<T>* const aKernel);
            void setKernelType(const KernelType aKernelType, const int aKernelLength = -1, const SignalWindowNameType aSignalWindow = SignalWindowNameType::SWN_NONE);

            // - positions to interpolate (it also possible to specify a constant shift)

            // single position
            void setInterpolationPosition(const T& aPosition);

            // vector of positions to be interpolated
            void setInterpolationPositionVector(const std::vector<T>& aPositionVector);

            // shift
            void setShift(const T& aShift);

            // -- getter
            const KernelType     getKernelType() const;
            T                    getInterpolationResult() const;
            const std::vector<T> getInterpolationResultVector() const;

            // -- conduct interpolation
            void interpolate();                      //< Must be called for interpolation
            T interpolate(const T& aPosition) const; //< Must be called for interpolation

        private:

            static const T m_c_LowerBorderPixelDifference;

            bool           m_bShiftSet;                //< (internal) what should be interpolated when interpolation is called
            bool           m_bPositionSet;             //< (internal) what should be interpolated when interpolation is called
            bool           m_bPositionVectorSet;       //< (internal) what should be interpolated when interpolation is called
            bool           m_bKernelSetExternally;

            KernelType     m_KernelType;
            Kernel<T>* m_Kernel;

            std::vector<T> m_InputSignal;

            T              m_Shift;
            T              m_InterpolationPosition;
            std::vector<T> m_InterpolationPositionVector;

            T              m_InterpolationResult;
            std::vector<T> m_InterpolationVectorResult;

            void basic_initialization();
            void free();

        };
    } // end namespace math
} // end namespace zfs
#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <cstdint>

#include <zfs/Math/math/tuple.h>

#include "window.h"

namespace zfs {
    namespace math {
        // ========================================================================================================
        // Available Kernel Set
        enum class KernelType: uint16_t { KS_UNDEF = 0, KS_TWO_POINT = 1, KS_FOUR_POINT = 2, KS_SIX_POINT = 3, KS_TRUNCATED_SINC = 4 };
        // ========================================================================================================
        /* \class Kernel
        *  \brief Base class for interpolation kernels
        *
        *  \author Juergen Holzner
        *
        *
        */
        template<class T> class ZFS_API Kernel
        {

        public:
            Kernel();
            Kernel(const KernelType& aKernelType);
            Kernel(const KernelType& aKernelType, const T& aSubPixelPosition);
            virtual ~Kernel();

            void initialize(const KernelType& aKernelType, const T& aSubPixelPosition);

            T operator[](const int aIndex);

            const std::vector<T> getKernelWeights() const;
            int                  getKernelLength() const;
            KernelType           getKernelType() const;
            T                    getPosition() const;

            void setKernelType(const KernelType& aKernelType);
            void setPosition(const T& aSubPixelPosition);

            // -- kernel dependent
            // Since this function is called during generation of object (in the constructor)
            // a virtual call solution does not work ... use static function
            // see: http://stackoverflow.com/questions/4869216/calling-derived-class-function-from-base-class
            static T KernelFunction(const T& /*aKernelSamplingPosition*/) { return T(0); }

        protected:
            // -- member variables
            KernelType           m_KernelType;
            int                  m_KernelLength;
            T                    m_SubPixelPosition;
            std::vector<T>       m_KernelWeights;

            // -- protected functions
            int                  KernelLengthOfKernelType(const KernelType& aKernelType);

            T                    ShiftCondition(const T& aSubPixelPosition);
            const Tuple2<int>    SupportBaseForKernelWeights(const T& aSubPixelPosition);
            virtual const std::vector<T> CalculateKernelWeights(const T& aSubPixelPosition);

        };

        // ========================================================================================================
        // Generation of specific kernel classes (simple cases)
        // ========================================================================================================
        #define SPECIFIC_SIMPLE_KERNEL_CLASS_GENERATOR_MACRO(KERNEL_CLASS_NAME, KERNEL_SPECIFIER)\
        template<class T> class ZFS_API KERNEL_CLASS_NAME: public Kernel<T>\
        {\
        public:\
            KERNEL_CLASS_NAME():Kernel<T>( KERNEL_SPECIFIER,T(0)){;}\
            KERNEL_CLASS_NAME( const T& aSubPixelPosition):Kernel<T>(KERNEL_SPECIFIER,aSubPixelPosition){;}\
            ~KERNEL_CLASS_NAME(){;}\
        \
            static T KernelFunction( const T& aKernelSamplingPosition );\
        };

        SPECIFIC_SIMPLE_KERNEL_CLASS_GENERATOR_MACRO(TwoPointKernel, KernelType::KS_TWO_POINT) // Class for 2-point/linear interpolation
        SPECIFIC_SIMPLE_KERNEL_CLASS_GENERATOR_MACRO(FourPointKernel, KernelType::KS_FOUR_POINT) // Class for interpolation using a four-point-kernel
        SPECIFIC_SIMPLE_KERNEL_CLASS_GENERATOR_MACRO(SixPointKernel, KernelType::KS_SIX_POINT) // Class for interpolation using a six-point-kernel

        #undef SPECIFIC_SIMPLE_KERNEL_CLASS_GENERATOR_MACRO

        // ========================================================================================================
        /* \class TruncatedSincKernel
        *  \brief Class for interpolation using a truncated sinc kernel
        *
        *  \author Juergen Holzner
        *
        */
        template <class T> class ZFS_API TruncatedSincKernel : public Kernel<T>
        {
        public:
            TruncatedSincKernel();
            TruncatedSincKernel(const int aKernelLength, const SignalWindowNameType aWindowName);
            TruncatedSincKernel(const int aKernelLength, const SignalWindowNameType aWindowName, const T& aSubPixelPosition);
            ~TruncatedSincKernel();

            void setSignalWindowType(const SignalWindowNameType aWindowName);
            SignalWindowNameType getSignalWindowType() const;

            void setPosition(const T& aSubPixelPosition);
            void setKernelLength(const int aKernelLength);
            void setApplyNormalization(const bool aSetflag = true);

            void initialize(const int aKernelLength, const SignalWindowNameType aWindowName, const T& aSubPixelPosition);

            T KernelFunction(const T& aKernelSamplingPosition) const;

        protected:
            bool             m_ApplyNormalization;
            bool             m_Recalculate;
            SignalWindow<T>* m_SignalWindow;

            void free();
            void initialize(const int aLength, const SignalWindowNameType aWindowName);
            void setSignalWindow(const SignalWindowNameType aWindowName);

            virtual const std::vector<T> CalculateKernelWeights(const T& aSubPixelPosition);

        };
        // ========================================================================================================
    } // end namespace math
} // end namespace zfs 

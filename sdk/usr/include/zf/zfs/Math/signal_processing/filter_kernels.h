#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include <zfs/zfsapi.h>

#include "window.h"

namespace zfs {
    namespace math {
        // ========================================================================================================
        // filter kernel class
        // ========================================================================================================
        /** \class FilterKernel
          * \brief  Class for providing filter kernels based on windowing functions.
          *
          * \author Juergen Holzner
          *
          * Class for providing filter kernels based on windowing functions.
          *
          * . The kernel is derived from the given signal window name, see "window.h": e.g. SWN_RECT, SWN_HANN, SWN_KAISER
          * . For some of the windows a parameter is necessary, for a kaiser window (SWN_KAISER) where a parameter beta (typical values between 4 and < 9) is used [see Kammeyer/Kroschel, "Digital Signal Processing"].
          * . The length of the kernel is determined from the given kernel length parameter (aKernelLengthParameter) which is a floating point length of the kernel.
          *   In case of the Gaussian Window this parameter can be omitted, then a length of 2* 3.5 * sigma is assumed.
          * . Sampling is symmetrically conducted on the set of (positive and negative) integer pixel indices.
          * . If parameter aEvenSampling is set to true sampling is symmetrically conducted at positive and negative integer pixel indices + 0.5, kernel length is then even.
          * . If parameter aNormalize is set to true the kernel is normalized so that the sum of all kernel values amounts to 1 (, otherwise the maximum for uneven sampling is 1).
          *
          */

        template<typename T> class ZFS_API SignalWindowBasedFilterKernel
        {

        public:

            SignalWindowBasedFilterKernel();
            SignalWindowBasedFilterKernel(const SignalWindowBasedFilterKernel<T>& aSrc);

            SignalWindowBasedFilterKernel(SignalWindowNameType aWindowName, T aKernelLengthParameter, bool aEvenSampling, bool aNormalize);
            SignalWindowBasedFilterKernel(SignalWindowNameType aWindowName, T aWindowParameter, T aKernelLengthParameter, bool aEvenSampling, bool aNormalize);

            virtual ~SignalWindowBasedFilterKernel();

            void initialize(SignalWindowNameType aWindowName, T aKernelLengthParameter, bool aEvenSampling, bool aNormalize);
            void initialize(SignalWindowNameType aWindowName, T aWindowParameter, T aKernelLengthParameter, bool aEvenSampling, bool aNormalize);

            std::vector<T> getSampledKernel() const;
            std::vector<T> getSampledKernelSupport() const;

            T operator[](const size_t index) const;

            SignalWindowBasedFilterKernel& operator=(const SignalWindowBasedFilterKernel<T>& aSrc);

        protected:

            // protected member variables
            bool              m_EvenSampling;
            bool              m_Normalize;

            T                 m_WindowParameter;
            T                 m_KernelLengthParameter;

            std::vector<T>    m_KernelSupport;
            std::vector<T>    m_SampledKernel;

            SignalWindow<T>* m_pSignalWindow;

            // protected functions
            virtual void      normalize();                               // used in generate_sampled_kernel(), virtual for this class and derived classes 
            virtual void      setType(SignalWindowNameType aWindowName); // used in initialize(.), virtual for this class and derived classes 

            void              basic_initialization();

            void              freeSignalWindow();

            void              generate_sampled_kernel();
            void              generate_sampled_kernel_support();



            void              setWindowParameter(T aWindowParameter);
            void              setKernelLengthParameter(T aKernelLengthParameter);

            void              setEvenSampling(bool aFlag);
            void              setNormalize(bool aFlag);


        };

        // ========================================================================================================
        // BandwidthEquivalentLength of sampled filter kernels with respect to a given
        // reference rect-Function
        // ========================================================================================================
        template<typename T> double ZFS_API BandwidthEquivalentLength(SignalWindowNameType Window,
            int32_t ReferenceRectWindowLength,
            bool bBestSuitableInteger,
            bool bEvenSampling,
            bool& bConverged,
            const T& WindowParameter = 0);

        // ========================================================================================================
        // Generation of differential kernel classes (without parameter)
        // ========================================================================================================
        #define SPECIFIC_DIFFERENTIAL_KERNEL_CLASS_GENERATOR_MACRO(CLASS_NAME, KERNEL_TYPE_NAME)\
        template<typename T> class ZFS_API CLASS_NAME: public SignalWindow<T>\
        {\
        public:\
            CLASS_NAME(const T& a_Tdk= 1.):SignalWindow<T>(KERNEL_TYPE_NAME), Tdk(a_Tdk){}\
            ~CLASS_NAME(){}\
        \
        protected:\
            T Tdk;\
            T WindowFunction( const T& aPos ) const;\
        };

        SPECIFIC_DIFFERENTIAL_KERNEL_CLASS_GENERATOR_MACRO(DifferentialHannKernelFunction, SignalWindowNameType::SWN_HANN)
        SPECIFIC_DIFFERENTIAL_KERNEL_CLASS_GENERATOR_MACRO(DifferentialBlackmanKernelFunction, SignalWindowNameType::SWN_BLACKMAN)
        SPECIFIC_DIFFERENTIAL_KERNEL_CLASS_GENERATOR_MACRO(DifferentialBlackmanHarrisKernelFunction, SignalWindowNameType::SWN_BLACKMAN_HARRIS)
        SPECIFIC_DIFFERENTIAL_KERNEL_CLASS_GENERATOR_MACRO(DifferentialBlackmanNuttalKernelFunction, SignalWindowNameType::SWN_BLACKMAN_NUTTAL) // MATLAB: NUTTAL Window or Nuttal defined Blackman-Harris window

        #undef SPECIFIC_DIFFERENTIAL_KERNEL_CLASS_GENERATOR_MACRO

        // ========================================================================================================
        // Generation of signal window classes (with parameter)
        // ========================================================================================================
        
        #define SPECIFIC_DIFFERENTIAL_KERNEL_WITH_PARAMETER_CLASS_GENERATOR_MACRO(CLASS_NAME, KERNEL_TYPE_NAME)\
                template<typename T> class ZFS_API CLASS_NAME: public SignalWindowWithParameter<T>\
                {\
                public:\
                    CLASS_NAME(const T& parameter, const T& a_Tdk= 1.)\
                        :SignalWindowWithParameter<T>(KERNEL_TYPE_NAME,parameter), Tdk(a_Tdk){}\
                    ~CLASS_NAME(){}\
                \
                protected:\
                    T Tdk;\
                    T WindowFunction( const T& aPos ) const;\
                };
        
        SPECIFIC_DIFFERENTIAL_KERNEL_WITH_PARAMETER_CLASS_GENERATOR_MACRO(DifferentialKaiserKernelFunction, SignalWindowNameType::SWN_KAISER)
        
        #undef SPECIFIC_DIFFERENTIAL_KERNEL_WITH_PARAMETER_CLASS_GENERATOR_MACRO
        
        // ========================================================================================================
        // Differential Gaussian Window class
        // ========================================================================================================
        template<typename T> class ZFS_API DifferentialGaussianKernelFunction : public GaussianWindow<T>
        {
        public:
            DifferentialGaussianKernelFunction(const T& a_sigma, const T& a_dt) : GaussianWindow<T>(a_sigma), dt(a_dt)
            {}

            DifferentialGaussianKernelFunction(const T& a_sigma, const T& a_extend, const T& a_dt = 1.) : GaussianWindow<T>(a_sigma, a_extend), dt(a_dt)
            {}

        protected:
            T dt{ 0 };
            T WindowFunction(const T& aPos) const;
        };

        // ========================================================================================================
        /** \class DifferentialFilterKernel
         * \brief  Class for providing differential filter kernels based on windowing functions.
         *
         * \author Juergen Holzner
         *
         * Class for providing differential filter kernels based on windowing functions.
         *
         */

        template<typename T> class ZFS_API SignalWindowBasedDifferentialFilterKernel : public SignalWindowBasedFilterKernel<T>
        {

        public:

            SignalWindowBasedDifferentialFilterKernel();
            SignalWindowBasedDifferentialFilterKernel(SignalWindowNameType aWindowName, T aKernelLengthParameter, bool aEvenSampling, bool aNormalize);
            SignalWindowBasedDifferentialFilterKernel(SignalWindowNameType aWindowName, T aKernelParameter, T aKernelLengthParameter, bool aEvenSampling, bool aNormalize);

            bool getInitOK() const;
            std::string getError() const;

        protected:

            bool        m_bInitOK{ false };
            std::string m_strError{ "" };

            void setType(SignalWindowNameType aKernelName);

            void normalize();

        };

        // ========================================================================================================
    } // end namespace math
} // end namespace zfs
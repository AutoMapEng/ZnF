#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // -- Signal window names
        enum class SignalWindowNameType : uint16_t { SWN_NONE, SWN_RECT, SWN_HAMMING, SWN_HANN, SWN_BLACKMAN, SWN_BLACKMAN_HARRIS, SWN_BLACKMAN_NUTTAL, SWN_KAISER, SWN_GAUSS } ;

        // ========================================================================================================
        // Signal Window Base Class
        // ========================================================================================================
        /** \class SignalWindow
         * \brief  Class Base class for providing signal windows
         *
         * \author Juergen Holzner
         *
         * Base class for providing signal windows
         *
         */
        template<class T> class ZFS_API SignalWindow
        {
        public:
            SignalWindow();
            SignalWindow(const SignalWindowNameType& aWindowName);
            virtual ~SignalWindow();

            void                 setWindowName(const SignalWindowNameType& aWindowName);
            SignalWindowNameType getWindowName() const;

            T                    WindowValAt(const T& aPos) const;

        protected:
            SignalWindowNameType m_SignalWindowName;

            bool CheckValidSignalWindowName(const SignalWindowNameType& aWindowName);

            virtual T WindowFunction(const T& aPos) const = 0;

        };
        // ========================================================================================================
        // Generation of signal window classes (without parameter)
        // ========================================================================================================
        #define SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(CLASS_NAME, WINDOW_NAME)\
        template<class T> class ZFS_API CLASS_NAME: public SignalWindow<T>\
        {\
        public:\
            CLASS_NAME():SignalWindow<T>(WINDOW_NAME){}\
            ~CLASS_NAME(){}\
        \
        protected:\
            T WindowFunction( const T& aPos ) const;\
        };

        SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(RectWindow, SignalWindowNameType::SWN_RECT)
        SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(HammingWindow, SignalWindowNameType::SWN_HAMMING)
        SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(HannWindow, SignalWindowNameType::SWN_HANN)
        SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(BlackmanWindow, SignalWindowNameType::SWN_BLACKMAN)
        SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(BlackmanHarrisWindow, SignalWindowNameType::SWN_BLACKMAN_HARRIS)
        SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO(BlackmanNuttalWindow, SignalWindowNameType::SWN_BLACKMAN_NUTTAL) // MATLAB: NUTTAL Window or Nuttal defined Blackman-Harris window

       #undef SPECIFIC_SIGNAL_WINDOW_CLASS_GENERATOR_MACRO

        // ========================================================================================================
        // signal window classes with parameter
        // ========================================================================================================
        template<class T> class ZFS_API SignalWindowWithParameter : public SignalWindow<T>
        {
        public:
            SignalWindowWithParameter();
            SignalWindowWithParameter(const SignalWindowNameType& aWindowName, const T& Parameter);

            const T& getParameter() const;

        protected:

            T parameter;

        };

        // ========================================================================================================
        // Generation of signal window classes (with parameter)
        // ========================================================================================================

        #define SPECIFIC_SIGNAL_WINDOW_WITH_PARAMETER_CLASS_GENERATOR_MACRO(CLASS_NAME, WINDOW_NAME)\
        template<class T> class ZFS_API CLASS_NAME: public SignalWindowWithParameter<T>\
        {\
        public:\
            CLASS_NAME(const T& parameter):SignalWindowWithParameter<T>(WINDOW_NAME,parameter){}\
            ~CLASS_NAME(){}\
        \
        protected:\
            T WindowFunction( const T& aPos ) const;\
        };

        SPECIFIC_SIGNAL_WINDOW_WITH_PARAMETER_CLASS_GENERATOR_MACRO(KaiserWindow, SignalWindowNameType::SWN_KAISER)

        #undef SPECIFIC_SIGNAL_WINDOW_WITH_PARAMETER_CLASS_GENERATOR_MACRO

        // ========================================================================================================
        // Gaussian Filter Window
        // ========================================================================================================
        template<class T> class ZFS_API GaussianWindow : public SignalWindowWithParameter<T>
        {
        public:
            GaussianWindow();
            GaussianWindow(const T& a_sigma);
            GaussianWindow(const T& a_sigma, const T& a_extend);
        
            T getMaxPos() const;
            T getSigma() const;
            T getExtend() const;
        
        protected:
        
            T sigma, sigma_sqr;
            T max_pos;
        
            void init(const T& a_sigma, const T& a_extend);
        
            T WindowFunction(const T& aPos) const;
        
        };

        // ========================================================================================================
        // Sampled window class
        // ========================================================================================================
        /** \class SampledSignalWindow
         * \brief  Class for providing several signal windows for signal analysis/operations (dft,fft).
         *
         * \author Juergen Holzner
         *
         * Class for providing several signal windows for signal analysis/operations (dft,fft).
         *
         */
        template<class T> class ZFS_API SampledSignalWindow
        {
        public:
            SampledSignalWindow();
            SampledSignalWindow(SignalWindowNameType aWindowName);

            SampledSignalWindow(SignalWindowNameType aWindowName, int aLength);
            SampledSignalWindow(SignalWindowNameType aWindowName, T aWindowParameter, int aLength);

            virtual ~SampledSignalWindow();

            void initialize(SignalWindowNameType aWindowName, int aLength);
            void initialize(SignalWindowNameType aWindowName, T  aWindowParameter, int aLength);

            T operator[](const size_t index) const;

            size_t               getLength() const;
            SignalWindowNameType getType()   const;
            std::vector<T>       getSampledWindow() const;
            std::vector<T>       getSampledWindowSupport() const;

        protected:

            size_t                m_Length;
            T                     m_WindowParameter;
            SignalWindow<T>*      m_pSignalWindow;

            std::vector<T>        m_SampledWindow;
            std::vector<T>        m_SampledWindowSupport;

            void                  basic_initialization();
            void                  freeSignalWindow();

            void                  generate_sampled_signal_window();

            void                  setLength(size_t aLength);
            void                  setParameter(const T& aWindowParameter);

            void                  setType(SignalWindowNameType aWindowName);

        };
        // ========================================================================================================
    } // end namespace math
} // end namespace zfs
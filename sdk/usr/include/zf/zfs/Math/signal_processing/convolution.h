#pragma once

#include <cstdlib>
#include <vector>

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        /*Forward declaration*/
        template <class T> class Kernel;

        /* @brief Type/Enum for selecting the correlation alignment options
        */
        typedef enum CONV_ALIGN_OPTION_SET
        {
            CONV_ALIGN_OPT_UNDEF,
            CONV_ALIGN_OPT_LEFT_ALIGN,
            CONV_ALIGN_OPT_RIGHT_ALIGN

        } ConvAlignOptionSetType;

        /* @brief Type/Enum for selecting the convolution options
        */
        typedef enum CONV_OPTION_SET
        {
            CONV_OPTION_UNDEF,
            CONV_OPTION_ALL,
            CONV_OPTION_VALID,
            CONV_OPTION_SAME,
            CONV_OPTION_SAME_VALID

        } ConvOutputOptionSetType;

        /* @brief  Class for conducting a convolution
        *  @author Jürgen Holzner
        *  Given a kernel (or its weights), a convolution is conducted
        *  with a given signal. Options include (all, valid, same):
        *  all       : all samples are returned
        *  valid     : only the valid samples are returned (fall-back for "same", "same-valid")
        *  same      : the output signal has the same size as the input signal (only with kernels of uneven length)
        *  same-valid: the output signal has the same size as the input signal, invalid samples are set to 0 (only with kernels of uneven length)
        */

        template<class T> class ZFS_API Convolution
        {

        public:

            Convolution() : m_SetOutputConvOption(CONV_OPTION_UNDEF), m_SetAlignConvOption(CONV_ALIGN_OPT_UNDEF)
            {};

            Convolution(const Kernel<T>& aKernel, const std::vector<T>& aSignalSamples, const ConvOutputOptionSetType aConvOutOption, const ConvAlignOptionSetType aConvAlignOption = CONV_ALIGN_OPT_LEFT_ALIGN)
                : m_SetOutputConvOption(aConvOutOption),
                m_SetAlignConvOption(aConvAlignOption),
                m_KernelWeights(aKernel.getKernelWeights()),
                m_SignalSamples(aSignalSamples)
            {
                m_ConvolutionResult = convolve(m_KernelWeights, m_SignalSamples, m_SetOutputConvOption, m_SetAlignConvOption);
            };

            Convolution(const std::vector<T>& aKernelWeights, const std::vector<T>& aSignalSamples, const ConvOutputOptionSetType aConvOutOption, const ConvAlignOptionSetType aConvAlignOption = CONV_ALIGN_OPT_LEFT_ALIGN)
                : m_SetOutputConvOption(aConvOutOption),
                m_SetAlignConvOption(aConvAlignOption),
                m_KernelWeights(aKernelWeights),
                m_SignalSamples(aSignalSamples)
            {
                m_ConvolutionResult = convolve(m_KernelWeights, m_SignalSamples, m_SetOutputConvOption, m_SetAlignConvOption);
            };

            // -- getter
            const std::vector<T> getResult() const
            {
                return m_ConvolutionResult;
            }

            const T* const getResultPtr() const
            {
                return &m_ConvolutionResult[0];
            }

            static const std::vector<T> convolve(const std::vector<T>& aKernelWeights,
                const std::vector<T>& aSignalSamples,
                const ConvOutputOptionSetType aConvOutOption,
                const ConvAlignOptionSetType  aConvAlignOption);

            static void convolve(const std::vector<T>& aKernelWeights,
                const T* const                aSignalSamples,
                const size_t                  aSignalLength,
                T* const                      aConvolutionResult,
                int& aConvResultSignalLength,
                const ConvOutputOptionSetType aConvOutOption,
                const ConvAlignOptionSetType  aConvAlignOption);

        private:

            // -- constants
            const ConvOutputOptionSetType m_SetOutputConvOption;
            const ConvAlignOptionSetType  m_SetAlignConvOption;

            const std::vector<T>          m_KernelWeights;
            const std::vector<T>          m_SignalSamples;

            std::vector<T>                m_ConvolutionResult;

        };
    } // end namespace math
} // end namespace zfs
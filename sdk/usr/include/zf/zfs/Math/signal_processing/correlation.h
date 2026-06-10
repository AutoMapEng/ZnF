#pragma once

#include <vector>

#include <zfs/zfsapi.h>

#include "kernels.h"

namespace zfs {
    namespace math {

        /* @brief Type/Enum for selecting the correlation alignment options
        */
        typedef enum CORR_ALIGN_OPTION_SET
        {
            CORR_ALIGN_OPT_UNDEF,
            CORR_ALIGN_OPT_LEFT_ALIGN,
            CORR_ALIGN_OPT_RIGHT_ALIGN

        } CorrAlignOptionSetType;

        /* @brief Type/Enum for selecting the correlation output signal options
        */
        typedef enum CORR_OUTPUT_OPTION_SET
        {
            CORR_OPTION_UNDEF,
            CORR_OPTION_ALL,
            CORR_OPTION_VALID,
            CORR_OPTION_SAME,
            CORR_OPTION_SAME_VALID
        } CorrOutputOptionSetType;

        /* @brief  Class for conducting a Correlation
        *  @author Jürgen Holzner
        *  Given a reference signal/ correlation kernel (samples in a vector), a correlation is conducted
        *  with a given signal (samples in a vector). Length of the reference signal should be smaller signal
        *  with which the reference signal is correlated with.
        *  Options include (all, valid, same):
        *  all       : all samples are returned (default/fall-back for undefined)
        *  valid     : only the valid samples are returned (fall-back for "same", "same-valid" when kernels have even lengths)
        *  same      : the output signal has the same size as the input signal (only with kernels of uneven length)
        *  same-valid: the output signal has the same size as the input signal, invalid samples are set to 0 (only with kernels of uneven length)
        */

        template<class T> class ZFS_API Correlation
        {

        public:

            Correlation() : m_SetCorrOutputOption(CORR_OPTION_UNDEF), m_SetCorrAlignOption(CORR_ALIGN_OPT_UNDEF)
            {}

            Correlation(const Kernel<T>& aKernel, const std::vector<T>& aSignalSamples, const CorrOutputOptionSetType aCorrOutputOption, const CorrAlignOptionSetType aCorrAlignOption = CORR_ALIGN_OPT_LEFT_ALIGN)
                : m_SetCorrOutputOption(aCorrOutputOption),
                m_SetCorrAlignOption(aCorrAlignOption),
                m_CorrelationKernelWeights(aKernel.getKernelWeights()),
                m_SignalSamples(aSignalSamples)
            {
                m_CorrelationResult = correlate(m_CorrelationKernelWeights, m_SignalSamples, m_SetCorrOutputOption, m_SetCorrAlignOption);
            }

            Correlation(const std::vector<T>& aCorrelationKernelWeights, const std::vector<T>& aSignalSamples, const CorrOutputOptionSetType aCorrOutputOption, const CorrAlignOptionSetType aCorrAlignOption = CORR_ALIGN_OPT_LEFT_ALIGN)
                : m_SetCorrOutputOption(aCorrOutputOption),
                m_SetCorrAlignOption(aCorrAlignOption),
                m_CorrelationKernelWeights(aCorrelationKernelWeights),
                m_SignalSamples(aSignalSamples)
            {
                m_CorrelationResult = correlate(m_CorrelationKernelWeights, m_SignalSamples, m_SetCorrOutputOption, m_SetCorrAlignOption);
            }

            const std::vector<T> getResult() const
            {
                return m_CorrelationResult;
            }

            const T* const getResultPtr() const
            {
                return &m_CorrelationResult[0];
            }

            static const std::vector<T> correlate(const std::vector<T>& aCorrelationKernelWeights,
                const std::vector<T>& aSignalSamples,
                const CorrOutputOptionSetType aCorrOutputOption,
                const CorrAlignOptionSetType aCorrAlignOption);

            static void                 correlate(const std::vector<T>& aCorrelationKernelWeights,
                const T* const                aSignalSamples,
                const size_t                  aSignalLength,
                T* const                      aCorrelationResult,
                int& aCorrResultSignalLength,
                const CorrOutputOptionSetType aCorrOutputOption,
                const CorrAlignOptionSetType  aCorrAlignOption);

        private:

            // -- constants
            const CorrOutputOptionSetType m_SetCorrOutputOption;
            const CorrAlignOptionSetType  m_SetCorrAlignOption;

            const std::vector<T>          m_CorrelationKernelWeights;
            const std::vector<T>          m_SignalSamples;

            std::vector<T>                m_CorrelationResult;

        };
    } // end namespace math
} // end namespace zfs
#pragma once

// The following line must be defined before including math.h to correctly define M_PI
#include "Common/def.h"
SWITCH_WARNINGS_OFF
#ifndef  _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>


#include <stdio.h>
#include <stdlib.h>
#include <complex>
#include <vector>

#include <zfs/zfsapi.h>
#include "window.h"

namespace zfs {
    namespace math {
        // ========================================================================================================
        /** \class FFT
         * \brief  Class for fast fourier transform (based on numerical recipes implementation).
         *
         * \author Juergen Holzner
         *
         * Class for fast fourier transform (based on numerical recipes implementation).
         *
         */
        class ZFS_API FFT
        {
        public:

            enum Direction { FORWARD, INVERSE };

            FFT();

            // FFT of real and complex valued signals
            FFT(const std::vector<double>& Signal, FFT::Direction FFTDirection);
            FFT(const std::vector<std::complex<double> >& Signal, FFT::Direction FFTDirection);

            // FFT of real and complex valued signals including oversampling
            FFT(const std::vector<double>& Signal, int OversamplingFactor, FFT::Direction FFTDirection);
            FFT(const std::vector<std::complex<double> >& Signal, int OversamplingFactor, FFT::Direction FFTDirection);

            ~FFT();

            int                                getFFTLength() const;
            int                                getOversamplingFactor() const;
            std::vector<std::complex<double> > getResult() const;

        private:

            int                                m_FFTLength;
            int                                m_OversamplingFactor;
            std::vector<std::complex<double> > m_FFTResult;

            std::vector<std::complex<double> > fft(const std::vector<double>& signal_vector, int oversampling_factor, FFT::Direction FFTDirection);
            std::vector<std::complex<double> > fft(const std::vector<std::complex<double> >& signal_vector, int oversampling_factor, FFT::Direction FFTDirection);

            /*  Numerical Recipes Routine: http://www.haoli.org/nr/bookcpdf/c12-2.pdf

                Function four1();

                FFT/IFFT routine. (see pages 507-508 of Numerical Recipes in C)

                Inputs:
                    data[] : array of complex* data points of size 2*NFFT+1.
                    data[0] is unused,
                    * the n'th complex number x(n), for 0 <= n <= length(x)-1, is stored as:
                      data[2*n+1] = real(x(n))
                      data[2*n+2] = imag(x(n))
                    if length(Nx) < NFFT, the remainder of the array must be padded with zeros

                    nn : FFT order NFFT. This MUST be a power of 2 and >= length(x).
                    isign:  if set to 1,
                        computes the forward FFT
                    if set to -1,
                        computes Inverse FFT - in this case the output values have
                        to be manually normalized by multiplying with 1/NFFT.
                Outputs:
                    data[] : The FFT or IFFT results are stored in data, overwriting the input.
             */

             /************************************************
             * FFT code from the book Numerical Recipes in C *
             * Visit www.nr.com for the licence.             *
             ************************************************/

            void four1(double data[], int nn, int isign);

        };
        // ========================================================================================================
        /** \class SpectralAnalysis
         * \brief  Class providing functionality for spectral analysis.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for spectral analysis, e.g.
         * calculating the maximum of a spectrum.
         *
         */
        class ZFS_API SpectralAnalysis
        {
        public:
            SpectralAnalysis();

            SpectralAnalysis(const std::vector<double>& RealSignal, SignalWindowNameType WindowType);
            SpectralAnalysis(const std::vector<std::complex<double> >& ComplexSignal, SignalWindowNameType WindowType);

            SpectralAnalysis(const std::vector<double>& RealSignal, SignalWindowNameType WindowType, int OversamplingFactor);
            SpectralAnalysis(const std::vector<std::complex<double> >& ComplexSignal, SignalWindowNameType WindowType, int OversamplingFactor);

            ~SpectralAnalysis();

            int                                getSpectrumLength() const;
            int                                getOversamplingFactor() const;

            std::vector<std::complex<double> > getComlexSpectrum() const;
            std::vector<double>                getMagnitudeSpectrum() const;

            double getFrequencyOfMaximumOfSpectrum() const;
            double getFrequencyOfMaximumOfHalfSpectrum() const;

            double getFrequencyOfMaximumOfSpectrumInterpolated() const;
            double getFrequencyOfMaximumOfHalfSpectrumInterpolated() const;

        private:
            int                  m_OversamplingFactor;
            int                  m_SignalLength;
            int                  m_SpectrumLength;
            SignalWindowNameType m_WindowType;

            std::vector<std::complex<double> > m_ComplexSpectrum;
            std::vector<double>                m_MagnitudeSpectrum;

            std::vector<double>                ApplyWindow(const std::vector<double>& Signal, SignalWindowNameType WindowType) const;
            std::vector<std::complex<double> > ApplyWindow(const std::vector<std::complex<double> >& Signal, SignalWindowNameType WindowType) const;

            void                              CalculateMagnitudeSpectrum();
            double                            MaximumFromQuadratic(const std::vector<double>& ampl, const std::vector<double>& pos) const;
        };
        // ========================================================================================================
    } // end namespace math
} // end namespace zfs
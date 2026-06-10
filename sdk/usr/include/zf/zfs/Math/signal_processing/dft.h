#pragma once

#include <complex>
#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <math.h>
SWITCH_WARNINGS_ON
#include <vector>

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // ==================================================================================
        /** \class dft_base
         * \brief Class providing basic functionality for discrete fourier transform.
         *
         * \author Juergen Holzner
         *
         * Class providing basic functionality for discrete fourier transform.
         *
         */
        template<class T> class ZFS_API dft_base
        {
            template <class S> friend class dft;
            template <class S> friend class idft;

        public:

            using real_line = std::vector<T>;
            using complex_line = std::vector< std::complex < T > >;

            dft_base();

            dft_base(const int a_ExponentSign);
            dft_base(const int a_OversamplingFactor, const int a_ExponentSign);

            dft_base(const size_t& length);
            dft_base(const real_line& a_Data);
            dft_base(const real_line& a_Data, const int a_OversamplingFactor, const int a_ExponentSign);

            dft_base(const complex_line& a_ComplexData);
            dft_base(const complex_line& a_ComplexData, const int a_OversamplingFactor, const int a_ExponentSign);

            virtual ~dft_base();

            // -- accessors
            void setData(const size_t& length);
            void setData(const real_line& a_Data);
            void setData(const complex_line& a_ComplexData);

            void setOversamplingFactor(const int a_Factor);       // < relative to a previous setting !!
            void setExponentSign(const int a_ExponentSign); // < -1 for "forward dft" / 1 for "backward dft"

            int  getExponentSign()       const;
            int  getOversamplingFactor() const;

            // -- further accessors 
            real_line getMagnitudeData()       const;
            real_line getRealPartOfData()      const;
            real_line getImaginaryPartOfData() const;
            real_line getPhaseOfData()         const;

            complex_line getComplexData()         const;

            // -- implemenation of dft/idft
            static complex_line transform(const complex_line& a_Data, const int a_ExponentSign= -1);
            static void transform(const real_line& a_Data, complex_line& a_Spectrum, const int a_ExponentSign= -1);
            static void transform(const complex_line& a_Data, complex_line& a_Spectrum, const int a_ExponentSign = -1);

        protected:

            // -- protected member
            int                                    m_ExponentSign;
            int                                    m_OversamplingFactor;
            std::vector< std::complex<T> >         m_ComplexData;

            // -- helpers
            void basic_initialization();

            virtual void InsertZeros() = 0;
        };

        // ==================================================================================
        /** \class dft
         * \brief Class providing discrete fourier transform.
         *
         * \author Juergen Holzner
         *
         * Class providing discrete fourier transform.
         *
         */

        template<class T> class idft;

        template<class T> class ZFS_API dft : public dft_base<T>
        {
            template <class S> friend class idft;

        public:

            using real_line = std::vector<T>;
            using complex_line = std::vector< std::complex < T > >;

            dft();

            dft(const real_line& a_Data);
            dft(const real_line& a_Data, const int a_OversamplingFactor);

            dft(const complex_line& a_ComplexData);
            dft(const complex_line& a_ComplexData, const int a_OversamplingFactor);

            dft(const idft<T>& a_IDFT);
            dft(const idft<T>& a_IDFT, const int a_OversamplingFactor);

            ~dft();

            // -- overridden
            void setData(const real_line& a_Data);
            void setData(const complex_line& a_ComplexData);
            void setOversamplingFactor(const int a_Factor);

            // -- accessors
            const complex_line& getComplexSpectrum() const;

            real_line getMagnitudeSpectrum()                const;

            real_line getMagnitudeHalfSpectrum()            const;
            real_line getRealPartOfSpectrum()               const;
            real_line getImaginaryPartOfSpectrum()          const;
            real_line getPhaseOfSpectrum()                  const;

            T getFrequencyOfMaximumOfHalfSpectrum() const;

        protected:

            complex_line m_ComplexSpectrum;

            void InsertZeros();
        };
        // ==================================================================================
        /** \class idft
         * \brief Class providing inverse discrete fourier transform.
         *
         * \author Juergen Holzner
         *
         * Class providing inverse discrete fourier transform.
         *
         */
        template<class T> class ZFS_API idft : public dft<T>
        {
            template <class S> friend class dft;

        public:

            using real_line = std::vector<T>;
            using complex_line = std::vector< std::complex < T > >;

            idft();

            idft(const real_line& a_SpectralData);
            idft(const complex_line& a_SpectralData);
            idft(const complex_line& a_SpectralData, int a_OversamplingFactor);

            idft(const dft<T>& a_DFT);
            idft(const dft<T>& a_DFT, const int a_OversamplingFactor);

            ~idft();

            void setComplexSpectrum(const real_line& a_SpectralData);
            void setComplexSpectrum(const complex_line& a_SpectralData);

        private:
            void basic_initialization();
            void InsertZeros();
        };
        // ==================================================================================
    } // end namespace math
} // end namespace zfs 
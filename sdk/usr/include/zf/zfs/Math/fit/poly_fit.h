#pragma once

#include <vector>
#include <complex>

#include <zfs/zfsapi.h>
#include <zfs/Math/math/range.h>
#include <zfs/Math/math/polynomial.h>
#include "svd.h"

#if (defined(_MSC_VER) && _MSC_VER >= 1600) || defined(__GNUG__)
    #define _DEPENDEND_NAME_INDICATOR  typename
#else
    #define _DEPENDEND_NAME_INDICATOR
#endif

// classes for a polynomial implementing several
// functions of the polynomial and
// for the fit (SVD -SVDPolyFit- and Matrix inversion -PolyFit-)
// of a polynomial to
// a set of data specified in vectors x and y.
// matrix operations are conducted on the basis of vectors

namespace zfs { 
    namespace math {
        //########################################################################################################

        template< class T> class Range;

        //########################################################################################################

        //
        //

        /** \class PolyFit
         * \brief  Fit of a polynomial to a set of data given in vector x and y by matrix inversion using the Gauss-Jordan-algorithm.
         *
         * \author Juergen Holzner
         *
         * Fit of a polynomial to a set of data given in vector x and y by matrix inversion using the Gauss-Jordan-algorithm.
         *
         */

        template<class T> class ZFS_API PolyFit
        {
        private:

            // -- private members
            bool m_bFitOK, m_bExternal;

            int m_ndata;
            int m_ma;

            long double m_chisq;

            std::vector<T>           m_OnesSig;

            std::vector<long double> m_CoefVar;
            std::vector<long double> m_covar;

            Polynomial               m_Poly;

            // -- pointers to data
            const T* m_pX;
            const T* m_pY;
            const T* m_pSig;

            // -- private member functions
            void Free();
            void Alloc(T* a_pX, T* a_pY, T* a_pSig);
            void basic_initialization();

        public:
            // -- constructors
            PolyFit();
            virtual ~PolyFit();

            PolyFit(const T               x[], const T               y[], const int n_data, const int n_polyorder);
            PolyFit(const std::vector<T>& x, const std::vector<T>& y, const int n_polyorder);


            PolyFit(const T               x[], const T               y[], const T               sig[], const int n_data, const int n_polyorder);
            PolyFit(const std::vector<T>& x, const std::vector<T>& y, const std::vector<T>& sig, const int n_polyorder);

            PolyFit(const std::vector<Tuple2<T> >& data, const int n_polyorder);
            PolyFit(const std::vector<Tuple3<T> >& data, const int n_polyorder);

            PolyFit(const _DEPENDEND_NAME_INDICATOR std::vector<T>::const_iterator data_begin, const int n_start, const int n_end, const int n_polyorder);
            PolyFit(const _DEPENDEND_NAME_INDICATOR std::vector<T>::const_iterator data_begin, const Range<int>& FitRange, const int n_polyorder);

            // -- initializer
            void initialize(const T               x[], const T               y[], const int n_data, const int n_polyorder);
            void initialize(const std::vector<T>& x, const std::vector<T>& y, const int n_polyorder);

            void initialize(const T               x[], const T               y[], const T               sig[], const int n_data, const int n_polyorder);
            void initialize(const std::vector<T>& x, const std::vector<T>& y, const std::vector<T>& sig, const int n_polyorder);

            void initialize(const std::vector<Tuple2<T> >& data, const int n_polyorder); // vector with tuples -> (x_i,y_i)
            void initialize(const std::vector<Tuple3<T> >& data, const int n_polyorder); // vector with tuples -> (x_i,y_i,sigma_i)

            void initialize(const _DEPENDEND_NAME_INDICATOR std::vector<T>::const_iterator data_begin, const int n_start, const int n_end, const int n_polyorder);

            // -- fit function
            void fit();

            // -- accessors
            Polynomial               getPolyFitResult() const;
            long double              get_chisq() const;
            bool                     get_bFitOK() const;
            std::vector<long double> get_covar() const;
            std::vector<T>           getCoefSig() const;
            std::vector<T>           getCoefVar() const;
            Polynomial               getPolyValStdDevFunc() const;
            Polynomial               getPolyValVarFunc() const;
        };

        // #############################################################################################################

        /** \class SVDPolyFit
         * \brief  Fit of a polynomial to a set of data given in vector x and y by matrix inversion using singular value decomposition (SVD).
         *
         * \author Juergen Holzner
         *
         * Fit of a polynomial to a set of data given in vector x and y by matrix inversion using singular value decomposition (SVD).
         *
         */

        class ZFS_API SVDPolyFit
        {
        private:

            // -- class constant
            static const long double TOL;

            // -- private members
            bool                     m_bFitOK;
            bool                     m_bExternal;

            int                      m_ndata;
            int                      m_ma;

            const double* m_pX;
            const double* m_pY;
            const double* m_pSig;

            long double              m_chisq;

            SVD<long double>         m_SVD;
            Polynomial               m_Poly;

            std::vector<double>      m_OnesSig;

            std::vector<long double> m_A;

            // -- private functions
            void basic_initialization();

            void constructDesignMatrix();
            void calculate_polynomial_coefficients();
            void calculate_chisq();

            void svdksb(const long double* u, const long double w[], const long double* v, int m, int n, long double b[], long double x[]);
        public:
            SVDPolyFit();
            // initialization of members and polynomial fit, each data value is wheighted equally
            SVDPolyFit(const std::vector<double>& x, const std::vector<double>& y, int npolyorder);
            // initialization of members and polynomial fit, each data value is wheighted according to given std.dev. in sig
            SVDPolyFit(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& sig, int n_polyorder);
            ~SVDPolyFit();

            // initialization of members
            void initialize(const std::vector<double>& x, const std::vector<double>& y, int npolyorder);
            // initialization of members
            void initialize(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& sig, int n_polyorder);
            // fit is conducted
            void fit();

            // fit result: polynomial of specified order
            Polynomial  getPolyFitResult() const;
            // chi-square value of fit; total quadratic error of fit
            long double get_chisq() const;

            // is fit OK?
            bool get_bFitOK() const;
        };
    } // end namespace math
} // end namespace zfs

#undef _DEPENDEND_NAME_INDICATOR

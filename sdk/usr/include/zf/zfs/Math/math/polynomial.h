#pragma once

#include <vector>
#include <complex>

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math
    {
        // ##########################################################################################################################################
        template <class T> class Range;
        // ##########################################################################################################################################
        /** \class Polynomial
         * \brief  Class for representing and providing functionality for polynomials (in a single variable) of arbitrary order.
         *
         * \author Juergen Holzner
         *
         * Class for representing and providing functionality for polynomials (in a single variable) of arbitrary order.
         * Partially based on numerical recipes implementation.
         *
         */

        class ZFS_API Polynomial
        {
        public:

            Polynomial();
            Polynomial(int order);
            Polynomial(const std::vector<long double>& coefficients);
            Polynomial(const Polynomial& Poly);
            ~Polynomial();

            Polynomial& operator=(const Polynomial& rhs);

            void                                   setOrder(int order);                                            // sets the order of the poly
            void                                   setCoefficients(const std::vector<long double>& coefficients);  // sets the coefficients and the order of the poly

            int                                    getOrder() const;             // returns the order of the polynomial.
            std::vector<long double>               getCoefficients() const;      // returns the coefficients of the polynomial.
            std::vector<double>                    getCoefficientsDBL() const;   // returns the coefficients of the polynomial.
            long double* getCoefficientsPtr() const;   // returns a pointer to the coefficients.

            Polynomial                             Derivative(int n) const;           // n-th derivative of polynomial
            std::vector<long double>               RealRoots(bool& b_success) const;  // returns the real roots of all roots (complex and real)
            std::vector<std::complex<long double>> Roots(bool& b_success) const;      // returns all roots (complex and real)

            template<class T> T               operator()(const T& position) const;
            template<class T> T               polyval(const T& position) const;                                               // calculates the polynomial value for a ordinate x
            template<class T> std::vector<T>  polyval(int n_start, int n_end) const;                                          // calculates the polynomial values for a range starting at n_start and ending at n_end (inclusive) with step 1
            template<class T> std::vector<T>  polyval(const Range<int>& eval_range) const;                                    // calculates the polynomial values for a given evaluation range with step 1
            template<class T> std::vector<T>  polyval(const std::vector<T>& x) const;                                         // calculates the polynomial values for a vector of given ordinates
            template<class T> void            polyval(const T* const aPolySupport, T* const aResult, const int Length) const; // calculates the polynomial values for a vector of given ordinates

            template<class T> void            polyval4AME(const T* const aPolySupport, T* const aResult, const int Length) const; // calculates the polynomial values for a vector of given ordinates, where the absolute maximum error is calculated (absoluter Größtfehler; propagation of uncertainty), input "std. dev. polynomial"
            template<class T> void            polyval4GEP(const T* const aPolySupport, T* const aResult, const int Length) const; // calculates the polynomial values for a vector of given ordinates, for gaussian error propagation (returns std.dev. values), input "variance polynomial"

            void                                   poly_base(const double& x, std::vector<long double>& values) const;  // returns the powers of x up to the polynomial degree

            int                                    n_poly_extrema_in_region(int n_start, int n_end); // finds the number of extrema in a given region

        private:

            int                                      m_Order;                                // stores the order of the polynomial
            std::vector<long double>                 m_Coefficients;                         // stores the coefficients of the polynomial
            std::vector<long double>::const_iterator m_Coefficients_it_begin;                // stores the start pos of the iterator for the coefficients of the polynomial (for fast eval)
            std::vector<long double>::const_iterator m_Coefficients_it_end;                  // stores the end pos of the iterator for the coefficients of the polynomial (for fast eval)
            long double* m_pCoefficients;                        // stores a pointer to the coefficients of the polynomial

            //--------------------------------------------------------
            // routines for root finding based on numerical recipes:
            //--------------------------------------------------------

            // numerical recipes routine zrhqr(.) see
            // http://www.haoli.org/nr/bookcpdf/c9-5.pdf
            // - finds the roots of a polynomial

            // Find all the roots of a polynomial with real coecients,
            // given the degree m and the coecients a[0..m].
            // The method is to construct an upper Hessenberg matrix whose
            // eigenvalues are the desired roots, and then use the routines balanc and hqr. The real and
            // imaginary parts of the roots are returned in rtr[1..m] and rti[1..m], respectively.

            bool zrhqr(long double* a, int m, long double* rtr, long double* rti) const;

            // numerical recipes routine balanc(.) see
            // http://www.haoli.org/nr/bookcpdf/c11-5.pdf
            // - used for root finding in zrhqr(.).

            // Given a matrix a[1..n][1..n], this routine replaces it by a balanced matrix with identical
            // eigenvalues. A symmetric matrix is already balanced and is unaffected by this procedure. The
            // parameter RADIX should be the machine's floating-point radix (usually 2, see ieee floating point format).
            void balanc(long double* a, int n) const;

            // numerical recipes routine hqr(.) see
            // http://www.haoli.org/nr/bookcpdf/c11-6.pdf
            // - used for root finding in zrhqr(.).

            // Finds all eigenvalues of an upper Hessenberg matrix a[1..n][1..n]. On input a can be
            // exactly as output from elmhes §11.5; on output it is destroyed. The real and imaginary parts
            // of the eigenvalues are returned in wr[1..n] and wi[1..n], respectively.

            bool hqr(long double* a, int n, long double* wr, long double* wi) const;

            // numerical recipes routine laguer(.) see
            // http://www.haoli.org/nr/bookcpdf/c9-5.pdf
            // - used for root finding in zroots(.).

            // Given the degree m and the m+1 complex coecients a[0..m] of a polynomial
            // and given a complex value x, this routine improves x by Laguerre's method until it converges,
            // within the achievable roundoff limit, to a root of the given polynomial. The number of iterations
            // taken is returned as its.

            bool laguer(const std::vector<std::complex<long double> >& a, int m, std::complex<long double>& x, int& its) const;

            // numerical recipes routine zroots(.) see
            // http://www.haoli.org/nr/bookcpdf/c9-5.pdf
            // - finds roots of a polynomial.

            // Given the degree m and the m+1 complex coecients a[0..m] of a polynomial
            // this routine successively calls laguer and finds all m complex roots in roots[1..m]. The
            // boolean variable polish should be input as true if polishing (also by Laguerre's method)
            // is desired, false if the roots will be subsequently polished by other means.

            void zroots(const std::vector<std::complex<long double> >& a, int m, std::vector<std::complex<long double> >& roots, bool polish) const;

            // Root finding using zroots(.)
            std::vector<std::complex<long double> > FindRootsLaguerrs(const std::vector<long double>& coefficients) const;

        };
        // ##########################################################################################################################################
        // helping function
        // ##########################################################################################################################################
        template <class T> void ZFS_API basepoly(const double& x, std::vector<T>& values, int order); // only floating point types
        // ##########################################################################################################################################
        // Test functions
        // ##########################################################################################################################################
        void                                   ZFS_API test_RootFinding();          // tests the root finding using sample polynomials
        // ##########################################################################################################################################
        std::vector<std::vector<long double> > ZFS_API test_RootFinding_Output();   // tests the root finding using sample polynomials
        // ##########################################################################################################################################
    } // end namespace math
} // end namespace zfs
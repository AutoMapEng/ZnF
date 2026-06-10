#pragma once

#include <vector>

#include <zfs/zfsapi.h>
#include <zfs/Math/math/tuple.h>
#include <zfs/Math/math/polynomial.h>

// author: juergen holzner

namespace zfs { 
    namespace math {

        /** \class RobustLinearFit
         * \brief  Class for a robust fit of a linear function to data having noisy behaviour (absolute norm, norm 1).
         *
         * \author Juergen Holzner
         *
         * Class for a robust fit
         * of a linear function
         * y= a+bx to data
         * by minimizing absolute deviation
         * (in contrast to mean square deviation)
         * basing on numerical recipes
         * see chapter 15.7 at
         * http://www.haoli.org/nr/bookcpdf.html
         *
         */

        class ZFS_API RobustLinearFit
        {
        private:

            // variables for data exchange between
            // medfit() and rofunc()
            int ndatat;
            double* xt, * yt;
            long double aa, abdevt;

            // member variables
            int m_ndata;
            std::vector<double> m_x, m_y;
            // coefficients of linear function
            long double m_a, m_b;
            Polynomial  m_RobLinPoly;
            // absolute deviation measured
            // for the specific fit
            long double m_abdev;

            // private functions
            void medfit(double x[], double y[], int ndata, long double& a, long double& b, long double& abdev);
            long double rofunc(long double b);
            long double select(unsigned long k, unsigned long n, long double arr[]);

        public:
            RobustLinearFit();
            RobustLinearFit(const std::vector<double>& x, const std::vector<double>& y, bool b_addressing_0);

            RobustLinearFit(const std::vector<Tuple2<double> >& data, bool b_addressing_0);

            ~RobustLinearFit();

            void initialize(const std::vector<double>& x, const std::vector<double>& y, bool b_addressing_0);

            void initialize(const std::vector<Tuple2<double> >& data, bool b_addressing_0);

            // get polynomial coefficients
            std::vector<long double> getCoef() const;
            // get polynomial
            Polynomial               getPolynomial() const;
            // get total absolute deviation of input values from fitted polynomial
            long double              getAbsDeviation() const;
            // get absolute deviation of input values from fitted polynomial
            // where the individual deviations are smaller than border -> outliers
            // are not considered in the abs. deviation calculation.
            long double              getAbsDeviationExclOutlier(double border) const;
        };

        template<class T>
        inline T SIGN(const T& a, const T& b)
        {
            return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
        }
    } // end namespace math
} // end namespace zfs
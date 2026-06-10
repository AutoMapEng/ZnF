#pragma once 

#include <cstddef>
#include <vector>
#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // -------------------------------------
        // -- lfit adapted to polyfit (y= a_0+ a_1*x + a_2* x^2 + ... + a_ma* x^(ma-1)) [to be unified later]
        // -- inverse calculation is based on gauss-jordan algorithm
        template<class T> void ZFS_API lfit4poly(const T x[], const T y[], const T sig[], const int ndat, long double a[], const int ma, long double* covar, long double& chisq);
        // -------------------------------------
        // -- general least squares fit ( base function must be supplied by application), multidim. fits can be realized by passing indices + use of global vars/object
        // -- inverse calculation is based on gauss-jordan algorithm
        typedef void(*basefunction_t)(const double&, std::vector<long double>&, const unsigned int);
        void ZFS_API lfit(const double x[], const double y[], const double sig[], const int ndat, long double a[], const int ma, basefunction_t const basefunction, long double* const covar, long double& chisq);
        // -------------------------------------
    } // end namespace math
} // end namespace zfs
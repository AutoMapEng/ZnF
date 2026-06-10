#pragma once 

#include <cstddef>
#include <vector>
#include <zfs/zfsapi.h>

namespace zfs { 
    namespace math {
        namespace numerical_recipes {
            // -------------------------------------
            // -- lfit adapted to polyfit (y= a_0+ a_1*x + a_2* x^2 + ... + a_ma* x^(ma-1)) [to be unified later]
            bool ZFS_API lfit4poly(const double x[], const double y[], const double sig[], int ndat, double a[], int ma, double covar[], double& chisq);
            // -------------------------------------
            // -- general least squares fit ( base function must be supplied by application), multidim. fits can be realized by passing indices + use of global vars/object
            typedef void(*basefunction_t)(const double& x, std::vector<double>& base_function_values, int order);
            bool ZFS_API lfit(const double x[], const double y[], const double sig[], int ndat, double a[], int ma, basefunction_t basefunction, double covar[], double& chisq);
            // -------------------------------------
            bool  ZFS_API gaussj(double a[], int n, double b[], int m);
            // -------------------------------------
            void ZFS_API covsrt(double covar[], int ma, int mfit);
            // -------------------------------------
            void ZFS_API covsrt(double covar[], int ma, int ia[], int mfit);
            // -------------------------------------
            void ZFS_API jacobi(double a[], int n, double d[], double v[], int& nrot, bool& bConverged);
            void ZFS_API eigsrt(double d[], double v[], int n);
            // -------------------------------------
        } // namespace numerical_recipes
    } // end namespace math
} // end namespace zfs
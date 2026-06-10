#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <array>

#include "surfacefit.h"

namespace zfs {
    namespace math {
        // ==============================================================================================
        // Class surface_fit_svd for a polynomial in two parameters (e.g. a N-th order surface for the x-component in 3D-space) of the form:
        // [Latex notation:] $f_x(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // of order N to given sample data using SVD-decomposition 
        // (which has additional analysis advantages - indicating whether polynomial terms can be omitted - when compared to gauss-jordan solution).
        // Necessary matrix inversion of the covariance-Matrix is done using SVD - Decomposition.
        // Repeated calls, e.g. for x, y and z don't need repeated (re-)setting (re-calculation of the SVD - dcmp of the model matrix.)
        // An bool-vector indicating outliers can be provided after the input parameter vectors are provided.
        // Setting of given SVD-decomposition matrices is possible.

        // TODO: implement if required

        class ZFS_API surface_fit_svd : public surface_fit
        {
        public:

            surface_fit_svd();
            surface_fit_svd(const int& order, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {});
            ~surface_fit_svd() {};

            void set_order(const int& order);

            // ...

        private:

            // ...
        };
    } // end namespace math
} // end namespace zfs
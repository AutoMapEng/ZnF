#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <array>

#include "surfacefit.h"

namespace zfs {
    namespace math {
        // ==============================================================================================
        // Class for fitting a polynomial (e.g. a N-th order surface in 3D-space) of the form:
        // $f(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // of order N to given sample data using gauss-jordan algorithm.
        // Necessary matix inversion is done using Gauss-Jordan algorithm.
        // After each call of the fit() member function, calls to the poly()- and getChisq()- member functions give the result polynomial
        // and the squared sum of residuals from the surface.
        // Conditions:  x.size() == y.size() == z.size(), 
        // enough number of data samples should be available to fit the coefficients, e.g. for a 1st order polynomial at least 3 samples
        // are required, for a 2nd order polynomial, at least 6 samples are required. The minimum number of necessary data can be 
        // enquired when a surface_poly of order n is generated and the function n_coefs() is called.

        class ZFS_API surface_fit_gj : public surface_fit
        {

        public:

            // constructors
            surface_fit_gj();
            surface_fit_gj(const int& order, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {});
            surface_fit_gj(const int& order, const vec& u, const vec& v, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {}, const vec_b& subsample = vec_b());
            ~surface_fit_gj() {};

            // init function of parent class is used
            using surface_fit::reset;

            // external initialization/set order
            void set_order(const int& order) final;

            // initialization (does the same as in the constructor, base initialization)
            // refit(): function refit is not hidden and is inherited as is.

            // fit functions
            // overload rather than hide the fit functions in surface_fit class -> fit()-routines of parent class surface_fit are available
            using surface_fit::fit;
            // refit(): function refit is not hidden and is inherited as is.

            // fit functions when multiple fits on the same parameter base are used, e.g. when calculating fits for x,y and z simultaneously
            bool fit(const matrix<double>& ext_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            bool fit(const matrix<double>& ext_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const double& sigma, const vec_b& subsample);

            // getter for use in conjunction with above fit functions in multi-fit
            const matrix<double>& get_data_covariance_matrix() const; // < enquire dcm for other fits

        private:

            // private member variables
            // < variables for inplace application of the Gauss-Jordan algorithm (avoid repeated allocation of matrix, to be able to efficiently handle outliers and possibly to be able to set covariance matrix when it is known)
            matrix<double> m_beta_inplace;

            // member variables and functions when multiple fits on the same parameter base are used, e.g. when calculating fits for x,y and z simultaneously
            // < variables for using an external data covariance matrix
            // < in order to save numerical operations when multiple surface fits are used
            const matrix<double>* m_p_ext_dcm; // < pointer to external data covariance matrix

            // -- reset functions
            // reset fit( overload if necessary)
            void reset(const std::set<reset_t>& reset_option);
            // reset LU specific external data when set for multi-fit use
            void reset_ext_data();
            // zero LU (implementation) specific numeric data
            void zero_numeric();

            using surface_fit::set_data; // import function from parent class (otherwise hidden, since overloaded here)

            // member functions when multiple fits on the same parameter base are used, e.g. when calculating fits for x,y and z simultaneously
            // < variables for using an external data covariance matrix
            // < in order to save numerical operations when multiple surface fits are used

            void set_data(const matrix<double>& ext_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            void set_data(const matrix<double>& ext_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const double& sigma = 1.0, const vec_b& subsample = vec_b());

            // method used for equation solving (used in solve_normal_equation_system(.)): Gauss-Jordan elimination
            void gaussj(matrix<double>& a, matrix<double>& b);

            // solution to the normal equations using Gauss-Jordan algorithm (gaussj()-routine)
            void solve_normal_equation_system();

        };
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs
#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <array>

#include "surfacefit.h"

namespace zfs {
    namespace math {
        // ==============================================================================================
        // Class surface_fit_lu for a polynomial in two parameters (e.g. a N-th order surface for the x-component in 3D-space) of the form:
        // [Latex notation:] $f_x(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // of order N to given sample data using LU-decomposition 
        // (which has timing advantages in terms of number of numeric operations when compared to gauss-jordan solution).
        // Necessary matrix inversion of the covariance-Matrix is done using LU - Decomposition.
        // In favourable cases, repeated calls, e.g. for x, y and z don't need repeated (re-)setting (re-calculation of the covariance matrix.)
        // An bool-vector indicating outliers can be provided after the input parameter vectors are provided.
        // Setting of given LU-decomposed covariance matrix is possible.

        class ZFS_API surface_fit_lu : public surface_fit
        {

        public:

            // constructors
            surface_fit_lu();
            surface_fit_lu(const int& order, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {});
            surface_fit_lu(const int& order, const vec& u, const vec& v, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {}, const vec_b& subsample = vec_b());
            ~surface_fit_lu() {};

            // init function of parent class is used
            using surface_fit::reset;

            // special initialization/setting functions (fit specific / overloaded)
            void set_order(const int& order) final;

            // fit functions
            // overload rather than hide the fit functions in surface_fit class -> fit()-routines of parent class surface_fit are available
            using surface_fit::fit;
            // refit(): function refit is not hidden and is inherited as is.

            // < functions to call when a solution base matrix ( e.g. covariance matrix or LU decomposition) is available, 
            // < e.g. see xyz_surface_fit, which uses several fits, e.g. for x,y and z
            bool fit(const matrix<double>& ext_LU_dcmp_of_dcm, const vec_i& ext_ppidx_for_LU_dcmp_of_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            bool fit(const matrix<double>& ext_LU_dcmp_of_dcm, const vec_i& ext_ppidx_for_LU_dcmp_of_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const double& sigma, const vec_b& subsample);

            // getter for use in conjunction with above fit functions in multi-fit
            const matrix<double>& LUcovar() const;
            const vec_i& ppidx() const;

            // calculation of the inverse of the data covariance matrix if needed
            void calculate_inverse_dcm();

        private:

            // -- member variables
            vec     m_beta_inplace;    // < storage for inplace backsubstitution when solving the normal equation system in routine solve_normal_equation_system() (increases efficiency by avoiding repeated vector allocation)
            vec_i   m_ppidx;           // < vector keeping pivotation indices used in LU decomposition (see routine ludcmp())
            matrix<double>  m_LUcovar; // < LU decomposition of data covariance matrix stored in a single matrix (see routine ludcmp())

            // -- reset functions
            // reset fit( overload if necessary)
            void reset(const std::set<reset_t>& reset_options);
            // reset LU specific external data when set for multi-fit use
            void reset_ext_data();
            // zero LU (implementation) specific numeric data
            void zero_numeric();

            // -- LU decomposition and solution specific functions
            // LU decomposition
            void conduct_lu_decomposition();
            // inplace LU decomposition from NR
            void ludcmp(matrix<double>& a, vec_i& pp_indx);
            // inplace backsubstitution with result from ludcmp from NR
            void lubksb(const matrix<double>& a, const vec_i& pp_indx, vec& b);
            // solution to the normal equations using LU decomposition of the covariance matrix
            // and backsubstitution routine lubksb from NR
            void solve_normal_equation_system();

            // members and functions when multiple fits on the same parameter base are used, e.g. when calculating fits for x,y and z simultaneously
            // < variables for using an external LU - Decomposition (short: LU-dcmp) of the data covariance matrix (short: dcm) 
            // < in order to save numerical operations when multiple surface fits are used
            const matrix<double>* m_p_ext_LU_dcmp_of_dcm;
            const vec_i* m_p_ext_ppidx_for_LU_dcmp_of_dcm; // pointer to external pivoting index vector for equation solving

            // overload rather than hide the set_data functions in surface_fit class -> set_data()-routines of parent class surface_fit are available
            using surface_fit::set_data;

            // -- helping functions for setting data (LU specific)
            void set_data(const matrix<double>& ext_LU_dcmp_of_dcm, const vec_i& ext_ppidx_for_LU_dcmp_of_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            void set_data(const matrix<double>& ext_LU_dcmp_of_dcm, const vec_i& ext_ppidx_for_LU_dcmp_of_dcm, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const double& sigma = 1.0, const vec_b& subsample = vec_b());
            bool check_input_LU_dcmp(const matrix<double>& ext_LU_dcmp_of_dcm, const vec_i& ext_ppidx_for_LU_dcmp_of_dcm);

            // routine for setting/calculating the errors of the coefficients of the surface polynomial
            void set_coefficient_fit_errors();
        };
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs
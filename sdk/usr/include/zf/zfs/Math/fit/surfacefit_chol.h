#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <array>

#include "surfacefit.h"

namespace zfs {
    namespace math {
        // ==============================================================================================
        // Class surface_fit_chol for a polynomial in two parameters (e.g. a N-th order surface for the x-component in 3D-space) of the form:
        // [Latex notation:] $f_x(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // of order N to given sample data using Cholesky-decomposition 
        // (which has timing advantages in terms of number of numeric operations when compared to LU - decomposition solution).
        // Necessary matrix inversion of the data covariance-Matrix is done using Cholesky - decomposition.
        // In favourable cases, repeated calls, e.g. for x, y and z don't need repeated (re-)setting (re-calculation of the covariance matrix.)
        // An bool-vector indicating outliers can be provided after the input parameter vectors are provided.
        // Setting of given Cholesky - decomposition is possible.

        class ZFS_API surface_fit_chol : public surface_fit
        {

        public:

            // constructors
            surface_fit_chol();
            surface_fit_chol(const int& order, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {});
            surface_fit_chol(const int& order, const vec& u, const vec& v, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {}, const vec_b& subsample = vec_b());
            ~surface_fit_chol() {};

            // init function of parent class is used
            using surface_fit::reset;

            // special initialization/setting functions (fit specific / overloaded)
            void set_order(const int& order) final;

            // fit functions
            // overload rather than hide the fit functions in surface_fit class -> fit()-routines of parent class surface_fit are available
            using surface_fit::fit;
            // refit(): function refit is not hidden and is inherited as is.

            // < functions to call when a solution base matrix ( e.g. covariance matrix or Cholesky - decomposition) is available,
            // < e.g. see xyz_surface_fit, which uses several fits, e.g. for x,y and z
            bool fit(const matrix<double>& ext_Chol_dcmp_of_dcm, const vec& ext_Chol_dcmp_dcm_diag, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            bool fit(const matrix<double>& ext_Chol_dcmp_of_dcm, const vec& ext_Chol_dcmp_dcm_diag, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const double& sigma = 1.0, const vec_b& subsample = vec_b());

            // getter for use in conjunction with above fit functions in multi-fit
            const matrix<double>& CholDecCovar() const;
            const vec& CholDecCovarDiag() const;

            // calculation of the inverse of the data covariance matrix if needed
            void calculate_inverse_dcm();

        private:

            // -- member variables
            vec     m_solution_buffer;          // < vector keeping the solution parameters (buffer to avoid repeated allocation when fit object is reused, e.g. in a loop)
            matrix<double>  m_unity_matrix;     // < (fixed) unity matrix for inversion operations
            vec     m_Chol_dcmp_dcm_diag;       // < vector keeping the diagonal of the Cholesky decomposition result matrix L (see NR)
            matrix<double>  m_Chol_dcmp_of_dcm; // < matrix keeping the Cholescy decomposition result (see NR)

            // -- reset functions
            // reset fit( overload if necessary)
            void reset(const std::set<reset_t>& reset_option);
            // reset Cholesky-decomposition specific external data when set for multi-fit use
            void reset_ext_data();
            // zero Cholesky-decomposition (implementation) specific numeric data
            void zero_numeric();

            // -- Cholesky decomposition and solution specific functions
            // Cholesky decomposition
            void conduct_chol_decomposition();
            // "inplace" Cholesky decomposition from NR
            void choldc(matrix<double>& a, vec& p);
            // solution to equation A*x= b using cholesky dcmp output from routine choldc (NR)
            void cholsl(const matrix<double>& a, const vec& p, const vec& b, vec& x);
            // solution to the normal equations using cholesky decomposition of the covariance matrix
            // and solution routine cholsl from NR
            void solve_normal_equation_system();

            // members and functions when multiple fits on the same parameter base are used, e.g. when calculating fits for x,y and z simultaneously
            // < variables for using an external Cholesky - decomposition (short: Chol-dcmp) of the data covariance matrix (short: dcm) 
            // < in order to save numerical operations when multiple surface fits are used
            const matrix<double>* m_p_ext_CholDecCovar;
            const vec* m_p_ext_CholDecCovarDiag;

            // overload rather than hide the set_data functions in surface_fit class -> set_data()-routines of parent class surface_fit are available
            using surface_fit::set_data;

            // -- helping functions for setting data (Chol-dcmp specific)
            void set_data(const matrix<double>& ext_Chol_dcmp_of_dcm, const vec& ext_Chol_dcmp_dcm_diag, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            void set_data(const matrix<double>& ext_Chol_dcmp_of_dcm, const vec& ext_Chol_dcmp_dcm_diag, const base_term_storage& ext_base_term_storage, const vec& u, const vec& v, const vec& data, const double& sigma, const vec_b& subsample = vec_b());
            bool check_input_chol_dcmp(const matrix<double>& ext_Chol_dcmp_of_dcm, const vec& ext_Chol_dcmp_dcm_diag);

            // routine for setting/calculating the errors of the coefficients of the surface polynomial
            void set_coefficient_fit_errors();
        };
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs
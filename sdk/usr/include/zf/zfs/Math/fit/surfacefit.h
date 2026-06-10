#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>
#include <vector>
#include <utility>
#include <array>
#include <iostream>

#include <zfs/Math/auxilliary/types.h>
#include <zfs/Math/fit/surface_for_error_analysis.h>
#include <zfs/Math/fit/surfacefit_base_term_storage.h>
#include <zfs/Math/math/matrix.h>
#include <zfs/Math/object/state.h>
#include <zfs/Math/object/multioptions.h>

namespace zfs {
    namespace math {

        using vec_b = std::vector<bool>;
        using vec_i = std::vector<int>;
        using tuple_i = std::pair<int, int>;

        // ==============================================================================================
        // Abstract class for fitting a polynomial (e.g. a N-th order surface in 3D-space) of the form:
        // $f(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // of order N to given sample data.
        // Necessary matix inversion is done using Gauss-Jordan algorithm.
        // After each call of the fit() member function, calls to the poly()- and getChisq()- member functions give the result polynomial
        // and the squared sum of residuals from the surface.
        // Conditions:  x.size() == y.size() == z.size(),
        // enough number of data samples should be available to fit the coefficients, e.g. for a 1st order polynomial at least 3 samples
        // are required, for a 2nd order polynomial, at least 6 samples are required. The minimum number of necessary data can be
        // enquired when a surface_poly of order n is generated and the function n_coefs() is called.

        class ZFS_API surface_fit
        {
        public:
            // states that may occur in the surface fit objects
            enum class state_t : uint32_t {

                INITIAL = 0, // < initial state

                BASE_DATA_SET_OK, // < base data (u, v, possibly subsampling) were set OK (for mode CONST_UV_VARYING_DATA)
                DATA_SET_OK, // < data was set OK (u, v, possibly subsampling, data and possibly sigma) were set OK
                EXTERNAL_DATA_SET_OK, // < external data set OK (external solution base: external dcm, external base term storage)
                BASE_TERM_SETUP_OK, // < set up of baseterms OK (for mode CONST_UV_VARYING_DATA)
                EQUATION_SYSTEM_LEFT_SIDE_SETUP_OK, // < set up of left side of equation system OK
                EQUATION_SYSTEM_SETUP_OK, // < equation system setup OK

                DCM_DECOMPOSITION_OK, // < decomposition of data covariance matrix (dcm) OK (when a decomposition method is used)
                DCM_INVERSION_OK, // < inversion of data covariance matrix (dcm) OK

                EQUATION_SYSTEM_SOLVED, // < equation system solved successfully

                RESIDUALS_CALCULATED, // < residuals calculated
                TOTAL_SQUARE_ERROR_CALCULATED, // < total square error calculated
                SURFACE_COEFFICIENT_ERRORS_CALCULATED, // < surface coefficient errors calculated

                SURFACE_DERIVATIONS_SET, // < surface derivations are set

                OUTLIER_SET, // < flagging that outliers where detected
                OUTLIER_REMOVED // < outlier were removed from equation system
            };

            enum class error_state_t : uint32_t {
                DATA_INPUT_ERROR, // < data input OK ( value check )
                DATA_INPUT_SIZE_ERROR, // < data input OK (size check)
                INSUFFICIENT_DATA, // < data not suitable for fit (maybe too less or insufficiently distributed data for a 3D surface)
                OUTLIER_CORRECTION_FAILED, // < indicator for failure of outlier correction of normal equation system: the normal equation system was not setup before the call of the routine refit(.)
                OUTLIER_FIT_FAILED, // < indicator for failure of outlier correction of normal equation system: probably too many data samples were removed, so that the normal equation system cannot be solved anymore (too less data samples available after outlier removal)
            };

            enum class type_t : uint32_t {
                UNSET, // < type is unset
                GAUSS_JORDAN, // < gauss - jordan elimination is used for inversion of the dcm/ solution of the normal equation system
                LU_DECOMPOSITION, // < LU - decompostion + backsubstitution is used for inversion of the dcm/ solution of the normal equation system
                CHOLESKY_DECOMPOSITION, // < Cholesky - decompostion + backsubstitution is used for inversion of the dcm/ solution of the normal equation system
                SVD_DECOMPOSITION // < SVD - decompostion is used for inversion of the dcm/ solution of the normal equation system
            };

            enum class operation_t : uint32_t {
                STANDARD_OPERATION, // standard operation of fit

                SUBSAMPLE_FIT, // subsample fit (data is subsampled, for the initial and the (final) refit)
                SUBSAMPLE_FIT_FULL_REFIT, // subsample fit (data is subsampled for the initial fit, for the refit full data vectors are used)
            };

            enum class option_t : uint32_t {
                DONT_CALCULATE_CHISQR, // < don't calculate total square sum error of fit
                DONT_CALCULATE_COEFFICIENT_ERRORS, // < don't calculate coefficient errors for the fit
                DONT_CALCULATE_RESIDUALS, // < don't calculate residuals to surface
                DONT_CALCULATE_ANY_ERRORS, // < don't calculate any of the above
                CENTER_UV_ABOUT_MEAN // < center u and v vectors about their mean
            };

            const std::set<option_t> all_fit_options_set{ { option_t::DONT_CALCULATE_CHISQR, option_t::DONT_CALCULATE_COEFFICIENT_ERRORS, option_t::DONT_CALCULATE_RESIDUALS, option_t::DONT_CALCULATE_ANY_ERRORS } };

            // constructors
            surface_fit();
            // const (1):
            surface_fit(const int& order, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {}); // operation_mode : VARYING_UV_VARYING_DATA: use function fit(u,v,data,...)
            // const (2):
            surface_fit(const int& order, const vec& u, const vec& v, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {}, const vec_b& subsample = vec_b()); // operation_mode : CONST_UV_VARYING_DATA: use function fit(data, ...)
            virtual ~surface_fit() {};

            // initialization (does the same as in the constructor, base initialization)
            void init(const int& order, const operation_t& operation_mode = operation_t::STANDARD_OPERATION, const std::set<option_t>& options = {});


            // fit with varying (u,v) at every fit: base terms are calculated for every function call of fit(.) (operation mode: VARYING_UV_VARYING_DATA)
            // fit (1):
            bool fit(const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            bool fit(const vec& u, const vec& v, const vec& data, const double& sigma, const vec_b& subsample = vec_b());

            // -- set data function corresponding to "const (2)"
            // operation mode : CONST_UV_VARYING_DATA
            void set_data(const vec& u, const vec& v, const vec_b& subsample); // only used within constructor for op-mode: CONST_UV_VARYING_DATA
                                                                               // fit with fixed (u,v) for every fit: base terms are calculated once (operation mode: CONST_UV_VARYING_DATA)
            // fit (2): (op-mode: CONST_UV_VARYING_DATA)
            bool fit(const vec& data, const vec& sigma = vec());

            // refit using outlier information
            bool refit(const vec_b& outlier);

            // refit using supplied constant sigma (estimation of sigma is done in a previous step)
            bool refit(const double& sigma);

            // adapt sigma to new sigma
            bool reset_coefficient_fit_errors(const double& sigma);

            // getter functions
            const size_t& ndata() const;
            size_t& ndata();
            const int& order() const;

            const vec& get_u() const;
            const vec& get_v() const;
            const vec& get_data() const;
            const vec& get_sigma() const;
            const vec_b& get_outl() const;
            const vec_b& get_sampl() const;

            const surface_for_error_analysis& get_poly() const;
            const double& get_chisq() const; // < fit total square error

            double get_var() const; // < fit variance
            double get_stdev() const; // < fit standard deviation

            const matrix<double>& get_dcm() const;
            const matrix<double>& get_idcm() const;

            // getter for getting access to the base term data storage of the object
            const base_term_storage& get_base_term_storage() const;

            // operator for evaluating fit
            double operator()(const double& u, const double& v) const;

            // special initialization/setting functions
            virtual void set_order(const int& order); // reimplementation may be necessary (in a GJ/LU/SVD - based implementation)
            void reset(); // < reset fit (order and option are kept)
            void zero_coefs(); // < zero coefficients of surface polynomials
            void reset_numeric(); // < zero all numeric data

            // functions for setting/getting derivative infos
            void set_derivations();
            const surface_for_error_analysis& get_poly_dx() const;
            const surface_for_error_analysis& get_poly_dy() const;
            double d_dx(const double& u, const double& v) const;
            double d_dy(const double& u, const double& v) const;

            bool check_current_state(const state_t& query_state)
            {
                return m_surface_fit_state == query_state;
            }

            bool check_current_state_no_error(const state_t& query_state)
            {
                return check_current_state(query_state) && m_surface_fit_state.no_error_occured();
            }

            bool find_state(const state_t& query_state)
            {
                return m_surface_fit_state.find_state(query_state);
            }

            bool find_state_no_error(const state_t& query_state)
            {
                return find_state(query_state) && m_surface_fit_state.no_error_occured();
            }

            bool error_occured()
            {
                return m_surface_fit_state.error_occured();
            }

            bool no_error_occured()
            {
                return m_surface_fit_state.no_error_occured();
            }
            // check operation/option function
            bool check_operation(const operation_t& operation) const; // < verify operation
            bool check_option(const option_t& option) const; // < verify option

            // check fit type
            type_t fit_type() const; // < returns type of fit

        protected:

            // operation mode of fit (internal state)
            enum class data_input_mode_t : uint32_t {
                UNSET,
                CONST_UV_VARYING_DATA, // (u,v) are provided at initialization  (: use fit(data,...))
                VARYING_UV_VARYING_DATA // (u,v) are provided at every call to the fit function (: use fit(u,v,data,(const vec& sigma)...))
            };

            // reset_t
            enum class reset_t : uint32_t {
                STATE, // < value for resetting state
                COEFS, // < value for resetting polynomial coefficients with 0, keeping polynomial order settings (.. keeping internal memory/vector sizes)
                NUMERIC, // < value for resetting internal numeric storage variables and/or elements thereof with 0, keeping matrix and vector sizes
                DATA_POINTERS, // < value for resetting data pointers to nullptr
                EXTERNAL_EQ_COMPS // < value for resetting preset data covariance, LU- or SVD-decompositions thereof with 0, keeping matrix and vector sizes
            };

            const std::set<reset_t> all_reset_options_set{ { reset_t::STATE , reset_t::COEFS , reset_t::NUMERIC, reset_t::DATA_POINTERS, reset_t::EXTERNAL_EQ_COMPS } };

            // member variables
            type_t                               m_fit_type; // < fit type {GAUSS_JORDAN, LU_DECOMPOSITION, CHOLESKY_DECOMPOSITION }

            operation_t                          m_operation; // < operation mode
            data_input_mode_t                    m_data_input_mode; // < data input mode

            bool                                 m_b_use_external_solution_base; // < flag indicating whether external precalculated dcms or decompositions (LU / SVD) thereof ("equation base") is available
            bool                                 m_b_use_single_sigma_value; // < use single sigma value

            size_t                               m_ndata; // < number of data used for fit

            const vec* m_pu; // < vector pointer to first fit parameter vector for internal reference (used when CENTER_UV_ABOUT_MEAN is NOT set for the fit_options)
            const vec* m_pv;// < vector pointer to second fit parameter vector for internal reference (used when CENTER_UV_ABOUT_MEAN is NOT set for the fit_options)

            vec                                  m_u, m_v; // < copy of data needed when CENTER_UV_ABOUT_MEAN is set in the fit_options list)

            const vec* m_pdata; // < vector pointer to data vector for internal reference
            const vec* m_psigma; // < vector pointer to data standard deviation vector for internal reference (can be used if data error estimates are available)
            const vec_b* m_poutl; // < vector pointer outliers for internal reference (see refit()-routine)
            const vec_b* m_psampl; // < vector indicating used, data points (see fit options: )

            double                               m_sigma; // < sigma value for refit

            surface_for_error_analysis           m_fit_poly; // < resulting/working polynomial

            double                               m_mean_position; // < mean position (for numerical stability: test)

            base_term_storage                    m_base_term_storage; // < keep base terms in a storage for faster access/calculations
            const base_term_storage* m_p_ext_base_term_storage; // < point to base terms in a storage for faster access/calculations when external solution bases are used, see derived specialized classes

            matrix<double>                       m_data_covariance_matrix; // < left side matrix of normal equation: A_T*A*p= A_T*b, C= A_T*A: data covariance matrix (short dcm), where is the "model matrix", A_T*b: beta, p: polynomial coefficients stored in surface poly - object
            matrix<double>                       m_inverse_data_covariance_matrix; // < inverse of the data covariance matrix (idcm) for coefficient error determination
            vec                                  m_beta; // < right side vector of normal equation: A_T*b: beta
            vec                                  m_residuals; // < residuals : data-fit_polynomial_value
            double                               m_total_square_error_of_fit; // < sum of square of residuals ( in case of outlier fit the number in m_total_square_error_of_fit differs from the square sum of all residuals by the outlier error contributions)

            surface_for_error_analysis           m_fit_poly_dx; // < derivative of fit polynomial with respect to x
            surface_for_error_analysis           m_fit_poly_dy;// < derivative of fit polynomial with respect to y

            object_state<state_t, error_state_t> m_surface_fit_state{ state_t::INITIAL };

            multi_options<option_t>              m_fit_options{ multi_options<option_t>(all_fit_options_set,{}) }; // < options for the fit
            multi_options<reset_t>               m_reset_options{ multi_options<reset_t>(all_reset_options_set,{}) }; // < reset options for the fit (internal)

            // -- reset functions (this function can be overloaded but must be called in the overloading function first!)
            void reset(const std::set<reset_t>& reset_options); // < reset variables of fit, reset_option is a combination of values from reset_t
            void reset_ptrs(); // < reset pointers
            virtual void reset_ext_data() {}; // << reset ext. data (pointers) in multifit use (implementation specific)

            // set_operation-mode/set_options function (internal)
            void set_operation(const operation_t& operation);
            void set_options(const std::set<option_t>& options);

            // -- set data functions
            // operation mode : CONST_UV_VARYING_DATA
            void set_data(const vec& data, const vec& sigma = vec());
            // operation mode: VARYING_UV_VARYING_DATA
            void set_data(const vec& u, const vec& v, const vec& data, const vec& sigma = vec(), const vec_b& subsample = vec_b());
            // operation mode: VARYING_UV_VARYING_DATA_SINGLE_SIGMA_VALUE
            void set_data(const vec& u, const vec& v, const vec& data, const double& sigma, const vec_b& subsample = vec_b());

            // -- set up equation system for the various application cases
            // base terms for operation mode CONST_UV_VARYING_DATA
            void setup_base_terms();
            void center_uv_about_mean();
            // correction of the covariance matrix and beta from outlier components for refit
            bool correct_normal_equation_system_from_outlier();
            // covariance matrix from the input data (left side of normal equation system), used when fixed (u,v)-pairs are used
            void setup_left_side_of_normal_equation_system();
            // routine for setting up the right side of the normal equation system (used when external solution base is used in specialized classes in a multifit, e.g. 3D Fit environment)
            void setup_right_side_of_normal_equation_system();
            // covariance matrix and beta from the input data (left and right side, respectively, of normal equation system)
            void setup_normal_equation_system();
            // routine for correcting the right side of the normal equation system for outlier removal (used when external solution base is used in specialized classes in a multifit, e.g. 3D Fit environment)
            bool correct_right_side_of_normal_equation_system_from_outlier();

            // -- solution of the equation system
            // solution to the normal equations (implementation specific: Gauss-Jordan, LU, SVD)
            virtual void solve_normal_equation_system() = 0;

            // routine for determining total square error from residuals
            void calculate_residuals();
            void calculate_residuals_external_solution_base();
            void calculate_total_square_error_of_fit();

            // routine for setting/calculating the errors of the coefficients of the surface polynomial
            void set_coefficient_fit_errors();

            // routine to check external base_term_storage when external solution base is used
            bool check_input_base_term_storage(const base_term_storage& ext_base_term_storage);

            // check operation mode function (CONST_UV_VARYING_DATA, VARYING_UV_VARYING_DATA, .. )
            bool check_data_input_mode(const data_input_mode_t& data_input_mode) const;

        };
        // ==============================================================================================
        std::ostream& operator<<(std::ostream& lhs, const surface_fit::state_t& state);
        // ==============================================================================================
        std::ostream& operator<<(std::ostream& lhs, const surface_fit::error_state_t& state);
        // ==============================================================================================
        std::ostream& operator<<(std::ostream& lhs, const surface_fit::type_t& type);
        // ==============================================================================================
        std::ostream& operator<<(std::ostream& lhs, const surface_fit::operation_t& operation);
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs

#include "surfacefit_gj.h"
#include "surfacefit_lu.h"
#include "surfacefit_chol.h"
#include "surfacefit_svd.h"

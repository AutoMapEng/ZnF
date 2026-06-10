#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <array>
#include <memory>

#include "surfacefit.h"
#include "surfacefit_factory.h"
#include "surfacefit_xyz_fit_handler.h"

namespace zfs 
{
    namespace math {
        // ==============================================================================================
        // Class for caching a value dependent on a parameter pair
        // ==============================================================================================
        class two_parameter_dependent_value_cache
        {
        public:
            enum class reset_option_t { MINIMUM, FULL };

            two_parameter_dependent_value_cache() :
                m_value_is_cached(false),
                m_p1(0),
                m_p2(0),
                m_value(0)
            {}

            void reset(const reset_option_t& reset_option)
            {
                switch (reset_option)
                {
                case reset_option_t::FULL:
                {
                    m_p1 = 0; m_p2 = 0; m_value = 0;
                }
                case reset_option_t::MINIMUM:
                {
                    m_value_is_cached = false;
                }
                }
            }

            void set(const double& p1, const double& p2, const double& value)
            {
                m_value_is_cached = true; m_p1 = p1; m_p2 = p2; m_value = value;
            }

            bool have_value(const double& p1, const double& p2) const
            {
                return (m_value_is_cached && m_p1 == p1 && m_p2 == p2);
            }

            const double& val() const
            {
                return m_value;
            }

        private:
            bool   m_value_is_cached; // < indicating a vector was cached
            double m_p1, m_p2; // < parameters
            double m_value; // < vector to be cached
        };
        // ==============================================================================================
        // Class for caching a vector dependent on a parameter pair
        // ==============================================================================================
        class two_parameter_dependent_vector_cache
        {
        public:
            enum class reset_option_t { MINIMUM, FULL };

            two_parameter_dependent_vector_cache() :
                m_vector_is_cached(false),
                m_p1(0),
                m_p2(0),
                m_vector(vec(3, 0))
            {}

            void reset(const reset_option_t& reset_option)
            {
                switch (reset_option)
                {
                case reset_option_t::FULL:
                {
                    m_p1 = 0; m_p2 = 0; m_vector = vec(3, 0);
                }
                case reset_option_t::MINIMUM:
                {
                    m_vector_is_cached = false;
                }
                }
            }

            void set(const double& p1, const double& p2, const vec& vector)
            {
                m_vector_is_cached = true; m_p1 = p1; m_p2 = p2; m_vector = vector;
            }

            bool have_value(const double& p1, const double& p2) const
            {
                return (m_vector_is_cached && m_p1 == p1 && m_p2 == p2);
            }

            const double& operator()(const int& i) const
            {
                return m_vector[i];
            }

            const vec& val() const
            {
                return m_vector;
            }

        private:
            bool   m_vector_is_cached; // < indicating a vector was cached
            double m_p1, m_p2; // < parameters
            vec    m_vector; // < vector to be cached
        };

        // ==============================================================================================
        // Class <xyz_surface_fit>
        //
        // Description:
        //
        // 3 vector component at once fit (x,y,z) with reference to a base surface ordinate set (p1,p2).
        // derived from class surface_fit_lu, i.o.t use same infra-structure.
        // Like class surface_fit_lu for a polynomial in two parameters (e.g. a N-th order surface for the x-component in 3D-space) of the form:
        // [Latex notation:] $f_x(p1,p2)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} p1^i p2^{j-i}$
        // of order N to given sample data.
        //
        // Notes:
        //
        // . Necessary matrix inversion of the covariance-Matrix is done using either Gauss-Jordan elimination, LU - Decomposition, Cholesky - Decomposition or SVD-Decomposition.
        // . Under favourate circumstances (see fit()-function declarations below), the same dcm or decomposition result of the covariance matrix is used for 
        //   x, y and z parameter fits avoiding as much as possible double computations
        // . all returned vectors (type vec) of the getter functions are of length/size 3 to resemble 3D co-ordinates of a location/ position co-ordinate 
        //   or a normal- or surface tangent- vector
        // . for efficiency, access to normals, normal components, surface tangent vectors and their respective errors are cached for a previously given position (u,v)

        class ZFS_API xyz_surface_fit : public surfacefit_factory
        {
        public:

            enum class option_t {
                STANDARD_OPERATION, // standard operation (residuals and chi sqr are calculated)

                DONT_CALCULATE_DISTANCE_RESIDUALS, // don't calculate distance residuals for the fit (residuals are necessary for outlier detection and total chisqr calculation w.r.t to distance)
                DONT_CALCULATE_DISTANCE_RESIDUAL_CHISQR // don't calculate distance residual chi-sqr for the fit 

            }; // options to the xyz_surface_fit

            // constructors
            xyz_surface_fit(); // < basic initializations for use as a class member (empty constructor)
            // set up fit for a polynomial of order poly_order
            // . depending on the problem choose the appropriate fit: surface_fit::GAUSS_JORDAN, surface_fit::LU_DECOMPOSITION, surface_fit::CHOLESKY_DECOMPOSITION or surface_fit::SVD_DECOMPOSITION
            // . if the best fit chi-square values and coefficient variances of the individual fits for x,y,z are required set option to surface_fit::STANDARD_OPERATION
            //   otherwise choose either surface_fit::DONT_CALCULATE_CHISQR, surface_fit::DONT_CALCULATE_COEFFICIENT_ERRORS or surface_fit::DONT_CALCULATE_ANY_ERRORS depending on your application
            // . if the distance residuals/ chi square value is not required set as option to the xyz fit xyz_surface_fit::DONT_CALCULATE_DISTANCE_RESIDUALS/ xyz_surface_fit::DONT_CALCULATE_DISTANCE_RESIDUAL_CHISQR, otherwise keep the STANDARD_OPERATION default
            // const (1):
            xyz_surface_fit(const int& order, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);
            // const (2):
            xyz_surface_fit(const int& order, const vec& u, const vec& v, const vec_b& subsample, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);

            // initialization (if an instance of the class is used as a data member in a using class), parameters are the same as for the second constructor
            // init (1):
            void init(const int& order, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);
            // init (2):
            void init(const int& order, const vec& u, const vec& v, const vec_b& subsample, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);

            // functions for fit
            // fit (1):
            bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec_b& subsample = vec_b()); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v (dcm, LU-, Cholesky- or SVD- decomposition can be reused). Use this function, when constructed initialized with "const (2)" or "init (2)."
            // fit (2):
            bool fit(const vec& x, const vec& y, const vec& z); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v (dcm, LU-, Cholesky- or SVD- decomposition can be reused). Use this function, when constructed initialized with "const (1)" or "init (1)."

            // getters and operators
            const size_t& ndata() const; // < get number of data used for fit
            const int& order() const; // < get order of fit

            const vec& get_u() const;// < get reference to first parameter fit vector
            const vec& get_v() const; // < get reference to second parameter fit vector

            double x(const double& u, const double& v) const; // x co-ordinate
            double y(const double& u, const double& v) const; // y co-ordinate
            double z(const double& u, const double& v) const; // z co-ordinate

            const double& chisqr_x() const; // chisq of x-fit (total square error)
            const double& chisqr_y() const; // chisq of y-fit (total square error)
            const double& chisqr_z() const; // chisq of z-fit (total square error)

            double var_x() const; // variance/ mean square error of x-fit
            double var_y() const; // variance/ mean square error of y-fit
            double var_z() const; // variance/ mean square error of z-fit

            const vec operator()(const double& u, const double& v)  const; // get a 3 component vector for x,y and z evaluating the fit surface polynomials at u and v
            const double distance(const double& u, const double& v)  const; // get the distance sqrt(x_fit(u,v)^2 + y_fit(u,v)^2 + z_fit(u,v)^2) to the position (x_fit(u,v),y_fit(u,v),z_fit(u,v)) of the polynomial fit from co-ordinate's 0-point.

            const double& distance_chisq() const; // < calculate and return total square error of fit with respect to distance: sum(i,0,ndata-1; d_r*d_r), where d_r[i]= sqrt(x_data[i]^2 + y_data[i]^2 + z_data[i]^2)-sqrt(x_fit(u[i],v[i])^2 + y_fit(u[i],v[i])^2 + z_fit(u[i],v[i])^2); u and v are the parameter vectors to the data vectors x, y, z

            // outlier operations
            void detect_distance_residual_outliers(const double& threshold); // < determine distance outliers with respect to given threshold. The number of outliers is provided in varialbe n_out. The outliers are reminded internally in a boolean vector. On return, the number of found outliers is returned
            bool refit_without_distance_outliers(); // < refit polynomials after outliers where determined using routine detect_distance_residual_outliers()

            // getters for retrieving outlier related information
            const vec& get_distance_residuals() const; // < after (re)fit was done this routine returns useful residual information
            vec get_surface_perpendicular_residuals(); // < determines the distances of the data points to the fit surface 
            const uint32_t& get_noutliers() const; // < get number of determined outliers with respect to distance criterion/threshold
            const vec_b& get_outliers() const; // < get outlier indicating vector (with respect to input data vectors supplied when routine fit() was called)

            // derivative operations
            void set_derivations(); // < actively set derivations of the x,y,z-component polynomials

            // getters for retrieving derivative related information
            // ( due to caching of the values and since references are returned, the same surface_fit_xyz object/instance should not be used across multiple threads! inconsistencies might occur.
            //   ... which is also not the recommended use of the class.)
            const vec& d_dx(const double& u, const double& v) const;       // < get partial derivative of surface polyinomial (surface tangent) at position (u,v), routine set_derivations() must be called beforehand
            const vec& d_dy(const double& u, const double& v) const;       // < get partial derivative of surface polyinomial (surface tangent) at position (u,v), routine set_derivations() must be called beforehand
            const vec& normal(const double& u, const double& v) const;     // < get normal of surface polyinomial at position (u,v), routine set_derivations() must be called beforehand
            const vec& d_dx_X_d_dy(const double& u, const double& v) const;// < get vector product of partial derivatives of surface polyinomial (surface tangents) at position (u,v), routine set_derivations() must be called beforehand
            const double& norm_d_dx_X_d_dy(const double& u, const double& v) const;// < get norm of the vector product of partial derivatives of surface polyinomial (surface tangents) at position (u,v), routine set_derivations() must be called beforehand

            const double& d_dx_x(const double& u, const double& v) const;     // < x co-ordinate of surface tangent d_dx vector at parameter setting (u,v)
            const double& d_dx_y(const double& u, const double& v) const;     // < y co-ordinate of surface tangent d_dx vector at parameter setting (u,v)
            const double& d_dx_z(const double& u, const double& v) const;     // < z co-ordinate of surface tangent d_dx vector at parameter setting (u,v)

            const double& d_dy_x(const double& u, const double& v) const;     // < x co-ordinate of surface tangent d_dy vector at parameter setting (u,v)
            const double& d_dy_y(const double& u, const double& v) const;     // < y co-ordinate of surface tangent d_dy vector at parameter setting (u,v)
            const double& d_dy_z(const double& u, const double& v) const;     // < z co-ordinate of surface tangent d_dy vector at parameter setting (u,v)

            const double& n_x(const double& u, const double& v) const; // x co-ordinate of normal vector at parameter setting (u,v)
            const double& n_y(const double& u, const double& v) const; // y co-ordinate of normal vector at parameter setting (u,v)
            const double& n_z(const double& u, const double& v) const; // z co-ordinate of normal vector at parameter setting (u,v)

            double dA(const double& u, const double& v, const double& du, const double& dv) const; // surface element size at parameter setting (u,v)

            // check option function
            bool check_option(const surface_fit::option_t& option) const;
            bool check_option(const option_t& xyz_option) const;

            // get fit state information
            bool check_current_state(const surface_fit::state_t& state) const; // < check current state of object
            bool find_state(const surface_fit::state_t& state) const; // < check current state of object
            bool check_current_state_no_err(const surface_fit::state_t& state) const; // < check state of object with the condition that no error occured
            bool error_occured() const; // < get error state of fit
            bool no_error_occured() const; // < get error state of fit

        protected:

            // member variables
            option_t     m_xyz_option; // < xyz_fit option setting

            size_t       m_ndata; // < number of data used for fit

            p_surfacefit m_surface_fit_x; // < polynomial representing the x-component data
            p_surfacefit m_surface_fit_y; // < polynomial representing the y-component data
            p_surfacefit m_surface_fit_z; // < polynomial representing the z-component data

            surface_fit_handler_base_map m_surfacefit_common_param_base_fit_handler_map; // < for fitting to a common param base

            uint32_t     m_n_distance_outlier; // < number of outliers
            double       m_distance_chisqr; // < chi sqr w.r.t scan distances
            vec_b        m_distance_outlier; // < vector for storint the outlier vector
            vec          m_distance_residuals; // < distance residuals of fit w.r.t to data, see also routine distance_chisq()

            vec          m_surface_perp_residuals; // < residuals perpendicular to surface ( (p-p0).n = dr ), for plane validation

            // caching for optimization of functions n_{x,y,z}(u,v), d_dx(u,v), d_dy(u,v)
            mutable two_parameter_dependent_vector_cache m_normal_cache;               // < cache for the normal vector (components)
            mutable two_parameter_dependent_vector_cache m_d_dx_X_d_dy_cache;          // < cache for the normal vector (components)
            mutable two_parameter_dependent_vector_cache m_d_dx_cache;                 // < cache for the surface tangent vector (components)
            mutable two_parameter_dependent_vector_cache m_d_dy_cache;                 // < cache for the surface tangent vector (components)
            mutable two_parameter_dependent_value_cache  m_norm_d_dx_X_d_dy_cache; // < current length of the cross-product of d_dx and d_dy
            // basic initialization
            void base_init();

            // internal reset() function
            enum class reset_t { NO_RESET, MINIMUM, FULL };
            void reset(reset_t reset_option);

            // set fit type
            // set (1):
            void set_fit(const int& order, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);
            // set (2):
            void set_fit(const int& order, const vec& u, const vec& v, const vec_b& subsample, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);

            // internal function for calculating distance residuals, used in set_data():
            void calculate_distance_residuals(const vec& x, const vec& y, const vec& z); // < internal routine for calculating distance residuals after fitting the polynomials for x,y and z

            // basic internal functions
            double distance(const double& x, const double& y, const double& z) const; // < internal routine for calculating distance from a position in 3D-space from co-ordinate's 0-point.
            vec crossproduct(const vec& a, const vec& b) const; // < internal routine for calculating the cross product from 2 3-component vectors a and b
            double dotproduct(const vec& a, const vec& b) const; // < internal routine for calculation of the dot product
            double norm(const vec& a) const; // < norm of a vector a
            double sqr_norm(const vec& a) const; // < square norm of a vector a
            vec normalize(vec a)  const; // < internal routine for normalization of a (non-zero) 3-component vector a
            vec normalize(vec a, double length_a)  const; // < internal routine for normalization of a (non-zero) 3-component vector a

        };
        // ==============================================================================================
        class ZFS_API xyz_surface_fit_with_error_analysis : public xyz_surface_fit
        {
        public:

            // constructors
            xyz_surface_fit_with_error_analysis(); // < basic initializations for use as a class member (empty constructor)

            // set up fit for a polynomial of order <order>
            // . depending on the problem choose the appropriate fit: surface_fit::GAUSS_JORDAN, surface_fit::LU_DECOMPOSITION, surface_fit::CHOLESKY_DECOMPOSITION or surface_fit::SVD_DECOMPOSITION
            // . if the best fit chi-square values and coefficient variances of the individual fits for x,y,z are required set option to surface_fit::STANDARD_OPERATION
            //   otherwise choose either surface_fit::DONT_CALCULATE_CHISQR, surface_fit::DONT_CALCULATE_COEFFICIENT_ERRORS or surface_fit::DONT_CALCULATE_ANY_ERRORS depending on your application
            // const (1):
            xyz_surface_fit_with_error_analysis(const int& order, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);
            xyz_surface_fit_with_error_analysis(const int& order, const vec& u, const vec& v, const vec_b& subsample, const surface_fit::operation_t& operation = surface_fit::operation_t::STANDARD_OPERATION, const surface_fit::type_t& type = surface_fit::type_t::GAUSS_JORDAN, const std::set<surface_fit::option_t>& options = {}, const option_t& xyz_fit_option = option_t::STANDARD_OPERATION);

            using xyz_surface_fit::fit;

            // fit (1: (u and v), x, y, and z vectors are different for every fit):
            bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec& sigma, const vec_b& subsample); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v  under consideration of data errors (standard deviations) for a combination/tuple/vector (x,y,z) (dcm, LU-, Cholesky- or SVD- decomposition can be reused), use with "const (1)"; set subsample to vec_b() if not used
            bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec& sigma_x, const vec& sigma_y, const vec& sigma_z, const vec_b& subsample); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v under consideration of different data errors (standard deviations) for x, y and z for a particular data index (dcm, LU-, Cholesky- or SVD- decomposition cannot be reused), use with "const (1)"; set subsample to vec_b() if not used
            bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma_x, const double& sigma_y, const double& sigma_z, const vec_b& subsample); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v under consideration of different data errors (standard deviations) for x, y and z for a particular data index (dcm, LU-, Cholesky- or SVD- decomposition cannot be reused), use with "const (2)"

            // fit (2: (u and v are reused from initialization) x, y, and z vectors are different for every fit):
            bool fit(const vec& x, const vec& y, const vec& z, const vec& sigma); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v  under consideration of data errors (standard deviations) for a combination/tuple/vector (x,y,z) (dcm, LU-, Cholesky- or SVD- decomposition can be reused), use with "const (2)"
            bool fit(const vec& x, const vec& y, const vec& z, const vec& sigma_x, const vec& sigma_y, const vec& sigma_z); // < routine for fitting x,y and z polynomials to the x,y and z position data vectors with respect to the parameter vectors u and v under consideration of different data errors (standard deviations) for x, y and z for a particular data index (dcm, LU-, Cholesky- or SVD- decomposition cannot be reused), use with "const (2)"

            // the fit "transports" also information on the uncertainty of the coefficients. in case of a single sigma for x, y, and z data the "old" sigma can be "replaced" by a "new" sigma, the coefficient errors are adapted;
            // false is returned in case a vector of sigma values was given, when the fit routine was initially called
            bool replace_sigmas(const double& sigma_x, const double& sigma_y, const double& sigma_z);

            // use also variance functions for mean square error of x,y or z parametric fits 
            using xyz_surface_fit::var_x;
            using xyz_surface_fit::var_y;
            using xyz_surface_fit::var_z;

            /*
             * estimated position error for a position evaluated from the result fit surface
             */
            const vec& var_xyz(const double& u, const double& v) const; // < variances as vector for x,y and z

            const double& var_x(const double& u, const double& v) const;
            const double& var_y(const double& u, const double& v) const;
            const double& var_z(const double& u, const double& v) const;

            double stdev_x(const double& u, const double& v) const; // < standard deviation for x
            double stdev_y(const double& u, const double& v) const;
            double stdev_z(const double& u, const double& v) const;

            /*
            * estimated surface tangent error for surface tangents d_dx() and d_dy() evaluated from the fit surface
            */

            const vec& var_d_dx(const double& u, const double& v) const;

            const double& var_d_dx_x(const double& u, const double& v) const;
            const double& var_d_dx_y(const double& u, const double& v) const;
            const double& var_d_dx_z(const double& u, const double& v) const;

            double stdev_d_dx_x(const double& u, const double& v) const;
            double stdev_d_dx_y(const double& u, const double& v) const;
            double stdev_d_dx_z(const double& u, const double& v) const;

            const vec& var_d_dy(const double& u, const double& v) const;

            const double& var_d_dy_x(const double& u, const double& v) const;
            const double& var_d_dy_y(const double& u, const double& v) const;
            const double& var_d_dy_z(const double& u, const double& v) const;

            double stdev_d_dy_x(const double& u, const double& v) const;
            double stdev_d_dy_y(const double& u, const double& v) const;
            double stdev_d_dy_z(const double& u, const double& v) const;

            /*
             *  estimated normal direction error for a normal evaluated from the result fit surface
             */
            const vec& var_normal(const double& u, const double& v) const; // < get variance of normal of surface polyinomial at position (u,v), routine set_derivations() must be called beforehand

            const double& var_nx(const double& u, const double& v) const; // < variance (estimate) of x-component of normal at (u,v)
            const double& var_ny(const double& u, const double& v) const; // < variance (estimate) of y-component of normal at (u,v)
            const double& var_nz(const double& u, const double& v) const; // < variance (estimate) of z-component of normal at (u,v)

            double stdev_nx(const double& u, const double& v) const; // < (estimate of) standard deviation of x-component of normal at (u,v)
            double stdev_ny(const double& u, const double& v) const; // < (estimate of) standard deviation of y-component of normal at (u,v)
            double stdev_nz(const double& u, const double& v) const; // < (estimate of) standard deviation of z-component of normal at (u,v)

        protected:

            // caching for error function var_{x,y,z}(u,v), var_n{x,y,z}(u,v), var_d_dx(u,v), var_d_dy(u,v) (not used when option surface_fit:: DONT_CALCULATE_ANY_ERRORS is set)
            mutable two_parameter_dependent_vector_cache m_position_var_cache; // < cache for the variances of the position components
            mutable two_parameter_dependent_vector_cache m_normal_var_cache; // < cache for the variances of the normal components
            mutable two_parameter_dependent_vector_cache m_var_d_dx_cache;   // < cache for the variances of the surface tangent vector components
            mutable two_parameter_dependent_vector_cache m_var_d_dy_cache;   // < cache for the variances of the surface tangent vector components

            void reset(reset_t reset_option);


        };
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs
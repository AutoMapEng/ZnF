#pragma once

#include <zfs/zfsapi.h>
#include <zfs/Math/3d/surface.h>

namespace zfs {
    namespace math {
        // ==============================================================================================
        // Class for a polynomial of the form (of order N):
        // $f(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // (e.g. a N-th order surface in 3D-space)
        // additionally errors to the coefficients can be given.

        class ZFS_API surface_for_error_analysis : public surface_poly
        {
        public:
            // constructors
            surface_for_error_analysis();
            surface_for_error_analysis(const int& order);
            surface_for_error_analysis(const int& order, const vec& coefficients);
            surface_for_error_analysis(const int& order, const vec& coefficients, const vec& coefficient_errors);

            // initializations (from parent and additional)
            using surface_poly::init;
            bool init(const int& order, const vec& coefficients, const vec& coefficient_errors);

            // special setters
            void set_order(const int& order); // < set order of coefficients
            bool set_coef_errors(const vec& errors); // < sets coefficent errors to values in provided vector for set polynomial order, if the number of elements in the error vector do not fit the polynomial order the errors are not set

            // getters
            const vec& coef_errors() const; // < returns the coefficient errors of the polynomial (read only)
            const double& coef_error(const int& i) const; // returns the error to the coefficient with index i
            double& coef_error(const int& i); // access (read/write) to the coefficient with index i

            double error(const double x, const double y) const;

            surface_for_error_analysis d_dx() const; //< surface polynomial of derivative w.r.t. x
            surface_for_error_analysis d_dy() const; //< surface polynomial of derivative w.r.t. y

        private:

            vec m_coef_errors; // < estimated or given errors to the surface polynomial coefficients

        };
    } // end namespace math
} // end namespace zfs

#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <array>

#include <zfs/Math/math/matrix.h>

namespace zfs {
    namespace math {

        // ==============================================================================================
        using vec = std::vector<double>;
        // ==============================================================================================
        // Class for a polynomial of the form (of order N):
        // $f(x,y)=\sum_{i=0}^{N}\sum_{j=0}^{i}a_{ij} x^i y^{j-i}$
        // (e.g. a N-th order surface in 3D-space)

        class ZFS_API surface_poly
        {
        public:
            // constructors
            surface_poly();
            surface_poly(const int& order);
            surface_poly(const int& order, const vec& coefficients);
            virtual ~surface_poly() {}

            // initializations
            void init(const int& order); // < init/set/reset order
            bool init(const int& order, const vec& coefficients); // < init/set/reset order and set coefficients

            // special setters
            virtual void set_order(const int& order); // < set order of coefficients
            void zero_coefs(); // < reset (zero polynomial coefficents, without changeing polynomial order)
            void zero_numeric(); // < reset base terms
            bool set_coefs(const vec& coefs); // < sets coefficents to values in provided vector for set polynomial order, if the number of coefficients does not fit to the polynomial order the polynomial remains unchanged

            // getters
            const int& n_coefs() const; // < given the polynomial order the number of coefficients is returned
            const int& order() const; // < returns the polynomial order
            const vec& coefs() const; // < returns the coefficients of the polynomial (read only)

            // operators and complex functions
            const double& operator()(const int& i) const; // < access to coefficients (read only)
            double& operator()(const int& i); // < access to coefficients (read/modify)

            double operator()(const double& x, const double& y) const; // < get the value of the surface polynomial at position (x,y)
            double operator()(const vec& base_terms_) const; // < get the value of the polynomial for provided base_terms (: 1, x, y, x^2, xy, y^2, x^3, ... ) provided in a vector suiting the order of the polynomial

            const vec& base_terms(const double& x, const double& y) const; // < returns the base terms of the polynomial (uses storage: m_base_terms avoiding reallocations)
            vec base_terms_(const double& x, const double& y) const; // < returns the base terms of the polynomial (uses local in function storage, hopes for compiler optimization, when data is stored into external storage)

            surface_poly d_dx() const; //< surface polynomial of derivative w.r.t. x
            surface_poly d_dy() const; //< surface polynomial of derivative w.r.t. y

        protected:

            // member variables
            int m_order;              //< polynomial order
            int m_n_coefs;            //< number of coefficients
            vec m_coefs;              //< coefficients describing the surface

            mutable vec m_base_terms; //< vector keeping base terms (efficiency: avoids re-allocations in this implementation)

            // internal member functions
            int no_of_coefs() const;  // < calculates the number of coeffients 
        };
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs
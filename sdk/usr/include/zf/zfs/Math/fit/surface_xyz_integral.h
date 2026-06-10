#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include <cmath>

#include "surfacefit_xyz.h"
#include "surface_integration_constants.h"

namespace zfs {
    namespace math {
        // ======================================================================
        using vec = std::vector<double>;
        // ======================================================================
        // approximate/rough integral for surface determined from surfacefit
        // ======================================================================
        class ZFS_API surface_xyz_integral
        {
        public:

            enum class sample_order_t : uint32_t { UNSET, NO_ORDER, PIXEL_LINE, LINE_PIXEL };
            enum class spacing_t : uint32_t { UNSET, EQUIDISTANT, ARBITRARY };

            // default constructor
            surface_xyz_integral();

            // integral approximation using sampled parameter vectors from fit
            surface_xyz_integral(xyz_surface_fit* p_xyz_surface_fit_, const sample_order_t& order_, const spacing_t& spacing_);

            // integral approximation with given parameter ranges and sampling
            surface_xyz_integral(xyz_surface_fit* xyz_surface_fit_,
                const range& u_range_,
                const range& v_range_,
                const double& du_,
                const double& dv_);

            double integrate() const;

        private:

            enum class mode_t : uint32_t {
                UNSET,
                SIMPLE_MODE,            // < given parameter ranges plus parameter differentials are used
                PARAMETER_SAMPLES     // < parameter sample vectors from fit are used
            };

            xyz_surface_fit* const p_xyz_surfacefit{ nullptr }; // < surface fit containing fitted surface

            surface_integration_parameter_and_constants surface_integration_parameters;

            std::shared_ptr<const vec> p_u{ nullptr }; // < sample base for parameter u
            std::shared_ptr<const vec> p_v{ nullptr }; // < sample base for parameter v

            mode_t  mode{ mode_t::UNSET }; // < used mode
            sample_order_t order{ sample_order_t::UNSET }; // <  order auxilliary information when a sample base for u is used
            spacing_t spacing{ spacing_t::UNSET };// <  spacing auxilliary information when a sample base for v is used

            double norm(const double& x, const double& y, const double& z) const; // < internal routine for calculating vector length
            double norm(const vec& vec_) const; // < internal routine for calculating vector length
        };

        // ======================================================================
    } // end namespace math
} // end namespace zfs
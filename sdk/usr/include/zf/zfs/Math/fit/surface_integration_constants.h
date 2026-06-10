#pragma once

#include <zfs/zfsapi.h>

#include <utility>

namespace zfs {
    namespace math {
        // ======================================================================
        using range = std::pair<double, double>;
        // ======================================================================
        class ZFS_API surface_integration_parameter_and_constants
        {
        public:
            surface_integration_parameter_and_constants();
            surface_integration_parameter_and_constants(const double& du_, const double& dv_, const range& u_range_, const range& v_range_);

            double du{ 0 }; // < parameter differential to use for u
            double dv{ 0 }; // < parameter differential to use for v

            range u_range; // < parameter range for u
            range v_range; // < parameter range for u

            int number_surface_elements_u{ 0 };
            int number_surface_elements_v{ 0 };

            int total_number_of_surface_elements{ 0 };

            double parameter_differential_product{ 0 };
            double constant_integration_multiplier{ 0 };

        };
    } // end namespace math
} // end namespace zfs
#pragma once

#include <zfs/zfsapi.h>

#include <vector>
#include "range.h"

namespace zfs {
    namespace math
    {
        // class representing a piecewise constant function
        class ZFS_API piecewise_constant
        {
        public:
            piecewise_constant::piecewise_constant()
            {}

            piecewise_constant::piecewise_constant(
                const std::vector< Range< double > >& ordinate_ranges,
                const std::vector< double >& function_values)
                : m_ordinate_ranges(ordinate_ranges),
                m_function_values(function_values)
            {
                // for every range there should be a function value
                // ranges do not have to be ordered or non-overlapping
            }

            double piecewise_constant::operator()(const double& ordinate) const
            {
                int n_ranges = int(m_ordinate_ranges.size());
                for (int i = 0; i < n_ranges; ++i)
                    if (m_ordinate_ranges[i].isInRange(ordinate))
                        return m_function_values[i];
                return 0.;
            }

        protected:

            std::vector< Range< double > > m_ordinate_ranges;
            std::vector< double >          m_function_values;
        };

    } // end namespace math
} // end namespace zfs
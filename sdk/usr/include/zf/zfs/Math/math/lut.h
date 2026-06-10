#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace zfs {
    namespace math
    {

        /**
        \brief look up table class for representing functions as look up table (lut)
        */
        class LUT
        {
        public:
            LUT();

            LUT(uint32_t n_table_entries,
                double valid_parameter_range_start,
                double valid_parameter_range_end,
                std::function<double(double parameter)> func,
                double scale = 1.);

            void init(uint32_t n_table_entries,
                double valid_parameter_range_start,
                double valid_parameter_range_end,
                std::function<double(double parameter)> func,
                double scale = 1.);

            double operator()(const double& parameter) const;

        private:

            uint32_t n_table_entries{ 0 };

            double scale{ 0 }; // (optional) scaling of the function
            double parameter_value_step{ 0 };
            double valid_parameter_range_start{ 0 };
            double valid_parameter_range_end{ 0 };

            std::function<double(double parameter)> func;

            std::vector<double> function_samples;

            void set();
        };

    } // end namespace math
} // end namespace zfs
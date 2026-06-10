#pragma once

#include <zfs/zfsapi.h>


namespace zfs {
    namespace math
    {

        /**
        \brief calculates average, standard deviation, min and max
        */
        class ZFS_API Average
        {
        public:
            Average();

            void clear();

            /// add a value
            void add(const double& neu);

            /// calculate average and sigma
            bool calc();

            /// negate average
            bool negate_();

            const double& getMin()   const { return min; }
            const double& getMax()   const { return max; }
            const double& getAvr()   const { return avr; }
            const double& getStd()   const { return std; }
            const int& getCount() const { return count; }

        protected:
            double min;             ///< minimum added value
            double max;             ///< maximum added value
            long double sum_avr;    ///< internal count
            long double sum_std;    ///< internal count
            double avr;             ///< the average after calculation
            double std;             ///< the sigma after calculation
            int count;              ///< number of values added

        };

    } // end namespace math
} // end namespace zfs
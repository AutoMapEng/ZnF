#pragma once

#include <zfs/zfsapi.h>


namespace zfs {
    namespace math
    {

        /**
        \brief calculates mean, standard deviation, min and max
        */
        class ZFS_API mean
        {
        public:
            mean();

            void clear();

            /// add a value
            void add(const double& neu);

            /// calculate mean and sigma
            bool calc();

            const double& getAvr()   const { return avr; }
            const int& getCount() const { return count; }

        protected:
            long double sum_avr;    ///< internal count
            double avr;             ///< the mean after calculation
            int count;              ///< number of values added

        };

    } // end namespace math
} // end namespace zfs
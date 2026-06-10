#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>

namespace zfs {
    namespace math
    {

        /**
        \brief calculates min and max of signed types
        */
        template<class T> class ZFS_API MinMax
        {
        public:
            MinMax();

            void clear();

            /// add a value
            void add(const T& neu);

            const T DiffMinMax() const { return max - min; }

            const T& getMin()   const { return min; }
            const T& getMax()   const { return max; }

        protected:
            T min;             ///< minimum added value
            T max;             ///< maximum added value

        };

    } // end namespace math
} // end namespace zfs
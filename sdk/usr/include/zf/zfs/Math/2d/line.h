#pragma once

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // ==================================================================
        template <class T> class Point;
        // ==================================================================
        /*
        * @brief class for representing a line
        * @author holzner
        */
        template<class T> class ZFS_API Line
        {
        public:
            Line();
            Line(const Point<T>& aP1, const Point<T>& aP2);

            T eval(const T& aX) const;
            bool onLine(const Point<T>& aP) const;

        private:
            T m_Slope, m_Offset, m_Pos;

        };
        // ==================================================================
    } // end namespace math
} // end namespace zfs
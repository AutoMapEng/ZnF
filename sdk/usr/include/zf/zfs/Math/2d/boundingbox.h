#pragma once

#include <zfs/zfsapi.h>

#include "point.h"

namespace zfs {
    namespace math {
        template<class T> class Vector2D;

        // ============================================================================================================
        class ZFS_API BoundingBox
        {
        private:

            Point<int> UpperLeft;
            Point<int> LowerRight;

            // -- helper functions
            const BoundingBox BoundingBoxFromPointList(const DoublePointListType& aPointList);
            const BoundingBox BoundingBoxFromPointList(const IntPointListType& aPointList);

        public:

            BoundingBox();
            BoundingBox(const Point<int> aUpperLeft, const Point<int> aLowerRight);
            BoundingBox(const DoublePointListType& aPointList);
            BoundingBox(const IntPointListType& aPointList);

            // accessors
            Point<int> getUpperLeft() const;
            Point<int> getLowerRight() const;

            Point<int>& getUpperLeft();
            Point<int>& getLowerRight();

            void setUpperLeft(const Point<int>& aUpperLeft);
            void setLowerRight(const Point<int>& aLowerRight);

            void set(const Point<int>& aUpperLeft, const Point<int>& aLowerRight);

            // helpers/auxilliary
            unsigned int          getHeight() const;
            unsigned int          getWidth() const;
            unsigned int          getArea() const;
            Point<double> getCentroid() const;

            void shift(const Vector2D<int> a2DShift);
            bool overlap(const BoundingBox& rhs) const;

            // friend functions
            friend bool ZFS_API operator==(const BoundingBox& a_lhs, const BoundingBox& a_rhs);
            friend bool ZFS_API operator!=(const BoundingBox& a_lhs, const BoundingBox& a_rhs);

        };

        // ============================================================================================================
        // two sided operators
        // ============================================================================================================
        bool ZFS_API operator==(const BoundingBox& a_lhs, const BoundingBox& a_rhs);
        bool ZFS_API operator!=(const BoundingBox& a_lhs, const BoundingBox& a_rhs);
        // ============================================================================================================

    } // end namespace math
} // end namespace zfs
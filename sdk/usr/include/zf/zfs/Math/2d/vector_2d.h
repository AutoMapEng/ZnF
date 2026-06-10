#pragma once

#include <list>

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // =====================================================================================
        template <class T> class Point;
        // =====================================================================================
        /** \class Vector2D
         * \brief  Class providing functionality for vectors in two dimensions.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for vectors in two dimensions.
         *
         */
        template<class T>
        class ZFS_API Vector2D
        {
        protected:
            T m_X;
            T m_Y;

        public:

            Vector2D();
            Vector2D(const T& m_X, const T& m_Y);
            Vector2D(const Point<T>& P);
            Vector2D(const Point<T>& P1, const Point<T>& P2);

            T getX() const;
            T getY() const;

            double getNorm() const;
            double getLength() const;

            const Vector2D<T>  operator +(const Vector2D<T>& V_rhs) const;
            const Vector2D<T>  operator -(const Vector2D<T>& V_rhs) const;
            const Vector2D<T>  operator -() const;
            const Vector2D<T>  operator /(const T& aFactor) const;
            const Vector2D<T>  operator *(const T& aFactor) const;

            Vector2D<T>& operator *=(const T& aFactor);
            Vector2D<T>& operator /=(const T& aFactor);
        };
        // =====================================================================================
        typedef std::list< Vector2D<double> > DoubleVector2DListType;
        // =====================================================================================
        // additional/auxiliary functions
        // =====================================================================================
        template<class T> T ZFS_API ZCompCross2D(const Vector2D<T>& V1, const Vector2D<T>& V2);
        // =====================================================================================
        // Vector / Point Operations
        // =====================================================================================
        // + operator
        // =====================================================================================
        template<class T> const Point<T> ZFS_API operator+(const Point<T>& aPoint, const Vector2D<T>& aVector);
        // -------------------------------------------------------------------------------------
        template<class T> const Point<T> ZFS_API operator+(const Vector2D<T>& aVector, const Point<T>& aPoint);
        // =====================================================================================
        // - operator
        // =====================================================================================
        template<class T> const Point<T> ZFS_API operator-(const Point<T>& aPoint, const Vector2D<T>& aVector);
        // =====================================================================================
    } // end namespace math
} // end namespace zfs
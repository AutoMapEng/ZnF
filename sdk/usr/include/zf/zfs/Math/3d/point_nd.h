#pragma once

#include <vector>
#include <list>
#include <iostream>

#include <zfs/zfsapi.h>
#include <Common/inttypes_.h>

namespace zfs {
    namespace math {
        // ============================================================================================================
        // -- necessary forward declaration
        // ============================================================================================================
        template<class T> class Point;
        template<class T, unsigned int N> class VectorND;
        template<class T> class Vector3D;
        template<class T> class SphericalCS;
        // ============================================================================================================
        /** \class PointND
         * \brief  Class providing functionality for points/co-ordinates in n-dimensional space.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for points/co-ordinates in n-dimensional space.
         *
         */
        template<class T, unsigned int N>
        class ZFS_API PointND
        {
            // friend classes
            template<class S> friend class Point3D;
            template<class S, unsigned int M> friend class PointND;
            template<class S, unsigned int M> friend class VectorND;

        protected:
            std::vector<T> point_elements;
        public:
            // -- constructors
            PointND();
            PointND(const T& value);
            PointND(const T* src);
            PointND(const std::vector<T>& src);
            PointND(const PointND<T, N>& P);
            template<class S, unsigned int M> PointND(const PointND<S, M>& src);
            // -- destructor
            ~PointND();
            // -- assingment
            PointND<T, N>& operator=(const PointND<T, N>& rhs);
            PointND<T, N>& operator=(const std::vector<T>& rhs);
            // -- accessors
            T operator[](unsigned int idx) const;
            T& operator[](unsigned int idx);

            // -- mathmatical operators
            // PointND<T, N> operator-() const;

            const T* const GetMemAdress();

            bool operator ==(const PointND<T, N>& rhs) const;
            bool operator !=(const PointND<T, N>& rhs) const;
            // -- conversion
            template<class R, unsigned int L> operator PointND<R, L>() const;
            // -- friend operators
            template<class S, unsigned int M> friend const VectorND<S, M> ZFS_API operator-(const PointND<S, M>& PointEnd, const PointND<S, M>& PointStart);
            template<class S, unsigned int M> friend const PointND <S, M> ZFS_API operator+(const PointND<S, M>& aPoint, const VectorND<S, M>& aVector);
            template<class S, unsigned int M> friend const PointND <S, M> ZFS_API operator-(const PointND<S, M>& aPoint, const VectorND<S, M>& aVector);

        };

        // ============================================================================================================
        /** \class Point3D
         * \brief  Specialization providing functionality for points/co-ordinates in
         *         3-dimensional space.
         *
         * \author Juergen Holzner
         *
         * Specialization providing functionality for points/co-ordinates in
         * 3-dimensional space.
         *
         */
        template<class T>
        class ZFS_API Point3D : public PointND<T, 3>
        {
            template<class S> friend class SphericalCS;

        public:

            Point3D();
            Point3D(const T& value);
            Point3D(const T& x, const T& y, const T& z);
            Point3D(const PointND<T, 3>& aPoint);
            template<class T1> Point3D(const Point3D<T1>& aPoint);

            void Set(const T& x, const T& y, const T& z);

            Point3D& operator=(const PointND<T, 3>& aPoint);
        };

        // ============================================================================================================

        typedef std::list  < Point3D< float> > FloatPoint3DList_t;
        typedef std::vector< Point3D< float> > FloatPoint3DVector_t;

        // ============================================================================================================

        typedef std::list  < Point3D< double> > DoublePoint3DList_t;
        typedef std::vector< Point3D< double> > DoublePoint3DVector_t;

        // ============================================================================================================

        template<typename T> std::ostream& operator << (std::ostream& ofs, const Point3D<T>& v);
        template<typename T> std::istream& operator >> (std::istream& is, Point3D<T>& v);

        // ============================================================================================================

        template<typename T, unsigned int N> std::ostream& operator << (std::ostream& ofs, const PointND<T, N>& v);
        template<typename T, unsigned int N> std::istream& operator >> (std::istream& is, PointND<T, N>& v);

        // ============================================================================================================

    } // end namespace math
} // end namespace zfs
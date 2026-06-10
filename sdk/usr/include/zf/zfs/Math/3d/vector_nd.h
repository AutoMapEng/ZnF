#pragma once

#include <vector>
#include <cstdint>
#include <zfs/zfsapi.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace zfs {
    namespace math {
        // ============================================================================================================
        // -- necessary forward declaration
        // ============================================================================================================
        template<class T, uint32_t N> class PointND;
        template<class T> class Point3D;
        template<class T> class Vector3D;
        // ============================================================================================================
        // class declaration
        // ============================================================================================================
        /** \class VectorND
         * \brief  Class providing functionality for vectors in N dimensions.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for vectors in N dimensions.
         *
         */
        template<class T, uint32_t N = 3>
        class ZFS_API VectorND
        {
            template<class S> friend class Vector3D;
            template<class S> friend class Point3D;
            template<class S, uint32_t M> friend class PointND;

        protected:
            std::vector<T> vector_elements;

        public:
            // -- constructors
            VectorND();
            VectorND(const T& val);
            VectorND(const PointND<T, N>& src);
            VectorND(const VectorND<T, N>& src);
            VectorND(const std::vector<T>& src);
            VectorND(const T* const src);
            VectorND(const PointND<T, N>& P1, const PointND<T, N>& P2);
            template<class T1, uint32_t N1> VectorND(const VectorND<T1, N1>& src);
            ~VectorND();

            // -- accessors
            T  getElement(uint32_t i) const;
            T& operator[](uint32_t i);
            T  operator[](uint32_t i) const;
            T& operator()(uint32_t i);
            T  operator()(uint32_t i) const;

            // -- setter
            void setTo(const T& val);

            // -- conversion
            template<class T1, uint32_t N1> operator VectorND<T1, N1>() const;

            // -- operators and math operations
            // const operators
            const VectorND<T, N>  operator-() const;

            const VectorND<T, N>  operator+(const VectorND<T, N>& V_rhs) const;
            const VectorND<T, N>  operator-(const VectorND<T, N>& V_rhs) const;

            template<class T1> const VectorND<T, N>  operator/(const T1& aFactor) const;
            template<class T1> const VectorND<T, N>  operator*(const T1& aFactor) const;

            // assignment operators
            VectorND<T, N>& operator=(const VectorND<T, N>& V_rhs);
            VectorND<T, N>& operator=(const T* const src);

            VectorND<T, N>& operator+=(const VectorND<T, N>& V_rhs);
            VectorND<T, N>& operator-=(const VectorND<T, N>& V_rhs);

            VectorND<T, N>& operator+=(const T& rhs);
            VectorND<T, N>& operator-=(const T& rhs);

            template<class T1> VectorND<T, N>& operator/=(const T1& aFactor);
            template<class T1> VectorND<T, N>& operator*=(const T1& aFactor);

            // getters            
            double                Norm()         const;
            double                Length()       const;

            T* const              GetMemAdress();

            const VectorND<T, N>  Normalized()   const;

            // mathmatical operations
            const VectorND<T, N>  CrossProd(const VectorND<T, N>& V_rhs) const;
            const VectorND<T, N>  CrossProd(const T* const V_rhs) const;

            double                ScalarProd(const VectorND<T, N>& V_rhs) const;
            double                ScalarProd(const T* const V_rhs) const;

            double                NormalizedScalarProd(const VectorND<T, N>& V_rhs) const;
            double                NormalizedScalarProd(const T* const V_rhs) const;

            double                Angle(const VectorND<T, N>& V_rhs) const;

            bool                  operator ==(const VectorND<T, N>& rhs) const;
            bool                  operator !=(const VectorND<T, N>& rhs) const;

            // friend functions/operators

            template<class S, uint32_t M> friend const PointND <S, M> ZFS_API operator+(const PointND<S, M>& aPoint, const VectorND<S, M>& aVector);
            template<class S, uint32_t M> friend const PointND <S, M> ZFS_API operator-(const PointND<S, M>& aPoint, const VectorND<S, M>& aVector);

            template<class S, uint32_t M> friend const VectorND<S, M> ZFS_API operator+(const S& lhs, const VectorND<S, M>& rhs);
            template<class S, uint32_t M> friend const VectorND<S, M> ZFS_API operator-(const S& lhs, const VectorND<S, M>& rhs);

            template<class S, uint32_t M> friend const VectorND<S, M> ZFS_API operator-(const PointND<S, M>& Point1, const PointND<S, M>& Point2);

        };
        // ============================================================================================================
        // auxiliary functions
        // ============================================================================================================
        // operators
        template<class T, uint32_t N> const PointND <T, N> ZFS_API operator+(const PointND<T, N>& aPoint, const VectorND<T, N>& aVector);
        template<class T, uint32_t N> const PointND <T, N> ZFS_API operator-(const PointND<T, N>& aPoint, const VectorND<T, N>& aVector);

        template<class T, uint32_t N> const VectorND<T, N> ZFS_API operator-(const PointND<T, N>& PointEnd, const  PointND<T, N>& PointStart);

        template<class T, uint32_t N> const VectorND<T, N> ZFS_API operator+(const T& lhs, const VectorND<T, N>& rhs);
        template<class T, uint32_t N> const VectorND<T, N> ZFS_API operator-(const T& lhs, const VectorND<T, N>& rhs);

        // for calculation
        template<class T> double ZFS_API VectorNorm(const T* const Vector, const uint32_t& nElements);

        // ============================================================================================================
        /** \class Vector3D
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
        class ZFS_API Vector3D : public VectorND<T, 3>
        {
            template<class S> friend class Point3D;
            template<class S> friend class Vector3D;

        public:

            Vector3D();
            Vector3D(const T& x, const T& y, const T& z);
            Vector3D(const VectorND<T, 3>& rhs);
            Vector3D(const PointND<T, 3>& rhs);
            Vector3D(const Point3D<T>& rhs);

            // -- operators
            Vector3D& operator=(const VectorND<T, 3>& rhs);
            Vector3D& operator=(const Point3D<T>& rhs);

            T& operator[](const uint32_t idx);
            T operator[](const uint32_t idx) const;

            // -- calculus
            const Vector3D CrossProd(const Vector3D<T>& rhs) const;
            const Vector3D NormCrossProd(const Vector3D<T>& rhs) const;
            const Vector3D Normalized() const;

            double Norm() const;
            double ScalarProd(const Vector3D<T>& rhs) const;
            double ScalarProd(const T& x, const T& y, const T& z) const;
        };
        // ============================================================================================================
    } // end namespace math
} // end namespace zfs
#pragma once

#include <math.h>
#include <iostream>

#include "../zfsapi.h"

namespace zfs {
	namespace math
	{
        //==========================================================================

		const double eps = 0.00000001;

		//==========================================================================

		/**
		\brief header only implementation of a vector class
		*/
        template<class T>
		class Vec3
		{
		public:
            Vec3() {}
            Vec3(int v) { x() = T(v); y() = T(v); z() = T(v); }
            Vec3(T v) { x() = v; y() = v; z() = v;}
            Vec3(double xx, double yy, double zz);
            Vec3(float xx, float yy, float zz);
            Vec3(const Vec3<T>& v);
            Vec3(const double* p); ///< array with 3 items
            Vec3(const float* p);	///< array with 3 items

            // ! no virtual destructor otherwise memory footprint will be wrong


            // get
            const T& x() const { return vec[0]; }
            const T& y() const { return vec[1]; }
            const T& z() const { return vec[2]; }
            const T& getX() const { return vec[0]; }
            const T& getY() const { return vec[1]; }
            const T& getZ() const { return vec[2]; }
            const Vec3<T>& xyz() const { return *this; }
            const T& operator [] (int idx) const;	///< access value by index. index will not be checked
            const T* getValue() const { return vec; }

            // set
            T& x() { return vec[0]; }
            T& y() { return vec[1]; }
            T& z() { return vec[2]; }
            void setX(T val) { vec[0] = val; }
            void setY(T val) { vec[1] = val; }
            void setZ(T val) { vec[2] = val; }
            void setXYZ(const Vec3<T>& v) { *this = v; }
            void setXYZ(double xx, double yy, double zz) { vec[0] = static_cast<T>(xx); vec[1] = static_cast<T>(yy); vec[2] = static_cast<T>(zz); }
            void setXYZ(float xx, float yy, float zz) { vec[0] = xx; vec[1] = yy; vec[2] = zz; }
            Vec3<T>& xyz() { return *this; }
            T& operator[] (int idx); ///< index will not be checked

			// operators
			bool operator == (const Vec3<T>& v) const;
			bool operator != (const Vec3<T>& v) const;
			Vec3<T>& operator = (const Vec3<T>& v);
			Vec3<T>& operator += (const Vec3<T>& v);
            Vec3<T>& operator -= (const Vec3<T>& v);
            Vec3<T>& operator /= (double fac);
            Vec3<T>& operator *= (double fac);
            Vec3<T> operator - () const;
			Vec3<T> operator - (const Vec3<T>& v) const;
            Vec3<T> operator * (double fac) const;
            Vec3<T> operator * ( const Vec3<T>& ) const;
            Vec3<T> operator / (double fac) const;
            Vec3<T> operator + (const Vec3<T>& v) const;

            /// true when all values exact zero
			bool isZero() const; 

			double dist(double xx, double yy, double zz) const;    ///< returns the length of the vector this-arg
			double dist(const Vec3<T>& vec) const;              ///< returns the length of the vector this-vec
            double dist2( const Vec3<T>& vec ) const;           ///< returns the length of the vecor in the square

			double length() const;                          ///< returns the length of the vector.
			void setLength(double l);                       ///< Set the vector to a certain length.
            Vec3<T>& normalize();                               ///< Scale this to length 1.
			Vec3<T> getNormalized() const;                  ///< Returns this scaled to length 1.

			void scale(double factor);                     ///< scale this vector by the given factor.
			Vec3<T> getScaled(double factor) const;               ///< Returns a scaled copy of this.
			double dotProd(const Vec3<T>& other) const;    ///< returns the dotproduct this * other
			Vec3<T> crossProd(const Vec3<T>& other) const;  ///< returns this x other
			double angle(const Vec3<T>& other) const;      /**< returns the angle between this and other. Values will be in the range [0,PI] */
			double norm() const;		   /**< returns norm of current vector  with eucledian distnace */
        
        protected:
            T vec[3] {};   // x,y,z
        };

		//==================================================================================================================================



		typedef Vec3<float> Vec3f;
		typedef Vec3<double> Vec3d;

        template class ZFS_API Vec3<float>;
        template class ZFS_API Vec3<double>;


		//########################################################## inlines #############################################################################


		template<class T>
        Vec3<T>::Vec3(const Vec3<T>& v)
        {
            vec[0] = v.vec[0];
            vec[1] = v.vec[1];
            vec[2] = v.vec[2];
        }

		template<class T>
        Vec3<T>::Vec3(double xx, double yy, double zz)
		{
            vec[0] = static_cast<T>(xx);
            vec[1] = static_cast<T>(yy);
            vec[2] = static_cast<T>(zz);
        }

		template<class T>
        Vec3<T>::Vec3(float xx, float yy, float zz)
        {
            vec[0] = static_cast<T>(xx);
            vec[1] = static_cast<T>(yy);
            vec[2] = static_cast<T>(zz);
        }

		template<class T>
        Vec3<T>::Vec3(const double* p)
		{
            vec[0] = static_cast<T>(p[0]);
            vec[1] = static_cast<T>(p[1]);
            vec[2] = static_cast<T>(p[2]);
        }

		template<class T>
        Vec3<T>::Vec3(const float* p)
		{
            vec[0] = static_cast<T>(p[0]);
            vec[1] = static_cast<T>(p[1]);
            vec[2] = static_cast<T>(p[2]);
        }



		template<class T> inline
	    double Vec3<T>::dist(double xx, double yy, double zz) const
		{
			const double dx = xx - vec[0];
			const double dy = yy - vec[1];
			const double dz = zz - vec[2];
			return std::sqrt(dx*dx + dy*dy + dz*dz);
		}

		template<class T> inline
		double Vec3<T>::dist(const Vec3<T>& v) const
		{
			const double dx = v.x() - x();
			const double dy = v.y() - y();
			const double dz = v.z() - z();
			return std::sqrt(dx*dx + dy*dy + dz*dz);
		}

        template<class T> inline
        double Vec3<T>::dist2( const Vec3<T>& v ) const
        {
            const double dx = v.x() - x();
            const double dy = v.y() - y();
            const double dz = v.z() - z();
            return dx * dx + dy * dy + dz * dz;
        }

		template<class T> inline
		double Vec3<T>::length() const
		{
			return std::sqrt(dotProd(*this));
		}

		template<class T> inline 
        void Vec3<T>::setLength(double set)
		{
            const double ln = length();
            if (ln)
            {
                scale(set/ln);
            }
		}

		template<class T> inline
		void Vec3<T>::scale(double factor)
		{
            vec[0] = static_cast<T>(factor * vec[0]);
            vec[1] = static_cast<T>(factor * vec[1]);
            vec[2] = static_cast<T>(factor * vec[2]);
		}

		template<class T> inline
		Vec3<T> Vec3<T>::getScaled(double factor) const
		{
			return Vec3<T>(vec[0] * factor, vec[1] * factor, vec[2] * factor);
		}

		template<class T> inline
		double Vec3<T>::dotProd(const Vec3<T>& other) const
		{
			return vec[0]*other.vec[0] + vec[1]*other.vec[1] + vec[2] *other.vec[2];
		}

		template<class T> inline
		Vec3<T> Vec3<T>::crossProd(const Vec3<T>& other) const
		{
			Vec3<T> ret;
			ret.vec[0] = vec[1]*other.vec[2] - vec[2] *other.vec[1];
			ret.vec[1] = vec[2]*other.vec[0] - vec[0] *other.vec[2];
			ret.vec[2] = vec[0]*other.vec[1] - vec[1] *other.vec[0];
			return ret;
		}

		template<class T> inline
		double Vec3<T>::angle(const Vec3<T>& other) const
		{
			const Vec3<T> normedThis = getNormalized();
			const Vec3<T> normedOther = other.getNormalized();
			return acos(normedThis.dotProd(normedOther));
		}

		template<class T> inline
		double Vec3<T>::norm() const
		{
			return length();
		}

		template<class T> inline
		Vec3<T> Vec3<T>::getNormalized() const
		{
			Vec3<T> v(*this);
            v.normalize();
			return v;
		}

		template<class T> inline
        Vec3<T>& Vec3<T>::normalize()
		{
			setLength( (T)1 );
            return *this;
		}

		template<class T> inline
		bool Vec3<T>::isZero() const
		{
			return vec[0] == 0. && vec[1] == 0. && vec[2] == 0.;
		}


		template<class T> inline
		Vec3<T> Vec3<T>::operator - () const
		{
			return Vec3<T>(-vec[0], -vec[1], -vec[2]);
		}


		template<class T> inline
		Vec3<T>& Vec3<T>::operator += (const Vec3<T>& v)
		{
			vec[0] += v.vec[0];
            vec[1] += v.vec[1];
            vec[2] += v.vec[2];
			return *this;
		}

        template<class T> inline
        Vec3<T>& Vec3<T>::operator -= (const Vec3<T>& v)
        {
            vec[0] -= v.vec[0];
            vec[1] -= v.vec[1];
            vec[2] -= v.vec[2];
            return *this;
        }

        template<class T> inline
		Vec3<T>& Vec3<T>::operator /= (double fac)
		{
			vec[0] /= T(fac);
			vec[1] /= T(fac);
			vec[2] /= T(fac);
			return *this;
		}

        template<class T> inline
        Vec3<T>& Vec3<T>::operator *= (double fac)
        {
            vec[0] *= T(fac);
            vec[1] *= T(fac);
            vec[2] *= T(fac);
            return *this;
        }

        template<class T> inline
        Vec3<T> Vec3<T>::operator * (double fac) const
        {
            return Vec3<T>(vec[0] *fac, vec[1] *fac, vec[2] *fac);
        }

        template<class T> inline
        Vec3<T> Vec3<T>::operator / ( double div ) const
        {
            return Vec3<T>(vec[0] /div, vec[1] /div, vec[2] /div);
        }

        template<class T> inline
        Vec3<T> Vec3<T>::operator * ( const Vec3<T>& b ) const
        {
            return Vec3<T>(vec[0] *b.vec[0], vec[1] *b.vec[1], vec[2] *b.vec[2]);
        }

        template<class T> inline
        Vec3<T> Vec3<T>::operator + (const Vec3<T>& b) const
        {
            return Vec3<T>(vec[0] +b.vec[0], vec[1] +b.vec[1], vec[2] +b.vec[2]);
        }

        template<class T> inline
        Vec3<T> Vec3<T>::operator - (const Vec3<T>& rhs) const
        {
            return Vec3<T>(vec[0] - rhs.vec[0], vec[1] - rhs.vec[1], vec[2] - rhs.vec[2]);
        }


        template<class T> inline
		const T& Vec3<T>::operator [] (int idx) const
		{
            return vec[idx];
		}

		template<class T> inline
		T& Vec3<T>::operator [] (int idx)
		{
			return vec[idx];
		}


		template<class T> inline
		Vec3<T>& Vec3<T>::operator = (const Vec3<T>& v)
		{
            vec[0] = v.vec[0];
            vec[1] = v.vec[1];
            vec[2] = v.vec[2];
			return *this;
		}


		template<class T> inline
		bool Vec3<T>::operator == (const Vec3<T>& rhs) const
		{
            return
                   (fabs(vec[0] - rhs.vec[0]) < eps)
                && (fabs(vec[1] - rhs.vec[1]) < eps)
                && (fabs(vec[2] - rhs.vec[2]) < eps);
		}

		template<class T> inline
		bool Vec3<T>::operator != (const Vec3<T>& rhs) const
		{
			return !(*this == rhs);
		}


		//==========================================================================

		inline std::ostream& operator<<(std::ostream& stream, const Vec3d& rhs)
		{
			stream << "(" << rhs.x() << "|" << rhs.y() << "|" << rhs.z() << ")";
			return stream;
		}

		inline std::ostream& operator<<(std::ostream& stream, const Vec3f& rhs)
		{
			stream << "(" << rhs.x() << "|" << rhs.y() << "|" << rhs.z() << ")";
			return stream;
		}

        inline std::istream& operator>>(std::istream& stream, Vec3f& rhs)
        {
            std::cout << "Vector im Format (x|y|z) eingeben: ";
            float x=0, y=0, z=0;
            char c1 = 0, c2 = 0, c3 = 0, c4=0;
            if (!(stream >> c1 >> x >> c2 >> y >> c3 >> z >> c4))
                return stream;

            if (c1 != '(' || c2 != '|' || c3 != '|' || c4 != ')')
                stream.setstate(std::ios::failbit); //Fehler!
                                               // => fail-Bit setzen.

            rhs = Vec3f(x, y, z);

            return stream;
        }

        inline std::istream& operator>>(std::istream& stream, Vec3d& rhs)
        {
            std::cout << "Vector im Format (x|y|z) eingeben: ";
            double x = 0, y = 0, z = 0;
            char c1 = 0, c2 = 0, c3 = 0, c4 = 0;
            if (!(stream >> c1 >> x >> c2 >> y >> c3 >> z >> c4))
                return stream;

            if (c1 != '(' || c2 != '|' || c3 != '|' || c4 != ')')
                stream.setstate(std::ios::failbit); //Fehler!
                                               // => fail-Bit setzen.

            rhs = Vec3d(x, y, z);

            return stream;
        }

    }
}

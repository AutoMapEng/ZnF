#pragma once

#include <cmath>

#include <zfs/zfsapi.h>
#include "../zfvec.h"
#include "../../trmatrix.h"


namespace zfs {
    namespace math {

            template<class T> 
            class ZFQuaternion
            {
            public:
                T x;
                T y;
                T z;
                T w;

                ZFQuaternion();
                ZFQuaternion(const T& w, const T& x, const T& y, const T& z);
                ZFQuaternion(const T& roll, const T& pitch, const T& yaw);

                // operator overloading
                ZFQuaternion operator*(const ZFQuaternion&) const;
                ZFQuaternion operator+(const ZFQuaternion&) const;
                ZFQuaternion operator*(const T&) const;

                void operator=(const ZFTRMatrix&);


                ZFQuaternion conj() const;


                // setting
                void fromRPY(const T& roll, const T& pitch, const T& yaw);
                void fromAxisAndRotAngle(const T& axis_x, const T& axis_y, const T& axis_z, const T& phi);

                // enquiry
                void toRPY(T& roll, T& pitch, T& yaw) const;
                Vec3<T> toRPY() const;

                // rotation using quaternions
                void rotate(T& x, T& y, T& z) const;

            };

            //############################################################################################################################

            typedef ZFQuaternion<double> ZFQuaternionD;
            typedef ZFQuaternion<float> ZFQuaternionF;

            template class ZFS_API ZFQuaternion<double>;
            template class ZFS_API ZFQuaternion<float>;


            //############################################### inlines #############################################################################


            template<class T>
            ZFQuaternion<T>::ZFQuaternion()
                : x(0),
                y(0),
                z(0),
                w(1)
            {
            }

            template<class T> 
            ZFQuaternion<T>::ZFQuaternion(const T& w_, const T& x_, const T& y_, const T& z_) :
                x(x_),
                y(y_),
                z(z_),
                w(w_)
            {
            }

            template<class T>
            ZFQuaternion<T>::ZFQuaternion(const T& roll, const T& pitch, const T& yaw)
            {
                fromRPY(roll, pitch, yaw);
            }



            template<class T> inline
            ZFQuaternion<T> ZFQuaternion<T>::operator+(const ZFQuaternion& b) const
            {
                return ZFQuaternion(w + b.w, x + b.x, y + b.y, z + b.z);
            }

            template<class T> inline
            ZFQuaternion<T> ZFQuaternion<T>::operator*(const ZFQuaternion& b) const
            {
                T aw = w * b.w - x * b.x - y * b.y - z * b.z;
                T ax = w * b.x + x * b.w + y * b.z - z * b.y;
                T ay = w * b.y - x * b.z + y * b.w + z * b.x;
                T az = w * b.z + x * b.y - y * b.x + z * b.w;

                return ZFQuaternion(aw, ax, ay, az);
            }

            template<class T> inline
            ZFQuaternion<T> ZFQuaternion<T>::operator*(const T& factor) const
            {
                T aw = w * factor;
                T ax = x * factor;
                T ay = y * factor;
                T az = z * factor;

                return ZFQuaternion<T>(aw, ax, ay, az);
            }

            template<class T> inline
            void ZFQuaternion<T>::operator=(const ZFTRMatrix& m)
            {
                double qw;
                double qx;
                double qy;
                double qz;

                m.getQuaternion(qw, qx, qy, qz);

                w = (T)qw;
                x = (T)qx;
                y = (T)qy;
                z = (T)qz;
            }

            template<class T> inline
            ZFQuaternion<T> ZFQuaternion<T>::conj() const
            {
                return ZFQuaternion<T>(w, -x, -y, -z);
            }

            template<class T> inline
            void ZFQuaternion<T>::fromRPY(const T& roll, const T& pitch, const T& yaw)
            {
                ZFQuaternion<T> Qx(cos(roll / 2), sin(roll / 2), 0, 0);
                ZFQuaternion<T> Qy(cos(pitch / 2), 0, sin(pitch / 2), 0);
                ZFQuaternion<T> Qz(cos(yaw / 2), 0, 0, sin(yaw / 2));

                *this = Qz * Qy * Qx;
            }

            template<class T> inline
            void ZFQuaternion<T>::fromAxisAndRotAngle(const T& axis_x, const T& axis_y, const T& axis_z, const T& phi)
            {
                w = static_cast<T>(cos(phi / 2));

                double s_phi2 = sin(phi / 2);

                x = static_cast<T>(axis_x * s_phi2);
                y = static_cast<T>(axis_y * s_phi2);
                z = static_cast<T>(axis_z * s_phi2);
            }

            template<class T> inline
            void ZFQuaternion<T>::toRPY(T& roll, T& pitch, T& yaw) const
            {
                /*
                // cardan angles: roll <=> alpha_c, pitch <=> beta_c, yaw <=> gamma_c
                //
                // Rotation Matrix IR:
                //
                //       / r_11 r_12 r_13 \
                //  IR = | r_21 r_22 r_23 | = IR_z(gamma_c)* IR_y(beta_c) * IR_x(alpha_c)
                //       \ r_31 r_32 r_33 /
                //
                //
                // Quaternion given so that transform happens into outer ("earth fixed") system (see Bronstein, p. 307).
                */

                roll = atan2(2 * (w * x + y * z), 1 - 2 * (y * y + x * x)); // rot. about x-axis: tan ( alpha_c ) = r_32 / r_33
                pitch = asin(2 * (w * y - x * z));                        // rot. about y-axis: sin ( beta_c ) = -r_31
                yaw = atan2(2 * (w * z + y * x), 1 - 2 * (y * y + z * z)); // rot. about z-axis: tan ( gamma_c ) = r_21 / r_11
            }


            template<class T> inline
            void ZFQuaternion<T>::rotate(T& x, T& y, T& z) const
            {
                ZFQuaternion<T> qvec(0, x, y, z);

                qvec = (*this) * qvec * (*this).conj();

                x = qvec.x;
                y = qvec.y;
                z = qvec.z;
            }

            template<class T> inline
            Vec3<T> ZFQuaternion<T>::toRPY() const
            {
                Vec3<T> v;
                toRPY(v.x(), v.y(), v.z());
                return v;
            }



    } // end namespace math
} // end namespace zfs
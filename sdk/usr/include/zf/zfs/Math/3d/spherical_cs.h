#pragma once

#include <zfs/zfsapi.h>

#define SPHERICAL_LS // for LS Angles from Point

namespace zfs 
{
    namespace math {
        // ==================================================================================
        template<class T> class Point;
        template<class T> class Point3D;

        // ==================================================================================
        /*
        *  Class for getting spherical coordinates for a
        *  point given in rectlinear coordinates
        *
        *  template types:
        *
        * Due to type conversion of Point3D class, also, e.g. Point3D<int> can be transfered
        *
        */
        template<typename T> class ZFS_API SphericalCS
        {
        public:

            SphericalCS();

            /*
             *  aPoint can be (x,y,z): -> angular position can be obtained
             *  aPoint can also be ( r: radius, theta: azimuth angle/ "longitude" , phi: elevation angle / "latitude" ):
             *
             */
            SphericalCS(const Point3D<T>& aPoint);

            T Radius() const;
            T Norm() const;

            // longitudinal
            T AzimuthAngle(const bool b2PI = false) const; //[-PI,PI[

            // latitudinal, elevation angles
            T PolarAngle() const;  // [0, PI] from Zenith
            T OffNadirAngle() const; // [0, PI] from Nadir
            T OffNadirAngle2PI() const;// [0, 2*PI] from Nadir
            T EquatorialAngle() const; //  [-PI, PI]

            const Point<T> AnglePosPolar() const;
            const Point<T> AnglePosOffNadir() const;
            const Point<T> AnglePosOffNadir2PI() const;
            const Point<T> AnglePosEquatorial() const;

            const Point3D<T> cartesian() const;
            // polar point
            // ( 
            //   r    : radius          / distance to center, [0, oo[
            //   theta: azimuth angle   / "longitude"       , [0,2*PI[ , 
            //   phi  : elevation angle / "latitude"        , [-PI/2,PI/2] 
            // )
            const Point3D<T> polar() const;

#ifdef SPHERICAL_LS
            // LS looking angles
            // 1) LS rotates clockwise
            // 2) LS CO-S rotated by PI/2 in counter-clockwise direction
            T AzimuthAngleLS2PI() const; // [0, 2PI[
            T OffNadirAngleLSPI() const; // [0, PI [
            const Point<T> AnglePosLS() const;
#endif

            const Point3D<T> getPoint() const;

        private:
            // -- constants to avoid casting -----------------------------------------------
            static const T    c_pi; // PI   
            static const T c_pi_d2; // PI/2 
            static const T  c_2_pi; // 2*PI 

            // -- storage for point --------------------------------------------------------
            const Point3D<T> Zero;         // for standard constructor
            const Point3D<T> UsedPoint;

            // -- for storage of repeatedly used values ------------------------------------
            T                   d_xy;

            // -- internal functions -------------------------------------------------------

            T calcDXY();

        };

        // ==================================================================================
    } // end namespace math
} // end namespace zfs
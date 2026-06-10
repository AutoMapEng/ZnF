#pragma once

#include <zfs/zfsapi.h>

#include <vector>

#include <zfs/Math/math/polynomial.h>

namespace zfs {
    namespace math {

        /** \class PTI
         * \brief  Class for interpolating tracks (polynomial based) given in x (can be time) and y.
         *
         * \author Juergen Holzner
         *
         * Class for interpolating tracks (polynomial based) given in x (can be time) and y.
         *
         */

        class ZFS_API PTI
        {
        public:
            PTI();
            PTI(const std::vector<double>& x, const std::vector<double>& y, int order);
            ~PTI();

            double at(double x);
            double derivative(double x);
            void   initialize(const std::vector<double>& x, const std::vector<double>& y, int order);

            Polynomial getTrackPolynomial() const;

        private:

            int                 m_order;          // Polynomial order of used polynomial
            std::vector<double> m_x;              // vector of x values
            std::vector<double> m_y;              // vector of y values
            Polynomial          m_TrackPolynomial; // Polynomial representation of track

        };

        struct ZFS_API PTIPt
        {
            double x, y, z;
            double t;

            PTIPt() { x = y = z = 0; t = 0; }
            PTIPt(double _x, double _y, double _z, double _t = 0.) { x = _x; y = _y; z = _z; t = _t; }
        };

        typedef std::vector<PTIPt> PTI_points_t;


        struct ZFS_API PTIPt2
        {
            double x, y, z;
            double vx, vy, vz;
            double t;
            PTIPt2() { x = y = z = 0; vx = vy = vz = 0; t = 0; }
            PTIPt2(double _x, double _y, double _z,
                double _vx, double _vy, double _vz, double _t = 0.) {
                x = _x; y = _y; z = _z;
                vx = _vx; vy = _vy; vz = _vz; t = _t;
            }
        };

        typedef std::vector<PTIPt2> PTI_points2_t;

        /** \class XYZPTI
         * \brief  Class for interpolating tracks (polynomial based) given in three dimensions.
         *
         * \author Juergen Holzner
         *
         * Class for interpolating tracks (polynomial based) given in three dimensions.
         * Points have (x,y,z,t).
         *
         */

        class ZFS_API XYZPTI
        {
        public:
            // initialization
            XYZPTI();
            XYZPTI(const PTI_points_t& points, int n_order);

            void setup(const PTI_points_t& points, int n_order);

            // interpolates position
            PTIPt interpolate(double t);
            PTIPt at(double t);

            // interpolates position and direction
            PTIPt2 interpolate2(double t);
            PTIPt2 at2(double t);

        private:
            // order of fit polynomial
            int m_order;
            // number of data points
            int m_n;
            // members for every dimension
            PTI m_PTI_x;
            PTI m_PTI_y;
            PTI m_PTI_z;
        };

        void ZFS_API PTI_test();
        
    } // end namespace math
} // end namespace zfs
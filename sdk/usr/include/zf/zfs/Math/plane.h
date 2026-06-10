#pragma once

#include <cstdint>
#include <cmath>

#include <utility>
#include <vector>

#include "../zfsapi.h"

#include "zfvec.h"
#include "line.h"
#include "stl/vector_op.h"

namespace zfs {
    namespace math
    {
        class Line3d;


        /**
        \brief hesse form of plane
        */
        class ZFS_API Plane
        {
        public:
            Vec3d nv;   ///< Normalenvector, must have length 1
            Vec3d p0;   ///< offspring
            double d0;  ///< Distance to offspring, this has negative values when the offspring is on the negative side of nv.
            
            Vec3d t1, t2; /// < plane tangents

            Plane() : d0(0.) {}
            Plane(const Vec3d& nv_, const Vec3d& p0_)
                : nv(nv_), p0(p0_)
            {
                calculateD0();
                calculatePlaneTangents();
            }

            bool isValid() const { return !nv.isZero() || !p0.isZero() || d0!=0.; }

            Vec3d get_normal() const { return nv; }
            Vec3d get_t1() const { return t1; }
            Vec3d get_t2() const { return t2; }
            Vec3d get_p0() const { return p0; }


            double z_from_xy(const double& x, const double& y) const;
            Vec3d project_point_onto_plane(const Vec3d& pt) const;
            double dist(const zfs::math::Vec3d& v) const; /// < distance of point to plane


            // return the intersection point of the line 
            // first=true if intersect 
            // second=the intersection point
            std::pair<bool, Vec3d> intersect(const Vec3d& point0OfLine, const Vec3d& point1OfLine ) const;
            std::pair<bool, Vec3d> intersect(const Line3d& ) const;


            // calculate or recalculate the perpendicular distance to origin
            void calculateD0();
            bool calculatePlaneTangents();


        };

        //##############################################################################

        inline double Plane::dist(const zfs::math::Vec3d& v) const
        {
            return nv.x() * v.x() + nv.y() * v.y() + nv.z() * v.z()+ d0;
        }
    
        inline void Plane::calculateD0()
        {
            d0 = - (p0.x() * nv.x() + p0.y() * nv.y() + p0.z() * nv.z());

            if (d0 < 0)
            {
                d0 = -d0;
                nv = -nv;
            }
        }

        inline bool Plane::calculatePlaneTangents()
        {
            if (nv == Vec3d(0)) return false;

            std::vector<double> abs_nv = zfs::math::abs(nv.getValue(),3);
            std::vector<int> indices = zfs::math::stable_sort_indices(abs_nv, "descend"); // sort: if elements are equal, the are sorted in order of appearance

            t1[indices[0]] = -nv[indices[1]];
            t1[indices[1]] = nv[indices[0]];
            t1[indices[2]] = 0;

            t1.normalize();

            t2 = nv.crossProd(t1);

            return true;
        }

        inline std::pair<bool, zfs::math::Vec3d> Plane::intersect(const Line3d& line) const
        {
            return intersect(line.point, line.point + line.vec);
        }

        inline std::pair<bool, zfs::math::Vec3d> Plane::intersect(const Vec3d& pt0, const Vec3d& pt1) const
        {
            const double epsilon = 1e-6;
            Vec3d u = pt1 - pt0;
            const double dot = nv.dotProd(u);
            if (fabs(dot) > epsilon)
            {
                const Vec3d w = pt0 - p0;
                const double fac = -nv.dotProd(w) / dot;
                u *= fac;
                return std::pair<bool, zfs::math::Vec3d>(true, Vec3d(pt0 + u));
            }
            else
            {
                return std::pair<bool, zfs::math::Vec3d>(false, Vec3d());
            }
        }

        inline Vec3d Plane::project_point_onto_plane(const Vec3d& pt) const
        {
            double d = nv.dotProd(pt - p0);
            return pt - nv * d;
        }

        inline double Plane::z_from_xy(const double& x, const double& y) const
        {
            double z{ 0 };

            if (nv[2] != 0)
                z = -(nv[0] * x + nv[1] * y + d0) / nv[2];

            return z;
        }

    }
}

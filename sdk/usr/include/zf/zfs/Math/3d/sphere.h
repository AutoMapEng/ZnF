#pragma once
#include <utility>
#include "../zfsapi.h"
#include "../zfvec.h"

namespace zfs {
	namespace math
	{
        /**
        \brief Sphere
        */
        class ZFS_API Sphere
        {
        public:
            Sphere() {}
            Sphere(const Vec3d& center, double radius ) : center_(center), radius_(radius) {}
            Sphere(double x, double y, double z, double radius) : center_(x,y,z), radius_(radius) {}

            /// distance of point to surface
            double dist2Surface(const Vec3d&);

            double radius() const { return radius_; }
            Vec3d centerPoint() const { return center_; }

        private:
            Vec3d center_;
            double radius_ = 0;
        };
	}
}

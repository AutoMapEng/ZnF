#pragma once
#include "zfsapi.h"

#include "trmatrix.h"
#include "Math/zfvec.h"

#include <vector>


namespace zfs
{
	struct ZFLine3d;

	//#######################################################################################################################
	/**
	\brief Defines a plane in 3D-space.

	The plane is defined in hesse-form.

	*/
	class ZFS_API ZFHessePlane
	{
		math::Vec3d planeNormal;
		double minusd0{};

	public:
		ZFHessePlane() = default;
		ZFHessePlane(const math::Vec3d& pointOnPlane, const math::Vec3d& planeNormal);
		ZFHessePlane(const std::vector<zfs::math::Vec3d>& pts) { approxHessePlane(pts); }
		ZFHessePlane(double* old) { if (old) { minusd0 = old[0];  planeNormal = { old[1],old[2],old[3] }; } } // interface for old structure
		ZFHessePlane(const ZFHessePlane& p) { *this = p; }

		// return a vector for old style functions
		std::vector<double> getOldStyle() const { return { minusd0, planeNormal.x(), planeNormal.y(), planeNormal.z() }; }

		/**
		 * Check length.
		 * 
		 * \return true if plane has a length
		 */
		bool isValid() const;

		// create a hesse plane using at least 3 points
		bool approxHessePlane(const std::vector<zfs::math::Vec3d>& pts);

		math::Vec3d getNormal() const { return planeNormal; }
		void setNormal(math::Vec3d normal);
		
		double getDistanceToNull() const { return -minusd0; }

		double pointToPlaneDistance(const math::Vec3d& point) const;

		bool pointIsOnPlane(const math::Vec3d& point) const;

		math::Vec3d projectPointToPlane(const math::Vec3d& point) const;

		ZFLine3d intersectWithOther(const ZFHessePlane& other) const;

		/**
		for the formula see http://en.wikipedia.org/wiki/Line-plane_intersection
		*/
		math::Vec3d intersectWithLine(const ZFLine3d& line) const;

		/**
		\brief Finds the parameters of the plane hesse for the form f(x,y) = a*x + b*y + c
		Keep in mind that this will give no results for vertical planes.
		*/
		void getParameterForm(double& a, double& b, double& c);
		math::Vec3d getParameterForm() { math::Vec3d v; getParameterForm(v.x(), v.y(), v.z()); return v; }

		ZFTRMatrix createTRMatrixSystem(const math::Vec3d& systemCenter) const;

		bool cut_pointnormal_onto_plane( zfs::math::Vec3d& Point) const;

	private:
//		void computeMinusD();
	};


	// utilities


	/**
	The returned matrix contains the axis of a coordinate system which has its z-axis coincide with the
	plane normal. The x-axis of the system will point in the direction of preferredXDirection.
	The translation of the matrix is defined by systemCenter.

	If the preferredXDirection is parallel to the plane normal a random x direction for the plane vector will be chosen.
	*/
	ZFS_API ZFTRMatrix createPlaneSystem(const ZFHessePlane& plane, const math::Vec3d& systemCenter, math::Vec3d preferredXDirection = math::Vec3d(1., 0., 0.));
}   // namespace

#ifndef ZFS_BOUNDINGBOX3D_H
#define ZFS_BOUNDINGBOX3D_H

#include "zfsapi.h"

#include "zfvector.h"

class ZFScan;
class ZFSLinebyline;
class ZFSHeader;

namespace zfs
{
	class BoundingBox3D;



	/**
	@brief Calculates the 3D bounding box of the scan
	*/
	ZFS_API BoundingBox3D boundingBoxCalculate(ZFScan*, int subsampling);
	ZFS_API BoundingBox3D boundingBoxCalculate(ZFSLinebyline*, int subsampling);

	/**
	@brief Set / Get bounding box from zfs-header
	*/
	ZFS_API BoundingBox3D getBoundingBoxFromZFSHeader(ZFSHeader&);
	ZFS_API void setBoundingBox2ZFSHeader(const BoundingBox3D&, ZFSHeader&);


	//===================================================================================

	/**
	\brief	3D Bounding box. Holds the 3D limits of a pointcloud in 3D space
	*/
	class ZFS_API BoundingBox3D
	{
	protected:
		double m_minX;	///< minimum X value in 3D space
		double m_maxX;	///< mamimal X value in 3D space
		double m_minY;	///< minimum Y value in 3D space
		double m_maxY;	///< maximal Y value in 3D space
		double m_minZ;	///< minimum Z value in 3D space
		double m_maxZ;	///< maximal Z value in 3D space

		int m_subsampleUsed;	///< subsampling used for calculating bounding box

	public:
		BoundingBox3D() : m_minX(0.), m_maxX(0.), m_minY(0), m_maxY(0.), m_minZ(0.), m_maxZ(0.), m_subsampleUsed(1) {}

		/// check if point is in the box
		bool isIn(double x, double y, double z)
		{
			return x >= m_minX && x <= m_maxX && y >= m_minY && y <= m_maxY && z >= m_minZ && z <= m_maxZ;
		}
		bool isIn(const ZFVec3d& v)
		{
			return isIn(v.x(), v.y(), v.z());
		}

		/// increase limits depending on new point
		void setBox(double x, double y, double z)
		{
			if (x < m_minX) m_minX = x;
			else if (x > m_maxX) m_maxX = x;

			if (y < m_minY) m_minY= y;
			else if (y > m_maxY) m_maxY= y;

			if (z < m_minZ) m_minZ = z;
			else if (z > m_maxZ) m_maxZ= z;
		}
		void setBox(const ZFVec3d& v)
		{
			setBox(v.x(), v.y(), v.z());
		}

		int subsamplingUsed() const { return m_subsampleUsed; }

		ZFVec3d getCenter();

		double xMin() const { return m_minX; }	///< get minimal x in 3D space
		double xMax() const { return m_maxX; }	///< get maximal x in 3D space
		double yMin() const { return m_minY; }	///< get minimal y in 3D space
		double yMax() const { return m_maxY; }	///< get maximal y in 3D space
		double zMin() const { return m_minZ; }	///< get minimal z in 3D space
		double zMax() const { return m_maxZ; }	///< get maximal z in 3D space

		void setSubsamplingUsed(int sub) { m_subsampleUsed = sub; }
		void setXMin(double v) { m_minX = v; }
		void setXMax(double v) { m_maxX = v; }
		void setYMin(double v) { m_minY = v; }
		void setYMax(double v) { m_maxY = v; }
		void setZMin(double v) { m_minZ = v; }
		void setZMax(double v) { m_maxZ = v; }
	};

	//===================================================================================
	


} // end namespace
#endif // ZFS_BOUNDINGBOX3D_H

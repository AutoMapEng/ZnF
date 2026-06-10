#pragma once
#include "../zfsapi.h"
#include "zfvec.h"
#include "../trmatrix.h"
#include <algorithm>

namespace zfs {
	namespace math
	{
        /**
        \brief	3D Bounding box. Holds the 3D limits of a pointcloud in 3D space
        */
        class ZFS_API Box3D
        {
        protected:
            double m_minX;	///< minimum X value in 3D space
            double m_maxX;	///< mamimal X value in 3D space
            double m_minY;	///< minimum Y value in 3D space
            double m_maxY;	///< maximal Y value in 3D space
            double m_minZ;	///< minimum Z value in 3D space
            double m_maxZ;	///< maximal Z value in 3D space

            ZFTRMatrix rot;

        public:
            Box3D() : m_minX(0.), m_maxX(0.), m_minY(0), m_maxY(0.), m_minZ(0.), m_maxZ(0.) {}
            Box3D(const Vec3d& v) { setCenter(v); }

            void setMaxExpanding();

            // check whether box is zero
            bool empty() const { return m_minX == 0 && m_maxX == 0 && m_minY == 0 && m_maxY == 0 && m_minZ == 0 && m_maxZ == 0; }

            /// check if point is in the box
            bool isIn(double x, double y, double z) const
            {
                return x >= m_minX && x <= m_maxX && y >= m_minY && y <= m_maxY && z >= m_minZ && z <= m_maxZ;
            }
            bool isIn(const Vec3d& v) const 
            {
                return isIn(v.x(), v.y(), v.z());
            }

            /// Rotate input point and check if point is in the box
            bool rotateAndCheck(const Vec3d& v, const ZFTRMatrix& m, const Vec3d& offset) const
            {
                const auto rotated_pt = m.GetRotate(v - offset) + offset;
                return isIn(rotated_pt.x(), rotated_pt.y(), rotated_pt.z());
            }

            void setRotation(const ZFTRMatrix& m)
            {
                rot = m;
                rot.ResetTranslation();
            }

            const ZFTRMatrix& getMatrix() const { return rot; }
            ZFTRMatrix& getMatrix() { return rot; }

            void setCenter(const Vec3d& v)
            {
                m_minX = m_maxX = v.x();
                m_minY = m_maxY = v.y();
                m_minZ = m_maxZ = v.z();
            }
            void set(const Vec3d& center, const Vec3d& width )
            {
                m_minX = center.x() - width.x() / 2.;
                m_maxX = center.x() + width.x() / 2.;
                m_minY = center.y() - width.y() / 2.;
                m_maxY = center.y() + width.y() / 2.;
                m_minZ = center.z() - width.z() / 2.;
                m_maxZ = center.z() + width.z() / 2.;
            }

            void expand(double x, double y, double z)
            {
                expandX(x);
                expandY(y);
                expandZ(z);
            }
            void expand(const Vec3d& v)
            {
                expandX(v.x());
                expandY(v.y());
                expandZ(v.z());
            }
            void expandX(double x)
            {
                m_minX -= x;
                m_maxX += x;
            }
            void expandY(double y)
            {
                m_minY -= y;
                m_maxY += y;
            }
            void expandZ(double z)
            {
                m_minZ -= z;
                m_maxZ += z;
            }

            /// increase limits depending on new point
            void increase(const Vec3d& v)
            {
                if (v.x() < m_minX) m_minX = v.x();
                else if (v.x() > m_maxX) m_maxX = v.x();

                if (v.y() < m_minY) m_minY = v.y();
                else if (v.y() > m_maxY) m_maxY = v.y();

                if (v.z() < m_minZ) m_minZ = v.z();
                else if (v.z() > m_maxZ) m_maxZ = v.z();
            }

            zfs::math::Vec3d getMin() const { return { m_minX, m_minY, m_minZ }; }
            zfs::math::Vec3d getMax() const { return { m_maxX, m_maxY, m_maxZ }; }

            Vec3d getCenter() const;

            double xMin() const { return m_minX; }	///< get minimal x in 3D space
            double xMax() const { return m_maxX; }	///< get maximal x in 3D space
            double yMin() const { return m_minY; }	///< get minimal y in 3D space
            double yMax() const { return m_maxY; }	///< get maximal y in 3D space
            double zMin() const { return m_minZ; }	///< get minimal z in 3D space
            double zMax() const { return m_maxZ; }	///< get maximal z in 3D space

            void setMin(const zfs::math::Vec3d& v) { m_minX = v.x(); m_minY = v.y(); m_minZ = v.z(); }
            void setMax(const zfs::math::Vec3d& v) { m_maxX = v.x(); m_maxY = v.y(); m_maxZ = v.z(); }
            void setXMin(double v) { m_minX = v; }
            void setXMax(double v) { m_maxX = v; }
            void setYMin(double v) { m_minY = v; }
            void setYMax(double v) { m_maxY = v; }
            void setZMin(double v) { m_minZ = v; }
            void setZMax(double v) { m_maxZ = v; }
            void setMinMax(double mi, double mx) { m_minX = m_minY = m_minZ = mi; m_maxX = m_maxY = m_maxZ = mx; }

            void setWidthX(double w) { const auto center = (m_minX + m_maxX) / 2.; m_minX = center - w / 2.; m_maxX = center + w / 2.;  }
            void setWidthY(double w) { const auto center = (m_minY + m_maxY) / 2.; m_minY = center - w / 2.; m_maxY = center + w / 2.; }
            void setWidthZ(double w) { const auto center = (m_minZ + m_maxZ) / 2.; m_minZ = center - w / 2.; m_maxZ = center + w / 2.; }

            double widthX() const { return fabs(m_maxX - m_minX); }
            double widthY() const { return fabs(m_maxY - m_minY); }
            double widthZ() const { return fabs(m_maxZ - m_minZ); }

            zfs::math::Vec3d width() const { return { widthX(), widthY(), widthZ() }; }
            zfs::math::Vec3d extended() const { return { widthX()/2., widthY()/2., widthZ()/2. }; }

            double minWidth() const { return (std::min)((std::min)(widthX(), widthY()), widthZ()); }
            double maxWidth() const { return (std::max)((std::max)(widthX(), widthY()), widthZ()); }

            void move(const Vec3d& m)
            {
                m_minX += m.x();
                m_maxX += m.x();
                m_minY += m.y();
                m_maxY += m.y();
                m_minZ += m.z();
                m_maxZ += m.z();
            }
        };


        //####################################################################################################################

        inline Vec3d Box3D::getCenter() const
        {
            Vec3d vec;
            vec.x() = (m_minX + m_maxX) / 2.;
            vec.y() = (m_minY + m_maxY) / 2.;
            vec.z() = (m_minZ + m_maxZ) / 2.;
            return vec;
        }

    }
}

#pragma once

#include <list>
#include <vector>
#include <zfs/Math/3d/point_nd.h>

namespace zfs {
    namespace math {
        // ============================================================================================================
        // typedefs
        // ============================================================================================================
        typedef std::list< PointND<double, 3> >   Double3DPointListType;
        // ============================================================================================================
        typedef std::vector< PointND<double, 3> > Double3DPointVectorType;
        // ============================================================================================================
        bool pointInPointListWithDistanceSmaller(const PointND<double, 3>& Point, const Double3DPointListType& PointList, const double& MaxDistance);
        bool pointInPointListWithDistanceSmaller(const PointND<double, 3>& Point, const Double3DPointVectorType& PointList, const double& MaxDistance);
        // ============================================================================================================
    } // end namespace math
} // end namespace zfs
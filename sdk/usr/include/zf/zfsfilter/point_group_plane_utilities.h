#include "filterapi.h"

#include <zfs/Math/math/point.h>

namespace zfsfilter
{
        void   FILTERDLL_API FitPlaneToPointGroupDataFeature(const zfmath::PointGroupDataType& a_PointGroupData, double coefficients[], double * const pHessePar, bool b_stddev, double& stddev);
        void   FILTERDLL_API FitLineToPointGroupDataFeature(const zfmath::PointGroupDataType& a_PointGroupData, double dir_vector[], double line_ref_point[], bool b_stddev, double& stddev);
        void   FILTERDLL_API MeanStddevPlaneFit(const double* const p_coefs, const zfmath::IntPointListType& point_list, const zfmath::DoubleListType& distance_list, double& mean_plane_fit, double stdev_plane_fit);
        double FILTERDLL_API PlaneValue(const double* const& coefficients, const double& x, const double& y);
}
#pragma once

#include <zfs/zfsapi.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>

#include <zfs/Math/3d/vector_nd.h>
#include "track_point.h"

namespace zfs {
    namespace math {
        // ===========================================================================================================
        /** \class CubicSplineInterpolation
         * \brief  Class providing functionality for interpolation with cubic splines.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for interpolation with cubic splines.
         *
         */
         // ===========================================================================================================
        class ZFS_API CubicSplineInterpolation
        {
        public:

            enum CubicSplineType { NATURAL, BOUNDARY_DERIVATIVES_KNOWN }; // for NATURAL: second derivatives for interpolated samples are assumed 0 at boundaries.

            CubicSplineInterpolation();
            CubicSplineInterpolation(const std::vector<double>& x, const std::vector<double>& y, const CubicSplineType aSplineType = NATURAL);

            void   initialize(const std::vector<double>& x, const std::vector<double>& y, const CubicSplineType aSplineType);

            // 1) when this function is not used when spline type was set to BOUNDARY_DERIVATIVES_KNOWN, interpolation is not done (0 is returned always)
            // 2) this function has no effect when spline type was set to NATURAL
            void setBoundaryDerivatives(const double& first, const double& last);

            double at(const double& xi) const;
            double interpolate(const double& xi) const;
            double derivative(const double& xi) const;

            double operator[](const double& xi) const;

            // object status
            bool DataSet() const;
            bool SplineSet() const;

            // access to reference points
            double GetRefX(const size_t position) const;
            double GetRefY(const size_t position) const;

            size_t GetNumberOfRefPoints() const;

            size_t size() const;

            double xAt(size_t idx) const { return idx < m_x.size() ? m_x[idx] : 0.; }
            double yAt(size_t idx) const { return idx < m_y.size() ? m_x[idx] : 0.; }

        private:
            // numerical recipes code
            void setup_spline(const double& yp1, const double& ypn);

            bool DataOK(const std::vector<double>& x, const std::vector<double>& y) const;

            bool                m_data_set;
            bool                m_spline_set;
            size_t              m_n, m_n_1, m_n_2;
            CubicSplineType     m_spline_type;
            std::vector<double> m_x, m_y, m_y2; // m_y2: vector for the second derivative, calculated by spline().

        };
        // ===========================================================================================================
        /** \class XYZCubicSplineInterpolation
         * \brief  Class providing functionality for cubic splines in 3D (representation, generation, derivation).
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for cubic splines in 3D (representation, generation, derivation).
         *
         */
         // ===========================================================================================================
        class ZFS_API XYZCubicSplineInterpolation
        {
        public:
            // initialization
            XYZCubicSplineInterpolation();
            XYZCubicSplineInterpolation(const track_sample_points_t& points, CubicSplineInterpolation::CubicSplineType spline_type = CubicSplineInterpolation::NATURAL);

            void setup(const track_sample_points_t& points, CubicSplineInterpolation::CubicSplineType spline_type = CubicSplineInterpolation::NATURAL);

            void setBoundaryDerivatives(const Vector3D<double>& first, const Vector3D<double>& last);

            bool DataOK() const;

            /// get number of items
            std::size_t size() const;
            /// get min max for value to call
            std::pair<double, double> getMinMax() const;

            // interpolates position
            const TrackInterpolationPt interpolate(const double& t) const;
            const TrackInterpolationPt at(const double& t) const;
            const TrackInterpolationPt operator[](const double& t) const;

            // interpolates position and direction
            const TrackInterpolationPt2 interpolate2(const double& t) const;
            const TrackInterpolationPt2 at2(const double& t) const;

        private:

            // CubicSpline members for every dimension
            CubicSplineInterpolation  m_spline_x;
            CubicSplineInterpolation  m_spline_y;
            CubicSplineInterpolation  m_spline_z;

        };


        class ZFS_API XYZCubicSplineInterpolation4
        {
        public:
            // initialization
            XYZCubicSplineInterpolation4();

            void setup(const track_sample_points4_t& points, CubicSplineInterpolation::CubicSplineType spline_type = CubicSplineInterpolation::NATURAL);

            // interpolates position
            const TrackInterpolationPt4 interpolate(const double& t) const;


        private:
            CubicSplineInterpolation  m_a;
            CubicSplineInterpolation  m_b;
            CubicSplineInterpolation  m_c;
            CubicSplineInterpolation  m_d;
        };









        // ===========================================================================================================
        void ZFS_API spline_test();
        // ===========================================================================================================
    } // end namespace math
} // end namespace zfs
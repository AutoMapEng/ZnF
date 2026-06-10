#pragma once

#include <zfs/zfsapi.h>

#include <cstddef>
#include <cmath>
#include <vector>
#include <utility>

#include "track_point.h"

namespace zfs {
    namespace math {

        // =======================================================================================================
        /** \class LinearTrackInterpolation
         * \brief  Linear interpolation of tracks (some robustness, speed advantages).
         *
         * \author Juergen Holzner
         *
         * Linear interpolation of tracks (some robustness, speed advantages).
         *
         */
         // =======================================================================================================
        class ZFS_API LinearTrackInterpolation
        {
        public:
            LinearTrackInterpolation();
            LinearTrackInterpolation(const std::vector<double>& x, const std::vector<double>& y);

            void   initialize(const std::vector<double>& x, const std::vector<double>& y);

            bool DataOK() const
            {
                return m_data_set;
            }

            size_t size() const;


            double interpolate(const double& x) const;
            double interpolate(double x, std::size_t lo, std::size_t hi) const;

            double at(const double& x) const
            {
                return interpolate(x);
            }
            double operator[](const double& x) const
            {
                return interpolate(x);
            }

            // take care that the interpolation is a 360deg angle
            double interpolate360deg(const double& x, std::size_t lo, std::size_t hi) const;

            double derivative(const double& x) const;

            /// get the low and high bounds
            bool lohi(double x, std::size_t& lo, std::size_t& hi) const;

            double xAt(size_t idx) const { return idx < m_x.size() ? m_x[idx] : 0.; }
            double yAt(size_t idx) const { return idx < m_y.size() ? m_x[idx] : 0.; }

        protected:

            bool                m_data_set;

            size_t              m_n, m_n_1; // number of samples
            std::vector<double> m_x;        // vector of x values
            std::vector<double> m_y;        // vector of y values

            bool DataOK(const std::vector<double>& x, const std::vector<double>& y) const;
        };
        // =======================================================================================================
        class ZFS_API XYZLinearTrackInterpolation
        {
        public:
            // initialization
            XYZLinearTrackInterpolation();
            XYZLinearTrackInterpolation(const track_sample_points_t& points);
            virtual ~XYZLinearTrackInterpolation() = default;


            void setup(const track_sample_points_t& points);

            size_t size() const;

            std::pair<double, double> getMinMax() const
            {
                if (size())
                    return { m_lti_x.xAt(0), m_lti_x.xAt(m_lti_x.size() - 1) };
                return {};
            }

            // interpolates position
            virtual TrackInterpolationPt interpolate(const double& t) const;
            TrackInterpolationPt at(const double& t) const { return interpolate(t); }

            // interpolates position and direction
            TrackInterpolationPt2 interpolate2(const double& t);
            TrackInterpolationPt2 at2(const double& t);

        protected:

            // CubicSpline members for every dimension
            LinearTrackInterpolation m_lti_x;
            LinearTrackInterpolation m_lti_y;
            LinearTrackInterpolation m_lti_z;

        };

        class ZFS_API XYZLinearTrackInterpolation360deg
            : public XYZLinearTrackInterpolation
        {
        public:
            // initialization
            XYZLinearTrackInterpolation360deg() : XYZLinearTrackInterpolation() {}
            XYZLinearTrackInterpolation360deg(const track_sample_points_t& points)
                : XYZLinearTrackInterpolation(points) {}

            // interpolates position
            TrackInterpolationPt interpolate(const double& t) const override;

        };


        class ZFS_API XYZLinearTrackInterpolation4
        {
        public:
            // initialization
            XYZLinearTrackInterpolation4();

            void setup(const track_sample_points4_t& points);

            // interpolates position
            TrackInterpolationPt4 interpolate(const double& t) const;


        private:
            LinearTrackInterpolation m_a;
            LinearTrackInterpolation m_b;
            LinearTrackInterpolation m_c;
            LinearTrackInterpolation m_d;
        };

        // =======================================================================================================
        void ZFS_API lti_test();
        // =======================================================================================================
    } // end namespace math
} // end namespace zfs

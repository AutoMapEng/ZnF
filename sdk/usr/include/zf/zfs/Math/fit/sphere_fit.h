#pragma once

#include <cstddef>
#include <vector>

#include <zfs/zfsapi.h>
#include <zfs/Math/3d/point_nd.h>
#include "fitting.h"

namespace zfs { 
    namespace math {

        /** \class ZFSphereFit
        * \brief  Fit of sphere parameters [sphere center position (x0,y0,z0) and radius R] given a set of points [x,y,z]
        *
        * \author Juergen Holzner
        *
        * Fit of sphere parameters [sphere center position (x0,y0,z0) and radius R] given a set of points [x,y,z]
        *
        */

        class ZFS_API ZFSphereFit
        {

        public:

            // constructors -------------------------------------------------------------------------------------
            ZFSphereFit();
            ZFSphereFit(std::vector<double>& a_PosX,
                std::vector<double>& a_PosY,
                std::vector<double>& a_PosZ,
                std::vector<double>& a_PosSig);

            ZFSphereFit(const pointsd_t& aPoints, const std::vector<double>& aDistanceStdDev);
            ~ZFSphereFit();

            // -- accessors for result --------------------------------------------------------------------------
            PointND<double, 3> getSphereCenter() const;
            PointND<double, 3> getSphereCenterStdDev() const;

            double                     getSphereRadius() const;
            double                     getSphereRadiusStdDev() const;
            double                     getSphereRadiusStdDevData() const;

            bool                       InitOK() const;
            bool                       FitOK() const;

        private:
            // -- private class members -----------------------------------------------------------------------------------

            static const bool m_c_bEstimatePositionIndependent;

            // -- number of spherical parameters
            const int                  m_nSphereParam;

            // -- class state
            bool m_bInitializationOK;
            bool m_bFitOK;
            bool m_bDataExternal;

            // -- input data size
            int                        m_nData;

            // -- result parameter
            double                     m_SphereRadius;
            double                     m_SphereRadiusStdDev;
            double                     m_SphereRadiusStdDevData;

            PointND<double, 3> m_PositionMean;
            PointND<double, 3> m_SphereCenter;
            PointND<double, 3> m_SphereCenterStdDev;

            // -- pointers to input data
            std::vector<double>* m_pPosX;
            std::vector<double>* m_pPosY;
            std::vector<double>* m_pPosZ;
            std::vector<double>* m_pPosSig;

            // -- private class member functions for fit -------------------------------------------------------
            void fit();

            // -- helper functions
            void Free();

            std::vector<double> calcBVector() const;
            std::vector<double> IndexVector() const;

            void setParameter(const std::vector< long double>& aParamVector,
                const std::vector< long double>& aCoVarVector);

            double calcRadius(const std::vector< long double>& aParamVector) const;
            double calcStdDevRadius(const PointND<double, 3>& aSphereCenterStdDev, const std::vector<long double>& aParamVar) const;
            double calcStdDevRadiusData() const;

            PointND<double, 3> calcSphereCenter(const std::vector< long double>& aParamVector) const;
            PointND<double, 3> calcSphereCenterStdDev(const std::vector< long double>& aParamVector) const;

            void calcPositionMeanAndShiftPositions();
            void squareSigma(bool bUnsquare);

        };
    } // end namespace math
} // end namespace zfs
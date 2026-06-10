#pragma once

#include <zfs/zfsapi.h>

#include <string>

#include "boundingbox.h"

#include <zfs/Math/stl/list_def.h>
#include <zfs/Math/stl/vector_def.h>

namespace zfs {
    namespace math {
        // ============================================================================================================
        // structure for managing the data used for constructing the convex hull
        // using QuickHull()
        struct ZFS_API ContPointLegData
        {
            Point<double>       StartPoint, EndPoint;
            DoublePointListType Points;
            DoubleListType      Distances;
            double              MaxDistance{ 0 };
            Point<double>       MaxDistancePoint;

            void clear()
            {
                StartPoint = EndPoint = MaxDistancePoint = Point<double>(0.0, 0.0);
                Points.clear();
                Distances.clear();
                MaxDistance = 0.0;
            }
        };

        typedef struct ContPointLegData ContPointLegDataType;
        typedef std::list<ContPointLegData> ContPointLegDataListType;
        // ============================================================================================================
        class ZFS_API ConvexHull
        {
        protected:

            int                 M, N;               //<<< image sizes
            double              m_PointOffset;      //<<< additional offset added to surrounding (input) points
            DoublePointListType m_ConvexHullData;   //<<< convex hull data

            // -- private helper

            // function to determine the convex hull of an object
            template<class DoublePointContainerType> const DoublePointListType QuickHull(const DoublePointContainerType& aPointList, const std::string& strOption, const double& aPointOffset = 0.5);
            const DoublePointListType QuickHull(const IntPointListType& aPointList, const std::string& strOption, const double& aPointOffset = 0.5);

        private:

            // function required in QuickHull
            void collectPointLegData(const Point<double>& StartPoint, const Point<double>& EndPoint, const DoublePointListType& EvaluationPointList,
                DoublePointListType& Points, DoubleListType& Distances, double& MaxDistance, Point<double>& PointMaxDistance);

        public:

            ConvexHull();
            ConvexHull(const ConvexHull& aConvexHull);
            ConvexHull(const IntPointListType& aPointList, const int& M_ = 0, const int& N_ = 0, const double& aPointOffset = 0.5);
            ConvexHull(const DoublePointListType& aPointList, const int& M_ = 0, const int& N_ = 0, const double& aPointOffset = 0.5);
            ConvexHull(const DoublePointVectorType& aPointVector, const int& M_ = 0, const int& N_ = 0, const double& aPointOffset = 0.5);

            // accessors
            // getter
            DoublePointListType getConvexHullData() const;
            BoundingBox         getBoundingBox() const;

            // setter 
            void                setFromPointList(const IntPointListType& aPointList);
            void                setFromPointList(const DoublePointListType& aPointList);
            void                setFromPointList(const DoublePointVectorType& aPointVector);
            void                shift(const Point<int>    a2DShift);
            void                shift(const Point<double> a2DShift);

            void                extend(const double& ExtentionDistance);

            // operator
            ConvexHull& operator=(const ConvexHull& rhs);

            // helper/auxilliary
            IntPointListType    samplesInsideConvexHull() const;
            bool                sampleInsideConvexHull(const double& px, const double& py) const;

            // -- tester
            void                testExtendedHullCalculation();

            // friend functions
            friend bool ZFS_API operator==(const ConvexHull& a_lhs, const ConvexHull& a_rhs);
            friend bool ZFS_API operator!=(const ConvexHull& a_lhs, const ConvexHull& a_rhs);

        };
        // ============================================================================================================
        // two sided operators
        // ============================================================================================================
        template<class T> bool ZFS_API operator==(const ConvexHull& a_lhs, const ConvexHull& a_rhs);
        template<class T> bool ZFS_API operator!=(const ConvexHull& a_lhs, const ConvexHull& a_rhs);
        // ============================================================================================================
        class ZFS_API RectConvexHull : public ConvexHull
        {
        private:

            BoundingBox m_BoundingBox; //<<< Data specifying the convex hull

            void ConvexHullFromBoundingBox();
            void CheckBoundingBoxOnImageSizesAndCorrect();

        public:

            RectConvexHull();
            RectConvexHull(const BoundingBox& aBoundingBox, const int& M_ = 0, const int& N_ = 0);
            RectConvexHull(const IntPointListType& aPointList, const int& M_ = 0, const int& N_ = 0);
            RectConvexHull(const DoublePointListType& aPointList, const int& M_ = 0, const int& N_ = 0);

            void set(const BoundingBox& aBoundingBox, const int& M_ = 0, const int& N_ = 0);

            void extend(const double& ExtentionFactor);
            void extend(const double& ExtentionFactorHeight, const double& ExtentionFactorWidth);
        };
    } // end namespace math
} // end namespace zfs
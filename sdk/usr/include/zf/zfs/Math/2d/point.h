#pragma once

#include <cstdint>

#include <zfs/zfsapi.h>

#include <zfs/Math/stl/list_def.h>
#include <zfs/Math/stl/vector_def.h>

typedef unsigned char byte;

namespace zfs {
    namespace math {
        // ============================================================================================================
        /** \class Point
         * \brief  Class providing functionality for 2D positions, in particular image coordinates.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for 2D positions, in particular image coordinates.
         *
         */
        template<class T> class ZFS_API Point
        {
            template<class S> friend class Point;

        private:

            T m_X;
            T m_Y;

        public:
            // constructors
            Point();
            Point(const T& value);
            Point(const T& a_X, const T& a_Y);
            template<class T1> Point(const Point<T1>& src);
            // destructor
            ~Point();
            // getter
            T getElement(int idx) const;
            T getX() const;
            T getY() const;
            T& getX();
            T& getY();
            T getRow() const;
            T getColumn() const;
            // operators
            T operator[](int idx) const;
            T& operator[](int idx);
            T operator()(int idx) const;
            T& operator()(int idx);
            Point& operator+=(const Point& rhs);
            Point& operator-=(const Point& rhs);
            // helping functions
            double Norm() const;
            double Dot(const Point& rhs) const;
            // setter
            void setElement(int idx, const T& value);
            void setX(const T& value);
            void setY(const T& value);
            void setRow(const T& value);
            void setColumn(const T& value);
            // friend functions

            template<class S> friend bool ZFS_API operator==(const Point<S>& a_lhs, const Point<S>& a_rhs);
            template<class S> friend bool ZFS_API operator!=(const Point<S>& a_lhs, const Point<S>& a_rhs);
            template<class S> friend const Point<S> ZFS_API operator-(const Point<S>& a_lhs, const Point<S>& a_rhs);
            template<class S> friend const Point<S> ZFS_API operator+(const Point<S>& a_lhs, const Point<S>& a_rhs);
        };
        /** \class Point
         * \brief  Class providing functionality for 2D positions, in particular image coordinates.
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for 2D positions, in particular image coordinates.
         *
         */
        template<> class ZFS_API Point<int32_t>
        {
            template<class S> friend class Point;

        private:

            int32_t m_X;
            int32_t m_Y;

        public:
            // constructors
            Point();
            Point(const int32_t value);
            Point(const int32_t a_X, const int32_t a_Y);
            template<class T1> Point(const Point<T1>& src);
            // destructor
            ~Point();
            // getter
            int32_t  getElement(const int32_t idx) const;
            int32_t  getX() const;
            int32_t  getY() const;
            int32_t& getX();
            int32_t& getY();
            int32_t  getRow() const;
            int32_t  getColumn() const;
            // operators
            int32_t operator[](const int32_t idx) const;
            int32_t& operator[](const int32_t idx);
            int32_t operator()(const int32_t idx) const;
            int32_t& operator()(const int32_t idx);
            template<class S> operator Point<S>() const;
            Point& operator+=(const Point& rhs);
            Point& operator-=(const Point& rhs);

            double Norm() const;
            double Dot(const Point& rhs) const;

            // setter
            void setElement(int32_t idx, const int32_t value);
            void setX(const int32_t value);
            void setY(const int32_t value);
            void setRow(const int32_t value);
            void setColumn(const int32_t value);
            // friend functions

            template<class S> friend bool           ZFS_API operator==(const Point<S>& a_lhs, const Point<S>& a_rhs);
            template<class S> friend bool           ZFS_API operator!=(const Point<S>& a_lhs, const Point<S>& a_rhs);
            template<class S> friend const Point<S> ZFS_API operator-(const Point<S>& a_lhs, const Point<S>& a_rhs);
            template<class S> friend const Point<S> ZFS_API operator+(const Point<S>& a_lhs, const Point<S>& a_rhs);
        };
        // ============================================================================================================
        typedef               std::list< Point<int32_t> >     IntPointListType;
        typedef               std::list< Point<float> >       FloatPointListType;
        typedef               std::list< Point<double> >      DoublePointListType;
        typedef               std::list< IntPointListType >   IntPointListListType;
        // ============================================================================================================
        typedef               std::vector< Point<int32_t> >   IntPointVectorType;
        typedef               std::vector< Point<float> >     FloatPointVectorType;
        typedef               std::vector< Point<double> >    DoublePointVectorType;
        typedef               std::vector< IntPointListType > IntPointListVectorType;
        // ============================================================================================================
        // auxiliary data types
        struct PointGroupData
        {
            IntPointListType  points;
            DoubleListType    feature_list;
            double            feature_min, feature_max, feature_mean;
            int               size;
            int               idx;
            PointGroupData() : feature_min(0.), feature_max(0.), feature_mean(0.), size(0), idx(0) {}
        };

        typedef struct PointGroupData           PointGroupDataType;
        typedef std::list<PointGroupDataType>   PointGroupDataListType;
        typedef std::vector<PointGroupDataType> PointGroupDataVectorType;
        // ============================================================================================================
        // two sided operators
        // ============================================================================================================
        template<class T>            bool            ZFS_API operator==(const Point<T>& a_lhs, const Point<T>& a_rhs);
        template<class T>            bool            ZFS_API operator!=(const Point<T>& a_lhs, const Point<T>& a_rhs);
        template<class T>            const Point<T>  ZFS_API operator- (const Point<T>& a_lhs, const Point<T>& a_rhs);
        template<class T>            const Point<T>  ZFS_API operator+ (const Point<T>& a_lhs, const Point<T>& a_rhs);
        template<class T1, class T2> const Point<T1> ZFS_API operator/ (const Point<T2>& a_lhs, const T1& a_rhs);
        // ============================================================================================================
        // auxiliary functions and typedefs
        // ============================================================================================================

        typedef enum neighbourhood_set { NH4, NH8 } neighbourhood_t;

        IntPointListType                 ZFS_API getNeighbours8List(const Point<int32_t>& a_Position, const int32_t a_RadiusX = 1, const int32_t a_RadiusY = 1);
        IntPointVectorType               ZFS_API getNeighbours8Vector(const Point<int32_t>& a_Position, const int32_t a_RadiusX = 1, const int32_t a_RadiusY = 1);
        IntPointListType                 ZFS_API getNeighbours4List(const Point<int32_t>& a_Position, const int32_t a_RadiusX = 1, const int32_t a_RadiusY = 1);
        IntPointVectorType               ZFS_API getNeighbours4Vector(const Point<int32_t>& a_Position, const int32_t a_RadiusX = 1, const int32_t a_RadiusY = 1);
        IntPointListType                 ZFS_API getNeighbours84(const Point<int32_t>& a_Position, const int32_t a_RadiusX = 1, const int32_t a_RadiusY = 1);
        IntPointListVectorType           ZFS_API getNeighbours8EvenSquareExtend(const Point<int32_t>& a_Position, const int32_t a_HalfExtendX, const int32_t aHalfExtendY);
        IntPointListVectorType           ZFS_API getNeighbours8RectAngle(const Point<int32_t>& a_Position, const int32_t a_WSE_X, const int32_t aWSE_Y);

        enum class quadrant_t: uint8_t { UpperLeft, LowerLeft, UpperRight, LowerRight };
        IntPointVectorType               ZFS_API getQuadrantOf8Neighbourhood(const quadrant_t a_quadrant, const Point<int>& a_point);

        unsigned int                     ZFS_API areaBoundingBox(const IntPointListType& aBoundingBox);

        bool                             ZFS_API pointWithinImage(const Point<int32_t>& Position, const int M, const int N);
        bool                             ZFS_API pointWithinImage(const Point<double>& Position, const int M, const int N);
        bool                             ZFS_API pointWithinImage(const int m, const int n, const int M, const int N);
        DoublePointListType              ZFS_API interpolatePointList(const DoublePointListType& PointList);
        DoublePointListType              ZFS_API interpolatePointListX(const DoublePointListType& PointList);
        void                             ZFS_API exchangeXYOfPoints(DoublePointListType& aPointList);
        IntPointListType                 ZFS_API subtractPointsFromPointList(const IntPointListType& aPointList, const IntPointListType& aPointsToSubtractFromPointList);
        void                             ZFS_API ShiftPointsInPointList(IntPointListType& aPointList, const Point<int32_t>& LowerLeft);
        IntListType                      ZFS_API indicesOfNeighbours(const IntPointListType& point_list, const Point<int32_t>& aPoint);
        int                              ZFS_API indexOfPointInPointList(const Point<int32_t>& aPoint, const IntPointListType& PointList);
        bool                             ZFS_API pointInPointList(const Point<int32_t>& Point, const IntPointListType& PointList);
        IntPointListType::const_iterator ZFS_API ItPointInPointList(const Point<int32_t>& Point, const IntPointListType& PointList);
        Point<double>                    ZFS_API CentroidFromIntPointList(const IntPointListType& point_list);
        BoolVectorType                   ZFS_API findPointGroupConnectedPoints(const IntPointListType& point_list_work, const DoubleListType& feature_list_work, const double& dFeatureMaxNeighbours); /// < point group of points with directly neighbouring pixels
        BoolVectorType                   ZFS_API findPointGroupLinearUnconnected(const DoubleListType& feature_list_work, const double& dFeatureMaxNeighbours); /// < point group of points of not necessarily directly neighbouring pixels
        PointGroupDataListType           ZFS_API findPointGroups(const IntPointListType& point_list, const DoubleListType& feature_list, const double& dFeatureMax_neighbours, const bool& bConnectedFlag); ///< determination on modus by flag: bConnectedFlag, uses findPointGroupUnconnected or findPointGroupConnectedPoints
        Point<int32_t>                   ZFS_API getUpperLeftPointOfEnclosingRectContour(const IntPointListType& point_list);
        Point<int32_t>                   ZFS_API getLowerRightPointOfEnclosingRectContour(const IntPointListType& point_list);
        DoublePointListType              ZFS_API toDoublePointList(const IntPointListType& point_list);
        // ============================================================================================================
        // templated
        template<class T> void ZFS_API MinMaxOfCoordinatesInPointList(const std::list< Point<T> >& point_list, T& min_x, T& max_x, T& min_y, T& max_y);
        template<class T> void ZFS_API XYExtendOfPointsInPointList(const std::list<Point<T>>& point_list, T& extend_x, T& extend_y);
        template<class T> void ZFS_API MinMaxOfCoordinatesInPointVector(const std::vector< Point<T> >& point_list, T& min_x, T& max_x, T& min_y, T& max_y);
        template<class T> void ZFS_API XYExtendOfPointsInPointVector(const std::vector<Point<T>>& point_list, T& extend_x, T& extend_y);
        template<class T> void ZFS_API write_list_to_file(const char* str_file_path, const std::list<T>& in_list);
        template<class T> void ZFS_API write_point_list_to_file(const char* str_file_path, const std::list< Point<T> >& point_list);
        template<class T> void ZFS_API write_list_to_file(const wchar_t* str_file_path, const std::list<T>& in_list);
        template<class T> void ZFS_API write_point_list_to_file(const wchar_t* str_file_path, const std::list< Point<T> >& point_list);
        // ============================================================================================================
    } // end namespace math
} // end namespace zfs
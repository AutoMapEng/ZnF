#pragma once

#include <utility>

#include "filterapi.h"

#include <zfs/Math/2d/boundingbox.h>
#include <zfs/Math/2d/convexhull.h>
#include <zfs/Math/2d/vector_2d.h>
#include <zfs/Math/stl/list_def.h>
#include <zfs/Math/stl/vector_def.h>

namespace zfsfilter
{
    //################################################################################

    template< class T> class Matrix;

    //################################################################################

    /** \class BinaryObject
     * \brief  Class for representating and conduct operations on a binary object (used within road sign filtering)
     *
     * \author Juergen Holzner
     *
     * Class for representating and conduct operations on a binary object (used within road sign filtering).
     *
     */
    class FILTERDLL_API BinaryObject
    {
    protected:
        int                      m_Idx; // < arbitrary index assigned to binary object
        int                      m_Area;

        std::pair<int, int>      m_MaxExtends; // < (x,y)

        std::pair<int, int>      m_MinMaxX; // < line max min
        std::pair<int, int>      m_MinMaxY; // < position max min

        zfs::math::Point<double>    m_Centroid;
        zfs::math::IntPointListType m_PositionsList;

        zfs::math::BoundingBox      m_BoundingBox;
        zfs::math::ConvexHull       m_ConvexHull;

        void AreaFromPositions();
        void CentroidFromPositions();

    public:
        BinaryObject();
        BinaryObject( const BinaryObject& aBinaryObject, const std::string& strOption="full");
        BinaryObject( const std::list<zfs::math::Point<int>>& a_PositionList, const int aIdx= 0 );

        virtual ~BinaryObject()
        {};

        // helper
        virtual void               TransformCoordinates( const zfs::math::Point<int>& UpperLeft );
        void                       ObjectAtImageEdges( const int& ImageLines, const int& ImageColumns, bool& bAtLowerX, bool& bAtUpperX, bool& bAtLowerY, bool& bAtUpperY ) const;
        const zfs::math::IntListType  IntensitiesAtBinaryObjectPositions( const Matrix<int>& a_IntensityMatrix ) const;

        // getter
        int                                    getArea()                 const;
        int                                    getNoOfPositions()        const;
        int                                    getIdx()                  const;
                                               
        const zfs::math::IntPointListType         getPositionList()         const;
        const zfs::math::IntPointListType* const  getPositionListConstPtr() const;
                                               
        const zfs::math::ConvexHull&              getConvexHull()           const;
                                               
        const zfs::math::BoundingBox              getBoundingBox()          const;
                                               
        const zfs::math::Point<int>               getMostCentralPosition()  const;
        const zfs::math::Point<double>            getCentroid()             const;
                                               
        const std::pair<int, int>&             getMaxExtend() const;

        const std::pair<int, int>&             getMinMaxX() const;
        const std::pair<int, int>&             getMinMaxY() const;

        // setter
        void  setIdx( int aIdx );
        void  setPositionList( const zfs::math::IntPointListType& a_PositionList );

        void setMaxExtend( const std::pair<int, int>& a_MaxExtends );
        void setMaxExtendFromPointList();
        void setMaxExtendFromExternalPointList( const zfs::math::IntPointListType& aPointList );

        // operators
        bool operator==( const BinaryObject& rhs ) const;
        bool operator!=( const BinaryObject& rhs ) const;
    };

	typedef std::list<BinaryObject>           BinaryObjectListType;
	typedef std::vector<BinaryObject>         BinaryObjectVectorType;
	typedef std::list<BinaryObjectVectorType> BinaryObjectVectorListType;
	typedef std::list<BinaryObjectVectorType> BinaryObjectVectorVectorType;

    // ==================================================================================
    // Operators for binary object lists and vectors
    // ==================================================================================
    bool FILTERDLL_API operator==( const BinaryObjectVectorType & lhs, const BinaryObjectVectorType & rhs );
    bool FILTERDLL_API operator!=( const BinaryObjectVectorType & lhs, const BinaryObjectVectorType & rhs );
    bool FILTERDLL_API operator==( const BinaryObjectListType & lhs, const BinaryObjectListType & rhs );
    bool FILTERDLL_API operator!=( const BinaryObjectListType & lhs, const BinaryObjectListType & rhs );
    //################################################################################
    // test function
    void test_BinaryObject();
} // namespace zfsfilter

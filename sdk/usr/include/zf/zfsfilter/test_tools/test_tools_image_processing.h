#include <zfsfilter/filterapi.h>

#include <zfsfilter/Matrix.h>

#include <zfsfilter/binaryobject.h>

// =====================================================================================================================================================================================
// Test Helper for (Binary) Image Processing Object(s) 
// =====================================================================================================================================================================================

namespace zfsfilter
{
    namespace test
    {
        // =====================================================================================================================================================================================
        
        FILTERDLL_API bool PaintRectAtEmptyPositions( Matrix<bool>& aInputMatrix,
                                                      const unsigned int col,
                                                      const unsigned int row,
                                                      const unsigned int ObjectSize );
        
        // =====================================================================================================================================================================================
        
        FILTERDLL_API size_t NumberOfPointsInBinaryObjectsList( const BinaryObjectVectorType& aBOList );
        
        // =====================================================================================================================================================================================
        
        FILTERDLL_API Matrix<bool> generateTestMatrixForBinaryImageProcessingTests( const unsigned int MatrixSize, const unsigned int NoOfObjects, const bool bLargeObjects, const bool bMedium=true, const bool bSmall=true, const bool bTiny=true );
        
        // =====================================================================================================================================================================================
    }
}
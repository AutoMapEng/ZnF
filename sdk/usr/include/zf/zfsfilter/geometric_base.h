#pragma once

#include "filterapi.h"

namespace zfs
{
    namespace math
    {
        template< class T> class Point;
        template< class T, unsigned int N> class VectorND;
    }
}

namespace zfsfilter
{
    template< class T> class Matrix;

    //================================================================================================================
    // Geometrical Tools
    //================================================================================================================
    template<class T> void                   FILTERDLL_API cross_prod( const T* const a, const T* const b, T* const c ); // three dimensions
    template<class T> void                   FILTERDLL_API scalar_prod( const T* const a, const T* const b, const int nDim, const bool& bNormalize, const bool& bAbs, T& sp );
    template<class T> void                   FILTERDLL_API normalize( T* const a, const int dim = 3 );

    template<class T> zfs::math::VectorND<T, 3> FILTERDLL_API CenterVector( const Matrix<T>& aPosXComp, const Matrix<T>& aPosYComp, const Matrix<T>& aPosZComp );

    template<class T> zfs::math::VectorND<T, 3> FILTERDLL_API NormalEstimation( const Matrix<T>   & aPosXComp,           /// < input : [m or mm] matrix for x-posit
                                                                             const Matrix<T>   & aPosYComp,           /// < input : [m or mm] matrix for y-posit
                                                                             const Matrix<T>   & aPosZComp,           /// < input : [m or mm] matrix for z-posit
                                                                             const Matrix<bool>& aPosMask,            /// < input : [true/false] invalid matrix
                                                                             const bool        & abDirectToScanner,   /// < input : [true/false] direct normal output vector to scanner
                                                                             bool              & abEnoughPointsFound );/// < output: [true/false] indicated if enough points were available

    template<class T> void                   FILTERDLL_API NormalEstimation( const Matrix<T>         &             aPosXComp,  /// < input : [m or mm] matrix for x-positions
                                                                             const Matrix<T>         &             aPosYComp,  /// < input : [m or mm] matrix for y-positions
                                                                             const Matrix<T>         &             aPosZComp,  /// < input : [m or mm] matrix for z-positions
                                                                             const Matrix<bool>      &             aInvalids,  /// < input : [true/false] invalid matrix
                                                                             const zfs::math::Point<int>&   aCenterMatrixIdxPos,  /// < input : [pixel] matrix position for which the normal is to be estimated
                                                                             const int               &            aColRadius,  /// < input : [pixel] radius in cross line direction around center wherein which positions are used for estimation.
                                                                             const int               &            aRowRadius,  /// < input : [pixel] radius in along line direction around center wherein which positions are used for estimation.
                                                                             bool                    &   abEnoughPointsFound,  /// < output: [true/false]were enough valid points found
                                                                             zfs::math::VectorND<T, 3>   &         aNormalVector ); /// < output: [m or mm]estimate normal vector
                                             
    template<class T> void                   FILTERDLL_API Norm3DInArrays( const Matrix<T>&         MatX, const Matrix<T>& MatY, const Matrix<T>& MatZ, Matrix<T>& MatNorm );
}
#pragma once

#include "filterapi.h"

namespace zfsfilter
{
    template< class T> class Matrix;

	//================================================================================================================
	// Image Processing Functions
	//================================================================================================================

	void                                         FILTERDLL_API PatchStartStopPositions( const int M, const int N, const int m, const int n, const int patch_m_size_rad, const int patch_n_size_rad,
										  	      									int& m_start_patch, int& m_end_patch, int& n_start_patch, int& n_end_patch );
                                                 
	void                                         FILTERDLL_API IdxStartStopPositions( const int Extend, const int PositionIdx, const int Radius, int& start_pos_idx, int& end_pos_idx );
                                                 
	Matrix<int>                                  FILTERDLL_API BlobDetektion( const Matrix<bool> & BinaryImage );
	Matrix<bool>                                 FILTERDLL_API Widening( const Matrix<bool> &        Mask, const Matrix<int>  & MatrixWideningRanges );
                                                 
	template<class T> void                       FILTERDLL_API DeclareSobel( Matrix<T>& Sobelx, Matrix<T>& Sobely );
                                                 
	template<class T> const Matrix<T>            FILTERDLL_API GaussWin( const int M, const int N, const T& alpha_m, const T& alpha_n );
                                                 
	template<class T> void                       FILTERDLL_API GradX( const Matrix<T>&         Mat, Matrix<T>& dMatX );
	template<class T> void                       FILTERDLL_API GradY( const Matrix<T>&         Mat, Matrix<T>& dMatY );
                                                 
	template<class T> void                       FILTERDLL_API CalculateAtanGrounded( const Matrix<T>&       AtanG, Matrix<T>& AtanGRounded );
                                                 
	template<class T> void                       FILTERDLL_API CalculateAbsAtanGrad( const Matrix<T>&          Gx, const Matrix<T>& Gy, Matrix<T>   &           AbsG, Matrix<T>& AtanG );
	template<class T> void                       FILTERDLL_API NonMaximumSuppression( const Matrix<T>&        AbsG, const Matrix<T>& AtanGRounded, Matrix<T>   &         AbsGNM );
	template<class T> const Matrix<bool>         FILTERDLL_API CannyHysteresisThresholding( const Matrix<T>&      AbsGNM, const T        & ThresholdUpper, const T           & ThresholdLower );
                                                 
    template<class T> void                       FILTERDLL_API ImageScaling( const Matrix<T>& aMatrix, T& aImageScale, T& aImageValMin );
    template<class SampleT, class ImageT> ImageT FILTERDLL_API sample_conversion( const SampleT& aSampleValue, const SampleT& aImageScale, const SampleT& aMinImageValue );
}
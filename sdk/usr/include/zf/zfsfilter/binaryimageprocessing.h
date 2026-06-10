#pragma once

#include <climits>
#include <string>

#include "filterapi.h"

#include "binaryobject.h"
#include "multithreading_base.h"
#include "Matrix.h"

namespace zfsfilter
{
    //#################################################################################################################

    /** \class BinaryImageProcessing
     * \brief  Class for determining connected components in a binary image and conduct various further operations on a binary image
     *
     * \author Juergen Holzner
     *
     * Class for determining connected components in a binary image and conduct various further operations on a binary image.
     *
     */
    class FILTERDLL_API BinaryImageProcessing: public MultithreadingBase
    {
    private:
        // -- type definitions
        enum class ProcessingFunctionSelectionType:uint8_t
        {
            BIP_PFS_Undef, BIP_PFS_Clean, BIP_PFS_Erode, BIP_PFS_Dilate, BIP_PFS_Close, BIP_PFS_Open, BIP_PFS_nElementsTrue
        }; // < for binary image operators (erode, dilate, close, nElemetnsTrue, clean)

        struct ProcessingOperatorData // < for binary image operators (erode, dilate, close, nElemetnsTrue, clean)
        {
            ProcessingFunctionSelectionType ProcessingFunctionSelection;
            zfs::math::Tuple2<int>          OperatorSize;
            std::string                     OperatorShape;
            Matrix<bool>                    OperatorMatrix;
            ProcessingOperatorData():ProcessingFunctionSelection(ProcessingFunctionSelectionType::BIP_PFS_Undef ), OperatorSize( 0 ), OperatorShape( "" ){}
        };

        // -- class constants
        static const int                m_c_BlobProcessingMaxSizeThresh;
        static const double             m_c_ProcessorPortionToUse;

        // -- member variables
        bool                            m_bUseBlobs, m_bBlobsDetected, m_bWorkingOnSameMatrix, m_bOutputProgress;
        int                             m_nImage_Rows, m_nImage_Columns;
        size_t                          m_nMinSize, m_nMaxSize;
        zfs::math::Tuple2<int>             m_OperatorSize;
        std::string                     m_OperatorShape; // < for binary image operators (erode, dilate, close, nElemetnsTrue, clean)
        Matrix<bool>* const             m_pBinaryImage;
        Matrix<int>                     m_Blobs;
        BinaryObjectVectorType          m_ObjectListInBinaryImage;

        // -- private functions
        // -- image operators
        static void ProcessMultithreaded( const Matrix<bool>          & InputMatrix,
                                          const ProcessingOperatorData& aProcessingOperator,
                                          const double                & ProcessorPortionToUse,
                                          Matrix<bool>                & Result,
                                          int                         & nElementsTrue );

        static void ThreadWorker( const Matrix<bool>          & input_matrix,
                                  const ProcessingOperatorData& ProcessingOperator,
                                  const zfs::math::Range<int>    & ProcessingRange,
                                  const zfs::math::Tuple2<int>   & ProcessingOverlaps,
                                  const zfs::math::Tuple2<int>   & ProcessingInfo,
                                  Matrix<bool>                  *p_result_matrix,
                                  int                           *n_elements_true );

        // -- connected component analysis
        void GetConnectedComponentsMT();
        static void GetConnectedComponentsInSubmatrixThreadWorker( BinaryImageProcessing * const aBinaryImageProcessing, const int line_start, const int line_end );
        static BinaryObjectVectorType CombineConnectedComponentsFromThreads( const BinaryObjectVectorListType& aBinaryObjectVectorList );

        // Blobs -----------------------------------------------------------------------------------------------------------------
        BinaryObjectVectorType   getConnectedComponentsPL( bool bDeleteAsConvexHull = false );

        zfs::math::IntPointListType getConnComponent( const zfs::math::Point<int>& StartPoint );
        zfs::math::IntPointListType getConnComponentV2(const zfs::math::Point<int>& StartPoint, const int lab, Matrix<int>& label, std::list<int>& sizes); // TEST
        BinaryObjectVectorType   getConnectedComponentsBlob();

    public:
        // Constructors
        // Standard Constructor
        BinaryImageProcessing();
        // For processing the image (Close, Dilate, Erode, ...)
        BinaryImageProcessing( Matrix<bool>& a_BinaryImage, const zfs::math::Tuple2<int>& OperatorSize, const std::string& OperatorShape );
        // For getting connected components (objects)
        BinaryImageProcessing( Matrix<bool>& a_BinaryImage, const bool& a_bUseBlobs = false, const size_t& a_MinSize = 0, const size_t& a_MaxSize = INT_MAX, bool a_bOutputProgress= false);

        // Functions for getting Connected Components (Objects)
        // Generic Function Calls
        BinaryObjectVectorType          getConnectedComponents( bool bDeleteAsConvexHull= false );
        // Processing using zfs::math::Point Lists ---------------------------------------------------------------------------------
        static void                     deleteObjectFromImage( const zfs::math::IntPointListType& ObjectPointList, Matrix<bool>& BinaryImageForProcessing );
        // Blobs -----------------------------------------------------------------------------------------------------------------
        static const Matrix<int>        BlobDetection( const Matrix<bool> & BinaryImage, int& no_of_blobs_found );
        const Matrix<int>               BlobDetection_ReferenceAlgorithm( const Matrix<bool>& BinaryImage, int& no_of_blobs_found ); // recursive, not for large datasets (stack overflow occurs)
        static int                      BlobSize( const Matrix<int>& BlobsMatrix, const int& BlobIndex );
        static zfs::math::IntPointListType GetPointListForBlobIdx( const Matrix<int>& BlobsMatrix, const int& BlobIndex );
        static void                     deleteBlobFromBlobsMatrix( Matrix<int>& BlobsMatrix, const int& BlobIdx );

        // Experimental (Not used, untested)
        zfs::math::IntPointListType        getMovingPixelBinaryNeighbours( const zfs::math::Point<int>& a_Position );

        // Image Processing Routines (mainly multithreaded and based on matrix)
        void setOperatorSizeAndShape( const int& operator_size, const std::string& operator_shape );

        // implementation multi threaded using
        const int nElementsTrue() const;
        const Matrix<bool> Erode() const;
        const Matrix<bool> Dilate() const;
        const Matrix<bool> Close() const;
        const Matrix<bool> Open() const;
        const Matrix<bool> Clean() const;

        void Erode(int OperatorSize);                                                                    // modify input image with square operator of size OperatorSize
        void Dilate(int OperatorSize);                                                                   // modify input image with square operator of size OperatorSize
        void Close(int OperatorSize);                                                                    // modify input image with square operator of size OperatorSize
        void Open(int OperatorSize);
        void Clean(int OperatorSize);                                                                    // modify input image with square operator of size OperatorSize

        void Erode(const zfs::math::Tuple2<int>& OperatorSize, const std::string& OperatorShape);           // modify input image; use ("cross" or "round") else for rectangular
        void Dilate(const zfs::math::Tuple2<int>& OperatorSize, const std::string& OperatorShape);          // modify input image; use ("cross" or "round") else for rectangular
        void Close(const zfs::math::Tuple2<int>& OperatorSize, const std::string& OperatorShape);           // modify input image; use ("cross" or "round") else for rectangular
        void Open(const zfs::math::Tuple2<int>& OperatorSize, const std::string& OperatorShape);            // modify input image; use ("cross" or "round") else for rectangular
        void Clean(const zfs::math::Tuple2<int>& OperatorSize, const std::string& OperatorShape);           // modify input image; use ("cross" or "round") else for rectangular

        void Erode (const Matrix<bool>& Operator);                                                       // modify input image; use arbitrary user defined operators
        void Dilate(const Matrix<bool>& Operator);                                                       // modify input image; use arbitrary user defined operators
        void Close (const Matrix<bool>& Operator);                                                       // modify input image; use arbitrary user defined operators
        void Open (const Matrix<bool>& Operator);                                                        // modify input image; use arbitrary user defined operators
        void Clean (const Matrix<bool>& Operator);                                                       // modify input image; use arbitrary user defined operators

        void Close (const Matrix<bool>& DilateOperatorMatrix, const Matrix<bool>& ErodeOperatorMatrix);  // modify input image; use arbitrary user defined operators
        void Open (const Matrix<bool>& DilateOperatorMatrix, const Matrix<bool>& ErodeOperatorMatrix);   // modify input image; use arbitrary user defined operators
                                                    
                                                    // Tool functions (not multithreaded so far)
        static void deleteSmallObjectsNH8( const int& AreaThreshold, Matrix<bool>& BinaryImageForProcessing );
    };
    //#################################################################################################################
} // namespace zfsfilter

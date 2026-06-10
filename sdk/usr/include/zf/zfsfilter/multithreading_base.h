#pragma once

#include "filterapi.h"

#include <vector>
#include <zfs/Math/math/range.h>
#include <zfs/Math/2d/boundingbox.h>

namespace zfsfilter {

    // ============================================================================================================================================
    // function to be called by processMT()
    // ============================================================================================================================================

    typedef void ( *MTFunctionType )( void* const aUserObject, const int32_t aStart, const  int32_t aEnd);

    // ============================================================================================================================================
    /** \class MultithreadingBase
     * \brief  Class for determining the number of processors and additional useful functionality
     *
     * \author Jürgen Holzner
     *
     * Determines number of processors and, given the proportion of the processors to use,
     * the used numbers of processors (minimum 1). Moreover, it implements useful helping functions
     * that determine the division or processing section size of a matrix for individual threads based on the
     * used number of processors.
     *
     */
    class FILTERDLL_API MultithreadingBase
    {
    protected:
        // -- member variables --
        bool    m_bProcessMultithreaded;
        bool    m_bProcessConditionMet;
        bool    m_NumberOfProcessorsToUseSetFromExternal;
        int     m_NumberOfProcessors;
        int     m_NumberOfProcessorsToUse;
        int     m_MinSizeForMTProcessing;
        double  m_UseProcessorsPortion;

        // -- helper functions --
        void    DetermineNumberOfProcessors();
        void    DetermineNumberOfProcessorsToUse();

        void processMT(MTFunctionType aFunction, void* pUserObject, const int32_t aProcessingLength);

    public:

        // -- constructors
        //MultithreadingBase();
        MultithreadingBase(const double& a_UseProcessorsPortion = 1.0, const bool& bProcessMultithreaded = true);
        MultithreadingBase(const int     a_NumberOfProcessorsToUse, const bool& bProcessMultithreaded = true);

        // -- getter
        int    MultithreadingOKForOperation( const int DataExtendLines, const int OperationOrFilterExtendLines );

        int    getNumberOfProcessors();
        int    getNumberOfProcessorsToUse();

        int    getNumberOfProcessors() const;
        int    getNumberOfProcessorsToUse() const;
        int    getNumberOfProcessorsToUse(const int aNoOfLines, const int aOverlap= 0) const;

        int    getMinSizeForMTProcessing(const int Overlap) const;
        double getProcessorPortion() const;
        bool   ProcessMultithreaded() const;

        // -- setter
        void   setProcessorPortion(const double& a_UseProcessorsPortion);
        void   setNumberOfProcessorsToUse(const int a_UseProcessorsPortion);
        void   setProcessMultithreaded( const bool& a_bProcessMultithreaded );
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------
        void   LoadBalance(const int total_load, int& load_per_processor, int& no_of_proc_to_use);
        void   ProcessingStartEndLine(const int aThreadIdx, const int aTotalNoOfLinesToProcess, int& aStartLine, int& aEndLine);
        void   ProcessingStartEndLineOverlap(const int aThreadIdx, const int aTotalNoOfLinesToProcess, const int aOverlap, int& aStartLine, int& aEndLine);
        int    ProcessingSectionSize(const int aTotalNoOfLinesToProcess);
    };
    // ============================================================================================================================================
    /** \class MTInfoContainerFC
    * \brief  Class for multithreaded filtering (with Filterchain)
    *
    * \author Juergen Holzner
    *
    * Class for multithreaded filtering (with Filterchain). Use this class to
    * when multithreading with with BasefilterFC and derived classes. Apply in callback
    * function where the filtered lines come out (each line carries info on the thread idx).
    * Use as global or static object.
    */
    class FILTERDLL_API MTInfoContainerFC: public MultithreadingBase
    {
    public:
        // ====================================================
        // -- constructors
        // ====================================================
        MTInfoContainerFC();

        MTInfoContainerFC( const double& aProcessorPortion,
                           const int     aStartLineDataSet,
                           const int     aEndLineDataSet,
                           const int     aOverlap,
                           const int     aNumberOfPixelsPerLine= -1 );
        // ====================================================
        // -- initializers
        // ====================================================
        void initialization( const double& aProcessorPortion,
                             const int     aStartLineDataSet,
                             const int     aEndLineDataSet,
                             const int     aOverlap,
                             const int     aNumberOfPixelsPerLine= -1);
        // ====================================================
        // -- accessors
        // ====================================================
        int  getNumberOfProcessorsToUse() const;

        int  ValidStartLine( const int threadIdx ) const;                                               /// < valid range in data matrix
        int  ValidEndLine( const int threadIdx ) const;                                                 /// < valid range in data matrix

        int  ProcessingStartLine( const int threadIdx ) const;                                          /// < processing range in data matrix
        int  ProcessingEndLine( const int threadIdx ) const;                                            /// < processing range in data matrix

        const zfs::math::Range<int> getValidRange(const int threadIdx) const;                              /// < valid range in data matrix
        const zfs::math::Range<int> getProcessingRange(const int threadIdx) const;                         /// < processing range in data matrix

        const zfs::math::BoundingBox getBoundingBoxDataMatrix( const int threadIdx ) const;                /// < source bounding box in data matrix (to extract for processing)
        const zfs::math::BoundingBox getBoundingBoxFilterMatrixValid( const int threadIdx ) const;         /// < source bounding box in processd sub matrix (to extract for result collection)
        const zfs::math::BoundingBox getBoundingBoxDataMatrixTargetForValids( const int threadIdx ) const; /// < target bounding box in result data matrix (to write to for result collection)

        bool Initialized() const;

    private:
        // ====================================================
        // -- member variables
        // ====================================================
        bool                            m_bInitialized;
        bool                            m_bNeedBoundingBoxes;

        std::vector<zfs::math::Range<int>>  m_ThreadInfoStartStopProcessingLines;   ///< contains valid start/valid end
        std::vector<zfs::math::Range<int>>  m_ThreadInfoStartStopValidLines;        ///< contains valid start/valid end
        std::vector<zfs::math::BoundingBox> m_BoundingBoxDataMatrixForThreads;      ///< (source) bounding box for selection of submatrices in data matrix to process
        std::vector<zfs::math::BoundingBox> m_BoundingBoxFilterMatrixValid;         ///< (source) bounding box for valids in filtered sub matrix processed
        std::vector<zfs::math::BoundingBox> m_BoundingBoxDataMatrixTargetForValids; ///< (target) bounding box for valids in result data matrix

        // -- private functions
        void initialization_itrnl( const int aStartLineDataSet,
                                   const int aEndLineDataSet,
                                   const int aOverlap,
                                   const int aNumberOfPixelsPerLine);

        int getNumberOfProcessorsToUse( const int aStartLineDataSet, const int aEndLineDataSet, const int   & aOverlap);

    };
    // ============================================================================================================================================
} // end namespace zfsfilter

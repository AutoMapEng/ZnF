#pragma once

#include "filterapi.h"

#include <vector>

#include "../zfs/filter/mixpix_algo.h"

#include "Matrix.h"
#include "multithreading_base.h"
#include "tilefilter.h"

namespace zfsfilter
{
//#define EN_MIXPIXENH_SINGLE

#ifdef EN_MIXPIXENH_SINGLE

    /** \class SinglePixelFilter
     * \brief  Class for single pixel filtering of the data, used within mixed pixel filter
     *
     * \author Juergen Holzner
     *
     * Class for single pixel filtering of the data, used within mixed pixel filter
     *
     */
    template <class T> class FILTERDLL_API SinglePixelFilter: public MultithreadingBase, virtual public TileFilter
    {
    private:
        // -- class constants
        static const bool           m_c_UseNew;                                     ///< indicator on if new approach is to be used
        static const int            m_c_FilterOverlap;                              ///< filter overlap 
        static const int            m_c_FilterSize;                                 ///< filter size
        static const int            m_c_NeighbourhoodPixels;                        ///< neighbourhood (standard 2D: 8)

        static const double         m_c_UseProcessorPortion;                        ///< processor portion to use when multithreading is used

        static const bool           m_c_WriteDebugFiles;                            ///< processor portion to use when multithreading is used
        static const std::wstring   m_c_DebugFileOutputDirectory;                   ///< processor portion to use when multithreading is used

        // -- private member variables
        int                         m_MinNN;                                       ///< Lowest Number of Neighbours required (nsinglePN)

        Matrix<T>*                  m_pD;                                          ///< range in m
        Matrix<bool>  *             m_pMaskPrevFilter;                             ///< mask previous filter operations
        Matrix<bool>  *             m_pMaskI;                                      ///< mask specifying intensities lower than threshold

        Matrix<bool>                m_DataMask;                                    ///< combined mask: m_pMaskI | m_pMaskPrevFilter
        Matrix<bool>                m_FilterResultMask;                            ///< result mask of the filter


        // -- private member functions
        void basic_initialization();

        void setSinglePixelFilterParameter();

        void filterMT();

        void filterST( const int a_LineStart, const int a_LineEnd, int* a_nPixelFiltered ); 

        static void filterThreadWorker( SinglePixelFilter* const       a_pSinglePixelFilter, /// < worker function for threads
                                        const int                      a_LineStart,      
                                        const int                      a_LineEnd,
                                        const int                      a_ThreadIdx,
                                        int*                           a_nPixelFiltered );

        // -- filtering using LRC standard filters
        ZFSFiltering                   m_Filter;                                      /// < Filterobject for filterparameter and single pixel filter function

        void SingleLineFilterFunc( const bool* const a_pMaskIn, const T* const a_pD, bool* a_pMaskOut );

        void ThreeLineFilterFunction( const bool* const a_pMaskInPre , const T* const a_pDPre ,
                                      const bool* const a_pMaskIn    , const T* const a_pD    ,
                                      const bool* const a_pMaskInNext, const T* const a_pDNext,
                                      bool* a_pMaskOut );

        /// -- new filtering fucntion

        void FilterFunction( const int a_LineStart, const int a_LineEnd, int* a_nPixelFiltered );
        zfs::math::IntPointListType FindNearPoints( const zfs::math::Point<int>& aPatchULPointRef, const zfs::math::Point<int>& aPos, const T& aMaxRangeDist, int aCurObjectLength, Matrix<bool>& aAlreadyChecked );

    public:
        // -- public class constants
        static const int m_c_DefaultLowestNN;
        static const int m_c_MinNoOfNeighboursVsUserStrength[3];

        // -- constructors
        SinglePixelFilter();

        SinglePixelFilter( Matrix<T>   & a_RangeMatrix,// in:  range in m
                           Matrix<bool>& a_MaskI,      // in:  intensity mask (from thresholding)
                           const int     a_MinNN);     // in:  lowest number of neighbours required (set -1 for using default)

        SinglePixelFilter( Matrix<T>   & a_RangeMatrix,                            // in:  range in m
                           Matrix<bool>& a_MaskI,                                  // in:  intensity mask (from thresholding)
                           Matrix<bool>& a_MaskFromPreviousFilteringOperations,    // in:  mask from previous filtering operations 
                           const int     a_MinNN);                                 // in:  lowest number of neighbours required (set -1 for using default) 

        // -- public member functions
        void initialize( Matrix<T>   & a_RangeMatrix,                            
                         Matrix<bool>& a_MaskFromPreviousFilteringOperations,
                         const int     a_MinNN);   
                                       
        void initialize( Matrix<T>   & a_RangeMatrix,                                      
                         Matrix<bool>& a_MaskI,                                            
                         Matrix<bool>& a_MaskFromPreviousFilteringOperations,
                         const int     a_MinNN);             

        void filter();

        // -- setter
        void setLowestNN(const int a_MinNN);

        // -- getter
        int getLowestNN() const;
        const Matrix<bool>& getMask() const;
    };

#endif // EN_MIXPIXENH_SINGLE

    // ===========================================================================================================
} // namespace zfsfilter
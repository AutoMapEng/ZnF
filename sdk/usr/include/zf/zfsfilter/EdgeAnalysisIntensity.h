#pragma once

#include <cstdint>
#include <vector>

#include "filterapi.h"

#include "Matrix.h"
#include "tilefilter.h"
#include "multithreading_base.h"

typedef unsigned char byte;

namespace zfsfilter {

     /** enum type for edgeanalysis
     */
     enum class EdgeAnalDirType: uint8_t { EDGEANALDIR_UNCHOOSEN = 0, EDGEANALDIR_ROW_WISE = 1, EDGEANALDIR_COLUMN_WISE = 2, EDGEANALDIR_ROW_AND_COLUMN_WISE = 3 };

     /** \class EdgeAnalysisIntensity 
     * \brief  Class implementing mixed pixel filtering based intensity drop at edges due to mixed pixel effect. Template for different types of range measurement input.
     *
     * \author Juergen Holzner
     *
     * Class implementing mixed pixel filtering based intensity drop at edges due to mixed pixel effect. Template for different types of range measurement input.
     *
     */
    template<class T> class FILTERDLL_API EdgeAnalysisIntensity: virtual public TileFilter, virtual public MultithreadingBase
    {

    private:
        // -- Class Constants --------------------------------------------------------------------------------
        static const bool            m_c_WriteDebugFiles;
                                     
        static const int32_t         m_c_clean_size;
        static const T               m_c_EdgeThreshold_dD;
                                     
        static const int32_t         m_c_MinNoOfLinesForMTProcessing;
        static const double          m_c_UseProcessorPortion;
                                     
        static const std::wstring    m_c_DebugFileOutputDirectory;

        // -- Member Variables ------------------------------------------------------------------------------
        bool                         m_InputMatricesOK;
        float                        m_MaxRelIChange;
        int32_t                      m_MeanMeasWidth;
        T                            m_dT;
        int32_t                      m_ndT;

        EdgeAnalDirType              m_EdgeAnalDir;

        // -- Data -----------------------------------------------------------------------------------------
        // -- Input ----------------------------------------------------------------------------------------
        Matrix<T>                   *m_pD;
        Matrix<int32_t>             *m_pI;
        Matrix<bool>                *m_pMaskI;
        Matrix<uint16_t>            *m_pMPB_r;
        Matrix<uint16_t>            *m_pMPB_a;
        Matrix<bool>                *m_pMaskMPExternal;

        // -------------------------------------------------------------------------------------------------

        // -- Member Functions (private)
        void basic_initialization();

        void CalculateEdgeMasks( const Matrix<T>   &                 D, // in: range in m
                                 const Matrix<bool>&             MaskI, // in: valid pixels [true/false]
                                 const T           &           ThreshD, // in: Threshold in m
                                 const int32_t     &         ThreadIdx, // in: Thread index
                                 Matrix<byte>      &         MaskEdgeX, // out: edge mask for x-direction
                                 Matrix<byte>      &         MaskEdgeY);// out: edge mask for y-direction

        void AddExternalMPMaskToEdgePixelMask( const Matrix<bool>& MaskMPExternal,
                                               Matrix<byte>      &   MaskEdgeX_MP,
                                               Matrix<byte>      &   MaskEdgeY_MP);

        void GetOptimalEdgeMask( const Matrix<T>   &         D,  // in: distances
                                 const Matrix<bool>&     MaskI,  // in: info on valids
                                 const T           &        dT,  // in: threshold step
                                 const int32_t     &       ndT,  // in: number of threshold trials
                                 const int32_t     & ThreadIdx,  // in: cur. thread idx
                                 Matrix<byte>      & MaskEdgeX,  // in/out: Optimum Edge Mask for X direction
                                 Matrix<byte>      & MaskEdgeY); // in/out: Optimum Edge Mask for Y direction

        void ConsiderIntensityAroundEdgesRowWise( const Matrix<int32_t>       &                  I,  // in: intensity [1]
                                                  const Matrix<uint16_t>      &              MPB_a,  // in: mp width in azimuth direction
                                                  const Matrix<byte>          &       MaskEdgeX_MP,  // in: mask containing information on edges and mixed pixel
                                                  const Matrix<bool>          &              MaskI,  // in: information on invalids
                                                  const float                 &      MaxRelIChange,  // in: maximum relative change for intensity [1]
                                                  const int32_t               &    mean_meas_width,  // in: width of area within which the mean of the intensity is measured
                                                  Matrix<bool>                & MaskIntensityDropX); // out: mps from consideration of intensity around edges

    protected:

        void calculateMaskST(); /// < Singlethreaded
        void calculateMaskMT(); /// < Multithreaded

        void calculateMask( const Matrix<T>               &                 D,
                            const Matrix<int32_t>         &                 I,
                            const Matrix<bool>            &             MaskI,
                            const Matrix<uint16_t>        &             MPB_r,
                            const Matrix<uint16_t>        &             MPB_a,
                            const Matrix<bool>            &    MaskMPExternal,
                            const int32_t                 &        thread_idx,
                            Matrix<bool>                  & MaskIntensityDrop);

        // -- member functions needed for multithreaded version
        void setMatrices( Matrix<T>       & a_D             ,
                          Matrix<int32_t> & a_I             ,
                          Matrix<bool>    & a_MaskI         ,
                          Matrix<uint16_t>& a_MPB_r         ,
                          Matrix<uint16_t>& a_MPB_a         ,
                          Matrix<bool>    & a_MaskMPExternal);

        static void calculateMaskThreadWorker( EdgeAnalysisIntensity* const pEdgeAnalysisIntensity,
                                               const int32_t              & line_start            ,
                                               const int32_t              & line_end              ,
                                               const int32_t              & line_offset_start     ,
                                               const int32_t              & line_offset_end       ,
                                               const int32_t              & thread_idx);

        bool checkOnInputOK();

    public:

        // -- public member constants
        static const int32_t m_c_MeanMeasWidth;
        static const int32_t m_c_MaxThresholdMult;
        static const T       m_c_ThresholdBaseVal;

        static const float  m_c_MaxRelIntChangeValues[3];

        // -- member functions (public)
        EdgeAnalysisIntensity();

        EdgeAnalysisIntensity( Matrix<T>            & a_D                    ,
                               Matrix<int32_t>      & a_I                    ,
                               Matrix<bool>         & a_MaskI                ,
                               Matrix<uint16_t>     & a_MPB_r                ,
                               Matrix<uint16_t>     & a_MPB_a                ,
                               Matrix<bool>         & a_MaskMPExternal       ,
                               const float          & a_m_MaxRelIChange      ,
                               const int            & a_m_MeanMeasWim_dTh    ,
                               const T              & a_m_dT                 ,
                               const int            & a_nm_dT                ,
                               const EdgeAnalDirType& a_Dir                  ,
                               const bool           & a_bProcessMultithreaded);

        void init_parameter( Matrix<T>            & a_D                    ,
                             Matrix<int32_t>      & a_I                    ,
                             Matrix<bool>         & a_MaskI                ,
                             Matrix<uint16_t>     & a_MPB_r                ,
                             Matrix<uint16_t>     & a_MPB_a                ,
                             Matrix<bool>         & a_MaskMPExternal       ,
                             const float          & a_m_MaxRelIChange      ,
                             const int            & a_m_MeanMeasWim_dTh    ,
                             const T              & a_m_dT                 ,
                             const int            & a_nm_dT                ,
                             const EdgeAnalDirType& a_Dir                  ,
                             const bool           & a_bProcessMultithreaded);

        void filter();

    };
} // namespace zfsfilter

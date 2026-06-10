#pragma once

#include <cstdlib>

#include <zfs/Math/math/range.h>

#include "filterapi.h"

#include "MixedPixelFilterDef.h"

#include "Matrix.h"
#include "tilefilter.h"
#include "multithreading_base.h"

#include "MixedPixelKalmanFilter.h"
#include "PostProcessLinearSections.h"
#include "PostProcessLinearSectionsYe.h"

namespace zfsfilter
{
    // =========================================================================================================
    // Struct MixedPixelCandKalmanData
    // =========================================================================================================
    template<typename T> class FILTERDLL_API MixedPixelCandKalmanData
    {
        public:
        // data members
        ProcessingDirectionType                                    m_ProcDir;
        MixedPixelKalmanFilterParameter<T> m_MixedPixelKalmanFilterParameter;

        // constructor for setting the values
        MixedPixelCandKalmanData( const ProcessingDirectionType           &                          a_ProcDir,
                                  const MixedPixelKalmanFilterParameter<T>&  a_MixedPixelKalmanFilterParameter)
            : m_ProcDir( a_ProcDir ),
              m_MixedPixelKalmanFilterParameter( a_MixedPixelKalmanFilterParameter ){}
    };

    // =========================================================================================================
    // Class MixedPixelKalmanProcessing
    // =========================================================================================================
    template<typename T> class FILTERDLL_API MixedPixelKalmanProcessing: public TileFilter, public MultithreadingBase
    {
        friend class MixedPixelFilterEnhanced;

    protected:
        // -- class constants -------------------------------------------
        static const double           m_c_UseProcessorPortion;

        // -- members ---------------------------------------------------
        bool                          m_bProcessPaperYe;

        // -- Data ----------------------------------------------------------------------------------------------------------------------------------

        // -- Input ---------------------------------------------------------------------------------------------------------------------------------
        ProcessingParameter<T>        m_ProcessingParameter;
        ProcessingDirOptionsType      m_FilterDirectionOption;

        Matrix<int32_t>*              m_pI;              ///< Measured Intensity    [inc]
        Matrix<T>*                    m_pD;              ///< Measured Distance     [m]

        Matrix<T>*                    m_pNoise;          ///< Expected Noise Figure [m]
        Matrix<T>*                    m_pNoise3Sigma;    ///< Expected Noise Figure 3 Sigma [m]
        Matrix<T>*                    m_pNoiseVar;       ///< Expected Noise Figure (Square = Variance) [m²]
        Matrix<T>*                    m_pNoiseVar3Sigma; ///< Expected Noise Figure (Square = Variance) 3 Sigma[m²]

        Matrix<bool>*                 m_pIntensityMask;  ///< Mask indicating (I lt Imin) OR (I gt Imax)

        Matrix<uint16_t>*             m_pMPB_r;          ///< Expected number of mixed pixel (at an edge) in elevation direction
        Matrix<uint16_t>*             m_pMPB_a;          ///< Expected number of mixed pixel (at an edge) in cross-elevation direction

        // -- Intermediate --------------------------------------------------------------------------------------------------------------------------
        Matrix<byte>                  m_MaskHor, m_MaskVert, m_MaskDiag45, m_MaskDiag135;
        Matrix<byte>                  m_MixedPixelCand;

        Matrix<bool>                  m_LinearSectionsMatrixForw, m_LinearSectionsMatrixBackw;

        // -- main functions for conducting kalman filtering and combining candidate matrices --------------------------------------------------------------
        void MixedPixelCandKalman( const T&                                   AngleStep, // in: scanning step angle for processing direction in rad
                                   const T&                      ScannerTypeNoiseOffset, // in: scanner double dependent offset for lower border of noise figure, 5006 contains sawtooth effect
                                   const T&                                        QVar, // in: process variance (preset variability)
                                   const ProcessingDirectionType   ProcessingDirection ); // in: 'vertical','horizontal','diag45','diag135': all four directions in which a matrix can be processed

        // -- Multithreading of Kalman based Mixed Pixel Processing
        void        MixedPixelCandKalmanProcessMT( const MixedPixelCandKalmanData<T>& a_MixedPixelCandKalmanData, const zfs::math::Range<int32_t>& aProcessingRange );
        static void MixedPixelCandKalmanThreadWorker( MixedPixelKalmanProcessing<T>* const a_MixedPixelKalmanProcessing, const MixedPixelCandKalmanData<T>& a_MixedPixelCandKalmanData, const zfs::math::Range<int32_t>& ProcessingRange );
        void        MixedPixelCandKalmanProcessSection( const MixedPixelCandKalmanData<T>& a_MixedPixelCandKalmanData, const zfs::math::Range<int32_t>& aProcessingRange );

        // -- Combination of Masks ------------------------------------------------------------------------------------------------------------------
        void CombineFBCandInfo( const std::vector<byte>& Mask_Line,        // IN:  Mask_Line:          Candidate line for 'forward' analysis direction
                                std::vector<byte>& MaskOutBack_Line );//      MaskOutBack_Line:   Candidate line for 'backward' analysis direction
                                                                     // OUT: MaskOutBack_Line:   Result line, returned in MaskOutBack_Line

        void CombineCandMatrices( const Matrix<byte>            &     MaskVert, // in: Candidate matrix for vertical analysis direction
                                  const Matrix<byte>            &      MaskHor, // in: Candidate matrix for horizontal analysis direction
                                  const Matrix<byte>            &   MaskDiag45, // in: Candidate matrix for diagonal (45°) analysis direction
                                  const Matrix<byte>            &  MaskDiag135, // in: Candidate matrix for diagonal (135°) analysis direction
                                  const ProcessingDirOptionsType&      ProcDir, // in: Processing Direction
                                  Matrix<bool>                  & CombinedCand );// out: Combined mixed pixel mask from input candidate information

        // Multithreading of CombineCandMatrices
        void CombineCandMatricesMT( const Matrix<byte>            &     MaskVert,// in: Candidate matrix for vertical analysis direction
                                    const Matrix<byte>            &      MaskHor,// in: Candidate matrix for horizontal analysis direction
                                    const Matrix<byte>            &   MaskDiag45,// in: Candidate matrix for diagonal (45°) analysis direction
                                    const Matrix<byte>            &  MaskDiag135,// in: Candidate matrix for diagonal (135°) analysis direction
                                    const ProcessingDirOptionsType&      ProcDir, // in: Processing Direction
                                    Matrix<bool>                  & CombinedCand );

        static void CombineCandMatricesSection( const Matrix<byte>            &        MaskVert, // in: Candidate matrix for vertical analysis direction
                                                const Matrix<byte>            &         MaskHor, // in: Candidate matrix for horizontal analysis direction
                                                const Matrix<byte>            &      MaskDiag45, // in: Candidate matrix for diagonal (45°) analysis direction
                                                const Matrix<byte>            &     MaskDiag135, // in: Candidate matrix for diagonal (135°) analysis direction
                                                const ProcessingDirOptionsType&         ProcDir, // in: Processing Direction
                                                const zfs::math::Range<int>      & ProcessingRange, // in: Processingrange
                                                Matrix<bool>                  *    CombinedCand );

    public:

        MixedPixelKalmanProcessing();

        MixedPixelKalmanProcessing( Matrix<int32_t>             &  a_I,
                                    Matrix<T>                   &  a_D,
                                    Matrix<T>                   &  a_Noise,
                                    Matrix<T>                   &  a_Noise3Sigma,
                                    Matrix<T>                   &  a_NoiseVar,
                                    Matrix<T>                   &  a_NoiseVar3Sigma,
                                    Matrix<bool>                &  a_MaskIModified,
                                    Matrix<uint16_t>            &  a_MPB_r,
                                    Matrix<uint16_t>            &  a_MPB_a,
                                    const ProcessingParameter<T>&  a_ProcessingParameter,
                                    const zfs::DatasetParameter &  a_DataSetParameter,
                                    const int32_t                  a_NumberOfTilesToProcess,
                                    const ProcessingDirOptionsType a_ProcessingDirOption,
                                    const bool                     a_bProcessPaperYe );

        void Init( Matrix<int32_t>             &  a_I,
                   Matrix<T>                   &  a_D,
                   Matrix<T>                   &  a_Noise,
                   Matrix<T>                   &  a_Noise3Sigma,
                   Matrix<T>                   &  a_NoiseVar,
                   Matrix<T>                   &  a_NoiseVar3Sigma,
                   Matrix<bool>                &  a_MaskIModified,
                   Matrix<uint16_t>            &  a_MPB_r,
                   Matrix<uint16_t>            &  a_MPB_a,
                   const ProcessingParameter<T>&  a_ProcessingParameter,
                   const zfs::DatasetParameter &  a_DataSetParameter,
                   const int32_t                  a_NumberOfTilesToProcess,
                   const ProcessingDirOptionsType a_ProcessingDirOption,
                   const bool                     a_bProcessPaperYe );

        void filter( const int32_t a_TileCount );

        const Matrix<bool>& getMaskResult() const;
    };
}

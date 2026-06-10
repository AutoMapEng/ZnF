#pragma once

// -- api macro definitions
#include "filterapi.h"

// -- crt
#include <cstdint>
#include <string>
#include <vector>

// -- definitions
#include "basic/all.h"
#include "MixedPixelFilterDef.h"

// -- base classes
#include "multithreading_base.h"
#include "tilefilter.h"

// -- filter components
#include <zfs/Tools/scanfilecopyhelper.h>
#include <zfs/laserspotsize.h>

#include <zfs/scaninfo.h>

#include <zfs/Math/math/tuple.h>

#include "datasetparam.h"
#include "mixedpixelwidth.h"

#include "Matrix.h"
#include "RollingBuffer.h"

// -- main filter classes
#include "MixedPixelKalmanProcessing.h"
#include "EdgeAnalysisIntensity.h"
#include "SmoothingFilter.h"
#include "SinglePixelFilter.h"
#include "NormalsAnalysisFilter.h"

// -- filter components forward declarations
class ZFSLinebyline;
class ZFScanZFS;
class ZFLineByLineWrite;
class ZFScan;

namespace zfsfilter
{
    namespace lbl
    {
    // ---------------------------------------------------------------------------------------------------------------------

    /** \class MixedPixelFilterEnhancedLBL
     * \brief  Main class for conducting mixed pixel filtering according to given requirements.
     *         Vereinfachte version des MixedPixelFilterEnhanced die nur mit lbl läuft
     *
     * \author Juergen Holzner
     *
     * Main class for conducting mixed pixel filtering according to given requirements.
     * Mixed pixel filtering on imager and profiler data. Multithreaded.
     *
     */

    class FILTERDLL_API MixedPixelFilterEnhancedLBL : virtual public TileFilter, public MultithreadingBase // virtuelle Vererbung erfordert explizite Initialisierung von Basisklassen in höheren Vererbungsstufen
    {
    public:

        // ---------------------------------------------------------------------------------------------------------------------
        // -- Public Member Functions ------------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------

        // -- constructors
        MixedPixelFilterEnhancedLBL() = delete;
        MixedPixelFilterEnhancedLBL(const std::wstring& a_FilePath,
            const int32_t& a_PixelStart,
            const int32_t& a_PixelEnd,
            const int32_t& a_LineStart,
            const int32_t& a_LineEnd,
            ParamsMixedpixelNew a_Params,
            RollingBuffer::cb_GetProfilerXYZLine_f a_GetProfilerXYZLineFunction = nullptr);

        // -- destructor
        ~MixedPixelFilterEnhancedLBL();

        // -- filter function
        FilterResultItem filter();

        // -- getter
        int32_t          getNoOfMaskedPixel() const;
        FilterResultItem getFilterResult() const;
        FilterResultItem getFilterStatus() const;
        std::wstring     getOutputFilePath() const;

        // -- setter
        void setProfileProgressFeed(const float& aProfileProgressFeed);

    public:
        // ---------------------------------------------------------------------------------------------------------------------
        // -- Public Class Constants --------------------------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------
        static const int32_t      m_c_TileSize_Imager;
        static const int32_t      m_c_TileSize_Profiler;
        static const int32_t      m_c_Overlap_Imager;
        static const int32_t      m_c_ReloadSize_Imager;
        static const int32_t      m_c_Default_TileSize;
        static const int32_t      m_c_Default_Overlap;
        static const int32_t      m_c_Overlap_Profiler;
        static const int32_t      m_c_ReloadSize_Profiler;

    protected:
        // ---------------------------------------------------------------------------------------------------------------------
        // -- Protected Class Constants --------------------------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------
        static const bool         m_c_bProcessPaperYe;
        static const bool         m_c_bUseSegmentationInNormalEval;
#ifdef EN_MIXPIXENH_SINGLE
        static const bool         m_c_bUseInternalSinglePixelFilter;
#endif
        static const bool         m_c_bUseMPWaFromVerticalAngle;

        static const int32_t      m_c_MessageBufferLength;

        static const int32_t      m_c_NoOfPixelToReadIn;

        static const int32_t      m_c_MPWMedianFilterSize;

        static const float        m_c_SegmentationThreshold;

        static const float        m_c_DefaultProfileFeed;

        static const float        m_c_profile_sampling_shift_noise_figure[3];
        static const float        m_c_profile_sampling_shift_noise_figure_zero[3];

        static const float        m_c_MPW_Factor;

        static const int32_t      m_c_MinSizeForMTProcessing;
        static const double       m_c_UseProcessorPortion;

        // -- debug support from tilefilter
        static const bool         m_c_WriteDebugFiles;
        static const std::wstring m_c_DebugFileOutputDirectory;
        static const std::wstring m_c_IntermediateFileOutputDirectory;

        // ---------------------------------------------------------------------------------------------------------------------
        // -- Protected Member variables ---------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------

        // -- Basic Information ---------------------------------------------------------------------------------------------------------------------
        FilterStrengthOptionType               m_FilterStrengthOption; /// < for profiler only (for now)
        ProcessingDirOptionsType               m_FilterDirectionOption;/// < procssing direction option

        // -- Filtering Options for Mixed Pixel Filtering ---------------
        bool                                   m_bSmooth;
        bool                                   m_bEdgeAnalysis;
        bool                                   m_bNormalAnalysis;
        bool                                   m_bKalmanAnalysis;
        bool                                   m_bMaskWasGenerated;

        bool                                   m_bMaskToOriginal;

        // -- Data Management / Tile Information ----------------------------------------------------------------------------------------------------
        bool                                   m_bExternalOpen;
        bool                                   m_bProfileSectionIsFiltered;      /// < indicates if a (profile) section (important for correctly considering smoothing result) of the scan is filtered
//        bool                                   m_bUseZFScan;

        // -- Filter Parameter ----------------------------------------------------------------------------------------------------------------------
        int32_t                                m_SmootherInvalidPixels; // < no. of invalid pixels from smoothing if selected
        float                                  m_MPW_Factor;            // < factor for heuristically reducing the max number of mixed pixel
        double                                 m_FilterStrengthValue;   // < for imager

        // -- Profiler : - Progress for use of Mixed Pixel Filter -----------------------------------------------------------------------------------
        float                                  m_ProfileProgressFeed;

        // -- Parameter -----------------------------------------------------------------------------------------------------------------------------
        ProcessingParameter<float>             m_ProcessingParameter;

        // -- Laser Properties
        zfs::RangeNoise                        m_RangeNoise;
        zfs::LaserSpotDiameter                 m_LaserSpotDiameter;  ///< laser spot diameter variation
        MixedPixelWidthLUT<uint16_t>           m_MixedPixelWidths;   ///< mixed pixel width variation

        // -- First angle/Encoder value of line for smoothing/calculation of mixed pixel width -----------------------------------------------------------------------------------------------------
        std::vector<int>                       m_VertEncoderFirstPixelInLine; /// < vertical angle of first pixel in line
        std::vector<float>                     m_VertAnglesFirstPixelInLine;  /// < vertical angle of first pixel in line
        std::vector<float>                     m_MutualLineShiftsTile;        /// < mutual line shifts in tile in [pixel] required for smoothing the data (bSmooth == true)

        // -- Navigation/Calibration Information and Processing Function-----------------------------------------------------------------------------
        std::vector<zfs::math::PointND<float, 3>> m_ProfilerTrackPositions; /// < positions of profiler for normal analysis

        // -- Data ----------------------------------------------------------------------------------------------------------------------------------
        Matrix<int32_t>                        m_I;                  ///< Measured Intensity    [inc]
        Matrix<float>                          m_D;                  ///< Measured Distance     [m]

        Matrix<float>                          m_X;                  ///< x-Position            [m]
        Matrix<float>                          m_Y;                  ///< y-Position            [m]
        Matrix<float>                          m_Z;                  ///< z-Position            [m]

        Matrix<float>                          m_Noise;              ///< Expected Noise Figure [m]
        Matrix<float>                          m_Noise3Sigma;        ///< Expected Noise Figure 3 Sigma [m]
        Matrix<float>                          m_NoiseVar;           ///< Expected Noise Figure (Square = Variance) [m²]
        Matrix<float>                          m_NoiseVar3Sigma;     ///< Expected Noise Figure (Square = Variance) 3 Sigma[m²]
        Matrix<float>                          m_Dsmoothed;          ///< Smoothed values for D [m]

        Matrix<bool>                           m_MaskI;              ///< Mask indicating (I lt Imin) OR (I gt Imax)
        Matrix<bool>                           m_MaskIModified;      ///< Mask indicating (I lt Imin) OR (I gt Imax)

        Matrix<float>                          m_VerticalAngles;     ///< Vertical angles 
        Matrix<uint16_t>                       m_MPW_r;              ///< Expected number of mixed pixel (at an edge) in elevation direction
        Matrix<uint16_t>                       m_MPW_a;              ///< Expected number of mixed pixel (at an edge) in cross-elevation direction

        Matrix<bool>                           m_MaskMixedPixel;     ///< Determined Mixed Pixel Mask for tile

        // -- full data matrices -------------------------------------------------------------------------------------------------------------------
        Matrix<bool>                           m_MaskMixedPixelFull; ///< Determined Mixed Pixel Mask for all tiles

        // -- Data Statistics ----------------------------------------------------------------------------------------------------------------------
        // statistics of current tile
        MPWStatistics<uint16_t>                m_MPWStatistics;

        // -- Data Management ----------------------------------------------------------------------------------------------------------------------
        RollingBuffer                          m_RB;

        // -- Evaluation of Intensity Around Edges --------------------------------------------------------------------------------------------------
        MixedPixelKalmanProcessing<float>      m_MixedPixelKalmanProcessing;

        // -- Evaluation of Intensity Around Edges --------------------------------------------------------------------------------------------------
        EdgeAnalysisIntensity<float>           m_EdgeAnalysisIntensity;

        // -- Analysis of Normals of point cloud ----------------------------------------------------------------------------------------------------
        NormalAnalysis<float>                  m_NormalAnalysis;

#ifdef EN_MIXPIXENH_SINGLE
        // -- Single Pixel Filter after Mixed Pixel Filter ------------------------------------------------------------------------------------------
        SinglePixelFilter<float>               m_SinglePixelFilter;
#endif

        // -- Smoothing of Distance Values Based On Noise Figure ------------------------------------------------------------------------------------
        SmoothingFilter<float>                 m_Smoother;

        // -- Input Data file -----------------------------------------------------------------------------------------------------------------------
        ZFSLinebyline* m_readZFS_LBL{};
//        ZFScanZFS* m_readZFS_ZFScan;

        // -- Corrected Output Data file ------------------------------------------------------------------------------------------------------------
        ZFLineByLineWrite* m_writeZFS_LBL;

        // ---------------------------------------------------------------------------------------------------------------------
        // -- Protected Member Functions _--------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------

        // -- important initializations
        void prepare_filteringZFSLinebyline();
//        void prepare_filteringZFScan();

        void set_processing_parameter();
        void set_basic_buffer_management_parameter();
        void set_laser_system_parameter(ZFSHeader* const apZFSHeader);

        void setDataSetParameter(ZFSHeader* const apZFSHeader,
            const int32_t& a_LineSubsample = 1,
            const int32_t& a_PixelSubsample = 1); // overloaded function in basefilter

// -- filtering
        void feed_data_matrices();
        void initialize_working_matrices();
        void get_mutual_line_shifts();
        void calculate_input_matrices(const int32_t& line_start, const int32_t& line_end);
        void smooth_MPW_and_calc_statistics(const int32_t& line_start, const int32_t& line_end, MPWStatistics<uint16_t>* const pMPWStatistics);
        void do_filtering_of_tile();
        void print_output_report_message();

        virtual void filter_config_check();

        // -- multithreading functions
        // -- intializations of matrices
        void initializeMT();
        static void initializeThreadWorker(MixedPixelFilterEnhancedLBL* const pMixedPixelFilterEnhancedLBL, MPWStatistics<uint16_t>* const pMPWStatistics, const int32_t& line_start, const int32_t& line_end);

        // -- mask information to output mask
        void updateOutputMaskMT(const zfs::math::BoundingBox& BoundingBoxSource,
            const zfs::math::BoundingBox& BoundingBoxTarget);

        void updateOutputMask(const Matrix<bool>& MaskI,
            const zfs::math::BoundingBox& BoundingBoxSource,
            const zfs::math::BoundingBox& BoundingBoxTarget,
            Matrix<bool>& MaskMixedPixel,
            Matrix<bool>& MaskMixedPixelFull);

        static void updateOutputMaskThreadWorker(MixedPixelFilterEnhancedLBL* const  pMixedPixelFilterEnhancedLBL,
            const zfs::math::BoundingBox& BoundingBoxSource,
            const zfs::math::BoundingBox& BoundingBoxTarget,
            const int32_t& line_start,
            const int32_t& line_end,
            const int32_t& thread_idx);
        // -- multithreading functions end

        void CombineBoolMaskMT(Matrix<bool>& a_Matrix_1, const Matrix<bool>& a_Matrix_2, const std::string& a_strOperator); // in: Matrix_1
                                                                                          // in: Matrix_2
                                                                                          // out: Matrix_1: Matrix_1|=Matrix_2.
        static void CombineBoolMaskMT_ThreadWorker(Matrix<bool>* const a_Matrix_1, const Matrix<bool>* const a_Matrix_2, const std::string& a_strOperator, const int32_t& line_start, const int32_t& line_end);

        // -- function for evaluating small objects in front of low intensity areas --------------------------------------------
        void GenMaskKeepSmallElementsInLowIntAreas(const Matrix<bool>& a_MaskI, const bool& bOnlyAlongProfile, Matrix<bool>& a_PositiveMaskForSmallElementsconst);
        void GenMaskKeepSmallElementsInLowIntAreas2(const Matrix<bool>& a_MaskI, const Matrix<bool>& a_MaskMixedPixel, const bool& bOnlyAlongProfile, Matrix<bool>& a_PositiveMaskForSmallElementsconst);
        // -- function for evaluating small objects in front of low intensity areas end ----------------------------------------

    private:

        // ---------------------------------------------------------------------------------------------------------------------
        // -- Private Member variables -----------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------

        // -- Basic Information ------------------------------------------------------------------------------------------------
        bool                                   m_bDiscardOriginal;
        bool                                   m_bOutputAllDerivedDataMatrices;

        RollingBuffer::cb_GetProfilerXYZLine_f m_GetProfilerXYZLineFunction;
        zfs::ScanFileCopyHelper                m_ScanFileCopyHelper;

        // -- Navigation/Calibration Information and Processing Function-----------------------------------------------------------------------------
        std::wstring                           m_NavFileName;
        std::wstring                           m_CalibFileName;

        // ---------------------------------------------------------------------------------------------------------------------
        // -- Private Member Functions -----------------------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------------------------------------
        void                   CloseFiles();
        void                   CloseOutputFile();
        void                   CloseInputFile();
        void                   CleanUpAfterAbort();

        void                   generate_output_file_paths();
        void                   update_tile_info();
        bool                   copy_input_data_set_to_output_directory(int32_t& nNumberOfLinesWritten);

        void                   transfer_data_to_file();

        void                   update_overall_mixed_pixel_mask();
        zfs::zfserr_t          ReOpenInputForMaskWrite_LBL();

        bool                   AddAndTransferMixedPixelMaskToOutputFile_LBL();
//        bool                   AddAndTransferMixedPixelMaskToOutputFile_ZFScan();

        void                   installWriteCallback(RollingBuffer::cb_GetProfilerXYZLine_f a_GetProfilerXYZLineFunction); /// < for profiler filtering using trajectory

        virtual void           initialize(const int32_t& line_start, const int32_t& line_end);

    };



}
} // namespace zfsfilter

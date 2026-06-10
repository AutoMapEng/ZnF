#pragma once

#include <utility>

#include <zfs/Math/2d/point.h>
#include <zfs/Math/3d/vector_nd.h>
#include <zfs/Math/3d/point_nd.h>

#include "filterapi.h"

#include "tilefilter.h"
#include "MPWStatistics.h"
#include "multithreading_base.h"
#include "mixedpixelwidth.h"
#include "Matrix.h"
#include "Segmentation.h"

// forward declarations

namespace zfsfilter
{
    template< class T > class Segmentation;

    class DatasetParameter;
}

namespace zfsfilter {
    // =============================================================================================================
    /** \class NormalAnalysis
     * \brief  Class for detecting mixed pixels based on the evaluation of look direction of scanner and plane normals of the scene.
     *
     * \author Juergen Holzner
     *
     * Class for detecting mixed pixels based on the evaluation of look direction of scanner and plane normals of the scene.
     *
     */

    template<class T> class FILTERDLL_API NormalAnalysis :virtual public TileFilter, public MultithreadingBase
    {
    protected:

        /** Class Constants ***/
        static const bool           m_c_bMaskCombinationModeEnabled;
        static const bool           m_c_bMaskCombinationModeSensitiveCheck;
        static const bool           m_c_bEnableDetailMode;
        static const bool           m_c_bAverargeNormalsAcrossProfilesAccOverlap;
        static const bool           m_c_EnableLogging;

        static const int32_t        m_c_GaussWindowExtendForNormalDiffNormEval;
        static const int32_t        m_c_NoIndepPixUsedNormalEst;

        static const float          m_c_GaussWindowSigmaForNormalDiffNormEval;
        static const float          m_c_DistanceStepForAngleThreshold;
        static const float          m_c_DropFactorForAngleThreshold;
        
        static const float          m_c_NormNormalDiffThreshold_along_profile;
        static const float          m_c_NormNormalDiffThreshold_cross_profile;

        static const float          m_c_DiffNormalsThresh1MPEdges;

        static const int32_t        m_c_MinNoOfLinesForMTProcessing;
        static const double         m_c_UseProcessorPortion;

        /** Types **/

        /** Enums **/
        typedef enum NORMAL_ANALYSIS_EVALUATION_OPTION_SET
        {
            NAEO_UNDEF = 0, NAEO_SIMPLE_ANGLE_THRESHOLD, NAEO_ANGLE_THRESHOLD_PLUS_NORMAL_DIFF_EVAL, NAEO_MASK_COMBINATION
        } normal_analysis_evaluation_option_t;

        /** Member Variables ***/
        bool                                    m_bExternalOpen;
        bool                                    m_bDeleteSpace;
        bool                                    m_bThresholdAngleSet;
        bool                                    m_bNormalsNeighborhoodCalculated;
        bool                                    m_bIsProfiler;

        bool                                    m_bDistanceDependendThresholding;
        bool                                    m_bUseNormalDiffNormEval;

        bool                                    m_bUse4InsteadOf8Neighbours;

        bool                                    m_bOutputProgress;

        normal_analysis_evaluation_option_t     m_EvaluationOption;

        unsigned short                          m_ExpectedMPWmax;
        unsigned short                          m_MPWr_TreshNormalEst;
        unsigned short                          m_MPWa_TreshNormalEst;

        int32_t                                 m_MaxLUTIdx;

        float                                   m_MPW_FactorNormalEst; // 0. ... 1.
        float                                   m_FilterStrengthValue; // 0. ... 1.
        float                                   m_MPW_FactorThreshAngles; // 0. ... 1.

        float                                   m_ThresholdAngle;

        std::vector<float>                      m_CriticalEdgesNearAccToFiltStrength;
        std::vector<float>                      m_CriticalEdgesFarMaxDistFacAccToFiltStrength;

        std::vector<double>                     m_vThreshold_dD;

        std::vector<float>                      m_vThresholdAngleVsDistance;
        Matrix<float>                           m_ThresholdAngleVsDistance;

        zfs::math::PointND<T, 3>                   m_PointCloudCenter;

        // in case of profiler ... multiple centers
        std::vector<zfs::math::PointND<T, 3>>      m_ProfilerCenterPositions;

        zfs::math::Point<int32_t>                  m_UpperLeftImagePosition;            // < position of provided (sub)matrix in scan

        // MPW statistics
        MPWStatistics<uint16_t>                 m_MPWStatistics;                     // < object for the MPW statistics


        // generated matrices
        Matrix<bool>                            m_MaskNNDiff;                        // < indication matrix as mask from norm of normal differences with the used thresholds in normal analysis
        Matrix<int>                             m_PixelsAssigned;                    // < indication matrix for which pixels were assigned
        Matrix<byte>                            m_MaskingIndicator;                  // < allow to indicate on which reason the pixel was masked out (invalid in neighborhood or due to angle)
        Matrix<float>                           m_CosAngles;                         // < matrix of cos(alpha) values

        Matrix<int32_t>                         m_SamplingMatrixNormalEst;           // < determines where normal estimation is to be conducted when MPW exceeds a certain threshold (save comp. power)
        Matrix<uint16_t>                        m_AssignRadaNormalEst;               // < determines the Estimation and Assign Size in Cross-Profile direction for every sampling point in m_SamplingMatrixNormalEst
        Matrix<uint16_t>                        m_AssignRadrNormalEst;               // < determines the Estimation and Assign Size in Along-Profile direction for every sampling point in m_SamplingMatrixNormalEst

        std::vector<Matrix<bool>>               mvM_ThresholdingMasks;

        // pointer to used (input) matrices
        Matrix<bool>                           *m_pMaskI;                            // < Intensity Mask
        Matrix<uint16_t>                       *m_pMPW_a, *m_pMPW_r;                 // < MPW across, along Profiles

        Matrix<T>                              *m_pX, *m_pY, *m_pZ;                  // < x,y,z - Positions in Matrix      [m]
        Matrix<T>                              *m_pD;                                // < Laser Range                      [m]
        Matrix<T>                              *m_pVAng;                             // < Vertical Pointing Angle of Laser [rad]

        Matrix<float>                          m_NX, m_NY, m_NZ;                     // < Normal matrices
        Matrix<float>                          m_NNdx, m_NNdy;                       // < norm of pixel by pixel difference of Normal matrices (last column= 0)
        Matrix<float>                          m_NNdx_smoothed, m_NNdy_smoothed;     // < smoothed norm of pixel by pixel difference of Normal matrices (last column= 0)

        Segmentation<float>                    m_Segmentation;                      // < segmentation object

        /** Protected Member Functions ***/

        // -- basic internal helper functions
        void basic_initialization();
        void basic_initialization_of_pointers();

        void initializeThresholds();
        void setFilterParameter();

        void freeSpace();

        /**
        * Function (inline) to set masking info (keep code simple)
        * \param m: line idx
        * \param n: column idx
        * \param ThresholdAngleLUTIndex: index into Threshold LUTs
        * \param : (updated) filtered pixel count
        * \author Juergen Holzner
        */
        void setMaskingInfo(int32_t m, int32_t n, int32_t ThresholdAngleLUTIndex, int32_t& filtered_pixel_counter);

        /**
        * Function for eval of norm of normal differences
        * \param  ThresholdAngleLUTIndex: LUT Idx for threshold table
        * \param  filtered_pixel_counter: incremented filtered pixel counter
        * \author Juergen Holzner
        */
        void EvalNormalDiffNorm(const int32_t m, const int32_t n, const int32_t ThresholdAngleLUTIndex, int32_t& filtered_pixel_counter);

        /**
        * Function to determine the necessary sampling for normal estimation (save comp. power), considers vertical scan angle which increases MPWa
        * \author Juergen Holzner
        * \param  StartLine              = StartLine (Working Range Start)
        * \param  StopLine               = StopLine  (Working Range Stop)
        * \param  SamplingMatrixNormalEst= Determined Sampling Matrix
        * \return void
        */
        void GenerateSamplingMatrix(const int32_t StartLine, const int32_t StopLine, Matrix<bool> & SamplingMatrixNormalEst, Matrix<uint16_t>      & AssignRadaNormalEst, Matrix<uint16_t>      & AssignRadrNormalEst) const;

        /**
        * Function to determine the mixed pixel with across line direction (horizontal movement of laser) where the effect of the vertical angle is taken into account
        * \author Juergen Holzner
        * \param  MPW_a                  = Mixed Pixel with across line direction (horizontal movement of laser)
        * \param  VertAngle              = Vertical Angle (increases MPWa)
        * \param  MPW_a_eff              = Mixed Pixel with across line direction (horizontal movement of laser) where the effect of the vertical angle is taken into account
        */
        void DetermineMPWaEffective(const Matrix<uint16_t>      & MPW_a, const Matrix<T>& VertAngle, Matrix<uint16_t>      & MPW_a_eff) const;

        /**
        * Function to determine the necessary sampling for normal estimation (save comp. power)
        * \author Juergen Holzner
        * \param  MPW_a                  = Mixed Pixel with across line diretion (horizontal movement of laser)
        * \param  MPW_r                  = Mixed Pixel with along line diretion (vertical movement of laser)
        * \param  StartLine              = StartLine (Working Range Start)
        * \param  StopLine               = StopLine  (Working Range Stop)
        * \param  SamplingMatrixNormalEst= Determined Sampling Matrix
        * \return void
        */
        void DetermineSamplingMatrix( const Matrix<uint16_t>& MPW_a                  , const Matrix<uint16_t>      & MPW_r,
                                      const int32_t         & StartLine              , const int32_t                   & StopLine,
                                      Matrix<int32_t>       & SamplingMatrixNormalEst,
                                      Matrix<uint16_t>      & AssignRadaNormalEst    , Matrix<uint16_t>            & AssignRadrNormalEst);

        // alternative version
        void DetermineSamplingMatrix2( const Matrix<uint16_t>& MPW_a                  , const Matrix<uint16_t>      & MPW_r,
                                       const int32_t         & StartLine              , const int32_t                   & StopLine,
                                       Matrix<int32_t>       & SamplingMatrixNormalEst,
                                       Matrix<uint16_t>      & AssignRadaNormalEst    , Matrix<uint16_t>            & AssignRadrNormalEst);

        // Function to estimate normals from point cloud given in matrices X,Y,Z (original)
        void estimateNormalsNeighborhoodSection(const int32_t start_line, const int32_t stop_line);
        void estimateNormalsNeighborhoodSectionV0(const int32_t start_line, const int32_t stop_line);

        /**
        * Functions to average the estimated normals
        * mainly across profile direction according to vertical angles to mitigate the effect of extreme
        * laserspot overlap in the region near the "poles". Precondition is that the normal estimation
        * is not already done according to effective MBPa (see MixedPixelKalmanEdgeIntensityFilter)
        */
        void        AverageNormals            ();
        void        AverageNormalsMT          ();
        void        AverageNormalsSection     ( const int32_t start_pixel, const int32_t stop_pixel);
        static void AverageNormalsThreadWorker( NormalAnalysis* pNormalAnalysis, const int32_t start_pixel, const int32_t stop_pixel);

        // Function to calculate normal differences dx,dy in a section starting with start_line, ending at stop_line
        void calculateNormOfNormalDifferencesSection(const int32_t start_line, const int32_t stop_line);

        // Function to smooth normal differences dx,dy in a section starting with start_line, ending at stop_line
        void smoothNormOfNormalDifferencesSection(const int32_t start_line, const int32_t stop_line);

        /**
        * Function to estimate a normal vector at a certain image position (used in estimateNormalsNeighborhood())
        * (new version)
        * \author Juergen Holzner
        * \param Position        = [input]  position at which the normal vector is to be determined
        * \param AssignRad       = [output] radii of assignment field [0]: line, field [1]: column, if any 0: only one pixel or line of width one pixel
        * \return estimated normal vector
        */
        void estimateNormal( zfs::math::Point<int32_t> Position, double& MPW_xp, double& MPW_p, zfs::math::Vector3D<double>& aNormal );
        void estimateNormal( zfs::math::Point<int32_t> Position, double& MPW_xp, double& MPW_p, zfs::math::Vec3d& aNormal );
        void estimateNormalV0(const zfs::math::Point<int32_t>& aPosition, zfs::math::Tuple2<unsigned short>& aAssignRad, bool abNormalize, zfs::math::Vector3D<T>& aNormal);


        /**
        * Function to estimate a normal vector at a certain image position using a Center Point and a Point List (Core/Base calculating function)
        * \author Juergen Holzner
        * \param a_NeighbourList  = [input] a list of neighbouring points
        * \param a_Pcx            = [input] center point x - co - ordinate
        * \param a_Pcy            = [input] center point y - co - ordinate
        * \param a_Pcz            = [input] center point z - co - ordinate
        * \param a_bNormalize     = [input] flag indicating if normal shall be normalized (when not required num. ops can be saved)
        * \return estimated normal vector (:a_Normal)
        */
        void NormalFromCenterPointAndNeighbourhoodList( const zfs::math::IntPointListType& a_NeighbourList,
                                                        const double& a_Pcx, const double& a_Pcy, const double& a_Pcz,
                                                        bool a_bNormalize, zfs::math::Vector3D<double>& a_Normal );

        void NormalFromCenterPointAndNeighbourhoodList( const zfs::math::IntPointListType& a_NeighbourList,
                                                        const zfs::math::Vec3d& a_Pcx,
                                                        bool a_bNormalize, zfs::math::Vec3d& a_Normal );

        // Helper function to calculate the lut-index for distance dependend angle and edge thresholding
        int32_t LUTIdxForTable(double D, double minD, double Dsampling);

        // Function to calculate the distance dependend thresholding angle
        void calculateThresholdAngles(const float &a_FilterStrength, std::vector<float> &a_ThresholdAngles, std::vector<double> &a_Threshold_dD);

        // Function to calculate the distance dependend thresholding angles for mask combination mode
        void calculateThresholdAnglesForMaskCombinationMode();

        // Function to check points from neighborhood
        bool checkAddNeighborPointsToListOnValidity( const zfs::math::Point<int32_t>&            P1,
                                                     const zfs::math::Point<int32_t>&            P2,
                                                     const bool                  &    bAddPoint2,
                                                     zfs::math::IntPointListType    &     PointList) const;

        // Function to estimate a normal vector at a certain image position (used in estimateNormalsNeighborhood()) (old - reference- version)
        zfs::math::VectorND<T, 3> estimateNormalRef(const zfs::math::Point<int32_t>& Position, bool bCalcNormalized);

        // Function to evalute the normals when calculated on its own
        void evaluatePointCloudMatrix();

        // Function to evalute the thresholding of matrix in conjunction with the calculated normals
        void  evaluateThresholding();

        // Function to postprocess/combine mutliple threshold evalution
        void postProcessMultipleThresholdMasks();

        /**
        * Function to check on mixed pixel condition in mask from normal check
        * \author Juergen Holzner
        * \return void (no return)
        */
        bool CheckOnMixedPixelWidthInNormalMask(const int32_t m, const int32_t n, const Matrix<bool>& NormalMask) const;

        // Function that uses segmentation to check/verify mask
        void evaluateMaskAccordingToSegmenation(bool bUsePointLists = false);

        // --- Multithreading ---

        // 1) -- Normal Estimation
        // Implements Multithreading for estimateNormalsNeighborhood() (original version)
        void estimateNormalsNeighborhoodMT();
        void estimateNormalsNeighborhoodMTV0();

        // Function that is the thread worker for the multithreaded normal estimation
        static void estimateNormalsNeighborhoodThreadWorker(NormalAnalysis* pNormalAnalysis, const int32_t line_start, const int32_t line_end);
        static void estimateNormalsNeighborhoodThreadWorkerV0(NormalAnalysis* pNormalAnalysis, const int32_t line_start, const int32_t line_end);

        // 1.a) -- Calculation of normal differences
        // Implements Multithreading for Calculation of normal differences
        void calculateNormOfNormalDifferencesMT();

        // Implements Multithreading for smoothing of normal differences
        void smoothNormOfNormalDifferencesMT();

        // Function to calculate normal differences dx,dy in a thread
        static void calculateNormOfNormalDifferencesThreadWorker(NormalAnalysis* pNormalAnalysis, const int32_t line_start, const int32_t line_end);

        //Function to smooth normal differences dx,dy in a thread
        static void smoothNormOfNormalDifferencesThreadWorker(NormalAnalysis* pNormalAnalysis, const int32_t line_start, const int32_t line_end);

        // 2) -- Evaluation of Point Cloud
        // Implements Multithreading for estimateNormalsNeighborhood()
        void evaluatePointCloudMatrixMT();

        // Function that uses segmentation to check/verify mask
        static void evaluatePointCloudMatrixThreadWorker(NormalAnalysis* pNormalAnalysis, const int32_t line_start, const int32_t line_end, int32_t& filtered_counter);

        // Function to evalute the normals when calculated on its own
        void evaluatePointCloudMatrixSection(const int32_t start_line, const int32_t stop_line, int32_t& filtered_pixel_counter);

        //Function to evalute the normals when calculated on its own for profiler data
        void evaluatePointCloudMatrixSectionProfiler(const int32_t start_line, const int32_t stop_line, int32_t& filtered_pixel_counter);

        // Function to evalute the normals when calculated on its own (distance dependent)
        void evaluatePointCloudMatrixSectionDistanceDependend(const int32_t start_line, const int32_t stop_line, int32_t& filtered_pixel_counter);

        // Function to evalute the normals when calculated on its own (distance dependent) for profiler data
        void evaluatePointCloudMatrixSectionDistanceDependendProfiler(const int32_t start_line, const int32_t stop_line, int32_t& filtered_pixel_counter);

        //Function to evalute the thresholding of matrix in conjunction with the calculated normals (actual calculator)
        void  evaluateThresholding(const Matrix<T>             & X,
                                   const Matrix<T>             & Y,
                                   const Matrix<T>             & Z,
                                   const Matrix<uint16_t>      & MPW_r,
                                   const Matrix<uint16_t>      & MPW_a,
                                   const Matrix<float>         & NX,
                                   const Matrix<float>         & NY,
                                   const Matrix<float>         & NZ,
                                   const bool                  & bNormalsNeighborhoodCalculated,
                                   Matrix<byte>                & MaskingIndicator);

        void  evaluateThresholdingMT();
        static void  evaluateThresholdingThreadWorker( NormalAnalysis* pNormalAnalysis, 
                                                       const zfs::math::Range<int32_t>& ProcessingRange, 
                                                       const zfs::math::Tuple2<int32_t>& LineOffsets, 
                                                       int32_t thread_idx);

        /**
        * Function to consider edges which are not found by normalanalysis because of 3x3 estimation window
        * \author Juergen Holzner
        * \return void (no return)
        */
        void evaluate1MPEdges();
        void evaluate1MPEdges(const Matrix<T>& a_D, const std::vector<double>& a_ThresholdTable, const T& a_D_sampling_mm, Matrix<byte>& a_MaskingIndicator);

        /**
        * Function to conduct don segmentation: see pcl: http://pointclouds.org/documentation/tutorials/don_segmentation.php#don2012
        * \author Juergen Holzner
        * \return void (no return)
        */
        void don_segmentation();

    public:

        //-- Public Class Constants 
        static const float m_c_SegmentationMaskingFractionThreshold;
        static const float m_c_NormalThresholdAngles[3];

        NormalAnalysis();

        NormalAnalysis(Matrix<T>                       & a_X,
                       Matrix<T>                       & a_Y,
                       Matrix<T>                       & a_Z,
                       Matrix<bool>                    & a_MaskI,
                       const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                       const float                       a_ThresholdAngle,
                       const bool                        a_bMultithreaded = true,
                       const bool                        a_bOutputReport = false);

        NormalAnalysis(Matrix<T>                          & a_X,
                       Matrix<T>                          & a_Y,
                       Matrix<T>                          & a_Z,
                       Matrix<T>                          & a_D,
                       Matrix<uint16_t>                   & a_MPW_r,
                       Matrix<uint16_t>                   & a_MPW_a,
                       Matrix<bool>                       & a_MaskI,
                       const MPWStatistics<uint16_t>      & a_MPWStatistics,
                       const zfs::DatasetParameter* const   a_DatasetParameter,
                       const zfs::math::PointND<T, 3>        & a_PointCloudCenter,
                       const zfs::math::Point<int32_t>       & a_ImageUpperLeft,
                       const double                         a_FilterStrengthValue = 0.5,  // 0. ... 1.
                       const bool                           a_bMultithreaded = true,
                       const bool                           a_bOutputReport = false);

        NormalAnalysis(Matrix<T>                                    & a_X,
                       Matrix<T>                                    & a_Y,
                       Matrix<T>                                    & a_Z,
                       Matrix<T>                                    & a_D,
                       Matrix<uint16_t>                             & a_MPW_r,
                       Matrix<uint16_t>                             & a_MPW_a,
                       Matrix<bool>                                 & a_MaskI,
                       const MPWStatistics<uint16_t>                & a_MPWStatistics,
                       const zfs::DatasetParameter* const             a_DatasetParameter,
                       const std::vector<zfs::math::PointND<T, 3>>     & a_ProfilerCenterPositions,
                       const double                                   a_FilterStrengthValue = .5,  // 0. ... 1.
                       const bool                                     a_bMultithreaded = true,
                       const bool                                     a_bOutputReport = false);


        NormalAnalysis(Matrix<T>                       & a_X,
                       Matrix<T>                       & a_Y,
                       Matrix<T>                       & a_Z,
                       Matrix<bool>                    & a_MaskI,
                       const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                       const bool                        a_bMultithreaded = false,
                       const bool                        a_bOutputReport = false);

        NormalAnalysis(Matrix<T>                       & a_X,
                       Matrix<T>                       & a_Y,
                       Matrix<T>                       & a_Z,
                       Matrix<uint16_t>                & a_MPW_r,
                       Matrix<uint16_t>                & a_MPW_a,
                       Matrix<bool>                    & a_MaskI,
                       const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                       const bool                        a_bMultithreaded = false,
                       const bool                        a_bOutputReport = false);

        ~NormalAnalysis();


        void initialization( Matrix<T>                       & a_X,
                             Matrix<T>                       & a_Y,
                             Matrix<T>                       & a_Z,
                             Matrix<bool>                    & a_MaskI,
                             const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                             const bool                        a_bMultithreaded,
                             const bool                        a_bOutputReport );

        void initialization(Matrix<T>                       & a_X,
                            Matrix<T>                       & a_Y,
                            Matrix<T>                       & a_Z,
                            Matrix<uint16_t>                & a_MPW_r,
                            Matrix<uint16_t>                & a_MPW_a,
                            Matrix<bool>                    & a_MaskI,
                            const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                            const bool                        a_bMultithreaded = false,
                            const bool                        a_bOutputReport = false);

        void initialization( Matrix<T>                       & a_X,
                             Matrix<T>                       & a_Y,
                             Matrix<T>                       & a_Z,
                             Matrix<bool>                    & a_MaskI,
                             const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                             const float                       a_ThresholdAngle, // threshold
                             const bool                        a_bMultithreaded= false,
                             const bool                        a_bOutputReport= false );

        void initialization(Matrix<T>                       & a_X,
                            Matrix<T>                       & a_Y,
                            Matrix<T>                       & a_Z,
                            Matrix<uint16_t>                & a_MPW_r,
                            Matrix<uint16_t>                & a_MPW_a,
                            Matrix<bool>                    & a_MaskI,
                            const zfs::math::PointND<T, 3>     & a_PointCloudCenter,
                            const float                       a_ThresholdAngle, // threshold
                            const bool                        a_bMultithreaded = false,
                            const bool                        a_bOutputReport = false);

        void initialization(Matrix<T>                          & a_X,
                            Matrix<T>                          & a_Y,
                            Matrix<T>                          & a_Z,
                            Matrix<T>                          & a_D,
                            Matrix<uint16_t>                   & a_MPW_r,
                            Matrix<uint16_t>                   & a_MPW_a,
                            Matrix<bool>                       & a_MaskI,
                            const MPWStatistics<uint16_t>      & a_MPWStatistics,
                            const zfs::DatasetParameter* const   a_DatasetParameter,
                            const zfs::math::PointND<T, 3>        & a_PointCloudCenter,
                            const zfs::math::Point<int32_t>       & a_UpperLeftImagePosition = zfs::math::Point<int32_t>(0,0),
                            const double                         a_FilterStrengthValue = .5,  // 0. ... 1.
                            const bool                           a_bMultithreaded = true,
                            const bool                           a_bOutputReport = false);

        void initialization(Matrix<T>                                    & a_X,
                            Matrix<T>                                    & a_Y,
                            Matrix<T>                                    & a_Z,
                            Matrix<T>                                    & a_D,
                            Matrix<uint16_t>                             & a_MPW_r,
                            Matrix<uint16_t>                             & a_MPW_a,
                            Matrix<bool>                                 & a_MaskI,
                            const MPWStatistics<uint16_t>                & a_MPWStatistics,
                            const zfs::DatasetParameter* const             a_DatasetParameter,
                            const std::vector<zfs::math::PointND<T, 3>>     & a_ProfilerCenterPositions,
                            const double                                   a_FilterStrengthValue = .5,  // 0. ... 1.
                            const bool                                     a_bMultithreaded = true,
                            const bool                                     a_bOutputReport = false);

        void filter( const int32_t TileIdx = 0,
                     bool bUseSegmenation = false, 
                     const float& SegmentationThreshold = 0.f);
         
        // -- setter
        void setThreshold(const float& a_Threshold);
        void setUse4InsteadOf8Neighbours(bool a_Flag= true);

        // -- accessors to filter results
        const Matrix<float>   & getCosAngles() const;
        const Matrix<bool>    & getNNDiffMask();
        const Matrix<int32_t> & getSamplingMatrixForNormalEst() const;
        const Matrix<int32_t> & getPixelsAssignedMatrix() const;
        const Matrix<int32_t> & getSegmentationResult() const;

        void getNormals(Matrix<float>& a_pNX, Matrix<float>& a_pNY, Matrix<float>& a_pNZ) const;
        void getNormOfNormalDifferences(Matrix<float>& a_pNNdx, Matrix<float>& a_pNNdy) const;
        void getAssignRadMatrices(Matrix<uint16_t>& AssignRada, Matrix<uint16_t>& AssignRadr) const;

        // -- calculators
        void estimateNormals();
        void estimateNormalsV0();
        void calculateNormOfNormalDifferences();
        void evaluatePointCloud();
    };
} // namespace zfsfilter

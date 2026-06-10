#pragma once

#include <vector>

#include <zfs/Math/math/tuple.h>
#include <zfs/Math/fit/poly_fit.h>
#include <zfs/Math/signal_processing/kernels.h>
#include <zfs/Math/signal_processing/kernel_bank.h>

#include "filterapi.h"

#include "multithreading_base.h"
#include "tilefilter.h"

#include "Matrix.h"

namespace zfsfilter
{
    /** \class SmoothingFilter
     * \brief  Class for smoothing range data based on available noise and masking information.
     *
     * \author Juergen Holzner
     *
     * Class for smoothing range data based on available noise and masking information.
     * Used in the mixed pixel filter class as additional/optional processing step.
     *
     */
    template<class T> class FILTERDLL_API SmoothingFilter: public MultithreadingBase, virtual public TileFilter
    {
    public:
        typedef enum directions
        {
            UNDEF = 0, HORIZONTAL = 1, VERTICAL = 2, HORIZONTAL_AND_VERTICAL = 3
        } direction_type;

    private:
        // -- class constants
        static const bool                         m_cUseTransSmoothing;                      ///< indicates if transition smoothing is used
        static const bool                         m_cEnableStrongFilteringNoiseDep;          ///< decides if strong, noise dep. filtering is enabled
        static const bool                         m_cEnableStrongFilteringNoiseInDep;        ///< decides if strong, noise indep. filtering is enabled
        static const bool                         m_cUsePolyExtension;                       ///< indicates if Polynomial extension is used
        static const bool                         m_cGenNoiseInfoFinal;                      ///< indicates if final noise info shall be generated
        static const bool                         m_cProcessMultithreaded;                   ///< for testing, in case of _DEBUG mode Filter is driven in single threaded mode
        static const bool                         m_cEnableXYZConversion;                    ///< decides if provided matrices x,y,z are converted after smoothing of D

        static const int                          m_cTransSmoothFilterLength;                ///< transition smoothing filter length
        static const zfs::math::Tuple2<int>          m_cPolyAssignOffs;                         ///< poly value assignment length reduction (combine with transition smoothing)
        static const zfs::math::Tuple2<int>          m_cDefaultFilterPolyOrders;                ///< order of used poly for fit, used if m_cUsePolyFit==true;
        static const zfs::math::Tuple2<int>          m_cDefaultFilterSizes;                     ///< default size of the filter
        static const zfs::math::Tuple2<int>          m_cStrongFilterPolyOrders;                 ///< strong filter order of used poly for fit
        static const zfs::math::Tuple2<int>          m_cStrongFilterSizes;                      ///< strong filter default size of the filter
        static const zfs::math::Tuple2<int>          m_cMinFilterSize;                          ///< minimum size of the filter

        static const bool                         m_cApplyShiftingOfProfiles;                ///< decides if shifting is applied to profiles before filtering of matrices
        static const zfs::math::KernelType           m_cDefaultResKernelTypeToUse;              ///< can be KS_TWO_POINT (linear interp.), KS_FOUR_POINT, KS_SIX_POINT or KS_TRUNCATED_SINC
        static const int                          m_cDefaultResKernelTruncSincLength;        ///< considered, when trunc. sinc is used
        static const zfs::math::SignalWindowNameType m_cDefaultResKernelWindow4TruncSinc;       ///< considered, when trunc. sinc is used
        static const float                        m_cPixelAccuracyForProfileShifts;          ///< resampling accuracy (subpixel)
        static const float                        m_cProfileShiftTol;                        ///< resampling accuracy tolerance (subpixel)

        static const float                        m_cUseProcessorPortion;                    ///< processor portion to use when multithreading is used

        static const bool                         m_cWriteDebugFiles;                        ///< processor portion to use when multithreading is used
        static const std::wstring                 m_cDebugFileOutputDirectory;               ///< processor portion to use when multithreading is used

        // -- private member variables

        // -- variables for testing
        bool                                      m_bGetInterpolatedD;                       ///< indicator indicating that the interpolated D is needed / matrix is reminded
        bool                                      m_bGetInterpolatedDsmoothed;               ///< indicator indicating that the interpolated Dsmoothed is needed / matrix is reminded
        // -- variables for testing end

        bool                                      m_bUsePrecalcFilters;                      ///< indicator whether precalculated filters shall be used
        bool                                      m_bApplyShifts;                            ///< indicator for applying shifts
        bool                                      m_bStrongFiltering;                        ///< indicates if strong filtering is enabled (only really noise dep. in profile direction, noise reduction by profile filtering is not considered)

        zfs::math::Tuple2<int>                       m_nSmoothFilterSizes;                      ///< length of smoothing filter (short: L)
        zfs::math::Tuple2<int>                       m_nSmoothFilterPolyOrders;                 ///< poly orders for smoothing filter
        int                                       m_MinNoOfLinesForMTProc;                   ///< minimum size of input data for mt processing
        int                                       m_PixelResolutionAsDivisor;                ///< pixel resolution as divisor : pixel resoltion for resampling rho= 1./m_PixelResolutionAsDivisor;
        int                                       m_SmootherInvalidPixels;                   ///< No. of invalid pixels from smoothing at profile (selection) start and end.

        T                                         m_drThresh;                                ///< difference in D for which a new distance is calculated

        direction_type                            m_eDirection;                              ///< direction to be filtered

                                                                                             ///< used settings for the kernels for shifting the profiles
        zfs::math::KernelType                        m_ResKernelTypeToUse;                      ///< can be KS_TWO_POINT (linear interp.), KS_FOUR_POINT, KS_SIX_POINT or KS_TRUNCATED_SINC
        int                                       m_ResKernelTruncSincLength;                ///< considered, when trunc. sinc is used
        zfs::math::SignalWindowNameType              m_ResKernelWindow4TruncSinc;               ///< considered, when trunc. sinc is used
        int                                       m_ResKernelLength;                         ///< kernel length of the used resampling kernel

        zfs::math::KernelBank<T>                     m_KernelBank;                              ///< kernel bank for resampling

        std::vector<T>*                           m_pMutualLineShiftsTile;                   ///< [pixel] for every line the mutual shifts of the lines (necessary when profile cross filtering is used, default for imager)

        Matrix<bool>                              m_InvalidsMask;                            ///< indicates the positions which are not valid due to shifting of the profiles (convolution with resampling kernel: ~ (kernelsize/2- 1))
        Matrix<byte>                              m_FilterIndicator;                         ///< indicates which points are filtered (and in which direction)
        Matrix<T>                                 m_Noise1Sigma_smoothed;                    ///< final std. dev. vals of range values after filtering
        Matrix<T>                                 m_Noise3Sigma_smoothed;                    ///< final std. dev. vals of range values after filtering

        Matrix<T>*                                m_pD;                                      ///< range in m
        Matrix<T>*                                m_pNoise1Sigma;                            ///< noise std. dev. [m]
        Matrix<T>*                                m_pNoise3Sigma;                            ///< noise std. dev. [m]
        Matrix<bool>  *                           m_pMask;                                   ///< mask specifying invalid samples
        Matrix<T>*                                m_pD_smoothed;                             ///< smoothed D matrix);
        Matrix<T>*                                m_pX;                                      ///< if a X-matrix is provided new x-values are calculated based on smoothed D, unit [m]
        Matrix<T>*                                m_pY;                                      ///< if a Y-matrix is provided new y-values are calculated based on smoothed D, unit [m]
        Matrix<T>*                                m_pZ;                                      ///< if a Z-matrix is provided new z-values are calculated based on smoothed D, unit [m]

        // -- for testing
        Matrix<T>                                 m_Dinterp;                                 ///< keeps the interpolated and smoothed D matrix before smoothing, when m_bGetDinterp is set;
        Matrix<T>                                 m_Dsmoothedinterp;                         ///< keeps the interpolated and smoothed D matrix before smoothing, when m_bGetDsmoothedinterp is set;

        // -- private member functions
        void filterMT();

        void filterST( const int                           threadIdx,
                       const Matrix<T>             * const pD,
                       const Matrix<T>             * const pNoise1Sigma,
                       const Matrix<T>             * const pNoise3Sigma,
                       const Matrix<bool>          * const pMask,
                       const zfs::math::Range<int>    * const pProcessingRangeLines,
                       const zfs::DatasetParameter * const pDatasetParameter,
                       Matrix<byte>                * const pFilterIndicator,
                       Matrix<T>                   * const pD_smoothed,
                       Matrix<T>                   * const pNoise1Sigma_smoothed,
                       Matrix<T>                   * const pNoise3Sigma_smoothed,
                       Matrix<T>                   * const pX,
                       Matrix<T>                   * const pY,
                       Matrix<T>                   * const pZ,
                       int                         * const pnPixelFiltered );

        static void filterThreadWorker( SmoothingFilter* const a_pSmoothingFilter, /// < worker function for threads
                                        const int              a_LineStart,
                                        const int              a_LineEnd,
                                        const int              a_LineOffsetStart,
                                        const int              a_LineOffsetEnd,
                                        const int              a_ThreadIdx,
                                        int*                   a_nPixelFiltered );

        // -- shifting of data lines necessary when x-profile filtering is required
        void ApplyMutualLineShiftsMT( const int aForwardBackward); // forward/backward for apply / reverse shifts

        void ApplyMutualLineShiftsST( const zfs::math::Range<int>& aProcessingRange, const int aForwardBackward);

        static void ApplyLineShiftsThreadWorker( SmoothingFilter* const   a_pSmoothingFilter,
                                                 const zfs::math::Range<int> a_ProcessingRange,
                                                 const int                a_ForwardBackward );

        void prepare_kernels(); // consider only a certain pixel positioning accuracy to reduce the number of operations
        void CopyMatrices();
        void copyMatricesMT();
        static void copyThreadWorker( SmoothingFilter* const a_pSmoothingFilter, /// < worker function for threads
                                      const int              a_LineStart,
                                      const int              a_LineEnd);

    public:

        // -- constructors
        SmoothingFilter();
        SmoothingFilter( zfs::math::Tuple2<int>                a_nSmoothFilterSizes,
                         direction_type                     a_eDirection,
                         bool                               a_bUsePrecalcFilters,
                         int                                a_PixelResDivisor,
                         const zfs::DatasetParameter* const a_DatasetParameter,
                         bool                               a_bMultiThreaded = false);

        // -- public member functions
        void init_parameter( zfs::math::Tuple2<int>                a_nSmoothFilterSizes,
                             direction_type                     a_eDirection,
                             bool                               a_bUsePrecalcFilters,
                             int                                a_PixelResDivisor,
                             const zfs::DatasetParameter* const a_DatasetParameter,
                             bool                               a_bMultiThreaded = false);

        void filter( Matrix<T>     &                      D,   // in     : range in m
                     Matrix<T>     &            Noise1Sigma,   // in     : noise std. dev. [m]
                     Matrix<T>     &            Noise3Sigma,   // in     : noise std. dev. [m]
                     Matrix<bool>  &                   Mask,   // in     : mask specifying invalid samples
                     std::vector<T>&   MutualLineShiftsTile,   // in     : angles of high encoder [rad]
                     Matrix<T>     &                      X,   // in/out : original/new x - coordinate (in place, from smoothed D)
                     Matrix<T>     &                      Y,   // in/out : original/new y - coordinate (in place, from smoothed D)
                     Matrix<T>     &                      Z,   // in/out : original/new z - coordinate (in place, from smoothed D)
                     Matrix<T>     &             D_smoothed ); // out    : smoothed D matrix);

        // -- setter
        void setFilterSizes( const zfs::math::Tuple2<int>& a_nSmoothFilterSizes);

        // set kernel: for truncated sinc also length and window has to be set (otherwise default values are used)
        void setResamplingKernelForProfileShifting( const zfs::math::KernelType aKernelType, const int aKernelLength=  0, const zfs::math::SignalWindowNameType aSignalWindowType= zfs::math::SignalWindowNameType::SWN_NONE);

        // -- getter
        zfs::math::Tuple2<int> getFilterSizes() const;
        const Matrix<bool>  getFilterIndicator() const;

        const Matrix<bool>& getInvalidsMask() const;
        int                 getSmootherInvalidPixels() const;

        // -- getter for testing
        void                setGetInterpolatedD();
        void                setGetInterpolatedDsmoothed();
        const Matrix<T>     getInterpolatedD() const;
        const Matrix<T>     getInterpolatedDsmoothed() const;

        // -- getter for const. settings
        static bool ProfileShiftingEnabled();
    };
    // ===========================================================================================================
} // namespace zfsfilter

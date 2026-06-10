#pragma once

#include <string>
#include <climits>
#include <vector>

#include "filterapi.h"
#include <zfs/scaninfo.h>
#include <zfs/Math/math/range.h>

class ZFSLinebyline;
class ZFScan;
class ZFSHeader;

namespace zfsfilter
{
    class RollingBuffer;
}

typedef unsigned char byte;

namespace zfs {
    /** \class DatasetParameter
     * \brief  Class for managing (reading and providing) the parameters of a data set.
     *
     * \author Juergen Holzner
     *
     * Class for managing (reading and providing) the parameters of a data set.
     *
     */

    class FILTERDLL_API DatasetParameter
    {
    public:
        // -- public member variables

        bool                  IsProfiler;                 /// < Indicator if data set was acquired by profiler of imager/other
        bool                  IsInProfilerMode;           /// < Indicates if data was acquired in profiler mode
        bool                  IsProfilerOrInProfilerMode; /// < Indicates if data was acquired with a profiler device or in profiler mode

        bool                  IsSinglePart;               /// < Indicates if data is a single part data set
        bool                  IsSection;                  /// < Indicator if data set is only a part of a full data set
        bool                  IsDataSetCut;               /// < Indicator if data set is cut
        bool                  IsDataSetCutProfile;        /// < Indicator if data set is cut (vertical   => no complete profiles)
        bool                  IsDataSetCutLines;          /// < Indicator if data set is cut (horizontal => not all lines)
		bool                  IsSubsampled;               /// < Indicator if this data set parameter object refers to a subsampled dataset
        bool                  IsNoiseInfoAvailable;       /// < Indicator if noise info is available
                                                          
        int                   Sr;                         /// < Pixel per Line of full 360 degree
        int                   Sa;                         /// < Lines all parts (in case of profiler, imager == Lines)
                                                          
        int                   HsInc360;                   /// < Number of high speed encoder clicks per 360°
                                                          
        int                   StartPixelPan;              /// < StartPixel in panorama view (when data set is cut, else 0)
        int                   StartLinePan;               /// < StartLine  in panorama view (when data set is cut, else 0)
        int                   StartPixel;                 /// < StartPixel in physical mode (when data set is cut, else 0)
        int                   StartLine;                  /// < StartLine  in physical mode (when data set is cut, else 0)
                                                          
		int                   Pixel;                      /// < Number of Pixel of Dataset
		int                   Lines;                      /// < Number of Lines of Dataset
		int                   PixelSubs;                  /// < Number of Pixel of subsampled Dataset, dependent on currently set subsampling
		int                   LinesSubs;                  /// < Number of Lines of subsampled Dataset, dependent on currently set subsampling
        int                   SaSubs;                     /// < Number of Lines all Parts of subsampled Dataset, dependent on currently set subsampling
		int                   PixelFull;                  /// < Number of Pixel of full data set (when data set is cut, this is an approximation)
        int                   LinesFull;                  /// < Number of Lines of full data set (when data set is cut, this is an approximation)
        int                   PanPixel;                   /// < Number of Pixel Panorama Image of Data Set
        int                   PanLines;                   /// < Number of Lines Panorama Image of Data Set
                                                          
        int                   Imin;                       /// < Lower Value for Intensity
        int                   Imax;                       /// < Upper Value for Intensity
        int                   Iover;                      /// < Over Rf Value for Intensity
                                
        int                   DataTyp;                    /// < Data/Scanner Typ (215, 315, ....)
        int                   ScannerType;                /// < Type of Scanner (1: imager, 2: profiler)
        int                   ScannerSerie;               /// < Hold the scanner series as int number (e.g. 5006, 5010, 5016, 9012)
                                                          
        int                   PixelShiftAtEdge;           /// < PixelShift in Panorama - Center due to Encoder Offset, dependent on currently set subsampling for pixels
                                                          
        int                   Clip1Start;                 /// < Vert. Encoder Cnt. first Pixel
        int                   Clip1Stop;                  /// < Vert. Encoder Cnt. last Pixel
        int                   EncoderClicksPerPixel;      /// < Vert. Encoder Clicks per Pixel
        int                   EncoderClicksPerLine;       /// < Hor. Encoder Clicks per Line
                                                          
		zfs::quality_t        DatasetQuality;             /// < Quality setting with which the scan was acquired
        zfs::resolution_t     DatasetResolution;          /// < Resolution setting with which the scan was acquired
		                                                  
		double                A_lenc;                     /// < low encoder angle range in [rad]
        double                Sq;                         /// < Rotational Speed of Scanner Head in [Hz]
        double                RFa;                        /// < Noise coefficients parameter RFa
        double                RFA;                        /// < Adapted to mode/rotational speed of laser head
        double                RFb;                        /// < Noise coefficients parameter RFb
        double                RFc;                        /// < Noise coefficients parameter RFc
        double                RFC;                        /// < Rfc, adapted to mode/rotational speed of laser head
        double                FilterFrequ;                /// < Filter frequency in [Hz]
        double                minD;                       /// < minimum measurable D of specific scanner
        double                maxD;                       /// < maximum measurable D of specific scanner
        double                AR;                         /// < angle resolution vertical, dependent on currently set subsampling for pixel
        double                AA;                         /// < angle resolution horizontal, dependent on currently set subsampling for lines
        double                IDF;                        /// < additional factor for calculation of mixed pixel width
        double                LaraFrequ;                  /// < lara frequency
        double                ScannerTypeNoiseOffset;     /// < additional noise offset (mixed pixel calculations)
        double                LasRefDist;                 /// < reference distance for which laser spot initial/reference size is given
        double                LaserSpotDiameter0;         /// < initial/reference laser spot diameter
        double                LaserSpotRadius0;           /// < initial/reference laser spot radius
        double                LaserFarFieldDiv;           /// < far field divergence of laser spot
        double                MaxViewStartAngle;          /// < Start View Angle in [°]/ [deg]
        double                MaxViewStopAngle;           /// < Stop  View Angle in [°]/ [deg]
        double                HorAcquistionRange;         /// < Horizontal (L-Encoder) Aquisition Range in [°]/ [deg]
        double                VertAcquistionRange;        /// < Vertical (H-Encoder) Aquisition Range in [°]/ [deg]
        double                VertEncoderOffset;          /// < Vertical (H-Encoder) Offset in [°]/ [deg]
        double                VertEncAnglePInc;           /// < Vert. Angle Tic/Inc of Vert. Encoder (H-Encoder) [rad]
        double                StartAngleHorEnc;           /// < Start Angle Hor. Encoder
        double                StopAngleHorEnc;            /// < Stop Angle Hor. Encoder
                                                          
        double                ToleranceOffset;            /// < Tolerance offset for filtering (dep. on Scanner Series)

        double                ProfileForwardFeed;         /// < User Supplied (!) profile forward feed (for mixed pixel width calculations)
                                                          
        zfs::math::Range<double> VertAngleRange;          /// < Angle Range of vertical angles (H-Encoder) [rad]
        zfs::math::Range<double> HorAngleRange;           /// < Angle Range of horizontal angles (L-Encoder) [rad]
                                                          
        std::vector<double>   VertAngles;                 /// < Vertical angles [rad] for every pixel a value (approximate!), dependent on set subsampling
        std::vector<double>   AbsSinVertAngles;           /// < Useful number for calculation of horizontal mixed pixel with, laser spot overlap across profiles, zenith calculations ...
        std::vector<double>   HorAngles;                  /// < Horizontal angles [rad] for every profile a value (approximate!), dependent on set subsampling
                                                          
        std::string           strScannerType;             /// < scanner type string (PROFILER, HDSxxxx, 5006, 5010xx)
        std::wstring          DatasetPath;                /// < full path to data set
                                                          
        zfs::ScannerInfo      ScannerInfoDesc;            /// < info on scanner type (descriptor)
        zfs::DeviceInfo       DeviceInfoDesc;             /// < info on device (particularly nonius jump distances)

    private:
        // -- private member variables

		// -- object state
        bool         m_bInternal;
        bool         m_bInitialized;
        bool         m_bErrorReadingData;
		bool         m_ParameterFromHeaderOK;

		// -- input data
        ZFSHeader*   m_zfsHeader;

		// -- subsampling state and numbers
		int          m_SubsamplingLines;        /// < current subsampling for lines
		int          m_SubsamplingPixels;       /// < current subsampling for pixels

        // -- laser spot size calculations
        // -- factor constant
        double       m_LaserSpotSize0_IDF;      /// < initial/reference laser spot diameter
        double       m_LaserSpotSizeConst;      /// < constant for fast(er) calculation of laser spot size

    public:
        // -- constructors --
        DatasetParameter();
		DatasetParameter( const zfs::ScannerInfo& aScannerInfo, 
						  int                     aNumberOfPixelsPerProfile,                      // number of pixels per profile
						  int                     aNumberOfPixelsPer360,                          // in case of Profiler = aNumberOfPixelsPerProfile
						  zfs::quality_t          aDatasetQuality    = quality_t::QUALITY_UNDEF,  // Quality setting with which the scan was acquired
						  int                     aNumberOfLines     = INT_MAX,                   // in case of Profiler set to INT_MAX otherwise set to number of lines 
						  unsigned int            aSubsamplingLines  = 1,
						  unsigned int            aSubsamplingPixels = 1 );

        DatasetParameter( const std::wstring&            a_FileName, unsigned int aSubsamplingLines= 1, unsigned int aSubsamplingPixels= 1); // read using scan file name 
        DatasetParameter( zfsfilter::RollingBuffer* a_RollingBuffer, unsigned int aSubsamplingLines= 1, unsigned int aSubsamplingPixels= 1); /// < read using zfsfilter::RollingBuffer
        DatasetParameter( ZFSLinebyline*             a_zfLineByLine, unsigned int aSubsamplingLines= 1, unsigned int aSubsamplingPixels= 1); /// < read using lbl scan data
        DatasetParameter( ZFScan*                             a_zfs, unsigned int aSubsamplingLines= 1, unsigned int aSubsamplingPixels= 1); /// < read using zfs scan data
        DatasetParameter( ZFSHeader*                    a_zfsHeader, unsigned int aSubsamplingLines= 1, unsigned int aSubsamplingPixels= 1); /// < read using zfs header

        ~DatasetParameter();

        // -- initializers
		void initialize( const zfs::ScannerInfo& aScannerInfo,
						 int                     aNumberOfPixelsPerProfile,                     // number of pixels per profile
						 int                     aNumberOfPixelsPer360,                         // in case of Profiler = aNumberOfPixelsPerProfile
						 zfs::quality_t          aDatasetQuality    = quality_t::QUALITY_UNDEF, // Quality setting with which the scan was acquired
						 int                     aNumberOfLines     = INT_MAX,                  // in case of Profiler set to INT_MAX otherwise set to number of lines 
						 unsigned int            aSubsamplingLines  = 1,
						 unsigned int            aSubsamplingPixels = 1 );

        void initialize( ZFScan*                   a_zfs          , unsigned int aSubsamplingLines = 1, unsigned int aSubsamplingPixels = 1 ); /// < read using zfs scan data
        void initialize( ZFSLinebyline*            a_lbl          , unsigned int aSubsamplingLines = 1, unsigned int aSubsamplingPixels = 1 ); /// < read using lbl scan data
        void initialize( ZFSHeader*                a_zfsHeader    , unsigned int aSubsamplingLines = 1, unsigned int aSubsamplingPixels = 1 ); /// < read using zfs header data
        void initialize( zfsfilter::RollingBuffer* a_RollingBuffer, unsigned int aSubsamplingLines = 1, unsigned int aSubsamplingPixels = 1 ); /// < read using zfsfilter::RollingBuffer

        // -- calculator functions

        // Coordinate Mapping

        void MapToPhysical( int& Pix, int& Line ) const; // if data set is cut add start pixel and start line in panorama image
		                                                 // for subsampling multiply with appropriate factors beforehand

        // Laser/Scanner Properties

        double              LaserSpotDiameterAtDistance                               (const double& aDistance)   const; /// < calc Laser Spot Diameter in [m] at distance given in [m]
        template<class T> T NoiseStdDevForIntensity                                   (const int& aIntensity_inc) const; /// < calc noise std. deviation [m] for a given intensity in [increments]
        template<class T> T ThreeSigmaNoiseForSmoothing                               (const T& aNoiseStdDev)     const; /// < calc 3* sigma number for smoothing of scanner distances from noise std. dev. given in [m]
        double              ImagerUsedAsProfilerExpectedMixedPixelAngleBeamHitsHousing()                          const; /// < calc the angle [rad] corresponding to the expected mixed pixels when an imager is used in profiler mode and the beam hits the housing

        // Scanner Information
        int scannerSerieToSerieInt( const  zfs::scannerserie_t& Serie ) const;

        // Object State
        bool   Initialized() const;
        bool   ErrorReadingParameter() const;

        // return pointer to header 
        const ZFSHeader* const getZFSHeader() const;
        ZFSHeader* const getZFSHeader();
        void setZFSHeader(ZFSHeader*const pHeader);

		// setter/getter (access must be done via accessors, due to parameter conversion)

        // set subsampling
		bool setSubSampling( const int aSubLines, const int aSubPixels ); /// < set/reset subsampling including parameter adaption 
		void getSubSampling( int& aSubLines, int& aSubPixels ) const; /// < get currently set subsampling factors

        // set profile feed
        void setProfileForwardFeed(const double& aProfileForwardFeed);

    private:

        // -- private member functions
        void prepare_parameter_object(ZFSHeader* a_zfsHeader, unsigned int aSubsamplingLines = 1, unsigned int aSubsamplingPixels = 1 );
		void read_from_header( );
		void set_constants();
		bool calc_derived_parameter( const unsigned int aSubLines, const unsigned int aSubPixels );
		void set_scanner_parameter();
        void basic_initialization();
		bool set_remaining_parameter_from_given_parameters( unsigned int aSubsamplingLines= 1, unsigned int aSubsamplingPixels= 1 );

        // Data set configuration and sizes
        void detDataSetCut();
        void setFullSizes();
        void setDatasetSizeConfigParams();
        void setStartPixelAndLine();

        // -- calculators
        unsigned int  ExpectedProfilePixels() const;
        unsigned int  ExpectedProfileLines() const;
        double        ExpectedProfileFullAngleRange() const;
        void          ExpectedPanLinesAndPixels(int& aPanPixel, int& aPanLines) const;

        // Swept angles as vectors
        void calcAngles();

		// subsampling
		bool ConvertParametersFromSubsampling( const unsigned int aSubLines, const unsigned int aSubPixels );

    };
} // namespace zfs

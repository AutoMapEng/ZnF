#pragma once

#include "../zfsapi.h"
#include "../noise.h"
#include "../scaninfo.h"
#include "filter_param.h"


class ZFSHeader;

/**
\brief The parameter of the standard filter
*/
class ZFS_API ZFSFilter
{
public:
    ///intensity filter
    ///Masks out pixels below minimum and above maximum intensity. Defaults depend on Scanner type.
    bool removeMinMaxRf{};		///< use minimum and maximum intensity filter
    int minRf{};					///< minimum intensity in inc
    int maxRf{};					///< minimum intensity in inc

    bool rfNorm{};
    double rfNormMin = 0;
    double rfNormMax = 1.0;

    // Mixedpixel filter
    //This filter will mask out pixel, which are on edges and therefore not valid.
    //On Edges you have mixed range values, these range values are often between the foreground and the background.
    bool mixedpixel{true};            ///< use mixed pixel filter (main filter)
    double mix_d{2}; ///< The bigger the value, the more pixel will be removed. Use 1 to 5 deg. Default: 2
    int nptS{2};					///< Number of points should be ok. Range 1..2. Less value = more pixel will be removed. Use for one line filtering. Default: 2
    int nptNP{9};					///< The same as above but for the three lines filter. Range 1..8 with diagonal or 1..4 without. Default: 6 with diagonal
    bool bHor{true};					///< mixed pixel horizontally
    bool bVert{true};					///< mixed pixel vertically
    bool bDiag{true};					///< mixed pixel diagonal, should be not used with lara sync

    // skipes filter
    bool spikes{};
    double spikesLimit1{2};
    double spikesLimit2{20};

    // filterflags set by firmware
    bool sigproc{};
    bool sigproc_weak{};
    bool sigproc_stability{};
    bool sigproc_strong{};
    bool sigproc_IP{true};

    // mixed pixel run on one line
    bool mxpxline{ false };
    enum { MPL_PROFILER=0, MPL_IMAGER=1 };
//    int mxpxline_ver = MPL_PROFILER;
    double mxpxline_wid{ 1. };
    double mxpxline_w5{ 5. };
    double mxpxline_w2{ 8. };
    double mxpxline_w0{ 15. };
    double mxpxline_g5{ .02 };
    double mxpxline_g2{ .02 };
    double mxpxline_g0{ .02 };

    //Jump filter
    //Detect jumps in range and mask out pixels.
    bool jump{};					///< detect jumps and remove it
    int jumpF;					///< Divisor of range for jump detection depending on current range. Default:10
    int nJumpsS;				///< Number of points should be ok. Range 1..2. Less value = more pixel will be removed. Use for one line filtering. Default: 2
    int nJumpsPN;				///< The same as above but for the three lines filter. Range 1..4 Default: 2

    //Edge filter
    //Masks pixels at the outer borders of the scan. The first and last line and the first and the last pixel of each line are filtered.
    bool edge{true};					///< remove points around scans

    // range filter
    //Masks out pixel which are not in the selected range.
    bool range{};
    double minRg{0.5};				///< Pixel below this range are masked out. Default: 0.5(m)
    double maxRg{};				///< Pixel greater then this range are masked out. Default depends on Scanner type.

    /// single pixel filter
    /// removes pixel with no valid neighbour
    bool useSingleFilter{};		///< use single pixel filter
    bool useSingleFilterAdvanced{ false };		///< use single pixel filter
    int nsingleS;				///< Number of points should be ok. Range 1..2. Less value = more pixel will be removed. Use for one line filtering. Default: 2
    int nsinglePN;				///< The same as above but for the three lines filter. Range 1..4 Default: 3
    double singleD;				///< Divisor of range for jump detection depending on current range. Default:10

    //single pixel filter removes only pixel with filtered pixels around the singel pixel filter above contains this filter
    bool useSingelMasked{};		///< use filter
    int nSingelMaskedPN{3};		///< number of not filtered pixels around by 3 lines filter
    int nSingelMaskedS{2};			///< number of not filtered pixels around by single line filter

    ///small objects filter
    bool    useSmallObjectsFilter{}; ///< determines if small objects filter is used
    int     smallObjectsLines{2};     ///< small objects do not extend over more than smallObjectsLines lines
    double  smallObjectsExtend{ 0.01 };    ///< small objects do not extend over more than smallObjectsExtend meters along the profile
    double  smallObjectsNSigma{ 4.0 };    ///< pixels are accounted to a small object if the distance between the pixels is less than NSigma times the expected intensity noise standard deviation

    // overload filter, only for 5006
    bool overload{};
    int overRf{ 4200000 };
    int overPreInc{};
    int overPostInc{};

    // overload filter for 5016 (firmware version)
    int overloadRf{};
    int overloadRfPixMargin{};

    // range intensitaetsfilter just for 5003
    bool rfrg{};

    //Nonius filter
    //Detects nonius jumps. Only for profiler scans used.
    bool useNoniusFilter{false};		//use nonius filter
    unsigned int nonFilterN{ 2 };
    double noniusAddDist{ 0.1 };		//Band of the nonius check range values in meter. Default 0.05m
    int noniusRfBand{};			//Band of the nonius check intensity values in inc. Default (maxRf / 100)
    int noniusPostRemove{1};
    int noniusPreRemove{2};

    //Nonius correction
    //Detect the nonius jumps and correct the values. This filter changes the pixel values. The scan file are not changed.
    bool noniusCorr{};			//use nonius correction
    int noniusIterations{ 1 };		//iterations for the nonius correction Default 1

    //Bad lines filter
    bool badLinesFilter{};		//Use bad line filter

    //Critical Compensation filter
    //Masks out lines which show too big tilt changes, because for these the compensation is critical.
    bool compensator{ false };  ///< critical compensator
    bool useCriticalCompensationFilter{};/// remove lines which show too big tilt changes => critical compensation
    bool flag_vec_compensator{};
    int borderCriticalCompensation{};
    std::vector<int> vec_compensator;

    // vertical invalid
    bool vertValid{ false };
    double vertValidFrom1{};
    double vertValidTo1{};
    double vertValidFrom2{};
    double vertValidTo2{};

    //Additional parameters
    int iterations{ 1 };				///< iterations

    bool xyz_in{ false };
    double xyz_in_top{};
    double xyz_in_bottom{};
    double xyz_in_left{};
    double xyz_in_right{};

    bool xyz_out{ false };
    double xyz_out_top{};
    double xyz_out_bottom{};
    double xyz_out_left{};
    double xyz_out_right{};

    bool ov_edge{};
    int ov_edge_value{};
    int ov_edge_remove{};

    bool enlarge{};
    int enlarge_before{ 1 };
    int enlarge_after{ 1 };

    // information to share with zfs::FilterParams
    // have to be at the end (because of usage of strings)
    zfs::FilterSetupParams setupPrm;


public:
    ZFSFilter() { setDefaults(); }
    ZFSFilter(const ZFSHeader*);
    ZFSFilter(const ZFSHeader&);
    ZFSFilter(const ZFSFilter&);

    /// internal calculation before filtering (mixed pixel & critical compensation)
    void calcParams(const ZFSHeader* h);

    /// copy the settings and set mIsSet flag
    void copyFrom(const ZFSFilter&);

    void setMixD(double d);
    double getMixD() const { return mix_d; }

    bool isOk() const { return mIsSet; }
    void setIsOk() { mIsSet=true; }

    bool setDefaultParams(const ZFSHeader* h); ///< set default params depending on scanner

    /// set all filter to disabled except min intensity
    void disableAllExceptMinRf();

    /// get border to filter critical compensation regions
    static int GetCriticalCompensatorBorder(const ZFSHeader* h); 

protected:
    void setDefaults();

public:
    bool mIsSet{}; ///< true if set by zfs-header or load param

    double threshold{};

    // Local
    double cosSubAngleHoriz{};
    double sinSubAngleHoriz{};
    double cosSubAngleVert{};
    double sinSubAngleVert{};
    double cosSubAngleDiag{};
    double sinSubAngleDiag{};

    // -- further auxilliary information about data set
    zfs::RangeNoise     RangeNoiseDataset;          ///< range noise info of data set

public:
    //Nonius Filter
    zfs::DeviceInfo devInfo;    ///< internal info required for filter

    // vertical invalid
    double vertFromRad1{};
    double vertToRad1{};
    double vertFromRad2{};
    double vertToRad2{};

    // thin filter
    bool thin{};
    int thinMinSub{ 1 };
    double thinGridSize{0.01};
    double thinLut[360]{};
    double thinAngleRad{};

    // firmmixpix
    double firmmixAR{};

};


//##################################################################################################################################################


#pragma once
#include <zfsfilter/filterapi.h>

#include <string>
#include <vector>

#include <zfs/Math/math/def.h>


#include "basic.h"

#include <zfs/filter/filter.h>
#include <zfsfilter/basic/filter_param_imager.h>
#include <zfsfilter/MixedPixelFilterDef.h>




namespace zfsfilter 
{



    /*
    * \brief filter a scan with the default standardfilter as well as with the filter added for imager which may require a whole scan in memory
    */
    FILTERDLL_API bool ImagerFilter(const std::wstring& scan, const zfs::FilterExecution& executionParameter=zfs::FilterExecution());
    FILTERDLL_API bool ImagerFilter(const std::wstring& scan, const zfsfilter::ImagerFilterParams& filterParameter, const zfs::FilterExecution& executionParameter=zfs::FilterExecution());





    //###########################################################################################################


    /// Filter errorcodes
    enum ferr_t {
        FERR_OK = 0,	 ///< ok
        FERR_UNDEFINED = 1,	 ///< undefined
        FERR_SKIPPED = 2,	 ///< nicht ausgefuehrt
        FERR_WRITEPROTECTED = 4,	 ///< can not write back result
        FERR_PARAMWRONG = 8,	 ///< param wrong
        FERR_ADDMASK = 16,	 ///< can not add mask to the scan
        FERR_WRITEMASK = 32,   ///< can not write mask to the scan
        FERR_NOTZFS = 64,	 ///< scan is not a zfs scan
        FERR_ABORT = 128,
        FERR_CANTOPENSCAN = 256    ///< scan can not be opend
    };


    /**
    The result of the filter for each scan and each filter
    */
    class FILTERDLL_API FilterResultItem {
    public:
        std::wstring filterParams;  ///< the params used
        std::wstring filterName;    ///< the internal name of the filter
        ferr_t error;               ///< errorcode or success
        int nRemoved;               ///< number of pixel filtered out
        int nChecked;               ///< number of pixels checked

        FilterResultItem() : error(FERR_UNDEFINED), nRemoved(0), nChecked(0) {}
    };
    typedef std::vector<FilterResultItem> filterresult_itemlist_t;

    //-------------------------------------------------------------------------------------------

    class FILTERDLL_API FilterResultScan {
    public:
        int currentScan{};
        ferr_t error;				    ///< for user abort
        size_t totalPixelOfScan;
        std::wstring scanFileName;
        filterresult_itemlist_t filter;

        FilterResultScan() :error(FERR_UNDEFINED), totalPixelOfScan(0) {}
        FilterResultScan(ferr_t err_) :error(err_), totalPixelOfScan(0) {}
        FilterResultScan(ferr_t err_, size_t pixel_) :error(err_), totalPixelOfScan(pixel_) {}
    };
    typedef std::vector<FilterResultScan> filterresult_scanlist_t;



    /**
    * Params for Invalidate filter
    */
    class FILTERDLL_API ParamsCommon
    {
    public:
        virtual ~ParamsCommon() {}
        bool use{ true };			///< use filter

        ParamsCommon() = default;

        std::wstring dump(const std::wstring& cr = L"") const;

        virtual std::string generate_parameter_string() const = 0;
    };


    /**
    *	Params for mixedpixel new filter
    */
    class FILTERDLL_API ParamsMixedpixelNew : public ParamsCommon
    {
    public:
        FilterStrengthOptionType m_FilterStrengthOption;
        ProcessingDirOptionsType m_FilterDirectionOption;
        bool                     m_bNormalAnalysis;
        bool                     m_bKalmanAnalysis;
        bool                     m_bEdgeAnalysis;
        bool                     m_bSmooth;
        bool                     m_bMaskToOriginal;
        bool                     m_bProcessMultithreaded;
        bool                     m_bOutputReport;
        bool                     m_bDiscardOriginal;
        int                      m_ProcessingTileSize;
        int                      m_ProcessingTileOverlapSize;
        float                    m_ProfileProgressFeed;
        double                   m_FilterStrengthValue;

        ParamsMixedpixelNew()
            :
            m_FilterStrengthOption(FilterStrengthOptionType::FILTER_STRENGTH_UNCHOOSEN),
            m_FilterDirectionOption(ProcessingDirOptionsType::PROC_DIR_OPT_PROFILE_AND_X_PROFILE),
            m_bNormalAnalysis(true),
            m_bKalmanAnalysis(false),
            m_bEdgeAnalysis(true),
            m_bSmooth(false),
            m_bMaskToOriginal(true),
            m_bProcessMultithreaded(true),
            m_bOutputReport(false),
            m_bDiscardOriginal(false),
            m_ProcessingTileSize(0),
            m_ProcessingTileOverlapSize(0),
            m_ProfileProgressFeed(0.f),
            m_FilterStrengthValue(0.0)
        {
        }

        std::string generate_parameter_string() const override;
    };


} // end namespace


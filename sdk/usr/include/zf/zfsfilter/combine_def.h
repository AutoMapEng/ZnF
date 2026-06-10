#pragma once

#include "filterapi.h"
#include <string>

#include <zfs/zfsheader.h>

namespace zfsfilter
{
	#define COMBINE_EXT		".combine.zfs"
	#define LCOMBINE_EXT	L".combine.zfs"


    enum combine_err_t { 
		ERR_COMBINE_OK, 
		ERR_COMBINE_UNDEF, 
		ERR_COMBINE_NOTSAMEPOS, 
		ERR_COMBINE_USERABORT, 
		ERR_CANT_OPEN_SCAN1, 
		ERR_CANT_OPEN_SCAN2, 
		ERR_CANT_OPEN_OUTPUT, 
		ERR_CANT_COPY_ORG,
        ERR_CANT_COPY_ZFI,
        ERR_CANT_DELETE_SCANS,
        ERR_NO_PROJECT
	};

	/**
	\brief params used for combining scans
	*/
    class FILTERDLL_API Combine2ScansParam
    {
    public:
        double d;				    ///< combine filter dist [meter]
        
		bool average;			    ///< average pixel from scan1 and scan2 and write that pixel out
        double avr;				    ///< limit for averaging points[meter]
        
        bool removeObsoleteScans;   ///< remove obsolete scans afterwards from project
        bool removeObsoleteFiles;   ///< remove obsolete scan files afterwards from project
        bool combineZFI;            ///< if there are two or more zfi, merge it together

		bool differentScanPos;	    ///< allow different scan position
        int threadsPercent;         ///< cpu usage [percent] default 100

        Combine2ScansParam() : d(0.005), average(false), avr(0.006), removeObsoleteScans(false), combineZFI(true), differentScanPos(false), threadsPercent(100) {}
    };


	/**
	\brief result struct of combining scans
	*/
    class FILTERDLL_API Combine2ScansResult
    {
    public:
        combine_err_t err;
        std::string log;
        std::wstring scanName;

        Combine2ScansResult() : err(ERR_COMBINE_UNDEF) {}
    };
    

    /**
    @brief convert the errorcode into a string
    */
    FILTERDLL_API std::string error2string(combine_err_t err);


} // namespace zfsfilter
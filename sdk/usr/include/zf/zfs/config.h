#pragma once

#include "compiler.h"

/**
\brief  This file will define system wide configurations values for zfs library

    Your project must define one of the followings
        _WINDOWS WIN32
        _WINDOWS WIN64
        LINUX
*/


#ifdef _WINDOWS

#ifdef WIN64
//------- start windows-64bit ----------
//#define ZFS_PTZ
#define ZFS_TIFF
#define ZFS_CXIMAGE
#define ZFS_JPG
#define ZFS_UK_CALIB
#define ZFS_PROGRESS
#define ZFS_MSG_CALLBACK
#define ZFS_INFO
#define ZFS_SCAN_FILTER
#define ZFS_LINEBYLINE_FILTER
#define ZFS_MATRIX
#define ZFS_SUPPORT_TEMP_POS
#define ZFS_MEM
#define ZFS_SCENEMODELER_SUPPORT
#define ZFS_5003_SUPPORT
#define ZFS_5006_SUPPORT
#define ZFS_5010_SUPPORT
#define ZFS_5016_SUPPORT
//#define ZFS_5018_SUPPORT // placeholder
#define ZFS_5024_SUPPORT

#define ZFS_COLOR_SUPPORT
#define LEVEL_CORRECTION // dynamic level values are used to correct calibrated angles (used dynamic level values are corrected based on centrifugal effects) SL
                         // !!! initLevel(ZFHeader* h) right now this->th_level = di.th_level_mrange; // check whole working range is set
                         //     this has to be changed back to=this->th_level = di.th_level_crange;


//#define RT_CALIBRATION  ///< Real Time Calibration


//------- end windows-64bit --------------
#else // WIN64
//------- start windows 32bit --------
#define ZFS_PTZ
#define ZFS_TIFF
#define ZFS_CXIMAGE
#define ZFS_JPG
#define ZFS_UK_CALIB
#define ZFS_PROGRESS
#define ZFS_MSG_CALLBACK
#define ZFS_INFO
#define ZFS_SCAN_FILTER
#define ZFS_LINEBYLINE_FILTER
#define ZFS_MATRIX
#define ZFS_SUPPORT_TEMP_POS
#define ZFS_MEM
#define ZFS_SCENEMODELER_SUPPORT
#define ZFS_5003_SUPPORT
#define ZFS_5006_SUPPORT
#define ZFS_5010_SUPPORT
#define ZFS_5016_SUPPORT
//#define ZFS_5018_SUPPORT // placeholder
#define ZFS_5024_SUPPORT

#define ZFS_COLOR_SUPPORT
#define LEVEL_CORRECTION // dynamic level values are used to correct calibrated angles (used dynamic level values are corrected based on centrifugal effects) SL
                         // !!! initLevel(ZFHeader* h) right now this->th_level = di.th_level_mrange; // check whole working range is set
                         //     this has to be changed back to=this->th_level = di.th_level_crange;

//------- end windows-32bit ----------------
#endif // WIN64

#else // _WINDOWS

//------- start linux (32 and 64bit) ----------------
// kein support geplant
//#define ZFS_PTZ
//#define ZFS_SCENEMODELER_SUPPORT

#define ZFS_UK_CALIB
#define ZFS_INFO
#define ZFS_SCAN_FILTER
#define ZFS_LINEBYLINE_FILTER
#define ZFS_MATRIX
#define ZFS_SUPPORT_TEMP_POS
#define ZFS_MEM
#define ZFS_5003_SUPPORT
#define ZFS_5006_SUPPORT
#define ZFS_5010_SUPPORT
#define ZFS_5016_SUPPORT
//#define ZFS_5018_SUPPORT
#define ZFS_5024_SUPPORT

#define ZFS_CXIMAGE
#define ZFS_COLOR_SUPPORT

// TODO: memove me since I'm unused
#define ZFS_MSG_CALLBACK
#define ZFS_PROGRESS
#define ZFS_JPG
#define ZFS_TIFF
#define LEVEL_CORRECTION

//#define RT_CALIBRATION

//------- end linux (32 and 64bit) ----------------

#endif //_WINDOWS

#pragma once
#include "../zfsapi.h"


#include "../../Common/inttypes_.h"
#include "filter_result.h"

class ZFSFilter;
class ZFLineAccess;

// ======================================================================================================================================================================
//       Filter Mask Indices Table 
// ======================================================================================================================================================================
// 
// overload             : 2
// intensity (min/max)  : 3
// wrong interval       : 4
// range                : 5
// mixed pixel filter   : 6
// jump filter          : 7
// single pixel         : 8
// nonius filter        : 10
// edge filter          : 11 ( delete frame of data set )
// small objects        : 12
// compensator /bad line: 20 ( is default in disable all )
//
// retrieval via: (1) uint8_t maskIdx= (ZFLineAccess*)zeile->getFilterTyp(PixelIdx);
//                (2) uint8_t maskIdx= (ZFSLinebyline*)lbl->GetLine()->getFilterTyp(PixelIdx);
// 
// ======================================================================================================================================================================
// Filter function interface
// ======================================================================================================================================================================

//ZFS_API bool FilterZeile( int pixelPerLine, ZFSFilter* filterParams, ZFLineAccess* zeile, ZFLineAccess *linePre, ZFLineAccess* lineNext, int& nFilteredOut, uint32_t fromPixel=0, uint32_t toPixel=0 );

namespace zfs
{

    /// Filter line using left and right line (3-line-filtering)
//    ZFS_API LineFilterResult FilterZeile(int pixelPerLine, ZFSFilter* filterParams, ZFLineAccess* zeile, ZFLineAccess *linePre, ZFLineAccess* lineNext, uint32_t fromPixel=0, uint32_t toPixel=0);
//    ZFS_API LineFilterResult FilterZeile(int pixelPerLine, ZFSFilter* filterParams, ZFLineAccess* zeile, ZFLineAccess *linePre, ZFLineAccess* lineNext);




}
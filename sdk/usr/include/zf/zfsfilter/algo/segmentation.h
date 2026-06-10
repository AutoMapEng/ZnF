#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include <zfs/layer.h>

#include "segmentation_tools.h"

namespace zfs
{
    typedef ZFLayerI32 ZFLayer_Segment;
}

namespace zfsfilter
{
    namespace algo
    {
        // ##############################################################

        #define LAYER_NAME_SCAN_GRADIENTS        L"scangradients"

        FILTERDLL_API bool scangradients( const std::wstring& a, const std::wstring& params );

        // ##############################################################

        #define LAYER_NAME_SEGMENTATION          L"segmentation"
        
        #define SEGMENTATION_LABEL_GRAD_EDGES    L"grad_edges"
        #define SEGMENTATION_LABEL_RELABELED     L"relabeled"
        #define SEGMENTATION_LABEL_SEGMENT_SIZES L"segment_sizes"

        FILTERDLL_API bool segmentation(const std::wstring& a, const std::wstring& params);

        FILTERDLL_API bool smallsegments(const std::wstring& a, const std::wstring& params);

        // ##############################################################

   }
}
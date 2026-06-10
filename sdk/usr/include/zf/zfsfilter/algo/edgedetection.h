#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include <zfs/layer.h>

namespace zfsfilter
{
    namespace algo
    {
        // ##############################################################

        #define LAYER_NAME_EDGE_NORMAL          L"normal_edge"
        #define LAYER_NAME_EDGE_GRADIENT_RF     L"rf_gradient_edge"
        #define LAYER_NAME_EDGE_GRADIENT_NORMAL L"normal_gradient_edge"

        // ##############################################################

        FILTERDLL_API bool normalEdgeDetection(const std::wstring& a, const std::wstring& normal_type_str);

        // ##############################################################

        FILTERDLL_API bool normalGradientEdgeDetection(const std::wstring& a, const std::wstring& normal_type_str);

        // ##############################################################

        FILTERDLL_API bool rfGradientEdgeDetection( const std::wstring& a, const std::wstring& params );

        // ##############################################################

    }
}
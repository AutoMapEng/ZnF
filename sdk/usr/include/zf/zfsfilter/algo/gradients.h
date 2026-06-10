#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include <zfs/layer.h>

namespace zfs
{
    //###################################################################

    typedef zfs::ZFLayerDouble     ZFLayer_RfGrad;
    typedef zfs::ZFLayerVec3_24bit ZFLayer_NormalGrad;

    //###################################################################
}

namespace zfsfilter
{
    namespace algo
    {

        // ##############################################################

        #define LAYER_NAME_NORMALGRADIENT L"normal_gradient" 

        // ##############################################################

        FILTERDLL_API bool normalGadientV1(const std::wstring& a, const std::wstring& param);

        // ##############################################################

        FILTERDLL_API bool normalGadientV2(const std::wstring& a, const std::wstring& param);

        // ##############################################################

        #define LAYER_NAME_RFGRADIENT L"rf_gradient"

        // ##############################################################

        FILTERDLL_API bool rfGradient( const std::wstring& a, const std::wstring& params );

        // ##############################################################

    }
}
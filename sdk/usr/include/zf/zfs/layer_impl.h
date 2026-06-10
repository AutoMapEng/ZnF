#pragma once

#include "layer.h"

namespace zfs
{

    /// some basic predefined layers names which are always available
    #define LAYER_NAME_XYZ               L"xyz"
    #define LAYER_NAME_RG                L"rg"
    #define LAYER_NAME_RF                L"rf"          ///< intensity
    #define LAYER_NAME_RFNORM            L"rfnorm"      ///< intensity (normalized)
    #define LAYER_NAME_NORMAL            L"normal"      ///< vector
    #define LAYER_NAME_QUALITY           L"quality"
    #define LAYER_NAME_COLOR             L"color"
    #define LAYER_NAME_TEMPERATURE       L"temperature"
    #define LAYER_NAME_MASK              L"mask"        ///< bit mask for disabling points


    typedef ZFLayerVec3d            ZFLayer_XYZ;

    typedef ZFLayerDouble           ZFLayer_Range;

    typedef ZFLayerI32              ZFLayer_Intensity;

    typedef ZFLayerDouble           ZFLayer_NormIntensity;

    typedef ZFLayerVec3f_16bit      ZFLayer_Normal;

    typedef ZFLayerI8               ZFLayer_Quality;

    typedef ZFLayerRGB_byte         ZFLayer_Color;

    typedef ZFLayerVec3f            ZFLayer_Temperature;

    typedef ZFLayerBit              ZFLayer_Mask;

}

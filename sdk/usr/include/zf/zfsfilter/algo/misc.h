#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include <zfs/layer.h>

namespace zfs
{
    //###################################################################

    typedef zfs::ZFLayerI8 ZFLayer_Dist;

    typedef zfs::ZFLayerFloat ZFLayer_Variance;

    typedef zfs::ZFLayerFloat ZFLayer_Mean;

    typedef zfs::ZFLayerFloat ZFLayer_Noise;

    typedef zfs::ZFLayerFloatPair ZFLayer_MPW;

    typedef zfs::ZFLayerVec3f ZFLayer_NormalSigma;

    typedef zfs::ZFLayerFloatPair ZFLayer_NormalAngleSigma;

    //###################################################################
}

namespace zfsfilter
{
    namespace algo
    {

        //###################################################################

        #define LAYER_NAME_INC L"incidence_angle"

        FILTERDLL_API bool incidenceAngleLayer(const std::wstring& a, const std::wstring& param);

        //###################################################################

        #define LAYER_NAME_DIST L"dist"

        FILTERDLL_API bool distanceLayer(const std::wstring& a);

        //###################################################################

        #define LAYER_NAME_NONIUS L"nonius"

        FILTERDLL_API bool noniusLayer(const std::wstring& a);
        FILTERDLL_API bool noniusLayerV2(const std::wstring& a);

        //###################################################################

        #define LAYER_NAME_NOISE L"noise" // rf noise

        FILTERDLL_API bool noiseLayer(const std::wstring& a, const std::wstring& params);

        //###################################################################

        #define LAYER_NAME_RANGE_VARIANCE L"range_variance"
        #define LAYER_NAME_RANGE_MEAN     L"range_mean"

        FILTERDLL_API bool rangeVarianceAndMeanLayer(const std::wstring& a, const std::wstring& params);

        //###################################################################

        #define LAYER_NAME_RF_VARIANCE L"rf_variance"
        #define LAYER_NAME_RF_MEAN  L"rf_mean"

        FILTERDLL_API bool rfVarianceAndMeanLayer(const std::wstring& a, const std::wstring& params);

        //###################################################################
      
        #define LAYER_NAME_MPW L"mpw" // mixed pixel width (not implemented yet)
      
        FILTERDLL_API bool mpwLayer(const std::wstring& a, std::wstring sky_class_sub_name );

        //###################################################################

        #define LAYER_NAME_NORM_RF_VARIANCE L"rfnorm_variance"
        #define LAYER_NAME_NORM_RF_MEAN  L"rfnorm_mean"
      
        FILTERDLL_API bool normRfVarianceAndMeanLayer(const std::wstring& a, const std::wstring& window_size_str);

        //###################################################################

        #define LAYER_NAME_RATIO_RG_STDDEV_2_EXP_NOISE  L"range_stdev2noise"

        FILTERDLL_API bool ratioRgStddevToNoiseLayer(const std::wstring& a, const std::wstring& param);

        //###################################################################

        #define LAYER_NAME_NORMAL_MEAN        L"normal_mean"
        #define LAYER_NAME_NORMAL_SIGMA       L"normal_stdev"
        #define LAYER_NAME_NORMAL_ANGLE_SIGMA L"normal_angle_stdev"

        FILTERDLL_API bool normalFiltLayer(const std::wstring& a, const std::wstring& params);

        //###################################################################

        #define LAYER_NAME_CUBE_MAP L"cube_map"
        #define LAYER_NAME_XYZ_CUBE L"xyz_cube"

        FILTERDLL_API bool cubeMapLayer(const std::wstring& a, const std::wstring& params);

        //###################################################################
    }
}
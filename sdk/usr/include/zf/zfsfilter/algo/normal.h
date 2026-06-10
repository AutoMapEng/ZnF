#pragma once

#include <zfsfilter/filterapi.h>

#include <string>

#include <zfs/layer.h>

#define LAYER_NAME_ANGLE0            L"angle0"
#define LAYER_NAME_ANGLE             L"angle"

namespace zfs
{
    typedef ZFLayerDoublePair ZFLayer_Angle;
}

namespace zfsfilter
{
    namespace algo
    {
        //#############################################################################

        /// calculate xyz and rf for every point
        enum { CREATE_XYZ = 1, CREATE_RF = 2, CREATE_RFNORM = 4, CREATE_COLOR = 8, CREATE_MASK = 0x10, CREATE_MISC=0x20, CREATE_ALL = 0xffff };
        FILTERDLL_API bool xyz4Scan(const std::wstring& a, const std::wstring& params=L"", int create=CREATE_ALL );

        //#############################################################################

        /// calculate normal vector for every point V1=fast
        FILTERDLL_API bool normal4ScanV1(const std::wstring& a);

        /// calculate normal vector for every point V2=accurate
        FILTERDLL_API bool normal4ScanV2(const std::wstring& a, std::wstring param_str= L"4");

        /// calculate normal vector for every point V3=test version using fixed distance for points
        FILTERDLL_API bool normal4ScanV3(const std::wstring& a, std::wstring param_str= L"0.005");

        /// calculate normal vector for every point V4=test version using laser spot diameter
        FILTERDLL_API bool normal4ScanV4(const std::wstring& a, std::wstring param_str);

        //#############################################################################

    }
}
#pragma once
#include "../zfsapi.h"

#include <string>

namespace zfs
{

    // the way how to execute the filter
    class FilterExecution
    {
    public:
        bool clearOldMasks = true;
        bool singleMask = true;
        bool createReport = true;
        bool createParamFile = true;
        bool runMixPixEnh{ true };
        int  cpuUsage{ 100 }; ///< percent of CPU usage

        std::wstring zfl_name{ L"mask" }; // the name of the zfl filter file

        FilterExecution() = default;
    };
}
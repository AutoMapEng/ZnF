#pragma once
#ifdef _WINDOWS 
#include "../filterapi.h"
#include <string>

#ifdef _WINDOWS
namespace zfsfilter
{
    /// Dauertest eines Profilerscans auswerten
    /// return filename of reportfile
    struct ProfilerCheckParams
    {
        int checkPosition{};
        double accuracy{ 0.1 };
        int subSample{ 1 };

        int posFrom{};
        int posTo;
    };

    FILTERDLL_API std::wstring checkProfilerScan(const std::wstring& zfs, const ProfilerCheckParams& );

} // end namespace
#endif

#endif // _WINDOWS

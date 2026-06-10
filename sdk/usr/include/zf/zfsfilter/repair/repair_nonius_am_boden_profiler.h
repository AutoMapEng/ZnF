#pragma once
#include <zfsfilter/filterapi.h>

#include <string>

namespace zfsfilter
{
    /**
    \brief Nonius Spruenge auf der Strasse bei einem Profiler 9012 zurueckrechnen.
    */
    FILTERDLL_API std::wstring repariereNoniusRectangle(const std::wstring& zfsFile);
}
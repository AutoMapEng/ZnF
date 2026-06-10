#pragma once

#include <string>



void Sample_LineByLineAccess_Profiler_V10(const std::wstring& win, const std::wstring& out, bool bSync=true, int subsample = 1);
void Sample_LineByLineAccess_Profiler(const std::wstring& win, const std::wstring& out, bool bSync = true, int subsample = 1);
// a multithreaded random access to the file
void Sample_LineByLineAccess_Profiler_MT(const std::wstring& win, bool bSync=true);


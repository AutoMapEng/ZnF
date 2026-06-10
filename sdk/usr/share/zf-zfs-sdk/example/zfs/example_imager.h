#pragma once

#include <string>

/// minimal example to start with
void Sample_LineByLineAccess_minimalCode(const std::wstring& win, const std::wstring& out, int subsample = 1);

/// a more advanced example
void Sample_LineByLineAccess_Imager_fullExample(const std::wstring& win, const std::wstring& out, int subsample=1);

/// a version which read a whole scan into memory (not recommended to use)
int Sample_WholeScan(const std::wstring& in, const std::wstring& out, int subsample = 1);


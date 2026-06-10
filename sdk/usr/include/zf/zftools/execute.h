

#pragma once

#include "zftools_api.h"

#include <cstdint>
#include <string>
#include <vector>
#include <functional>


namespace zftools
{
    /// open document
    ZFTOOLS_API bool shellExecute(const std::wstring& open);

    /// execute app with param enclosed in "
    ZFTOOLS_API bool execute(const std::wstring& app, const std::wstring& param=L"" );

    /// execute app with param enclosed not in "
    ZFTOOLS_API bool execute2(const std::wstring& app, const std::wstring& param);

    /// create windows process
    ZFTOOLS_API uint32_t startProc(const std::wstring& app, const std::wstring& param, const std::wstring& dir = L"", bool hide_flag = false, std::function<void()> cbWait=nullptr );

    /// open windows explorer and select files
    ZFTOOLS_API void openSelectFilesInExplorer(const std::wstring &folder, const std::vector<std::wstring> &fileList = std::vector<std::wstring>() );

} // end namespace zftools


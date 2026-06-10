#pragma once
#ifdef _WIN32

#include "zfcore_api.h"

#include <cstdint>
#include <string>
#include <vector>
#include <functional>


namespace zf
{
    /**
     * throw exception if app does not exist or crashed
     * \return exit code of the application
     */
    ZFCORE_API int executeX(const std::wstring& app, const std::wstring& param = L"", bool show=false, bool throwIfErrorcode=true );


    /// open .exe app
    ZFCORE_API bool schellExecuteOpenexe(const std::string& open, const std::string& parameter);
    /// open document
    ZFCORE_API bool shellExecute(const std::wstring& open);

    /// open FTP explorer
    ZFCORE_API bool shellExecuteFTP(const std::string& addr);

    /// execute app with param enclosed in "
    ZFCORE_API bool execute(const std::wstring& app, const std::wstring& param=L"" );

    /// execute app with param enclosed not in "
    ZFCORE_API bool execute2(const std::wstring& app, const std::wstring& param);

    /// create windows process
    ZFCORE_API int32_t startProc(const std::wstring& app, const std::wstring& param, const std::wstring& dir = L"", bool hide_flag = false, std::function<void()> cbWait=nullptr );

    /// open windows explorer and select files
    ZFCORE_API void openSelectFilesInExplorer(const std::wstring &folder, const std::vector<std::wstring> &fileList = std::vector<std::wstring>() );


    // experimental
    ZFCORE_API int execute_console(const std::wstring& exe, const std::wstring& prm, std::function<void(const std::string& )> cb );

} // end namespace zftools

#endif

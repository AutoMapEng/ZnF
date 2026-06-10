#pragma once

#ifdef _WINDOWS
#include "zfcore_api.h"
#include <string>

#include <windows.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <Dbghelp.h>
#pragma warning(pop)

namespace zf
{
    /// set some information required for crash dump creation
    ZFCORE_API void WINAPI setAppInfo(const std::wstring& productName, const std::wstring& versionNumber, const std::wstring& buildNumber, const std::wstring& buildDetails);

    /**
    \brief crash dump handler
        example: SetUnhandledExceptionFilter(TopUnhandledExceptionFilter);
    */
    ZFCORE_API LONG WINAPI TopUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo);
}

#endif // _WINDOWS

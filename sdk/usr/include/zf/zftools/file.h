#pragma once

/**
 * @file
 *
 * Contains <stdio.h> style fopen() and close() functions which handle narrow
 * and multibyte strings.
 *
 * On Windows these functions are just forwarded to fopen() respectively
 * _wfopen() for multibyte. Other platforms do a UTF16 to UTF8 conversion
 * before calling fopen().
 *
 * In all cases, a system dependend FILE* pointer is returned
 */

#include <string>
#include <cstdio>
#include <zfcore/unicode.h>

namespace zftools {

#ifdef _WINDOWS
inline FILE* fopen(const wchar_t* path, const wchar_t* mode)
{
    return ::_wfopen(path, mode);
}

inline FILE* fopen(const std::wstring& path, const wchar_t* mode)
{
    return ::_wfopen(path.c_str(), mode);
}
#else
inline FILE* fopen(const wchar_t* path, const wchar_t* mode)
{
    return ::fopen(zf::utf16_to_utf8(path).c_str(), zf::utf16_to_utf8(mode).c_str());
}

inline FILE* fopen(const std::wstring& path, const wchar_t* mode)
{
    return ::fopen(zf::utf16_to_utf8(path).c_str(), zf::utf16_to_utf8(mode).c_str());
}
#endif

inline FILE* fopen(const char* path, const char* mode)
{
    return ::fopen(path, mode);
}

inline FILE* fopen(const std::string& path, const char* mode)
{
    return ::fopen(path.c_str(), mode);
}

inline int fclose(FILE *stream)
{
    return ::fclose(stream);
}

}

#pragma once
// include only


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#define ZFSPRINTF_MAX_CHARS 8096

#ifdef _WINDOWS
#define VSNPRINTF    _vsnprintf
#define VSNWPRINTF   _vsnwprintf
#else
#define VSNPRINTF    vsnprintf
#define VSNWPRINTF   vswprintf
#endif

#ifdef __GNUC__
#define CHECK_PRINTF_ARGS(fmtpos, checkpos) __attribute__((format(printf, fmtpos, checkpos)))
#define CHECK_WPRINTF_ARGS(fmtpos, checkpos)
#else
#define CHECK_PRINTF_ARGS(fmtpos, checkpos)
#define CHECK_WPRINTF_ARGS(fmtpos, checkpos)
#endif

//==============================================================================================================

CHECK_PRINTF_ARGS(1, 2)
inline std::string zfsprintf( const char* szFormat,...)
{
    std::string res;
    std::vector<char> _buffer(ZFSPRINTF_MAX_CHARS);
    va_list argList;
    va_start(argList,szFormat);
    int ret = VSNPRINTF( &_buffer[0], ZFSPRINTF_MAX_CHARS, szFormat, argList );
    va_end(argList);
    res.assign( &_buffer[0], ret );
    return res;
}

// ----------------------------------------------------------------------------------------

CHECK_WPRINTF_ARGS(1, 2)
inline std::wstring zfswprintf( const wchar_t* szFormat,...)
{
    std::wstring res;
    std::vector<wchar_t> _buffer(ZFSPRINTF_MAX_CHARS);
    va_list argList;
    va_start(argList,szFormat);
    int ret = VSNWPRINTF( &_buffer[0], ZFSPRINTF_MAX_CHARS, szFormat, argList );
    va_end(argList);
    res.assign( &_buffer[0], ret );
    return res;
}

//==============================================================================================================

CHECK_WPRINTF_ARGS(2, 3)
inline std::wstring zfswprintfcol( int col, const wchar_t* szFormat,...)
{
    std::wstring res;
    std::vector<wchar_t> _buffer(ZFSPRINTF_MAX_CHARS+40);
    va_list argList;
    int l = swprintf( &_buffer[0], ZFSPRINTF_MAX_CHARS, L"<font color='#%06x'>", col );
    va_start(argList,szFormat);
    int ret = VSNWPRINTF(&_buffer[l],ZFSPRINTF_MAX_CHARS,szFormat,argList);
    va_end(argList);
    wcscat( &_buffer[0], L"</font>" );
    res.assign(&_buffer[0],ret+l+7);
    return res;
}

//==============================================================================================================

CHECK_PRINTF_ARGS(2, 3)
inline std::string zfsprintfcol( int col, const char* szFormat,...)
{
    std::string res;
    std::vector<char> _buffer(ZFSPRINTF_MAX_CHARS+40);
    va_list argList;
    va_start(argList,szFormat);

    int l = sprintf(&_buffer[0], "<font color='#%06x'>", col);
    int ret = VSNPRINTF(&_buffer[l],ZFSPRINTF_MAX_CHARS,szFormat,argList);

    va_end(argList);
    strcat(&_buffer[0], "</font>");
    res.assign(&_buffer[0],ret+l+7);
    return res;
}

//==============================================================================================================

CHECK_PRINTF_ARGS(2, 3)
inline std::string zfsprintf( int sz, const char* szFormat,...)
{
    std::string res;
    std::vector<char> _buffer(sz+1);
    va_list argList;
    va_start(argList,szFormat);
    int ret = VSNPRINTF( &_buffer[0], sz, szFormat, argList );
    va_end(argList);
    res.assign( &_buffer[0], ret );
    return res;
}

//==============================================================================================================

namespace zf
{

    typedef std::vector<std::string> stringlist;
    typedef std::vector<std::wstring> wstringlist;


} // end namespace zf

inline std::string string_list_to_string(const zf::stringlist& string_list, bool omit_empty_lines= false)
{
    std::string stringlist_in_string;

    for (auto line : string_list)
    {

        if(omit_empty_lines)
        {
            if(line.find_first_not_of(" ") != std::string::npos)
                stringlist_in_string += line + "\n";
        }
        else
        {
            stringlist_in_string += line + "\n";
        }
    }

    return stringlist_in_string;
}

//==============================================================================================================

//ZFTOOLS_API int StrSplit( std::string& s, std::vector<std::string>& lst, const char t=' ' );
inline int StrSplit( const std::string& s, std::vector<std::string>& lst, char t )
{
    if( s.length() )
    {
        const char* p = s.c_str();
        std::string i;
        while( *p )
        {
            if( *p==t )
            {
                if( i.length() )
                {
                    lst.push_back(i);
                }
                i.clear();
            }
            else
                i += *p;

            p++;
        }
        if( i.length() )
        {
            lst.push_back(i);
        }
    }
    return static_cast<int>(lst.size());
}

inline int StrSplit( const std::string& s, std::vector<std::string>& lst, const char* t )
{
    // this is broken! takes only the first char in t into account!!!
    return  StrSplit( s, lst, t[0] );
}

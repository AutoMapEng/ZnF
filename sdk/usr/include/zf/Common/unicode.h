#pragma once
#include "common_api.h"

#include <cstdint>
#include <stdexcept>
#include <string>


namespace zfsdll 
{

    class COMMON_API ZFConvert
    {
    public:
        static std::string& UTF8_to_CP(const char* szText, std::string& resultString);

        static std::string& CP_to_UTF8(const char* szText, std::string& resultString);

        static std::wstring& CP_to_UTF16(const char* szText, std::wstring& resultString);
        static std::wstring CP_to_UTF16(const std::string& szText);

        static std::string& UTF16_to_CP(const wchar_t* szText, std::string& resultString);
        static std::string UTF16_to_CP(const std::wstring& szText);

        /**
         * @brief Legacy wstring -> utf8 conversion
         *
         * @warning despite the name, this function does not neccessarily a
         *          conversion from utf16! Instead it converts from L"" -> u8""
         *          which is under Linux UTF32 and under Windows UTF16!
         *
         * @param ws   wide string (Linux: UTF32, Windows: UTF16)
         * @param dest Destination of conversion
         * @return UTF8 encoded string. Errors are replaced with a '?' char
         */
        static std::string& UTF16_to_UTF8(const std::wstring& ws, std::string& dest);
        /** @overload */
        static std::string UTF16_to_UTF8(const std::wstring& ws);

        /**
         * @brief Legacy utf8 -> wstring conversion
         *
         * @warning despite the name, this function does not neccessarily a
         *          conversion to utf16! Instead it converts from u8"" -> L""
         *          which is under Linux UTF32 and under Windows UTF16!
         *
         * @param cs   utf8 encdoed source string
         * @param dest Destination receiving the result of the conversion
         * @return wide string (Linux: UTF32, Windows UTF16)
         */
        static std::wstring& UTF8_to_UTF16(const std::string& cs, std::wstring& dest);
        /** @overload */
        static std::wstring UTF8_to_UTF16(const std::string& cs);

    #if defined(WIN32) || defined(_WIN32)
        static const std::wstring convert_filename(const std::wstring& in)
        {
            return in; // nothing to do
        }
    #else
        static const std::string convert_filename(const std::wstring& in)
        {
            std::string dst;
            return zfsdll::ZFConvert::UTF16_to_UTF8(in, dst);
        }
    #endif
    };


} // end namespace zfsdll

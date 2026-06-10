#pragma once
#include "zfcore_api.h"

/**
 * @file
 *
 * Common string functions
 *
 * Provides toupper(), tolower() as templates.
 *
 * @note The correctness of these functions may depend on file encoding!
 */

#include <algorithm> 
#include <iterator>
#include <string>
#include <iostream>
#include <codecvt>


namespace zf {

    namespace detail {

    template<typename T>
    T upper_func(T c) {
        return static_cast<T>(::toupper(static_cast<int>(c)));
    }

    template<typename T>
    T lower_func(T c) {
        return static_cast<T>(::tolower(static_cast<int>(c)));
    }

    } // namespace detail








    /**
     * @brief Convert letters in str to upper case
     *
     * @note If T is std::string, non ASCII letters may not get converted!
     *       (e.g. if input is encoded as UTF8)
     *
     * @param str String to convert
     * @return independent copy of str with all letters to upper case converted
     */
    template<typename T>
    T toupper(const T& str)
    {
        T tmp;
        tmp.reserve(str.size());

        std::transform(std::begin(str), std::end(str),
                       std::back_inserter(tmp),
                       detail::upper_func<typename T::value_type>);

        return tmp;
    }

    /**
     * @overload T toupper(const T& str)
     */
    inline std::string toupper(const char* str)
    {
        return toupper<std::string>(str);
    }

    /**
     * @overload T toupper(const T& str)
     */
    inline std::wstring toupper(const wchar_t* str)
    {
        return toupper<std::wstring>(str);
    }

    /**
     * @brief Converts letters in str to lower case
     *
     * @note If T is std::string, non ASCII letters may not get converted
     *       (e.g. if input is encoded as UTF8)
     *
     * @param str input string to convert to lower case
     * @return copy of str with all letters converted to lower case
     */
    template<typename T>
    T tolower(const T& str)
    {
        T tmp;
        tmp.reserve(str.size());

        std::transform(std::begin(str), std::end(str),
                       std::back_inserter(tmp),
                       detail::lower_func<typename T::value_type>);

        return tmp;
    }

    /** @overload T tolower(const T& str) */
    inline std::string tolower(const char* str)
    {
        return tolower<std::string>(str);
    }

    /** @overload T tolower(const T& str) */
    inline std::wstring tolower(const wchar_t* str)
    {
        return tolower<std::wstring>(str);
    }

    //=================================================================

    inline std::string format_number_with_separator(int64_t number) 
    {
        std::string formatted_string = std::to_string(abs(number));
        int comma_counter = 0;
        for (int i = static_cast<int>(formatted_string.length() - 1); i >= 0; i--) {
            if (comma_counter == 2) {
                if(i!=0)
                    formatted_string.insert(i, "'");
                comma_counter = 0;
            }
            else
                comma_counter++;
        }
        if(number<0)
            formatted_string.insert(0, "-");
        return formatted_string;
    }

#ifdef WIN32
    inline std::wstring wformat_number_with_separator(int64_t number)
    {
        std::string formatted_string__ = std::to_string(abs(number));
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring formatted_string = converter.from_bytes(formatted_string__);
        int comma_counter = 0;
        for (int i = static_cast<int>(formatted_string.length() - 1); i >= 0; i--) {
            if (comma_counter == 2) {
                if (i != 0)
                    formatted_string.insert(i, L"'");
                comma_counter = 0;
            }
            else
                comma_counter++;
        }
        if (number < 0)
            formatted_string.insert(0, L"-");
        return formatted_string;
    }
#endif


    /**
     * Increments the last numer of a name or add a number
     * example name1 -> name2
     * example name -> name1
     * Empty strings will not be changed
     * 
     * \param s string to manipulate
     */
    ZFCORE_API
        void incrementName( std::wstring& s );

} // end namespace


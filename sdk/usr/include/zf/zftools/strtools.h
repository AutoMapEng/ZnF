#pragma once

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

namespace zftools {

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

}

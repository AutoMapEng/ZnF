#pragma once
#include "common_api.h"

#include <cstdint>
#include <stdexcept>
#include <string>


namespace zfsdll
{

    namespace utf
    {

        #if (defined(_MSC_VER) && (_MSC_VER > 1800)) || defined(__GNUC__)
        #define UNICODE_HAVE_CXX11

        using u16string = std::u16string;
        using u32string = std::u32string;
        #define UNICODE_NOEXCEPT noexcept

        #else
        // VS 2013 (_MSC_VER == 1800) has verry incomplete C++11 support.
        // There is no support for Unicode (char16_t, char32_t, u-, u8-, U- prefixes)
        // No support for constexpr or noexcept keywords

        using char32_t = uint32_t;
        using char16_t = uint16_t;

        using u16string = std::basic_string<char16_t>;
        using u32string = std::basic_string<char32_t>;
        #define UNICODE_NOEXCEPT throw()

        #endif

        /**
         * @brief Possible decoding errors
         */
        enum class error {
            ok, ///< no error occured
            need_input, ///< need more input characters to decode code point
            overly_long, ///< utf-8 code sequence is overly long
            not_a_start_seq, ///< expected a start sequence
            not_trail, ///< expected a trail sequence
            not_valid, ///< decoded codepoint is not valid
        };

        COMMON_API const char* to_string(error err);

        #ifdef WIN32
        __pragma(warning(push))
        __pragma(warning(disable: 4275)) /* Warning about DLL-Interface */
        #endif

        /**
         * @brief Base class for all utf related errors.
         *
         * Derived from std::invalid_argument, since errors occur because of
         * invalid input strings
         */
        class COMMON_API unicode_error : public std::invalid_argument
        {
        public:
            unicode_error(const unicode_error&) = default;
            unicode_error& operator=(const unicode_error&) = default;

        #ifdef UNICODE_HAVE_CXX11
            unicode_error(unicode_error&&) = default;
            unicode_error& operator=(unicode_error&&) = default;
        #endif

            explicit unicode_error(const char* msg)
                : std::invalid_argument(msg)
            {}
            ~unicode_error() override;

            unicode_error()
                : unicode_error("unicode error")
            {}
        };

        /**
         * @brief Error thrown if encoding utf-32 to utf-8 or utf-16 has failed
         */
        class COMMON_API encoding_error : public unicode_error
        {
        public:
            encoding_error(const encoding_error&) = default;
            encoding_error& operator=(const encoding_error&) = default;

        #ifdef UNICODE_HAVE_CXX11
            encoding_error(encoding_error&&) = default;
            encoding_error& operator=(encoding_error&&) = default;
        #endif

            explicit encoding_error(char32_t code_point)
                : unicode_error {"Failed to encode codepoint"},
                  m_code_point {code_point}
            {}
            ~encoding_error() override;

            /**
             * @brief Get the code point causing the encoding failure
             * @return code point
             */
	        char32_t code_point() const UNICODE_NOEXCEPT
            {
                return m_code_point;
            }

        private:
            char32_t m_code_point; ///< code point resulting in encoding error
        };

        //#############################################################################

        /**
         * @brief Error thrown if decoding utf-8 or utf-16 fails
         */
        class COMMON_API decoding_error : public unicode_error
        {
        public:
            decoding_error(const decoding_error&) = default;
            decoding_error& operator=(const decoding_error&) = default;

        #ifdef UNICODE_HAVE_CXX11
            decoding_error(decoding_error&&) = default;
            decoding_error& operator=(decoding_error&&) = default;
        #endif

            explicit decoding_error(error reason)
                : unicode_error {to_string(reason)},
                  m_code {reason}
            {}
            ~decoding_error() override;

            /**
             * @brief get the reason why decoding has failed
             * @return error code
             */
	        error error_code() const UNICODE_NOEXCEPT
            {
                return m_code;
            }

        private:
            error m_code; ///< the reason why decoding has failed
        };

        #ifdef WIN32
        __pragma(warning(pop))
        #endif


        /// unicode REPLACEMENT code point (�-character)
        #ifdef UNICODE_HAVE_CXX11
        constexpr char32_t replacement = U'\ufffd';
        #else
        static const char32_t replacement = 0xfffd;
        #endif


        /* The following functions transcode between utf-8, utf-16 and utf-32
         * and vice versa.
         *
         * If the replacement character is set to U'\0' (the default) the functions
         * throw in case an wrongly encoded character is discovered in the input stream.
         * Otherwise the replacement character is placed instead of the broken
         * character.
         * For this, you may use the unicode REPLACEMENT code point (U'\ufffd' or �).
         *
         * NOTE: wstring is defined in the way that it matches the compilers
         *       L".."-prefix, which is plattform dependent.
         *       For Linux this is utf-32, for Windows it is utf-16!
         */

        #if __cplusplus >= 201703L

        // *************************************************************************
        // convert utf8 -> utfx
        // *************************************************************************

        COMMON_API std::u16string utf8to16(std::string_view v, char32_t repl=0);
        COMMON_API std::u32string utf8to32(std::string_view v, char32_t repl=0);
        COMMON_API std::wstring utf8_to_wstring(std::string_view v, char32_t repl=0);

        // *************************************************************************
        // convert utfx -> utf8
        // *************************************************************************

        COMMON_API std::string utf16to8(std::u16string_view v, char32_t repl=0);
        COMMON_API std::string utf32to8(std::u32string_view v, char32_t repl=0);
        COMMON_API std::string wstring_to_utf8(std::wstring_view v, char32_t repl=0);

        // *************************************************************************
        // repair utf8 or utf16 strings
        // *************************************************************************

        /**
         * @brief Repair string @c v by replacing invalid encoding via @c repl
         *
         * @param v Input string to check
         * @param repl Replacement char (!= 0)
         * @return repaired string
         */
        COMMON_API std::string repair(
            std::string_view v, char32_t repl = replacement);

        /** @overload */
        COMMON_API std::u16string repair(
            std::u16string_view v, char32_t repl = replacement);

        #else



        // *************************************************************************
        // convert utf8 -> utfx
        // *************************************************************************

        // utf8 -> utf16
        COMMON_API u16string utf8to16(const char* v, char32_t repl=0);
        COMMON_API u16string utf8to16(const std::string& v, char32_t repl=0);

        // utf8 -> utf32
        COMMON_API u32string utf8to32(const char* v, char32_t repl=0);
        COMMON_API u32string utf8to32(const std::string& v, char32_t repl=0);

        // utf8 -> wstring (Linux: utf32; Windows: utf16)
        COMMON_API std::wstring utf8_to_wstring(const char* v, char32_t repl=0);
        COMMON_API std::wstring utf8_to_wstring(const std::string& v, char32_t repl=0);

        // *************************************************************************
        // convert utfx -> utf8
        // *************************************************************************

        // utf16 -> utf8
        COMMON_API std::string utf16to8(const char16_t* v, char32_t repl=0);
        COMMON_API std::string utf16to8(const u16string& v, char32_t repl=0);

        // utf32 -> utf8
        COMMON_API std::string utf32to8(const char32_t* v, char32_t repl=0);
        COMMON_API std::string utf32to8(const u32string& v, char32_t repl=0);

        // wstring (Linux: utf32; Windows: utf16) -> utf8
        COMMON_API std::string wstring_to_utf8(const wchar_t* v, char32_t repl=0);
        COMMON_API std::string wstring_to_utf8(const std::wstring& v, char32_t repl=0);

        // *************************************************************************
        // repair utf8 or utf16 strings
        // *************************************************************************

        /**
        * @brief Repair string @c v by replacing invalid encoding via @c repl
        *
        * @param v Input string to check
        * @param repl Replacement char (!= 0)
        * @return repaired string
        */
        COMMON_API std::string repair(
            const std::string &v, char32_t repl = replacement);

        /** @overload */
        COMMON_API std::u16string repair(
            const std::u16string &v, char32_t repl = replacement);

        #endif // string_view support

    } // end namespace utf
} // end namespace zfsdll

#pragma once

#include <zfsfilter/filterapi.h>

#include <Common/unicode.h>
#include <string>

namespace zfsfilter
{
    namespace algo
    {
        namespace strutils
        {
            // ####################################################################################################################

            // . arguments should be std::wstring* (mind pointer!)
            // . arguments are filled subsequently
            // . (hence) arguments must be in a order so that optional arguments are at the end of the argument list
            bool FILTERDLL_API decompose_argument_string( std::wstring params, std::wstring seperator, ... );
            bool FILTERDLL_API decompose_argument_string( std::string params, std::string seperator, ... );
            
            std::pair<std::string, std::string> getNormalVersions( const std::string& param_string );
            std::pair<std::wstring, std::wstring> getNormalVersions( const std::wstring& param_string );

            std::wstring FILTERDLL_API deleteEndDezPt( const std::wstring& number_str );

            std::string FILTERDLL_API shorten( const std::string& input, size_t length );
            std::wstring FILTERDLL_API shorten( const std::wstring& input, size_t length );

            // ####################################################################################################################
        }
    }
}
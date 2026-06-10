#pragma once

#include "zfcore_api.h"

#include <string>
#include <mutex>

namespace zf
{
    #define MAX_LOG2FILE_PRINT_LEN   1024


    /**
    * log messages to file
    */
    class ZFCORE_API Log2File
    {
    public:
        Log2File() = default;
        Log2File( const std::wstring& file, bool append=true, bool withTime=true );
        Log2File( const Log2File& ) = default;

        Log2File& operator = (const Log2File&) = default;
        Log2File& operator = ( Log2File&& ) = default;


        /// \n will be added automatically
        void print(const std::string&);
        void print(const std::wstring&);

        void printf( const char*, ...);


    protected:
        std::wstring m_fn;
        mutable std::mutex m_mut;
        bool m_withTime{};
    };
}

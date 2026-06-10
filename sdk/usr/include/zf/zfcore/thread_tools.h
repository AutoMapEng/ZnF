#pragma once
#include "zfcore_api.h"

#include <string>
#include <thread>

namespace zf
{
    // set current thread name
    ZFCORE_API
        void set_thread_name(const std::wstring& n);
    ZFCORE_API
        void set_thread_name(const std::string& n);

    // set spezified thread to name
    ZFCORE_API
        void set_thread_name(std::thread& th, const std::wstring& n);
    ZFCORE_API
        void set_thread_name(std::thread& th, const std::string& n);

    ZFCORE_API
        void set_thread_name(void* handle, const std::wstring& n);
    ZFCORE_API
        void set_thread_name(void* handle, const std::string& n);

    // just for windows. Disable power throttling for background threads
    ZFCORE_API
        bool qos_disable_powerthrottling();


    ZFCORE_API
        bool backgroundprocessing_max_cpu_speed(bool enable);
}

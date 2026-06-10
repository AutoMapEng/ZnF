#pragma once
#include <cstdint>


namespace lrcsocket
{
#ifdef _WINDOWS
#ifdef WIN64
    typedef uint64_t zfsocket;
#else
    typedef uint32_t zfsocket;
#endif
#else
    typedef int zfsocket;
#endif
}


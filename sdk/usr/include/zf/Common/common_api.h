#pragma once
#if defined(_WIN32)
#if 0
# ifdef COMMON_EXPORTS
#  define COMMON_API __declspec(dllexport)
# else
#  define COMMON_API __declspec(dllimport)
# endif
#endif
#define COMMON_API
#else
    #define COMMON_API __attribute__ ((visibility ("default")))
    #define COMMON_PUBLIC __attribute__ ((visibility ("default")))
    #define COMMON_LOCAL  __attribute__ ((visibility ("hidden")))
#endif

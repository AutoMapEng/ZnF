#pragma once

#ifdef _WINDOWS
# ifdef ZFTOOLS_EXPORTS
#  define ZFTOOLS_API __declspec(dllexport)
# else
#  define ZFTOOLS_API __declspec(dllimport)
# endif
#elif __GNUC__ >= 4
# define ZFTOOLS_API __attribute__ ((visibility ("default")))
#endif

#ifdef _WINDOWS
# pragma warning( disable: 4251 )
#endif


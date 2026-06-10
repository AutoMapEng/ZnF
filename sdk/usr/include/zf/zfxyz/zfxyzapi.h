#pragma once

#ifdef _WINDOWS
# ifdef ZFXYZ_EXPORTS
#  define ZFXYZ_API __declspec(dllexport)
#  define ZFXYZ_API_F __declspec(dllexport)
# else
#  define ZFXYZ_API __declspec(dllimport)
#  define ZFXYZ_API_F __declspec(dllimport)
# endif
#elif __GNUC__ >= 4
# define __forceinline inline
# define ZFXYZ_API __attribute__ ((visibility ("default")))
# define ZFXYZ_API_F __attribute__ ((visibility ("default")))
#endif

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#ifdef _WINDOWS
# pragma warning( disable: 4251 )
#endif


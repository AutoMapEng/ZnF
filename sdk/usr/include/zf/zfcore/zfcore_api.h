#pragma once

#ifdef _WINDOWS
# ifdef ZFCORE_EXPORTS
#  define ZFCORE_API __declspec(dllexport)
#  define ZFCORE_API_F __declspec(dllexport)
# else
#  define ZFCORE_API __declspec(dllimport)
#  define ZFCORE_API_F __declspec(dllimport)
# endif
#elif __GNUC__ >= 4
# define ZFCORE_API    __attribute__ ((visibility ("default")))
# define ZFCORE_API_F  __attribute__ ((visibility ("default")))
# define __forceinline inline
#else
# define __forceinline inline
# define ZFCORE_API
# define ZFCORE_API_F  extern
#endif

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#ifdef _WINDOWS
#pragma warning( disable: 4251 4100 4127 4800 )
// 4100: Unreferenzierter formaler Parameter
// 4251: DLL Schnittstelle
// 4127: Bedingter Ausdruck ist konstant
// 4800: 'int' : Variable wird auf booleschen Wert
#endif

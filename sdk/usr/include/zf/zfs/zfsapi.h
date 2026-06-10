#pragma once

#ifdef _WINDOWS
# ifdef ZFS_EXPORTS
#  define ZFS_API __declspec(dllexport)
#  define ZFS_API_F __declspec(dllexport)
# else
#  define ZFS_API __declspec(dllimport)
#  define ZFS_API_F __declspec(dllimport)
# endif
#elif __GNUC__ >= 4
# define ZFS_API    __attribute__ ((visibility ("default")))
# define ZFS_API_F  __attribute__ ((visibility ("default")))
# define __forceinline inline
#else
# define __forceinline inline
# define ZFS_API
# define ZFS_API_F  extern
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

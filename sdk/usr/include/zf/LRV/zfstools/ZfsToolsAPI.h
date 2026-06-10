#pragma once

/**
\namespace zfstools
\brief The namespace of the zfstools target.
*/

#ifdef _WINDOWS
# ifdef ZFSTOOLS_EXPORTS
#  define ZFSTOOLS_API __declspec(dllexport)
# else
#  define ZFSTOOLS_API __declspec(dllimport)
# endif
#else
# define ZFSTOOLS_API __attribute__((visibility ("default")))
#endif


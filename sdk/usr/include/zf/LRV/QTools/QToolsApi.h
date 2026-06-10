#ifndef __QTOOLSAPI_H__
#define __QTOOLSAPI_H__


#ifdef _WINDOWS
# ifdef QTOOLS_EXPORTS
#  define QTOOLS_API __declspec(dllexport)
# else
#  define QTOOLS_API __declspec(dllimport)
# endif
#else
# define QTOOLS_API __attribute__((visibility("default")))
#endif


#endif //__QTOOLSAPI_H__

#pragma once

#ifdef _WINDOWS
#ifdef FILTERDLL_EXPORTS
#define FILTERDLL_API __declspec(dllexport)
#else
#define FILTERDLL_API __declspec(dllimport)
#endif
#else
#define FILTERDLL_API __attribute__((visibility("default")))
#endif

#ifdef _WINDOWS
#pragma warning( disable: 4251 )
#endif

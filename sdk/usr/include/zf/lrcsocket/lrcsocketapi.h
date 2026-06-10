#ifndef LRCSOCKET_API_H_
#define LRCSOCKET_API_H_

#ifdef _WINDOWS

#ifdef LRCSOCKET_EXPORTS
#define LRCSOCKET_API __declspec(dllexport)
#else
#define LRCSOCKET_API __declspec(dllimport)
#endif
#define LRCSOCKET_API_L

#else // LINUX

#define LRCSOCKET_API   __attribute__((visibility("default")))
#define LRCSOCKET_API_L __attribute__((visibility("hidden")))

#endif

#ifdef _WINDOWS
#pragma warning(disable: 4251 )// DLL Schnittstelle
#endif

#endif // LRCSOCKET_API_H_

#ifndef ZFNAV_API_h
#define ZFNAV_API_h

#ifdef _WINDOWS

# ifdef ZFNAV_EXPORTS
#  define ZFNAV_API __declspec(dllexport)
# else
#  define ZFNAV_API __declspec(dllimport)
# endif
#elif __GNUC__ >= 4
# define ZFNAV_API __attribute__((visibility ("default")))
#endif

#endif // ZFNAV_API_h


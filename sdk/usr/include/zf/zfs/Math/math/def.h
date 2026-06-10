#pragma once

#ifndef PI
# define PI 3.1415926535897932384626433832795
#endif
#ifndef PI2
# define PI2 (PI/2.)
#endif
#ifndef TWOPI
# define TWOPI (2*PI)
#endif

#ifndef EULER
# define EULER 2.7182818284590452353602874713526
#endif

#ifndef TORAD
# define TORAD(grad)  ( (double)(grad)*(PI/180.0))
#endif

#ifndef TOGRAD
# define TOGRAD(rad)  ( (double)(rad )*(180.0/PI) )
#endif

#if (defined(_MSC_VER) && _MSC_VER >= 1700) || defined(__GNUG__)
  #ifndef _CPP11
    #define _CPP11
  #endif
#endif

#if defined(__LP64__) || defined (WIN64)
    #ifndef _64BIT
        #define _64BIT
    #endif
#endif

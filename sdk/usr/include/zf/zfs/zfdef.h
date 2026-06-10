#pragma once

#include "compiler.h"

/**
\brief usefull defines
*/

#ifndef PI
# define PI 3.1415926535897932384626433832795
//# define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899
#endif




#ifndef TORAD
# define TORAD(grad)  (static_cast<double>(grad)*(PI/180.))
#endif

#ifndef TOGRAD
# define TOGRAD(rad)  (static_cast<double>(rad )*(180./PI) )
#endif


#ifdef UNUSED
#elif defined (__GNUC__)
# define UNUSED(x) x __attribute__((unused))
#elif defined (__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

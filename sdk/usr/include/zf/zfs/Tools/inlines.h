#ifndef INLINES_H
#define INLINES_H

#include <math.h>

#include "../Math/constants.h"

//double inline abs(double d) {if(d<0.0) return (-d);else return d;}

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false;
#endif


//(2pi-Winkel von x,y [-pi/2,+3pi/2])
template <typename T> constexpr
T inline phi(const T& x, const T& y) {
    if (x > 0) return atan(y / x);
    else if (x < 0) return pi<T> + atan(y / x);
    else {//x==0
        if (y < 0) { return 0.5* pi<T>; }
        else { return 1.5* pi<T>; }
    }
}

template <class T> constexpr bool inline IsIn(T x, T lower, T upper) {
    return(x >= lower&&x <= upper);
}

template <class T> constexpr T inline sign(T x) {
    if (x > 0) return (T)1;
    else if (x < 0) return (T)(-1);
    else return (T)0;//x==0
}

//bool IsQuad(int i){int r=0;while (r*r<=i){if (r*r==i){break;return true;}else{r++;} }return false;}






#endif

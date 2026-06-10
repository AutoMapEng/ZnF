#ifndef _ZFS_MATH_H
#define _ZFS_MATH_H

#include "zfsapi.h"

#include "Math/constants.h"

#include <limits>

namespace zfs
{

   /**  
   Returns true if a and b are equal within 10 times of epsilon. 
   This will not work for large numbers a and b, because of precision loss in the difference. 
   Implement a better function in this case.
   */
   template <typename T>
   inline bool fuzzyEqual( T a, T b)
   { 
       return fabs(a-b) < 10*std::numeric_limits<T>::epsilon();
   }
   
   /**
   \brief map angle into interval [0,2 Pi)
   */
   inline double normAngleRad(double x)
   {
       while ( x < 0 ) x += twopi<double>;
       while ( x > twopi<double>) x -= twopi<double>;
   
       return x;
   }

} // end namespace zfs

#endif // _ZFS_MATH_H

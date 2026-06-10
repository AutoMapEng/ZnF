// Name:
//   MathUtils
//
// Synopsis:     miscellaneous math utils
// Description:

// Examples:
// Files:
// Bugs:
// See also:
// Type: C++-HEADER
// Copyright 1998-2000, Cyra Technologies. All rights reserved.
//X////////////////Y/////////////////////////////Z/////////////////////////////

// always undefine min and max...otherwise the SafeMath::max/min gets replaced by macro body
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifndef _MathUtils_hpp_
#define _MathUtils_hpp_

#include <float.h>
#include <cmath>
// #include <math.h>

// PI constants
#ifndef PI
#define PI              3.1415926535897932384626433832795
#endif // !PI
#define PI_OVER_2       1.5707963267948966192313216916398
#define PI_OVER_4       0.78539816339744830961566084581988
#define RADS_PER_DEGREE 0.017453292519943295769236907684886
#define DEGREES_PER_RAD 57.295779513082320876798154814105
#define GONS_PER_RAD 63.661977236758134307553505349006

#define FORTY_DEGREES 0.69813170079773183076947630739545

namespace SafeMath
{
// inline templated utility functions
template <class T>
inline T
abs(const T &x)
{ return (x > 0)? x : -x; }

template <class T>
inline T
max(const T &x1, const T &x2)
{ return (x1 > x2)? x1 : x2; }

template <class T>
inline T
min(const T &x1, const T &x2)
{ return (x1 < x2)? x1 : x2; }

inline double acos(const double &value)
{
  if (value >= 1.0)
    return 0.0;
  else if (value <= -1.0)
    return PI;
  return ::acos(value);
}

inline double asin(const double &value)
{
  if (value >= 1.0)
    return PI_OVER_2;
  else if (value <= -1.0)
    return -PI_OVER_2;
  return ::asin(value);
}

}

template <class T>
inline T
max_abs(const T &x1, const T &x2)
{
  T ax = SafeMath::abs(x1);
  T ay = SafeMath::abs(x2);
  return (ax > ay)? ax : ay;
}

template <class T>
inline T
min_abs(const T &x1, const T &x2)
{
  T ax = SafeMath::abs(x1);
  T ay = SafeMath::abs(x2);
  return (ax < ay)? ax : ay;
}

inline int nearly_equal(const double &a, const double &b, const double &EpsMultiplier = 10.0)
{
  return (a == b || SafeMath::abs(a-b) < EpsMultiplier * DBL_EPSILON * max_abs(a, b)) ? 1 : 0;
}

inline int nearly_equal(const float &a, const float &b, const float &EpsMultiplier = 10.0f)
{
  return (a == b || SafeMath::abs(a-b) < EpsMultiplier * FLT_EPSILON * max_abs(a, b)) ? 1 : 0;
}


inline double degrees_to_rads(const double &degrees)
{
  return degrees * RADS_PER_DEGREE;
}

inline double rads_to_degrees(const double &degrees)
{
  return degrees * DEGREES_PER_RAD;
}

inline int angle_in_range(double angle, double startAngle, double endAngle, double EpsMultiplier = 10.0)
{
  // add room in the range for roundoff error
  double epsilon = EpsMultiplier * DBL_EPSILON;
  startAngle -= epsilon;
  endAngle += epsilon;
  // first ensure start is less than end
  const double two_pi = 6.283185307179586476925286766559; // 2 * PI
  while (startAngle > endAngle)
    startAngle -= two_pi;

  // now if angle is larger than start, drop it down by 2pi until it's less than the start
  while (angle >= startAngle)
  {
    if (angle <= endAngle)
      return 1;
    angle -= two_pi;
  }
  while (angle <= endAngle)
  {
    if (angle >= startAngle)
      return 1;
    angle += two_pi;
  }
  return 0;
}

// converts any value to be between minValue and maxValue incrementing by the given  incrementBy number
// useful for places where for example you have to ensure an angle to be between 0 and 2 * PI..
// thus EnsureValueBetweenLimits(400, 0, 360, 360) would always return 40 and -400  would always return 320
template <class T>
inline T
EnsureValueBetweenLimits(T rValue, const T &minValue, const T &maxValue, const T &incrementBy)
{
  if (rValue < minValue)
  {
    while (rValue < minValue)
      rValue += incrementBy;
  }
  else if (rValue > maxValue)
  {
    while (rValue > maxValue)
      rValue -= incrementBy;
  }
  ASSERT( rValue >= minValue && rValue <= maxValue);
  return rValue;
}

#endif // _MathUtils_hpp_

#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>

namespace zfs {
    namespace math {
        template<class T> T                  ZFS_API fmin(const T& a, const T& b);
        template<class T> T                  ZFS_API fmax(const T& a, const T& b);
        template<class T> T                  ZFS_API pythag(const T& a, const T& b);
        template<class T> T                  ZFS_API sign(const T& a, const T& b);
        template<class T> int                ZFS_API sgn(const T& number);                               // < indicates sign of value (signed types and float types)
        template<class T> int                ZFS_API round(const T& number);                               // < round to nearest integer (float input types), returns int
        template<class T> T                  ZFS_API round(const T& number, const unsigned int precision); // < round to precision
        template<class T> int                ZFS_API ceil(const T& number);                               // < ceil to given number (float input types), returns int
        template<class T> int                ZFS_API floor(const T& number);                               // < floor to given number (float input types), returns int
        template<class T> T                  ZFS_API modulus(const T& X, const T& Y);
        template<class T> T                  ZFS_API remainder(const T& X, const T& Y);
        template<class T> bool               ZFS_API even(const T& number);                                // < check if given integer number is even
        template<class T> T                  ZFS_API wrap_into_PI(T angle);                                        // - wrap into +-PI
        template<class T> T                  ZFS_API wrap_into_2PI(T angle);                                        // - wrap into +-2*PI
        template<class T> int                ZFS_API first_valid_digit(const T& number);                               // - first valid digit (position)/number of digits (based on log10()-calculations)
        template<class T> T                  ZFS_API eps_floating(const T& number);                               // - epsilon for comparing to numbers in floating represenation (round off errors ...), mind number1, number2 might differ, if eps for a single number-> use number1= number2
        template<class T> bool               ZFS_API floating_equal(const T& lhs, const T& rhs);                     // - compare two floating point numbers
        template<class T> T                  ZFS_API ipow(T base, T exp);                                  // - integer power 
        template<class T> uint64_t           ZFS_API factorial(const T& number, bool& bSuccess);               // - faculty of a number n! (works up to number == 22)

        template<class T1, class T2> double  ZFS_API logarithm(const T1& value, const T2& base);                // - logarithm of a value to a given base 

        double                               ZFS_API factorial(const double& number, bool& bSuccess);          // - faculty of a number n! (works up to number == 170)
    } // end namespace math
} // end namespace zfs
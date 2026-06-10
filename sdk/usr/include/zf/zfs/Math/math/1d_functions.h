#pragma once

/*
\file 1DFunctions.h

This file provides basic mathematical functions which only return one numerical value.
e.g. faster approximate solutions for existing cmath functions or not so commonly used functions.
*/

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <math.h>
#include <cmath>
SWITCH_WARNINGS_ON
#include <stdlib.h>
#
#include <zfs/zfsapi.h>

#include <zfs/Math/math/def.h>

namespace zfs {
    namespace math
    {
        inline double atanApprox(double x); ///< Approximate solution of the atan().
        inline double atanApprox(double x, double eps); ///< Approximate solution of the atan() with maximal relative error eps.
        inline double atan2Approx(double y, double x); ///< Approximate solution of the atan2().
        inline float invSqrtApprox(float x);    ///< Cool hack providing and approx implementation of the inverse sqrt.
        inline int roundToInt(double x);  ///< assymetrical round (0.5 values are always rounded up) followed by cast to int
        inline int assymToInt(double x);  ///< for x > 0 this behaves like a normal int cast. For x < 0 it rounds to the lower int ( -3.2 -> -4 ).
        inline double assymRound(double x);   ///< assymetrical round (0.5 values are always rounded up)
        inline double normAngleRad(double x);    ///< takes an angle in rad and maps it back to the interval [0,2Pi)
        inline double normAngleRadSym(double x);   ///< takes an angle in rad and maps it back to the interval [-Pi,Pi)
        inline double normAngleDegreeSym(double x);  ///< Takes an angle in grad and maps it back to the interval [-180 deg, 180 deg)
        inline double bound(double leftBorder, double x, double rightBorder); ///< limits x to the values in the interval [leftborder,rightborder]. If it lies outside it will be set to the nearest border.
        inline int assymIntDiv(int numerator, int denominator); ///< integer division, that rounds to the next smaller integer instead of rounding towards zero. (example: -1/3 = -3, 1/3 = 0 )


        // --------------------------------------------------------------------------------------------
        /**
        Highest error is < 0.005 rad.
        Speedup vs. atan() ~= 6.7

        @param
        @return
        */
        double atanApprox(double x)
        {
            if (x > 1) {
                return (PI2 - x / (x * x + 0.28));
            }
            else if (x >= -1) {
                return (x / (1 + 0.28 * x * x));
            }
            else {
                return (-PI2 - x / (x * x + 0.28));
            }
        }

        // --------------------------------------------------------------------------------------------
        /**
        relative error is < eps
        Speedup = ?

        @param  eps must be positive
        @return
        */
        double atanApprox(double x, double eps)
        {
            // use formula atan(x) = 2 * atan( x / (1 + sqrt(1+x^2) )) to decrease argument
            double fac = 1;
            if (fabs(x) > 0.7) {
                while (fabs(x) > 0.7) {
                    x /= (1 + std::sqrt(1 + x * x));
                    fac = fac * 2;
                }
            }

            double res = x;
            double xpower = x;
            double xsquare = x * x;
            double mysign = 1;

            for (int k = 0; k < 100; k++) {
                xpower *= xsquare;
                mysign *= -1;
                double newterm = mysign * xpower / (2. * k + 1);
                res = res + newterm;

                if (fabs(newterm / res) < eps) {
                    break;
                }
            }
            return fac * res;
        }

        // --------------------------------------------------------------------------------------------
        /**
        Approximate implementation of atan2(y,x) using the wikipedia definition:
                (http://en.wikipedia.org/wiki/Inverse_trigonometric_functions).
                Highest error is < 0.005 rad.
                Speedup = ?
                @param  Parameter follow the definition of the atan2 in the wikipedia.
                @return
        */
        double atan2Approx(double y, double x)
        {
            if (x > 0)
                return atanApprox(y / x);
            else if (x < 0) {
                if (y >= 0)
                    return (PI + atanApprox(y / x));
                else
                    return (-PI + atanApprox(y / x));
            }
            else {
                if (y > 0)
                    return PI2;
                else if (y < 0)
                    return (-PI2);
                else
                    return 0;
            }
        }

        // --------------------------------------------------------------------------------------------
        /**
                Let me just add that this trick is really a hack. It only works on machines that use
                32 bit IEEE 754 floats and the same byte order for ints and floats.
                Also the casting isn�t without problems. At least the C++ standard says it invokes undefined behaviour.
                ATTENTION! This implementation may only works on microsoft windows systems

                Highest relative error is < 0.0007.
                Speedup vs. 1/sqrt() ~= 3.2
        @param
        @return text
        */
#if 0
        float invSqrtApprox(float x)
        {
            uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
            float tmp = *(float*)&i;
            return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
        }
#endif

        // --------------------------------------------------------------------------------------------
        /**
                quote: "Let me just add that this trick is really a hack. It only works on machines that use
                32 bit IEEE 754 floats and the same byte order for ints and floats.
                Also the casting isn�t without problems. At least the C++ standard says it invokes undefined behaviour."
                ATTENTION! This implementation may only work on microsoft windows systems

                Highest relative error is < 0.0007.
                Speedup vs. 1/sqrt() ~= 3.2
        @param
        @return text
        */
#if 0
        float sqrtApprox(float x)
        {
            uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
            float tmp = *(float*)&i;
            return x * tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
        }
#endif


        // --------------------------------------------------------------------------------------------
        /**
        @return rounded integer
        */
        int roundToInt(double x)
        {
            if (x > 0)
            {
                return (int)(x + 0.5);
            }
            else
            {
                return (int)ceil(x - 0.5);
            }
        }

        // --------------------------------------------------------------------------------------------
        /**

        for x > 0 this behaves like a normal int cast. For x < 0 it rounds to the lower int ( -3.2 -> -4 ).
        \return rounded integer
        */
        int assymToInt(double x)
        {
            return (int)floor(x);
        }

        // --------------------------------------------------------------------------------------------
        /**
        @return rounded double
        */
        double assymRound(double x)
        {
            return floor(x + 0.5);
        }

        // --------------------------------------------------------------------------------------------
        /**

        */
        double normAngleRad(double x)
        {
            while (x < 0) x += 2 * PI;
            while (x > 2 * PI) x -= 2 * PI;

            return x;
        }

        // --------------------------------------------------------------------------------------------
        /**

        */
        double normAngleRadSym(double x)
        {
            while (x < -PI) x += 2 * PI;
            while (x > PI) x -= 2 * PI;

            return x;
        }

        // --------------------------------------------------------------------------------------------
        double normAngleDegreeSym(double x)
        {
            return TOGRAD(normAngleRadSym(TORAD(x)));
        }

        // --------------------------------------------------------------------------------------------
        /**

        */
        double bound(double leftBorder, double x, double rightBorder)
        {
            x = (x < leftBorder ? leftBorder : x);
            x = (x > rightBorder ? rightBorder : x);
            return x;
        }

        // --------------------------------------------------------------------------------------------
        /**

                (example: -1/3 = -3, 1/3 = 0, -1/(-3) = 0, 1/(-3) = -3 )
                The function relies on the user to not divide by zero.
        */
        int assymIntDiv(int numerator, int denominator)
        {
            if (numerator * denominator > 0)
                return numerator / denominator;
            else if (numerator * denominator < 0)
                return -1 * ((std::abs(numerator) + denominator) / std::abs(denominator));
            else return 0;
        }
    } // end namespace math
} // end namespace zfs
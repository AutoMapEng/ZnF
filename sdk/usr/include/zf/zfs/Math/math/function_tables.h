#pragma once

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <math.h>
SWITCH_WARNINGS_ON
#include <vector>

#include <zfs/zfsapi.h>

#include "1d_functions.h"

#ifndef PI
#define PI 3.141592653589793
#endif

#ifndef PI2
#define PI2 1.570796326794897
#endif

//namespace zfs { namespace math
//{
/** 
\brief Class for functions that are implemented with lookup tables.
*/
class ZFS_API FunctionTables
{
private:
    
    std::vector<double> tanArray, atanArray;
    std::vector<double> sinArray, cosArray, absSinArray;

    int functionValues;

public:

    enum
    {
        SIN = 0x01, COS = 0x02, TAN = 0x04, ASIN = 0x08, ACOS = 0x10, ABS_SIN = 0x20, ABS_COS = 0x40, ATAN = 0x80
    };


    FunctionTables(int samples);
    FunctionTables(int samples, int tables);
    
    void setupSinTable();
    void setupCosTable();
    void setupTanTable();

    void setupAbsSinTable();
    void setupAtanTable();

    // --------------------------------------------------------------------------------------------
    inline double interpolate(const double& x, const std::vector<double>& FunArray) const
    {
        // map x to index space
        double xIndex = x * ( int64_t(functionValues) - 1 );

        int leftIndex = (int)xIndex;

        double y1 = FunArray[leftIndex];
        
        if( leftIndex < functionValues-1 ) // normal case
        {
            double y2 = FunArray[int64_t(leftIndex)+1];
            return ((y2-y1) * (xIndex-leftIndex) + y1);   // linear interpolation between table-values
        }
        else    // We are on the right x-interval border. No right-hand array neighbour available.
        {
            return y1;
        }
    }
    // --------------------------------------------------------------------------------------------
    /**
    Call setupSinTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double sinTable(const double& x) const
    {
        return interpolate( zfs::math::normAngleRad(x)/ (2*PI) , sinArray);
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupCosTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double cosTable(const double& x) const
    {
        return interpolate( zfs::math::normAngleRad(x)/ (2*PI) , cosArray);
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupTanTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double tanTable(const double& x) const
    {
        double cosValue= interpolate(zfs::math::normAngleRad(x) / (2 * PI), cosArray);
        if (cosValue != 0.)
            return interpolate(zfs::math::normAngleRad(x) / (2 * PI), sinArray)/ cosValue;
        else
            return 0.;
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupAbsSinTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double absSinTable(double x) const
    {
        x = zfs::math::normAngleRadSym( x );

        if(x < 0) x *= -1;

        return interpolate( x/ PI , absSinArray);
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupAbsSinTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double absCosTable(double x) const
    {
        x = zfs::math::normAngleRadSym( x + PI2 );

        if(x < 0) x *= -1;

        return interpolate( zfs::math::normAngleRadSym( x )/ PI , absSinArray);
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupAtanTable once before using this function 1024 samples will yield an absolute error ~= 10^-9
    */
    inline double atanTable(double x) const
    {
        double fac = 1;

        // mirror
        if (x < 0){
            x *= -1;
            fac = -1;
            
        }

        // map to [0,1]
        if (x > 1){
            x /= (1 + sqrt(1+x*x));
            fac *= 2;
        }
        
        return fac* interpolate( x , atanArray);
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupAtanTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double asinTable(const double& x) const
    {
        if(x < -1 || x > 1) return 0;
        else return 2* atanTable(x/(1+sqrt(1-x*x)));
    };

    // --------------------------------------------------------------------------------------------
    /**
    Call setupAtanTable once before using this function, max error for 1024 spls: 5e-6, for 2048 spls: 2e-6, for 4096 spls: 3e-7.
    */
    inline double acosTable(const double& x) const
    {
        return PI2- asinTable(x);
    };
    // --------------------------------------------------------------------------------------------
    /**
    Call setupAtanTable once before using this function 1024 samples will yield an absolute error ~= 10^-9
    */
    inline double atan2Table(const double& y, const double& x) const
    {
        if (x > 0)
            return atanTable(y/x);
        else if (x < 0) {
            if ( y >= 0)
                return (PI + atanTable(y/x));
            else
                return (-PI + atanTable(y/x));
        } else {
            if (y > 0)
                return PI2;
            else if (y < 0)
                return (-PI2);
            else
                return 0;
        }
    };
};

//} // end namespace
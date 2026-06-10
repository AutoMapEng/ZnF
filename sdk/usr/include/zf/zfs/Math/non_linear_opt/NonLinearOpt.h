#pragma once

//#define ENABLE_NONLIN   // libbasics.a liboptimize.a Bei Windows 2000 weden Plugins die ZFMath benutzen nicht mehr geladen


#ifdef ENABLE_NONLIN

#include <zfs/Math/ZFMathAPI.h>

#include <zfs/Math/math/lin_alg_common.h>



class SearchLocalMinPoint
{
public:
    int dim;
    vector_t *mini, *start;

public:
    SearchLocalMinPoint( int dim );
    ~SearchLocalMinPoint();
    void setMin( int idx, double val );
    void setStart( int idx, double val );
    double getMin( int idx );
    typedef double (*error_func)(vector_t *param);
    int search( error_func func, double exactness, int max_iteration_steps, int stepmode=0 );
};


#define VE( v, i)      ((v)->element[(i)])

#endif // ENABLE_NONLIN
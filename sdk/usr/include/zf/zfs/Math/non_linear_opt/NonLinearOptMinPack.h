#ifndef _NONLINEAROPTMINPACK_H_
#define _NONLINEAROPTMINPACK_H_

#include "../ZFMathAPI.h"

#define ENABLE_MINPACK
#ifdef ENABLE_MINPACK

/* Parameters controlling MINPACK's lmdif() optimization routine. */
/* See the file lmdif.f for definitions of each parameter.        */
#define REL_SENSOR_TOLERANCE_ftol    1.0E-5      /* [pix] */
#define REL_PARAM_TOLERANCE_xtol     1.0E-7
#define ORTHO_TOLERANCE_gtol         0.0
#define MAXFEV                       (1000*n)
#define EPSFCN                       1.0E-16     /* Do not set to zero! */
#define MODE                         1           /* variables are scalled internally */
#define FACTOR                       100.0 

#define ERR_ALLOC   -1
#define MAXPARAM    100

typedef long int integer;
typedef double doublereal;

class ZFMATH_API SearchLocalMinPointMinpack
{
private:

    int err;

    // Parameters needed by MINPACK's lmdif() 
    integer     n;              //NPARAMS
    integer     m;              //cd.point_count;
    doublereal  x[MAXPARAM];
    doublereal *fvec;
    doublereal  ftol; // = REL_SENSOR_TOLERANCE_ftol;
    doublereal  xtol; // = REL_PARAM_TOLERANCE_xtol;
    doublereal  gtol; // = ORTHO_TOLERANCE_gtol;
    integer     maxfev; // = MAXFEV;
    doublereal  epsfcn; // = EPSFCN;
    doublereal  diag[MAXPARAM];
    integer     mode; // = MODE;
    doublereal  factor; // = FACTOR;
    integer     nprint; // = 0;
    integer     info;
    integer     nfev;
    doublereal *fjac;
    integer     ldfjac; // = m;
    integer     ipvt[MAXPARAM];
    doublereal  qtf[MAXPARAM];
    doublereal  wa1[MAXPARAM];
    doublereal  wa2[MAXPARAM];
    doublereal  wa3[MAXPARAM];
    doublereal *wa4;
    
public:
    SearchLocalMinPointMinpack( int nbPnts, int nbParams );
    int set_ftol(double val);
    int set_epsfcn(double val);
    double  get_epsfcn();
    int get_maxfev();
    int set_diag( double *diag );
    int set_mode( int mode );
    int set_maxfev(int val);
    ~SearchLocalMinPointMinpack();
    int statusReport(int &info_, int &nfev_, int &err);
    int setMin( int idx, double val );
    int setStart( int idx, double val );
    double getMin( int idx );
    int search( int (*err_func) (integer *m_ptr, integer  *n_ptr, doublereal *params, doublereal *err, integer *iflag), 
        integer mode=1 );
};

#endif // ENABLE_MINPACK
#endif // _NONLINEAROPTMINPACK_H_

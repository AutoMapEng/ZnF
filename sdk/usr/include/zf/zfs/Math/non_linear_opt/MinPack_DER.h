#pragma once

#include <zfs/zfsapi.h>

#define ENABLE_MINPACK_DER
#ifdef ENABLE_MINPACK_DER


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

class ZFS_API SearchLocalMinPointMinpack_DER
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
    integer     njev;

public:
    SearchLocalMinPointMinpack_DER( int nbPnts, int nbParams);
    int set_ftol(double val);
    int set_gtol(double val);
    int set_xtol(double val);
    double getFactor();
    void setFactor(double f);
    int set_epsfcn(double val);
    double  get_epsfcn();
    int get_maxfev();
    int set_diag( double *diag );
    int set_mode( int mode );
    int set_maxfev(int val);
    ~SearchLocalMinPointMinpack_DER();
    int statusReport(int &info_, int &nfev_, int &err);
    int setMin( int idx, double val );
    int setStart( int idx, double val );
    double getMin( int idx );
    int search( int (*fcn) (long *m, long *n, double *x,
        double *fvec, double *fjac, long *ldfjac, long *iflag),
        long mode=1 );
};

#endif // ENABLE_MINPACK
#ifndef mat_h
#define mat_h

#ifndef _STDIO_H
#include <stdio.h>
#endif

// used for function return
#define NR_OK                  0
#define NR_MALLOC_FAIL         1
#define SVD_WRONG_SIZE         2
#define SVD_NO_CONVERGE        3
#define LU_SINGULAR            4
#define CHOL_NON_SYMPOSDEF     5

// general function

#ifdef NT
#define stdmin std::_cpp_min
#define stdmax std::_cpp_max
#else
#define stdmin std::min
#define stdmax std::max
#endif

int cppsign(const double val); // return the sign of val (0 if val == 0)
double cppabs(const double val); // return the absolute value of val (double precision)
double dsign(const double va, const double vb); // transfer the sign from vb to va

// 

double *mtmp(int n);
void mclr(void);
int mrpt(void);
void mrst(int nptr);
double *mset( int n, double *A,double  s); 
double *midn(int m,int  n,double *A);
double *mcpy(int n, const double *A, double *B);
double *mxtr(int ma,int  na,int  mi,int  ni,int  mb,int  nb, const double *A, double *B);
double *mins(int ma,int  na,int  mb,int  nb,int  mi,int  ni, const double *A, double *B);
void mprt(FILE *out,char *fmt,int m,int  n, const double *A);
void mpfrt(FILE *out,char *fmt,int m,int  n, const double *A);
void mpr(int m,int  n, const double *A);
void mpfr(int m,int  n, const double *A);
void mathmat(int m, int  n, const double *A);
double *madd(int n, const double *A, const double *B, double *C);
double *msub(int n, const double *A, const double *B, double *C);
double mtr(int n, const double *A);
double mlen(int n, const double *A);
double *mtp(int m,int  n, const double *A, double *B);
double *mtps(register int n, register double  *A);
double *msmpy(int n, double s, const double *A, double *B);
double *mmpy(int l,int  m,int  n, const double *A, const double *B, double *C);
double *mmpya(int l,int  m,int  n, const double *A, const double *B, double *C);
double *msdiv(int n, double s, const double *A, double *B);
void norm3( double *A);
double *mgran(int n,double  m,double d,double *A);

/* exponential.c */
double angle(const double *r);
double f(double a);
double g(double a);
double *exp_to_mat(const double *r,double *R);
double *mat_to_exp(const double *R, double *r);
double fp(double a);
double gp(double a);
double *exp_to_dmat(const double *r, int c, double *D);
double *r3solve(const double *u1, const double *v1, const double *u2, const double *v2, double *r);
double *p3solve(const double *u1, const double *v1, const double *u2, const double *v2, const double *u3, const double *v3, double *T);

/* factor.c */
double *ud_factor(int n,double *vi,double *vo);
double *ud_unfactor(int n,double *vi,double *vo);

/* mfactor.c */
double *mud_factor(int n, double *vi, double *vo);
double *mud_unfactor(int n, double *vi, double *vo);

/* minv.c */
/* double *minv(int n, const double *A, double *B); */
double **choleskiDecomp(double **inputMatrix, int dimensions, double **decomposition);
void choleskiBackSubst(double **decomposition,int dimensions,double *input,double *result );
double choleskiSqrtDet(double **decomposition,int dimensions );
double *choleskyInv(int dimensions,double *input, double *result);

/* choleski.cc */
int cholDecomp(double *a, int n, double *p);
void cholBackSub(double *a, int n, double *p, double *b, double *x);

/* udinvert.c */
double *ud_invert(int n,int md,double *vi,double *vo);

/* matS.c */
double *mSidn(int n,double *A);
double mStr(int n, double *A);
double *mSxtr(int na, int mi, int ni, int mb, int nb, const double *A, double *B);
double *mSins(int ma, int na, int nb, int mi, int ni, const double *A, double *B);
double *mSmpy(int m, int n, const double *A, const double *B, double *C);
double *mSSmpy(int n, const double *A, const double *B, double *C);
double *mm2S(int n, const double *A, double *B);
double *mS2m(int n, const double *A, double *B);
void mSpr(int n, const double *A);

/* r3solved.c */
int r3solved (const double *u1,const double *v1,const double *u2,const double *v2,double *r);

/* gauss.c */
double gasdev(double m, double d);

/* svdinvert.c */
int svdcmp(double **a, int m, int n, double *w, double **v);
double *svd_invert(int n, double *A, double *B);

/* nrutil.c */
//  modification of error function : previously it called the exit function,
//  we now just print a message and set an error flag ; the error value is
//  returned when possible
//void numericalRecipesError( char *error_text );
int numericalRecipesError( char *error_text, int wrong );
extern int numericalRecipesErrno;

// XOFF 18/12/97
//  functions dvector & free_dvector => defined in nrutil.cc, used in ludcmp.cc
//  quite dangerous concerning memory access
// 
//double *dvector( long nl, long nh );
//void free_dvector( double *v, long nl, long nh );

/* ludcmp.c */
double **mknr( int n, int m, double *a );
int ludcmp( int n, double **a, int *indx, double *d );
void lubksb( int n, double **a, int *indx, double *b );

double *minv(int n, const double *a, double *b);
//void freenr( double **nr ); // used in ludcmp.cc only ?

double *createHomogeneous(double *R, double *T);



// Inline speedups for the released version 
#ifdef RELEASE
    inline int cppsign(const double val)
    {
        return (val < 0 ? -1 : (val > 0 ? 1 : 0));
    }

    inline double cppabs(const double val)
    {
        return (val < 0 ? -val : val);
    }

    inline double dsign(const double va, const double vb)
    {
        return (vb < 0 ? -cppabs(va) : cppabs(va));
    }
#endif

#endif


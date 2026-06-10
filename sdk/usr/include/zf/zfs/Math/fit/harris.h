#pragma once

#include <zfs/zfsapi.h>



#define SATTELFLAECHE   0
#define CORNER          1
//#define EDGE_            2

/*class ZFS_API harris_t
{
public:
    double *d2x, *d2y, *d2z;
    double *d3x, *d3y, *d3z;
    int n;
    double threshold;
    int flag;
    int ID;
    int HESS_ID;
    double *evWhite_a;
    double *evWhite_b;
} ;*/

ZFS_API int calc_min_along_PCs( double *p, double *min_u, double *min_v  );

typedef struct harris_
{
    double *d2x, *d2y, *d2z;
    double *d3x, *d3y, *d3z;
    int n;
    double threshold;
    int flag;
    int ID;
    int HESS_ID;
    double *evWhite_a;
    double *evWhite_b;
} harris_t;

#ifndef BYTE
typedef unsigned char BYTE;
#endif

int  ZFS_API zf_harris     ( BYTE *data, int w, int h, harris_t *ha, int mode );
void ZFS_API zf_harris_init( int n, harris_t *ha, int ID );
void ZFS_API zf_harris_del ( harris_t *ha );
//int ZFS_API zf_harris_Target( double *img, int w, int h, int filt, int n1, int n2, double offset_u, double offset_v,  double M[4][3] );
int ZFS_API barycenter    ( const double * const img, const unsigned int w, const unsigned int h, double * const bc_u, double * const bc_v);
int ZFS_API zf_conv_Target( const double * const img, const unsigned int w, const unsigned int h, const unsigned int offset_u, const unsigned int offset_v, double * const max_u, double * const max_v);
int ZFS_API recLocally    ( const double * const img, const unsigned int w, const unsigned int h, const unsigned int u, const unsigned int v, const unsigned int n, double * const p );
int ZFS_API medfilt       ( const double * const img, const unsigned int w, const unsigned int h, const unsigned int N, double* const filtImg );
int ZFS_API smooth_data   ( const double * const img, const unsigned int w, const unsigned int h, double * const retImg );

/*class ZFS_API harris : public harris_t
{
    int w;
    int h;

public:
    harris( int w, int h, int ID );
    virtual ~harris();

    int calc(  BYTE *data, int mode );
};
*/

inline double poly_deg_2( double *p, double x, double y )
{
    return (p[0] + p[1]*x + p[2]*y + p[3]*x*x + p[4]*y*y + p[5]*x*y);
}
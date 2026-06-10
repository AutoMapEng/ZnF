#pragma once

#include <zfs/zfsapi.h>

ZFS_API int eigen( double a [4][4], double r [4][4], int MatSize, int need_eigenvectors);
ZFS_API int calcSVD(int m, int n, double *a, double *w, double *v, double *rv1);

ZFS_API void dfpmin(float p[], int n, float gtol, int *iter, float *fret,float(*func)(float []), void (*dfunc)(float [], float []));

#pragma once

#include <zfs/zfsapi.h>



void ZFS_API twoNiveauSegm_simple( int *img, int w, int h, int offset_x, int offset_y,
                          double *bx, double *by, double *bz, int *m, 
                          double *wx, double *wy, double *wz, int *n );

void ZFS_API twoNiveauSegm_simple( int *x, int *y, int *z, int N, 
                          double *bx, double *by, double *bz, int *m, 
                          double *wx, double *wy, double *wz, int *n );


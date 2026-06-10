#pragma once

#include <zfs/zfsapi.h>

#include "lin_alg_common.h"

// -----------------------------------------------------------------------------
//          calc lin Alg basis functions
// -----------------------------------------------------------------------------

ZFS_API void matadd(const matrix_t *a, const matrix_t *b, matrix_t *res);
ZFS_API void scalar_mult(const matrix_t *m, double s, matrix_t *res);
ZFS_API void transpose(const matrix_t *m, matrix_t *mtr);
ZFS_API void multiply(const matrix_t *m1, matrix_t *m2, matrix_t *res);
ZFS_API double triangle_matrix_det(const matrix_t *A);
ZFS_API double matrix_det(const matrix_t *A);
ZFS_API void vector_multiply(const matrix_t *m1, const vector_t *v, vector_t *res);
ZFS_API void matrix_inverse(const matrix_t *A, matrix_t *iA);
ZFS_API void I_matrix(matrix_t *i_matrix, int n);


// ev staff
ZFS_API int zf_calc_Eigenvals( double a [4][4], double r [4][4], int MatSize, int ev_flag );

#define RANGE    1.0e-12
#define mat_type dmat

#define lb1 0
#define lb2 0

#pragma once

#include <zfs/zfsapi.h>

#include "lin_alg_common.h"



#define POLY_X_DEG_0_ID         2
#define POLY_XY_DEG_1_ID        3
#define POLY_XY_DEG_2_ID        6
#define POLY_XY_DEG_3_ID        10
#define AFFIN_LINEAR_ID         4
#define ROT_TRANS_ID            8

#define APPROX      0
#define INTERPOL    1

#ifndef OK
#define OK 0
#endif
#define NOT_ENOUGH_MEM 1

#define POLY_XY_DEG_2_8N_ALL        90
#define POLY_XY_DEG_2_8N_RED        91
#define POLY_XY_DEG_2_8N_GREEN      92
#define POLY_XY_DEG_2_8N_BLUE       93
#define HESS_POLY_XY_DEG_2_8N       94

#define POLY_XY_DEG_2_24N_ALL       250
#define POLY_XY_DEG_2_24N_RED       251
#define POLY_XY_DEG_2_24N_GREEN     252
#define POLY_XY_DEG_2_24N_BLUE      253
#define HESS_POLY_XY_DEG_2_24N      254

#define POLY_XY_DEG_2_4N_ALL        40
#define POLY_XY_DEG_2_4N_RED        41
#define POLY_XY_DEG_2_4N_GREEN      42
#define POLY_XY_DEG_2_4N_BLUE       43

#define ROUND_1N_ALL                10
#define ROUND_1N_RED                11
#define ROUND_1N_GREEN              12
#define ROUND_1N_BLUE               13

#define MAX_MEM                     2000000     //1200*1200


#ifndef BYTE
typedef unsigned char BYTE;
#endif

/* ---------------------------------------------------- */
/* -- householder Algorithm (Compare Deuflhard p'83) -- */
/* -----------------------------------------------------*/

void sub_fct_Q(matrix_t *m, matrix_t *res);
void ZFS_API lsq_householder(matrix_t *A, vector_t *b);
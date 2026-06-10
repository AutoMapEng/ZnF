#ifndef _NONLINEAROPT_LINEAR_H_
#define _NONLINEAROPT_LINEAR_H_


/*
----------------------------------------------------------------------
N: linear.h
----------------------------------------------------------------------
C: Author : Stefan Kusterer, Walter Koller, FORWISS
C: Revised: Juergen Haas, Uni Passau
C: Date   : 15.09.1994
C: Project: EEYE
C: Version: 1.0
C:
D: Basic routines for vector and matrix operations
----------------------------------------------------------------------
*/

/*----------------------------------------------------------------------
  ---------------------    CONSTANTS AND MACROS      -------------------*/

/*** <i>-th element of vector <v> ***/
#define VE( v, i)      ((v)->element[(i)])

/*** element <mrow>,<mcol> of matrix <a> ***/
#define ME( a, mrow, mcol)   ((a)->mdata[(mrow) * (a)->coldim + (mcol)])


/*** prepare array for VE_FAST ***/
#define VE_FAST_PREP(v, v_arr) (v_arr) = ((v)->element)

/* <i>-th element of vector <v>
 * fast access, <v_arr> must point to start of vector data
 */
#define VE_FAST(v_arr, i)      ((v_arr)[(i)])


/*** prepare array for ME_FAST ***/
#define ME_FAST_PREP(m, m_arr) (m_arr) = ((m)->mdata)

/*** prepares fast relative access to matrix <m> offset for (0, 0) ***/
#define ME_FAST_OFFSET_PREP(m, offset, arr) ((arr) = (m)->mdata + (offset))

/* element <mrow>,<mcol> of matrix <a>
 * fast access, <a_arr> must point to start of matrix data
 */
#define ME_FAST(a, a_arr, mrow, mcol)   ((a_arr)[(mrow) * (a)->coldim + (mcol)])


/*** number of elements in a vector ***/
#define VECTOR_ELEMENTS(a) ((a)->dim)

/*** pointer to data array of vector ***/
#define VECTOR_ARRAY(a)    ((a)->element)


/*** number of matrix rows ***/
#define MATRIX_ROWS(a)     ((a)->rowdim)

/*** number of matrix columns ***/
#define MATRIX_COLS(a)     ((a)->coldim)

/*** number of matrix elements, rows * columns ***/
#define MATRIX_ELEMENTS(a) ((a)->coldim * (a)->rowdim)

/*** pointer to row sorted matrix data array ***/
#define MATRIX_ARRAY(a)    ((a)->mdata)


/*** check matrix size (ROWS, COLUMNS) ***/
#define MATRIX_CHECK(a, n, m) \
  (MATRIX_ROWS(a) == (n) && MATRIX_COLS(a) == (m))


/* Dynamic local vector allocation without calloc
 * ---
 * Example:
 * {
 *   DECLARE_VECTOR(3, v, v_arr);
 *   INIT_VECTOR(3, v, v_arr);
 *   VE(&v, 0) = VE(&v, 1) = VE(&v, 2) = 1.0;
 *   normalize_vector(&v);
 * }
 */

#define DECLARE_VECTOR(vn, vec, vec_arr) \
  vector_t vec; double vec_arr[(vn)]
/*** belongs to DECLARE_VECTOR ***/
#define INIT_VECTOR(vn, vec, vec_arr) \
  (vec).dim = (vn); (vec).element = (vec_arr)


/* Dynamic local matrix allocation without calloc
 * ---
 * Example:
 * {
 *   DECLARE_MATRIX(3, 3, m, m_arr);
 *   INIT_MATRIX(3, 3, m, m_arr);
 *   zero_matrix(&m);
 *   ME(&m, 0, 0) = ME(&m, 1, 1) = ME(&m, 2, 2) = 1.0;
 *   ...
 * }
 */

#define DECLARE_MATRIX(mrows, mcols, mat, mat_arr) \
  matrix_t mat; double mat_arr[(mcols)*(mrows)]
/*** belongs to DECLARE_MATRIX ***/
#define INIT_MATRIX(mrows, mcols, mat, mat_arr) \
  (mat).coldim = (mcols); (mat).rowdim = (mrows); (mat).mdata = (mat_arr)

/*----------------------------------------------------------------------
  ---------------------      TYPE DEFINITIONS        -------------------*/

#include "../../Math/lin_alg_common.h"

#if 0
typedef struct vector_ {
  int    dim;         /* number of matrix elements (private, use macros) */
  double *element;        /* pointer to data array (private, use macros) */
} vector_t;

typedef struct matrix_ {
  int    rowdim;          /* matrix rows (private, use macros) */
  int    coldim;          /* matrix columns (private, use macros) */
  double *mdata;        /* pointer to data array (private, use macros) */
} matrix_t;
#endif

/*----------------------------------------------------------------------
  ---------------------      VISIBLE FUNCTIONS       -------------------*/

int
alloc_vector(int size, vector_t **vec_ptr);

void
free_vector(vector_t **vec_ptr);

int
init_vector(int size, vector_t *vec_ptr);

void
destroy_vector(vector_t *vec_ptr);

void
add_vector(vector_t *vec1, vector_t *vec2, vector_t *res);

void
zero_vector(vector_t *vec);

void
sub_vector(vector_t *vec1, vector_t *vec2, vector_t *res);

void
put_vector(vector_t *vec, vector_t *res);

void
clone_vector(vector_t *vec, vector_t *res);

double
norm2_vector(vector_t *vec);

double
normalize_vector(vector_t *vec, vector_t *res);

double
scp_vector(vector_t *vec1, vector_t *vec2);

void
smult_vector(double factor, vector_t *vec, vector_t *res);

void
negate_vector(vector_t *vec, vector_t *res);

extern "C"
{
    int alloc_matrix(int rows, int cols, matrix_t **mat_ptr);
}

void
free_matrix(matrix_t **mat_ptr);

extern "C"
{
    int init_matrix(int rows, int cols, matrix_t *mat_ptr);
}

void
destroy_matrix(matrix_t *mat_ptr);

void
matrix_line(matrix_t *mat, int line, vector_t *vec);

void
mult_matrix_vector_save(matrix_t *mat, vector_t *vec, vector_t *res_vec);

void
mult_matrix_vector_fast(matrix_t *mat, vector_t *vec, vector_t *res_vec);

void
mult_matrix_transposed_vector_save(
  matrix_t *mat,
  vector_t *vec,
  vector_t *res_vec
);

void
mult_matrix_transposed_vector_fast(
  matrix_t *mat,
  vector_t *vec,
  vector_t *res_vec
);

void
transpose_matrix_save(matrix_t *mat, matrix_t *res);

void
transpose_matrix_fast(matrix_t *mat, matrix_t *res);

void
zero_matrix(matrix_t *res);

void
add_matrix_matrix(matrix_t *mat1, matrix_t *mat2, matrix_t *res);

void
sub_matrix_matrix(matrix_t *mat1, matrix_t *mat2, matrix_t *res);

void
mult_matrix_matrix_save(matrix_t *mat1, matrix_t *mat2, matrix_t *res);

void
mult_matrix_matrix_fast(matrix_t *mat1, matrix_t *mat2, matrix_t *res);

void
mult_matrix_transposed_matrix_save(
  matrix_t * mat1,
  matrix_t * mat2,
  matrix_t * res
);

void
mult_matrix_transposed_matrix_fast(
  matrix_t * mat1,
  matrix_t * mat2,
  matrix_t * res
);

void
put_matrix(matrix_t *mat, matrix_t *res);

void
clone_matrix(matrix_t *mat, matrix_t *res);

void
put_matrix_to_vector(matrix_t *mat, int row, vector_t *res_vec);

void
put_vector_to_matrix(vector_t *vec, int row, matrix_t *res);

void
diag_matrix(matrix_t *res, double diag_entry);

double
max_norm_vector(vector_t *vec);

void
smult_matrix(double factor, matrix_t *mat, matrix_t *res);

void
sadd_matrix(double value, matrix_t *mat, matrix_t *res);

double
matrix_2x2_fast_inverse(matrix_t *a, matrix_t *a_inv);

double
matrix_3x3_fast_inverse(matrix_t *a, matrix_t *a_inv);

#endif // _NONLINEAROPT_LINEAR_H_

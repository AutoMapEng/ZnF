/*
----------------------------------------------------------------------
N: bfgs.h
----------------------------------------------------------------------
C: Author : Eder Bernhard, Universitaet Passau
C: Date   : 12.07.1996
C: Revised: Robert Wagner, Universitaet Passau
C: Date   : 17.07.1996
C: Project: EEYE, ANGIO
C: Version: 0.2
C:
D: Definitions for solving general non-linear optimization problems 
----------------------------------------------------------------------
*/


#include "linear.h"



/*----------------------------------------------------------------------
  ---------------------    CONSTANTS AND MACROS      -------------------*/

/*** block: available algorithms to determine valid step length ***/
#define GOLDSTEIN_ARMIJO  0
#define POWELL_WOLFE      1

/*** block: available algorithms for conjugate gradient minimization ***/
#define FLETCHER_REEVES       0
#define POLAK_RIBIERE_POLYAK  1
#define LIPPOLD               2

/*----------------------------------------------------------------------
  ---------------------      TYPE DEFINITIONS        -------------------*/

/*----------------------------------------------------------------------
  ---------------------      VISIBLE FUNCTIONS       -------------------*/

#if 1
double goldstein_armijo_step_length(
  vector_t *x,
  double (*error_func)(vector_t *param),
  double func_val_x,
  vector_t *grad_x,
  vector_t *dir,
  vector_t *x1,
  double *func_val_x1
);

double powell_wolfe_step_length(
  vector_t *x,
  double (*error_func)(vector_t *param),
  double func_val_x,
  void (*gradient_func)(vector_t *param, vector_t *gradient),
  vector_t *grad_x,
  vector_t *dir,
  vector_t *x1,
  double *func_val_x1,
  vector_t *grad_x1
);
#endif

int search_for_local_min_point_bfgs(
  vector_t *min_point,
  double (*error_func)(vector_t *param),
  void (*gradient_func)(vector_t *param, vector_t *gradient),
  vector_t *start,
  double exactness,
  int max_iteration_steps,
  int stepmode
);

#if 0
int search_for_local_min_point_steep_descent(
  vector_t *min_point,
  double (*error_func)(vector_t *param),
  void (*gradient_func)(vector_t *param, vector_t *gradient),
  vector_t *start,
  double exactness,
  int max_iteration_steps,
  int stepmode
);

int search_for_local_min_point_conj_grad(
  vector_t *min_point,
  double (*error_func)(vector_t *param),
  void (*gradient_func)(vector_t *param, vector_t *gradient),
  vector_t *start,
  double exactness,
  int max_iteration_steps,
  int min_mode,
  int stepmode
);

int search_for_local_min_point_gauss_newton(
  vector_t *min_point,
  void (*error_vals)(vector_t *param, vector_t *err),
  void (*jacobian_func)(vector_t *param, matrix_t *jacobian),
  vector_t *start,
  double exactness,
  int max_iteration_steps,
  int dim_error_vals,
  int stepmode
);

int search_for_local_min_point_gauss_newton_extra(
  vector_t *min_point,
  double (*error_func)(vector_t *param),
  void (*gradient_func)(vector_t *param, vector_t *gradient),
  void (*error_vals)(vector_t *param, vector_t *err),
  void (*jacobian_func)(vector_t *param, matrix_t *jacobian),
  vector_t *start,
  double exactness,
  int max_iteration_steps,
  int dim_error_vals,
  int stepmode
);

void compute_numeric_gradients(
  vector_t *param,
  double (*error_func)(vector_t *param),
  vector_t *gradient
);

void compute_numeric_jacobian(
  vector_t *param,
  void (*error_vals)(vector_t *param, vector_t *err),
  matrix_t *jacobian_x
);
#endif



/*
----------------------------------------------------------------------
N: standard.h
----------------------------------------------------------------------
C: Author : Juergen Haas, Uni Passau
C: Date   : 15.09.1994
C: Project: EEYE
C: Version: 1.0
C:
D: Include file for all .c files
C:
C: Must be included in all other .c files, before any other project
C: .h file and after any system include file.
----------------------------------------------------------------------
*/

/*----------------------------------------------------------------------
  --------------------     CONSTANTS AND MACROS     --------------------*/


#define DEBUG_FLAG      /* flag to activate debugging routines */


#ifndef TRUE
#  define TRUE 1        /* <boole_t> value, if not defined somewhere else */
#endif

#ifndef FALSE
#  define FALSE 0       /* <boole_t> value, if not defined somewhere else */
#endif

#ifdef __cplusplus
#  undef NULL
#  define NULL 0
#else
#  ifndef NULL
#    define NULL ((void *) 0)
#  endif
#endif

#ifndef M_PI
/*** constant pi ***/
#  define M_PI 3.14159265358979323846
#endif

/*** block: some squareroots ***/
#define SQRT2 1.41421356237309504880
#define SQRT3 1.73205080756887729352
#define SQRT5 2.23606797749978969640
#define SQRT6 2.44948974278317809819
#define SQRT7 2.64575131106459059050
#define SQRT8 2.82842712474619009760
#define SQRT2DIV2 .70710678118654752440
#define SQRT3DIV2 .86602540378443864676

/*** routine return: all went well ***/
#define STATUS_OK 0
/*** routine return: an error occured ***/
#define STATUS_ERROR 1
/*** routine return: missing memory ***/
#define STATUS_MEMERROR 2

#ifdef DEBUG_FLAG
#  define WRITE(s) printf s;
#else
#  define WRITE(s)
#endif

/*** round double value to integer, always away from zero ***/
#define ROUND(x) ((int) ((x) >= 0.0 ? (x) + 0.5 : (x) - 0.5))

/*** fast round for positive numbers only ***/
#define POS_ROUND(x) ((int) ((x) + 0.5))

#ifndef MAX
/*** maximum of two numbers ***/
#  define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifndef MIN
/*** minimum of two numbers ***/
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
/*** absolute value of a number ***/
#  define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef SQR
/*** square of a number ***/
#  define SQR(x) ((x) * (x))
#endif

/*** eps for uncritical thresholds ***/
#define UEPS 0.00001

/*** uncritical eps for float ***/
#define FEPS 1.19209289551e-07

/*** uncritical eps for double ***/
#define DEPS 2.22044604925e-16

/*** number of grayvalues in <grayvalue_t> ***/
#define GRAYVALUES 256
/*** maximum useable grayvalue ***/
#define MAXGRAY (GRAYVALUES-1)

/*** block: memory allocation and deallocation ***/
#define ALLOC_MEM(dest, size) (*(dest) = malloc(size))
#define CLEAR_MEM(dest, size) (memset((dest), 0, (size)))
#define FREE_MEM(dest) \
  { \
    free(dest); \
    (dest) = NULL; \
  }
#define CHK_ALLOC_MEM(dest, size) \
  { \
    *(dest) = malloc(size); \
    if(*(dest) == NULL) \
    { \
      EXIT_MSG(ERR_MEM, NULL); \
    } \
  }

/*** system compatibility ***/
#ifdef SUN
# ifndef size_t
typedef unsigned long size_t; /* bad joke ... */
# endif
# ifndef RAND_MAX
#  define RAND_MAX ((1<<16)-1)  /* should be somewhere in system include */
# endif
# ifndef SEEK_SET
#  define SEEK_SET 0    /* should be somewhere in system include */
#  define SEEK_CUR 1    /* should be somewhere in system include */
#  define SEEK_END 2    /* should be somewhere in system include */
# endif
#endif

/*----------------------------------------------------------------------
  --------------------       TYPE DEFINITIONS       --------------------*/

typedef int boole_t;

typedef unsigned char grayvalue_t;

typedef struct rgb_ {
  grayvalue_t red;      /* red   channel */
  grayvalue_t green;    /* green channel */
  grayvalue_t blue;     /* blue  channel */
} rgb_t;

typedef struct bgr_ {
  grayvalue_t blue;     /* blue  channel */
  grayvalue_t green;    /* green channel */
  grayvalue_t red;      /* red   channel */
} bgr_t;

typedef void    (*void_func_t)(void *);
typedef int     (*int_func_t)(void *);
typedef boole_t (*boole_func_t)(void *);

typedef void    (*void_func_2_t)(void *, void *);
typedef int     (*int_func_2_t)(void *, void *);
typedef boole_t (*boole_func_2_t)(void *, void *);

typedef int (*alloc_func_t)(void **);
typedef void (*free_func_t)(void **);

/*----------------------------------------------------------------------
  --------------------       VISIBLE FUNCTIONS      --------------------*/

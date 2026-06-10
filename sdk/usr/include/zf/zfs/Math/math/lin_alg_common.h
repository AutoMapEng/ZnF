#pragma once

#pragma once

#include <vector>
#include <zfs/zfsapi.h>

// ---------------------------------------------------------------------------
//          define typedefs ans structs
// ---------------------------------------------------------------------------

typedef struct vector_
{
    vector_()
        : dim(0)
    {}

    vector_(int dim)
    {
        init(dim);
    }

    void init(int dim);

    int dim;
    std::vector<double> element;
} vector_t;

typedef struct matrix_
{
public:
    matrix_()
        : rowdim(0)
        , coldim(0)
    { }

    matrix_(int rowdim, int coldim)
    {
        init(rowdim, coldim);
    }

    void init(int rowdim, int coldim);

    int rowdim;
    int coldim;
    std::vector<double> element;
} matrix_t;




// -----------------------------------------------------------------------------
//          define basis functions for vector/ matrix handling
// -----------------------------------------------------------------------------

// matrix
void ZFS_API init_matrix(matrix_t *m, int rowdim, int coldim);
double ZFS_API get_matrix_element(const matrix_t *m, int row, int col);
void ZFS_API matrix_insert_element(matrix_t *m, double element, int row, int col);
void ZFS_API print_matrix(const matrix_t *m);
void ZFS_API clear_matrix(matrix_t *m);

// vector
void ZFS_API print_vector(const vector_t *v);
void ZFS_API init_vector(vector_t *v, int dim);
double ZFS_API get_vector_element(const vector_t *v, int nb);
void ZFS_API vector_insert_element(vector_t *v, double element, int nb);

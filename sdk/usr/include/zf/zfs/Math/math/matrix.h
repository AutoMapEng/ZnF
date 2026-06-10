#pragma once

#include <zfs/zfsapi.h>

#include <vector>

#include "../zfvec.h"

namespace zfs 
{
    namespace math
    {

        // =================================================================

        template<class T> class ZFS_API matrix
        {
        public:

            using vec = std::vector<T>;

            matrix();
            matrix(const int& n);
            matrix(const int& m, const int& n);

            void set(const int& n, const T& val = 0);
            void set(const int& m, const int& n, const T& val = 0);

            matrix& operator=(const T& value);

            matrix operator*(const matrix& rhs) const;

            Vec3<T> operator*(const Vec3<T>& vector) const;
            vec operator*(const vec& v) const;
            vec operator*(const T* v) const;

            T mat_mul_component(const T* v, int row_idx) const;

            vec col_vec(const int& j) const;
            void col_vec(const int& j, const vec& col_vec);
            void col_vec(const int& j, const T* col_vec);

            const vec& row_vec(const int& i) const;
            vec& row_vec(const int& i);
            void row_vec(int i, const T* row_vec);

            void pivote_row(const int& i1, const int& i2); // pivote/swap row function

            T operator()(const int& i, const int& j) const;
            T& operator()(const int& i, const int& j);

            int rows() const;
            int cols() const;

            void set_unity();
            bool issquare() const;
            bool issymmetric() const;

            matrix transp() const;
            matrix inv(bool& valid) const;

        protected:

            std::vector<vec> m_data;

        };

        // =================================================================

        // Eigenvalues from Jacobi-transformations ( rotations ) :
        // >Computes all eigenvalues and eigenvectors of a real symmetric matrix a[1..n][1..n].On
        // output, elements of a above the diagonal are destroyed.d[1..n] returns the eigenvalues of a.
        // v[1..n][1..n] is a matrix whose columns contain, on output, the normalized eigenvectors of
        // a.nrot returns the number of Jacobi rotations that were required.<
        // "The Jacobi method is absolutely foolproof for all real symmetric matrices. For
        // matrices of order greater than about 10, say, the algorithm is slower, by a significant
        // constant factor, than the QR method we shall give in §11.3.However, the Jacobi
        // algorithm is much simpler than the more efficient methods.We thus recommend it
        // for matrices of moderate order, where expense is not a major consideration." (Num. Rec.)
        // a     : input : real symmetric input matrix for which eigenvectors shall be obtained
        // max_it: input : maximum number of iterations to use before routine returns false (= indicating no convergence)
        // cs    : check symmetry of a, routine returns false, if matrix is asymmetric
        // d     : output: contains the eigen values of a
        // v     : output: contains the eigen vectors of a
        // nrot  : output: number of Jacobi rotations that were required
        // if true is returned the algorithm has obtained a solution within the given maximum number of iterations
        template<class T> bool ZFS_API jacobi(matrix<T> a, int max_it, bool cs, std::vector<T>& d, matrix<T>& v, int& nrot);

        // =================================================================

        // Gauss-Jordan Algorithm
        // a : input/output : should be square ( same number of unknowns as equations ), e.g. m x m
        // b : input/output : can be m x n, n "query" vectors, on output the solution to the equation system is returned in b
        // On success or error (singular matrix a: determinand det(a)= 0) of the inversion, true or false, respectively, is returned.

        template<class T> bool ZFS_API gaussj(matrix<T>& a, matrix<T>& b);

        // =================================================================

        // LU decomposition [1] as used for equation solving of a system a.x=b
        // a      [0..n-1][0..n-1]: input/output : square(!) matrix, on output contains the LU decomposition of a
        // pp_indx[0..n-1]        : output       : row permutation effected by the partial pivoting

        template<class T> bool ZFS_API ludcmp(matrix<T>& a, std::vector<int>& pp_indx);

        // Solves the set of n linear equations A·X = B
        // a[0..n-1][0..n-1]: input        : square(!) matrix: Here a[1..n][1..n] is input, not as the matrix a (of the equation system)  
        //                                  but rather as its LU decomposition, determined by the routine ludcmp
        // pp_indx[0..n-1]  : input        : row permutation effected by the partial pivoting, determined by the routine ludcmp
        // b[0..n-1]        : input/output : input as the right-hand side vector b, and returns with the solution vector x
        //
        // note: a and indx are not modified by this routine and can be left in place for successive calls with different right-hand sides b.

        template<class T> bool ZFS_API lubksb(const matrix<T>& a, const std::vector<int>& indx, std::vector<T>& b);

        // Calculation of the matrix inverse using the above 2 routines (alternative to function gaussj(.), has about the same operations count as gaussj(.)) [< numerical recipes in C 2nd ed.]
        // a    [0..n-1][0..n-1]: input        : square(!) matrix to be inversed (calculations are inplace)
        // a_inv[0..n-1][0..n-1]: output       : inverse matrix to a

        // This routine is mainly for verification of the LU decomposition method. 
        // Compared to gaussj() it has the same operations count.

        template<class T> bool ZFS_API matrixInverseLU(matrix<T> a, matrix<T>& pp_indx);

        // =================================================================
    } // end namespace math
} // end namespace zfs
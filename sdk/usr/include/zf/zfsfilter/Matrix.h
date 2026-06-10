#pragma once

#include "filterapi.h"

#include <cstdint>
#include <exception>
#include <iosfwd>
#include <list>
#include <set>
#include <string>
#include <vector>

#include <zfs/Math/zfvec.h>
#include <zfs/Math/auxilliary/exception.h>
#include <zfs/Math/math/tuple.h>
#include <zfs/Math/2d/boundingbox.h>

// forward declarations

namespace zfs
{
    namespace math
    {
        template<class T, unsigned int N> class VectorND;

        template<class T> class Point;
        template<class T> class Range;
    }
}

namespace zfsfilter
{
    class RollingBuffer;
    class BoundingBox;
}


namespace zfsfilter
{
    //================================================================================================================
    /**
     * @class MatrixException
     * @brief  Class for exceptions for matrix access, i.e. wrong index.
     *
     * @author Juergen Holzner
     *
     * Class for exceptions for matrix access, i.e. wrong index.
     *
     */
    class MatrixException:public std::exception
    {
    public:
        std::string m_ExceptionString;
        MatrixException( const std::string& a_ExceptionString ): m_ExceptionString( a_ExceptionString )
        {}
        ~MatrixException() NOEXCEPT
        {}
        const char* what() const NOEXCEPT
        {
            return m_ExceptionString.c_str();
        }
    };
    //================================================================================================================
    /**
     * @class MatrixBase
     * @brief  Base class for Matrix class. Provides memory management, access and common functionality.
     *
     * @author Juergen Holzner
     *
     * Base class for Matrix class. Provides memory management, access and common functionality.
     *
     */

    template< class T> class Matrix; // forward decl
    template<> class Matrix<bool>; // forward decl

    template <class T> class FILTERDLL_API MatrixBase
    {
        template<class S> friend class MatrixBase;
        template<class S> friend class Matrix;
        friend class RollingBuffer;
    protected:
        // Attributes of the class
        bool b_delete;  ///< indicates if storage has to be deleted
        int  N;         ///< number of columns
        int  M;         ///< number of rows
        T    *A;        ///< pointer to data

    public:
        // -- constructors -------------------------------------------------
        MatrixBase();
        MatrixBase( size_t _M, size_t _N );
        MatrixBase( size_t _M, size_t _N, const T& preset_value );
        // init matrix from vector
        MatrixBase( size_t _M, size_t _N, const std::vector<T>& matrix_vector, const bool& bRowsFastIndex = true );
        MatrixBase( size_t _M, size_t _N, const T* const matrix_vector, const bool& bRowsFastIndex = true );
        MatrixBase( const wchar_t* filename );
        MatrixBase( const MatrixBase<T> &src ); // copy constructor
        MatrixBase( const MatrixBase<T> &src, const zfs::math::Range<int>& LineRange ); // Line-Section Matrix of Source (for use src-Matrix must exist over life-time of section matrix!!!)
        template<class T1> MatrixBase( const MatrixBase<T1>& src );  // conversion constructor
        // -- destructor -------------------------------------------------
        virtual ~MatrixBase();
        // -- helper -----------------------------------------------------
        void Create( size_t _M, size_t _N );
        void Create( size_t _M, size_t _N, const T& value );
        void Clear();

        void NullAllElements();
        void SetAllElementsToValue( T value );
        void SetToValue( const Matrix<bool>& aPositions, const T& value );
        void SetToValue( const Matrix<bool>& aPositions,  const MatrixBase<T>& aSrc); // take values from matrix aSrc where matrix aPositions == true
        void SetToValue( const zfs::math::IntPointListType& aPointList, const T& value );
        void SetToValue( const zfs::math::IntPointListType* const apPointList, const T& value );
        template<class T1> bool SameSize( const MatrixBase<T1>& aMatrix ) const
        {
            return Lines() == aMatrix.Lines() && Columns() == aMatrix.Columns();
        }

        int  NoOfMatrixElements() const;
        // -- sizes in height and width ----------------------------------
        int Lines() const
        {
            return M;
        }
        int Columns() const
        {
            return N;
        }
        // --------------------------------------------
        int getWidth() const
        {
            return Columns();
        }
        int getHeight() const
        {
            return Lines();
        }
        // -- access operators/functions ----------------------------------
        T &operator()( int m, int n );
        const T operator()( int m, int n ) const;

        T &operator()( const zfs::math::Point<int>& Position );
        const T operator()( const zfs::math::Point<int>& Position ) const;

        T * const operator[]( int m );
        const T * const operator[]( int m ) const;

        T &operator[]( const zfs::math::Point<int>& Position );
        const T operator[]( const zfs::math::Point<int>& Position ) const;

        const bool SetVal( int m, int n, T value );
        const T    GetVal( int m, int n ) const;

        const bool setVal( int n, int m, T value ); // reverse addressing (means, first: pixel, second: line), for compatibility to layer, linebyline, zfscan
        const T    getVal( int n, int m ) const;    // reverse addressing (means, first: pixel, second: line), for compatibility to layer, linebyline, zfscan

        // -- assignment operators ---------------------------------------
        MatrixBase &operator=( const MatrixBase<T>& src );
        // -- comparison operators ---------------------------------------
        const bool operator==( const MatrixBase<T> &rhs ) const;
        const bool operator!=( const MatrixBase<T> &rhs ) const;
        // -- type conversion --------------------------------------------
        template<class T1> operator MatrixBase<T1>() const;
        // -- get submatrix -----------------------------------------------
        const MatrixBase GetSubMatrix( const int StartLine, const int EndLine, const int StartColumn, const int EndColumn ) const;
        const MatrixBase GetSubMatrix( const zfs::math::BoundingBox& aBoundingBox ) const;
        // -- subsampling -------------------------------------------------
        const MatrixBase GetSubsampled(const int SubLine, const int SubColumn, const int SubLineOffset= 0, const int SubColumnOffset= 0) const;
        // Bounding Box: zfs::math::IntPointListType: Points: [UpperLeft, LowerRight], X is Row, Y is Column!, Upper has lower X coordinate, Left has lower Y coordinate.
        void       SetSubMatrix( const MatrixBase<T>& SubMatrix, const zfs::math::BoundingBox& BoundingBoxSrc, const zfs::math::BoundingBox& BoundingBoxTarget );
        void       SetSubMatrix( const MatrixBase<T>& SubMatrix, const zfs::math::BoundingBox& BoundingBoxTarget );
        const bool SetSubmatrixToValue( const T& value, const int start_line, const int end_line, const int start_column, const int end_column );
        const bool SetSubmatrixToValue( const T& value, const zfs::math::Range<int>& LineRange, const zfs::math::Range<int>& ColumnRange );
        // -- copy --------------------------------------------------------
        void CopySection(const MatrixBase<T>& src, const zfs::math::BoundingBox& aCopySection); // input matrix must have same size
        // -- info --------------------------------------------------------
        const int GetNoOfDiagonalLines() const;
        // -- get lines from matrix ---------------------------------------
        const std::vector<T> GetLine( int m ) const;
        void GetLine( int m, std::vector<T>& Line ) const;

        const std::vector<T> GetColumn( int n ) const;
        void                 GetColumn( int n, std::vector<T>& Column ) const; /// std::vector of bool
        void                 GetColumn( int n, T Column[] ) const; ///< array of bool !

        const std::vector<T> GetDiagonalLine45( int i ) const;
        const std::vector<T> GetDiagonalLine135( int i ) const;

        void GetDiagonalLine45( int i, std::vector<T>& Line45 ) const;
        void GetDiagonalLine135( int i, std::vector<T>& Line135 ) const;
        // -- set matrix from vector --------------------------------------
        void setMatrixFromVector( size_t _M, size_t _N, const std::vector<T>& matrix_vector, const bool& bRowsFastIndex = true );
        void setMatrixFromVector( size_t _M, size_t _N, const T* const matrix_vector, const bool& bRowsFastIndex = true );
        // -- get vector from matrix --------------------------------------
        void  getVectorFromMatrix( T* pVector, const bool& bRowsFastIndex = true );
        void  getVectorFromMatrix( std::vector<T>& Vector, const bool& bRowsFastIndex = true );
        // -- set lines/columns from matrix ---------------------------------------
        const bool SetLine( int m, const std::vector<T>& Line );
        const bool SetColumn( int n, const std::vector<T>& Column );
        const bool SetColumn( int n, const T* const Column );
        const bool SetDiagonal45( int i, const std::vector<T>& Diagonal45 );
        const bool SetDiagonal135( int i, const std::vector<T>& Diagonal135 );
        // -- helper for line access --------------------------------------
        void LineCoordinatesDiagonal45( int i, int& first, int& last ) const;
        void LineCoordinatesDiagonal135( int i, int& first, int& last ) const;
        void LineDiagonalLength( int i, int& length ) const;
        // -- matrix functions --------------------------------------------
        // flip
        MatrixBase flipped_lr() const;
        MatrixBase flipped_ud() const;
        void flip_lr();
        void flip_ud();
        // transpose
        const MatrixBase transposed() const;
        void             Transpose();
    };
    //================================================================================================================
    /**
     * @class Matrix
     * @brief  Matrix class providing matrix functionality particulary adapted and optimized for image processing operations.
     *
     * @author Juergen Holzner
     *
     * Matrix class providing matrix functionality particulary adapted and optimized for image processing operations.
     * Particular functions for numeric type matrices are provided.
     *
     */
    template <class T>
    class FILTERDLL_API Matrix: public MatrixBase<T>
    {
    private:
        static const int MmaxDet; /// < maximum number of lines/columns of a N x N Matrix for which the determinant is calculated

        T determinant( const Matrix<T>& aMat ) const;

    public:
        // -- Constructors -----------------------------------------------------
        // Standard Constructor
        Matrix();
        // "Normal Constructor" Matrix elements are initialized to 0;
        Matrix( size_t _M, size_t _N );
        // Matrix elements are initialized to preset_value
        Matrix( size_t _M, size_t _N, T preset_value );
        // Matrix is initialized from vector
        Matrix( size_t _M, size_t _N, const std::vector<T>& matrix_vector, const bool& bRowsFastIndex );
        Matrix( size_t _M, size_t _N, const T* const matrix_vector, const bool& bRowsFastIndex );
        // Matrix is read from ASCII-File
        Matrix( const wchar_t* filename );
        // Copy Constructors
        Matrix( const Matrix<T> &src );
        Matrix( const MatrixBase<T> &src );
        // Line-Section Matrix of Source (for use src-Matrix must exist over life-time of section matrix!!!)
        Matrix( const Matrix<T> &src, const zfs::math::Range<int>& LineRange );
        Matrix( const MatrixBase<T> &src, const zfs::math::Range<int>& LineRange );
        // Conversion constructor
        template<class T1> Matrix( const Matrix<T1>& src );
        // -- Helpers --------------------------------------------------------
        const Matrix GetSubMatrix( const int start_line, const int end_line, const int start_column, const int end_column ) const;
        const Matrix GetSubMatrix( const zfs::math::BoundingBox& aBoundingBox ) const;
        // -- Subsampling -------------------------------------------------
        const Matrix GetSubsampled(const int SubLine, const int SubColumn, const int SubLineOffset= 0, const int SubColumnOffset= 0) const;
        // -- Sampling -------------------------------------------------
        const std::vector<T>             Sample(const Matrix<bool>& aMatrix) const;
        const zfs::math::IntPointVectorType SamplePositions(const Matrix<bool>& aMatrix) const;
        // -- Operators ------------------------------------------------------
        const Matrix operator-() const;

        const Matrix operator/( const T& rhs ) const;
        const Matrix operator*( const T& rhs ) const;
        const Matrix operator+( const T& rhs ) const;
        const Matrix operator-( const T& rhs ) const;

        const Matrix operator*( const Matrix<T> &rhs ) const;
        const Matrix operator+( const Matrix<T> &rhs ) const;
        const Matrix operator-( const Matrix<T> &rhs ) const;

        Matrix & operator/=( const T& rhs );
        Matrix & operator*=( const T& rhs );
        Matrix & operator+=( const T& rhs );
        Matrix & operator-=( const T& rhs );

        Matrix & operator*=( const Matrix<T> &rhs );
        Matrix & operator+=( const Matrix<T> &rhs );
        Matrix & operator-=( const Matrix<T> &rhs );

        Matrix & operator=(const Matrix<T> &rhs);

        // -- Matrix Functions ------------------------------------------------
        // Transposition
        const Matrix transposed() const;
        void         Transpose();
        // Matrix Inverse
        const Matrix inverse( bool& bInversionSuccess ) const;
        void         Inverse( bool& bInversionSuccess );
        // Matrix Multiplication
        // -- matrix - matrix
        const Matrix         mat_mul( const Matrix<T>     & rhs ) const;
        // -- matrix - vector
        const std::vector<T> mat_mul( const std::vector<T>& rhs ) const;                            // << rhs.size() == N, result.size()==M !
        const void           mat_mul( const std::vector<T>& rhs, std::vector<T>& result ) const;    // << rhs.size() == N, result.size()==M !
        void                 mat_mul( const T* rhs, T* result ) const;                              // << rhs.size() == N, result.size()==M !
        // -- Helper Functions ------------------------------------------------
        void   SetAllElementsToZero();
        void   setDiag(const std::vector<T>& aValues);
        void   setDiag(const T* const aValues, const int N);
        // -- determinant
        T      Det() const;
        // -- sqrt of each element
        void   Sqrt();
        const Matrix sqrt() const;
        // -- sqr of each element
        void  Sqr();
        const Matrix sqr() const;
        // -- convolution
        const Matrix conv2( const Matrix<T>& Filter ) const;
        // -- convolution for seperable two dimensional filters 
        const Matrix conv2( const std::vector<T>& FilterRow, const std::vector<T>& FilterColumn ) const;
        // -- convolution in row direction with a vector filter 
        const Matrix conv_row( const std::vector<T>& FilterRow ) const;
        // -- convolution in column direction with a vector filter 
        const Matrix conv_col( const std::vector<T>& FilterColumn ) const;
        // -- abs value
        const Matrix abs() const; // Abs. Value of each matrix element, matrix returned
        void Abs();
        // -- minimum of matrix
        const T Min() const; // minimum in matrix
        const T Max() const; // maximum in matrix
        const T Min(const T& not_value) const; // minimum in matrix
        const T Max(const T& not_value) const; // maximum in matrix
        const T Min( int &min_index_row, int &min_index_column ) const; // minimum in matrix with position
        const T Max( int &max_index_row, int &max_index_column ) const; // maximum in matrix with position
        // -- median filter
        void MedianFilter( const int n_filter_width, const bool& bSetBegEndRows = true );
        // -- thresholding
        typedef enum  MatCompOp
        {
            GT, GE, LT, LE, EQ
        } MatCompOpType;
        const Matrix<bool> Thresholding( T Threshold, MatCompOpType Op ) const;     // Thresholding as matrix member function
        // -- statistics
        const double mean() const;                   // -- mean of matrix values
        const double std( bool unbiased ) const;     // -- std. dev. of matrix values
        const double var( bool unbiased ) const;     // -- variance of matrix values

        const std::set<T> unique() const;            // -- unique values in matrix

        const std::vector<double> ColMeans() const;  // -- means of Columns of matrix
        const std::vector<double> LineMeans() const; // -- means of Lines of matrix

    };

    //================================================================================================================
    /**
     * @class Matrix<bool>
     * @brief  Matrix class providing matrix functionality particulary adapted and optimized for binary image processing operations.
     *
     * @author Juergen Holzner
     *
     * Matrix class providing matrix functionality particulary adapted and optimized for binary image processing operations.
     * Particular functions for boolean type matrices are provided.
     *
     */

    template<> class FILTERDLL_API Matrix<bool>: public MatrixBase<bool>
    {
        template<class S> friend class Matrix;

    public:
        // -- Constructors -----------------------------------------------------
        // Standard Constructor
        Matrix();
        // "Normal Constructor" Matrix elements are initialized to 0;
        Matrix( size_t _M, size_t _N );
        // Matrix elements are initialized to preset_value
        Matrix( size_t _M, size_t _N, bool preset_value );
        // Matrix is initialized from vector
        Matrix( size_t _M, size_t _N, const std::vector<bool>& matrix_vector, const bool& bRowsFastIndex = true );
        Matrix( size_t _M, size_t _N, const bool* const matrix_vector, const bool& bRowsFastIndex = true );
        // Matrix is read from ASCII-File
        Matrix( const wchar_t* filename );
        // Copy Constructors
        Matrix( const Matrix<bool> &src );
        Matrix( const MatrixBase<bool> &src );
        // Line-Section Matrix of Source (for use src-Matrix must exist over life-time of section matrix!!!)
        Matrix( const Matrix<bool> &src, const zfs::math::Range<int>& LineRange );
        Matrix( const MatrixBase<bool> &src, const zfs::math::Range<int>& LineRange );
        // -- Helper ----------------------------------------------------------
        void              SetAllElementsToFalse();

        const             Matrix GetSubMatrix( const int start_line, const int end_line, const int start_column, const int end_column ) const;
        const             Matrix GetSubMatrix( const zfs::math::BoundingBox& aBoundingBox ) const;

        const             Matrix GetSubsampled(const int SubLine, const int SubColumn) const;

        void              GetColumn( const int n, bool Column[] ) const;
        std::vector<bool> GetColumn( const int n ) const;
        void              GetColumn( int n, std::vector<bool>& Column ) const;
        // -- Operators -------------------------------------------------------
        Matrix & operator&=( const Matrix<bool> &rhs );
        Matrix & operator|=( const Matrix<bool> &rhs );
        Matrix & operator^=( const Matrix<bool> &rhs );

        const Matrix   operator& ( const Matrix<bool> &rhs ) const;
        const Matrix   operator| ( const Matrix<bool> &rhs ) const;
        const Matrix   operator^ ( const Matrix<bool> &rhs ) const;

        const Matrix   operator~ () const;
        // -- Bool Type Functions ---------------------------------------------
        // no. of elements in matrix true
        int nElementsTrue() const;
        // is any element in matrix true
        bool AnyTrue() const;
        // Erosion Operation for bool matrices
        // erosion with boolean (true) square of size
        // operator_size* operator_size
        void Erode( zfs::math::Tuple2<int> operator_size, std::string operator_shape );
        void Erode( const Matrix<bool>& Operator );
        void ErodeHor( const std::vector<bool>& aHorOperator );
        void ErodeVert( const std::vector<bool>& aVertOperator );
        // Dilation operation(s) for bool matrices
        void Dilate( zfs::math::Tuple2<int> operator_size, const std::string& operator_shape );
        void Dilate( const Matrix<bool>& Operator );
        void DilateHor( const std::vector<bool>& aHorOperator );
        void DilateVert( const std::vector<bool>& aVertOperator );
        // Closing operation
        void Close( zfs::math::Tuple2<int> operator_size, const std::string& operator_shape );
        // Clean operation for bool matrices
        // 1s surronded by 0s are deleted
        void Clean();
        // Edge Thinning
        Matrix EdgeThinning( int nCount ) const;

        zfs::math::IntPointListType PositionsTrue() const;
    };

    //================================================================================================================
    /**
    * @class Matrix
    * @brief  Matrix class providing matrix functionality particulary adapted and optimized for image processing operations.
    *         Specialized for zfs::math::Vec3f()
    *
    * @author Juergen Holzner
    *
    * Matrix class providing matrix functionality particulary adapted and optimized for image processing operations.
    * Particular functions for numeric type matrices are provided.
    *
    */
    template<> class FILTERDLL_API Matrix<zfs::math::Vec3f> : public MatrixBase<zfs::math::Vec3f>
    {

        template<class S> friend class Matrix;

        Matrix<float> *x_el, *y_el, *z_el;

    public:
        // -- Constructors -----------------------------------------------------
        // Standard Constructor
        Matrix();
        // "Normal Constructor" Matrix elements are initialized to 0;
        Matrix(uint64_t _M, uint64_t _N);

        ~Matrix();

        void ToElemMatrices();

        const Matrix<float>& x() const;
        const Matrix<float>& y() const;
        const Matrix<float>& z() const;

    };

    //================================================================================================================
    /**
    * @class Matrix
    * @brief  Matrix class providing matrix functionality particulary adapted and optimized for image processing operations.
    *         Specialized for zfs::math::Vec3d()
    *
    * @author Juergen Holzner
    *
    * Matrix class providing matrix functionality particulary adapted and optimized for image processing operations.
    * Particular functions for numeric type matrices are provided.
    *
    */
    template<> class FILTERDLL_API Matrix<zfs::math::Vec3d> : public MatrixBase<zfs::math::Vec3d>
    {

        template<class S> friend class Matrix;

        Matrix<double> *x_el, *y_el, *z_el;

    public:
        // -- Constructors -----------------------------------------------------
        // Standard Constructor
        Matrix();
        // "Normal Constructor" Matrix elements are initialized to 0;
        Matrix(uint64_t _M, uint64_t _N);

        ~Matrix();

        void ToElemMatrices();

        const Matrix<double>& x() const;
        const Matrix<double>& y() const;
        const Matrix<double>& z() const;

    };

    //================================================================================================================
    // Auxiliary Functions
    //================================================================================================================
    template<class T> const Matrix<T> FILTERDLL_API GenerateRandomMatrix( const int M, const int N );
    template<class T> void            FILTERDLL_API copyMatrixSection( const zfs::math::Range<int>& aCopyRange, const Matrix<T>* const pSrc, Matrix<T>* const pDest );
    //================================================================================================================
    // File Functions
    //================================================================================================================

    /**
     * @brief Write matrix into file
     *
     * @note Perfer the stream operator @c operator<<() to avoid problems with
     *       filename encoding
     *
     * @param filename Path to the file to write the matrix to.
     * @param a_Matrix Matrix to write
     * @return true on success
     */
    template<typename T> bool FILTERDLL_API
    WriteASCFile( const wchar_t* filename, const MatrixBase< T> & a_Matrix );

    /**
     * @brief Read matrix from file
     *
     * @note Prefer the stream operator @c operator>>() to avoid problems with
     *       filename encoding
     *
     * @param filename Path to the file to read from
     * @param a_Matrix Matrix to read
     * @return true on success
     */
    template<typename T> bool FILTERDLL_API
    ReadASCFile( const wchar_t* filename, MatrixBase< T> & a_Matrix );

    // stream based ASCI access

    /**
     * @brief serializes the Matrix mat into the stream os
     * @param os output stream
     * @param mat reference to matrix
     * @return reference to stream os
     */
    template<typename T> FILTERDLL_API
    std::ostream& operator << (std::ostream& os, const MatrixBase< T> & mat);

    /**
     * @brief read Matrix mat from stream is
     * @param is input stream
     * @param mat reference to matrix to read
     * @return reference to stream is
     */
    template<typename T> FILTERDLL_API
    std::istream& operator >> (std::istream& is, MatrixBase< T> & mat);

} // zfsfilter

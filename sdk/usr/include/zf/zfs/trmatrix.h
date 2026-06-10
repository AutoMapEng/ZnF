#pragma once

#include "zfsapi.h"

#include <string>
#include <vector>

#include "zfbase.h"
#include "zfvector.h"
#include "Math/zfvec.h"

namespace zfs
{
    class BoundingBox3D;
}

//================================================================================================================
/**
\brief The 4x4 matrix that is used as a transformation matrix between 3d-coordinate systems.

Transformations Matrix

How the matrix translates into an 1D array m[16]
Matrix Members:                     Array Indizes:
a1 b1 c1 dTranslateX                0  1  2  3
a2 b2 c2 dTranslateY                4  5  6  7
a3 b3 c3 dTranslateZ                8  9 10 11
a3 b4 c4 scale                     12 13 14 15

ZFTRMatrix::World(x) is like m*x
*/
class ZFS_API ZFTRMatrix : public ZFBase
{
    DEFINE_ZFBASE(ZFBase)

    static const double eps;    ///< Epsilon used for operator==()

protected:
    bool   hasTR{};
    double dRotateX{}, dRotateY{}, dRotateZ{};    ///< (obsolete) angle in degree for rotations around the coordinate axes.

    ///< internal matrix used for translation/rotation
    double a1{ 1. }, b1{ 0. }, c1{ 0. }, dTranslateX{}; ///< rad and meter
    double a2{ 0. }, b2{ 1. }, c2{ 0. }, dTranslateY{};
    double a3{ 0. }, b3{ 0. }, c3{ 1. }, dTranslateZ{};
    double a4{ 0. }, b4{ 0. }, c4{ 0. }, scale{ 1. };  

public:
    ZFTRMatrix();                       ///< The matrix is initialized as a unit matrix.
    ZFTRMatrix( const ZFTRMatrix& );
    ZFTRMatrix( const std::string& str );
    ZFTRMatrix( const std::vector<double>& vec);
    ZFTRMatrix( const std::vector<float>& vec);
    virtual ~ZFTRMatrix() {}

    ZFTRMatrix& set(const ZFTRMatrix&);

    /// exchanges the matrix order od the local matrix
    ZFTRMatrix& transpose();
    /// return a transposed matrix. Let local unchanged
    ZFTRMatrix transposed() const;

    ZFTRMatrix& SetTranslateDirect(double x, double y, double z);///< Sets the member dTranslateX, dTranslateY, dTranslateZ
    ZFTRMatrix& SetTranslateDirect(const zfs::math::Vec3d& translation);   ///< Sets the member dTranslateX, dTranslateY, dTranslateZ
    ZFTRMatrix& SetTranslate(double x, double y, double z) { dTranslateX = x; dTranslateY = y; dTranslateZ = z; return*this; } // meter
    ZFTRMatrix& SetTranslateR( double x, double y, double z); // with rotation

    ZFTRMatrix& SetRotate( double xRotDeg, double yRotDeg, double zRotDeg ); ///< Set the matrix to a rotation matrix, by giving the rotation angles around the coordinate axis in degree.
    ZFTRMatrix& SetRotate( double x, double y, double z, double angleDeg );   // what is this doing?
    ZFTRMatrix& SetRotate( ZFVec3d rotAxis, double angleRad);    /**< Sets the rotation of the matrix to rotate around axis with angle. rotAxis must be of length 1 */
    ZFTRMatrix& SetRotate( const ZFVec3d& active, const ZFVec3d& passive); /** Sets the rotation by rotating active into passive. Afterwards ZFTRMatrix::World(active) == passive*/
    ZFTRMatrix& SetRotateQ( double q1, double q2, double q3, double q4 );
    bool getQuaternion( double& qw, double& qx, double& qy, double& qz ) const;
    ZFTRMatrix& SetRotateYawPitchRoll( double yaw, double pitch, double roll); /**< Angles after http://de.wikipedia.org/wiki/Eulersche_Winkel  Section „z, y', x''-Konvention“ in der Fahrzeugtechnik (Gier-Nick-Roll). The resulting matrix transforms from the "earth"-system to the "vehicle"-system. */
    
    void GetRotationFromMatrix(double& x, double& y, double& z) const; ///< Euler xyz deg
    zfs::math::Vec3d getRotationFromMatrix_rad() const;

    zfs::math::Vec3d GetRollPitchYawFromMatrix() const; ///< used for mapcam and mobile [rad]
    zfs::math::Vec3d GetYawPitchRollFromMatrix() const; ///< used for mobile & Slam [rad]
    void GetRotationAroundAxes(double& xRotDeg, double& yRotDeg, double& zRotDeg) const;    /**< Factorizes the 3x3 rotation matrix Rn into Rz * Ry * Rx == Rn, where the Ri are the rotations around the axes. The returned values are the rotation angles in degree around those axes. */
    int solveTR(double &omega, double &phi, double &kappa, double &tx, double &ty, double &tz) const; ///< Euler xyz deg + translation

    // from SynCaT:
    ZFTRMatrix& LStoIMU(double alpha, double beta, double gamma); // Rotationsmatrix scanner -> IMU simultan zu "applyZYX" aus SynCaT
    ZFTRMatrix& IMUtoGLO(double alpha, double beta, double gamma); // Rotationsmatrix IMU -> Global simultan zu "applyZXY" aus SynCaT


    /// set the matrix by a 9 (3x3) or 16 (4x4) value vector
    ZFTRMatrix& SetTRMatrix(const std::vector<double>&);
    ZFTRMatrix& SetTRMatrix(const std::vector<float>&);
    ZFTRMatrix& SetTRMatrix4x4( const double* m ); // must be a 4x4 matrix (rad,m)
    ZFTRMatrix& SetTRMatrix4x4( const float* m ); // must be a 4x4 matrix (rad,m)
    ZFTRMatrix& SetTRMatrix3x3( const double* m ); /**< must be a 3x3 matrix (rad) for rotation only, m = {a1, b1, c1, ..., c3} */
    ZFTRMatrix& SetTRMatrix3x3(const double m[3][3]); /**< must be a 3x3 matrix (rad) for rotation only, m = {a1, b1, c1, ..., c3} */
    ZFTRMatrix& SetTRMatrix3x3( const float* m); /**< must be a 3x3 matrix (rad) for rotation only, m = {a1, b1, c1, ..., c3} */

    ZFTRMatrix& orthonormalize();           //<! orthonormalize the matrix
    bool isOrthonormal() const;             //<! check if the matrix is orthogonal

    void GetTRMatrix4x4( double* m ); // must be a 4x4 matrix
    void GetTRMatrix4x4( double* m ) const; // must be a 4x4 matrix
     /// return the 16 values of the matrix
    std::vector<double> GetTRMatrix4x4() const; // must be a 4x4 matrix
    /// return the 9 rotation values
    std::vector<double> GetTRMatrix3x3() const; // must be a 3x3 matrix

    ZFTRMatrix& Multiply3x3(double* m); // m is the matrix to the right, ref: Documentation of Matrices
    ZFTRMatrix& Multiply4x4(double* m); // m is the matrix to the right. Use this if you want to combine two transformations into one.
    ZFTRMatrix& Multiply3x3( const ZFTRMatrix& m); // m is the matrix to the right
    ZFTRMatrix& Multiply4x4( const ZFTRMatrix& m); // m is the matrix to the right. Use this if you want to combine two transformations into one.
    ZFTRMatrix& operator *=(const ZFTRMatrix& m);
    ZFTRMatrix operator *(const ZFTRMatrix& m) const;

    ZFTRMatrix Inverse() const; ///< Returns the unit matrix if the matrix is not invertible.

    void SetScale(double s) { scale = s; }
    double GetScale() const { return scale; }

    void ResetTR();         ///< Sets this to the unit matrix.
    ZFTRMatrix& ResetRotation();   ///< Sets the 3x3 rotation matrix to the unit matrix. Let translation unchanged
    ZFTRMatrix& ResetTranslation();   ///< Sets translation to zero. Let the 3x3 rotation matrix unchanged
    bool HasTR() const;
    bool IsEinheitsmatrix() const;

    /// calculate world using rotation and ignoring translation
    zfs::math::Vec3d angle2world(const zfs::math::Vec3d& v) const;

    /// transfer a local coordinate to world space
    zfs::math::Vec3d world(const zfs::math::Vec3d&) const;
    void World(zfs::math::Vec3d&) const;
    void World(zfs::math::Vec3f&) const;
    void World( double& x, double& y, double& z ) const ;
    void World(zfs::BoundingBox3D&) const;
    ZFVector GetTransformed(ZFVector vec) const;  ///< Returns the transformed vec.

    void Rotate( double& x, double& y, double& z ) const; ///< Applies the 3x3 rotation matrix to vec.
    ZFVector GetRotate( ZFVector vec) const;    ///< Applies the 3x3 rotation matrix to vec.



    /// get translation
    zfs::math::Vec3d getT() const { return zfs::math::Vec3d(dTranslateX, dTranslateY, dTranslateZ); }
    ZFVec3d getTranslation() const;   ///< Get the translation in a vector
    double tx() const { return dTranslateX; } // Translation meter
    double ty() const { return dTranslateY; }
    double tz() const { return dTranslateZ; }
    zfs::math::Vec3d translation() const;

    bool compare(const ZFTRMatrix& mat, double epsilon) const;
    bool operator==( const ZFTRMatrix& mat ) const; ///< equal within eps
    bool operator!=( const ZFTRMatrix& mat ) const;
    
//    double at( int rowIndex, int colIndex) const;   /**< Returns an element of the matrix with the given index pair. Indices range from 0 to 3. */
    const double at( int rowIndex, int colIndex) const; ///< generates an exception if index out of range
    double& refAt(int rowIndex, int colIndex); ///< generates an exception if index out of range
    ZFVec3d getRotationColumnVector( int colIndex) const;     ///< Returns a column vector of the 3x3 rotation matrix.
    void setRotationColumnVector( const ZFVec3d& vec, int colIndex); /**< Set the values for a column of the 3x3 rotation matrix. Vec is normalized in the function. */

    ZFTRMatrix GetDistMatrix( const ZFTRMatrix& mat ) const;

    double getDeterminant3x3() const;                           //!< Returns the determinant of the rotation part of the matrix

    /// build a string. Just a single line
    std::string toString(const std::string& separator = " ", const std::string& cr = {}) const { return toString4x4(separator, "%1$.9f",cr); }
    std::string toString3x3(const std::string& separator=" ", const std::string& format = "%1$.9f") const;
    std::string toString4x4(const std::string& separator = " ", const std::string& format = "%1$.9f", const std::string& cr = {}) const;
    std::wstring toWString4x4(const std::wstring& separator = L" ", const std::wstring& format = L"%1$.9f", const std::wstring& formatXYZ = L"%1$.3f", const std::wstring& cr = {}) const;

    // JSON
    std::string toJsonString() const;
    bool fromJson(const std::string& str);

    // XML
    std::string toXML() const;

    /// parse a 9 or 16 value string, otherwise return false. Values can be in a single or multiple lines
    bool fromString(const std::string& str) { return parse(str); }
    bool parse(const std::string&);

    ZFTRMatrix& from_RPY(const zfs::math::Vec3d&); // apply yaw, pitch, roll (ZYX)
    ZFTRMatrix& from_ZXY(const zfs::math::Vec3d&); // apply yaw, roll, pitch (ZXY)
    zfs::math::Vec3d to_RPY() const;

protected:
    void CalcTR(double dRotateX, double dRotateY, double dRotateZ);  ///< Uses the rotation angles to compute the matrix elements a1...c3

private:
    bool rotationMatrixIsValid() const;   ///< returns true if the 3x3 rotation matrix R has det == 1 and  R*Rtranspose == 1
    double computeDeterminant3x3() const; ///< Computes the determinant of the 3x3 rotation matrix.
    ZFTRMatrix getTransposeOf3x3() const; ///< Returns a matrix that has
    bool rotationMatrixIsUnitMatrix() const;  ///< returns true if the rotation matrix is a unit matrix within ten times of the numerical limit.
    ZFTRMatrix getRotationMatrixAroundX(double angleRad) const;
    ZFTRMatrix getRotationMatrixAroundY(double angleRad) const;
    ZFTRMatrix getRotationMatrixAroundZ(double angleRad) const;
    double getRootOfAngleEquation( double a, double b) const;
};


//================================================================================================================

ZFS_API void uk_rt2matrix(const Vector_3d &rr,const Vector_3d &tt,double *RT);
ZFS_API void uk_matrix2rt(double *RT, Vector_3d &rr, Vector_3d &tt );

//================================================================================================================

inline zfs::math::Vec3d ZFTRMatrix::world( const zfs::math::Vec3d& v ) const
{
    return zfs::math::Vec3d(
        ( v.x()*a1 + v.y()*b1 + v.z()*c1 )*scale + dTranslateX,
        ( v.x()*a2 + v.y()*b2 + v.z()*c2 )*scale + dTranslateY,
        ( v.x()*a3 + v.y()*b3 + v.z()*c3 )*scale + dTranslateZ );
}

inline zfs::math::Vec3d ZFTRMatrix::angle2world(const zfs::math::Vec3d& v) const
{
    return zfs::math::Vec3d(
        (v.x() * a1 + v.y() * b1 + v.z() * c1),
        (v.x() * a2 + v.y() * b2 + v.z() * c2),
        (v.x() * a3 + v.y() * b3 + v.z() * c3) );
}




inline void ZFTRMatrix::World( double& x, double& y, double& z ) const
{
    const double xx = ( x*a1 + y*b1 + z*c1 )*scale + dTranslateX;
    const double yy = ( x*a2 + y*b2 + z*c2 )*scale + dTranslateY;
    const double zz = ( x*a3 + y*b3 + z*c3 )*scale + dTranslateZ;
    x = xx;
    y = yy;
    z = zz;
}

inline void ZFTRMatrix::World(zfs::math::Vec3d& v) const
{
    const double xx = ( v.x()*a1 + v.y()*b1 + v.z()*c1 )*scale + dTranslateX;
    const double yy = ( v.x()*a2 + v.y()*b2 + v.z()*c2 )*scale + dTranslateY;
    const double zz = ( v.x()*a3 + v.y()*b3 + v.z()*c3 )*scale + dTranslateZ;
    v[0] = xx;
    v[1] = yy;
    v[2] = zz;
}
inline void ZFTRMatrix::World(zfs::math::Vec3f& v) const
{
    const double xx = ( v.x()*a1 + v.y()*b1 + v.z()*c1 )*scale + dTranslateX;
    const double yy = ( v.x()*a2 + v.y()*b2 + v.z()*c2 )*scale + dTranslateY;
    const double zz = ( v.x()*a3 + v.y()*b3 + v.z()*c3 )*scale + dTranslateZ;
    v[0] = static_cast< float >( xx );
    v[1] = static_cast< float >( yy );
    v[2] = static_cast< float >( zz );
}


inline zfs::math::Vec3d ZFTRMatrix::translation() const
{
    return zfs::math::Vec3d(dTranslateX, dTranslateY, dTranslateZ);
}


#pragma once

/**
\file geom_Objects.h

This file contains functions that do calculations with geometric objects.
Those objects are mostly represented by raw c-arrays.

\todo
This "c-style" approach with raw pointers to doubles leads to clumsy and error prone code.
The functionality should be moved into classes that represent the geometric
objects. The classes are ZFVector3d, ZFLine3d and ZFHessePlane.

*/

#include <zfs/zfsapi.h>


// ------------------
// Plane
// ------------------
typedef struct plane_
{
    double * d2x, * d2y, * d2z;
    double * d3x, * d3y, * d3z;
    int n;
    double a[4];
} plane_t;

/*
Hesse plane notation:
The plane is represented by an 4 komponents double array.
hesse[0] holds -n*r0 (ro is a point on the plane), which is the distance of the plane to the offspring.
The other three komponents hold the normalized plane normal vector n
*/

// Functions for 2D arrays
double ZFS_API line_length_2D( double x1, double y1, double x2, double y2);
inline double sqare_line_length_2D(int x1, int y1, int x2, int y2) { double x2_ = x2, y2_ = y2;  return ((x2_ - x1) * (x2_ - x1) + (y2_ - y1) * (y2_ - y1)); }   ///< this can be used when you only need to compare distances.

// Functions for 3D arrays
/**
\param x Pointer to array of x values with size n.
\param y Pointer to array of y values with size n.
\param z Pointer to array of z values with size n.
\param a Pointer to an array of size 4 that follows the hesse plane convention.
*/
void ZFS_API zf_approxHessePlane( double *x, double *y, double *z, int n, double *a );
/**
\param normal Pointer to an array of size 3 that holds the normal vector of the plane. The vector can be of any length.
\param point Pointer to an array of size 3 that holds a point that lies on the plane.
\param hesse Pointer to an array of size 4 to which the calculated hesse plane will be written.
*/
void ZFS_API zf_calculate_hesseplane_with_point_and_normal_3D( const double* const normal, const double* const point, double* const hesse);
/**
// I: (x1,y1), .. , (x3,y3)
// ------------------------------------------------------------------------
// D: plane(t1,t2) = vec0 + t1*vec1 + t2*vec2
*/
void ZFS_API zf_plane_in_Paraform_2D( double *vec0, double *vec1, double *vec2, double *x, double *y );
/**
\brief Finds the parameters of the plane hesse for the form f(x,y) = a*x + b*y + c
Keep in mind that this will give no results for vertical planes.
*/
void ZFS_API zf_Plane_Parameters_3D( const double* const hesse, double &a, double &b, double &c);
/**
\param linePt1 = first point on the line
\param linePt2 = second point on the line
\param ptToProj = the point that shall be projected on the line
*/
void ZFS_API zf_proj_point_onto_line_3D( const double* const linePt1, const double* const linePt2, double* const ptToProj);
/// Project a point onto a plane
void ZFS_API zf_proj_point_onto_plane_3D( double *hesse, double x, double y, double z, double *out_x, double *out_y, double *out_z);
void ZFS_API zf_proj_point_onto_plane_3D( const double *hesse, double& x, double& y, double& z); /**< \overload */
void ZFS_API zf_proj_point_onto_plane_3D( const double *hesse, double* point); /**< \overload */
void ZFS_API zf_proj_point_onto_plane_3D( double *hesse, double& x, double& y, double& z, double &distance); /**< \overload */
/// Mirrors a point at a plane.
void ZFS_API zf_mirror_point_on_plane_3D( double *hesse, double& x, double& y, double& z);
/// Calculates the coordinates to the corresponding base (specified by vec(i))
void ZFS_API zf_paraForm_coo_2D( double *vec0, double *vec1, double *vec2, double x, double y, double *res_coo);
/// ???
double ZFS_API zf_dist_line_2D( double x, double y, double *a);
/// Calculates the distance from a point to a line in 3d where the line is defined by two points.
double ZFS_API zf_distance_point_to_line_3D( const double* const linePt1, const double* const linePt2, const double* const point);
double ZFS_API zf_distance_point_to_line_3D( const double* const linePt1, const double* const linePt2, const double* const point, double* const projectedPoint); /**< \overload */
/// Calculates the distance from a point to a line in 3d where the line is defined by a point and a spanning vector.
double ZFS_API zf_distance_point_to_line_3D_VecForm( const double* const supporter, const double* const spanner, const double* const point);
double ZFS_API zf_distance_point_to_line_3D_VecForm( const double* const supporter, const double* const spanner, const double* const point, double* const projectedPoint);/**< \overload */
/// Calculates the distance between a point and a plane.
double ZFS_API zf_dist_plane_3D(double x, double y, double z, const double* const a );
double ZFS_API zf_dist_plane_3D( const double* const point, const double* const hessePlane );/** \overload */
/**
// I: plane a1 in hesseform
//    plane a2 in hesseform
//    plane a3 in hesseform
// O: res_x: x-coo
//    res_y: y-coo
//    res_z: z-coo
*/
int ZFS_API zf_point_as_3_plane_cut( const double *a1, double *a2, double *a3, double *res_x, double *res_y, double *res_z);
/**
// I: plane a1 in hesse-form
//    plane a2 in hesse-form
//    z Coo
// O: res_x: x-coo
//    res_y: y-coo
*/
int ZFS_API zf_line_as_2_plane_cut( double *a1, double *a2, double z, double *res_x, double *res_y);
int ZFS_API zf_line_as_2_plane_cut( const double* const hesse1, const double* const hesse2, double* pointOnLine, double* spanningVector);/** \overload */
/// Returns the absolute value of the smaller of the two angles between two planes.
double ZFS_API zf_pointed_angle_between_planes( double *hessePlane1, double* hessePlane2);
/// Returns values between 0 and PI
double ZFS_API zf_angle_between_vectors( const double* const v1, const double* const v2);
/**
\return values between 0 and PI, 0 if v1 and v2 are parallel, PI if they are anti-parallel.
*/
double ZFS_API zf_angle_between_vectors(double x1, double y1, double z1, double x2, double y2, double z2);
int ZFS_API zf_cut_point_onto_plane_3D( const double *hesse, double& x, double& y, double& z, double *fac = 0 );   ///< not sure what that does
/// Calculates the intersection point of a line and a plane in 3d.
void ZFS_API zf_intersect_line_and_plane_3D( const double* const hessePlane, const double* const linePoint, const double* const lineSpanningVector, double* const intersectionPoint);
int ZFS_API zf_parallProj( double n0, double n1, double n2, double d, double x, double y, double z, double &resx, double &resy, double &resz);
int ZFS_API zf_zentralproj( double Zx, double Zy, double Zz, double n0, double n1, double n2, double d, double x, double y, double z, double &resx, double &resy, double &resz);
/// Copy arrays of size 3
void ZFS_API zf_copy_vector_3D( double* const to, const double* const from);
/// v1-v2
void ZFS_API zf_substract_vectors_3D( const double* const v1, const double* const v2, double* const result);
/// v1+v2
void ZFS_API zf_add_vectors_3D( const double* const v1, const double* const v2, double* const result);
/// Calculates the length of the vector from pt1 to pt2.
double ZFS_API zf_twoPointDistance3D( const double* const pt1, const double* const pt2 );
/// Calculates the length of u
double ZFS_API zf_length_vector_3D( const double *u );
double ZFS_API zf_length_vector_3D( double x, double y, double z);   /**< \overload */
/// Sets the length of a vector to one and returns its former length.
int ZFS_API zf_norm_vector( double& x, double& y, double& z, double &length_u );
/// Sets the length of a vector to one.
void ZFS_API zf_norm_vector_3D(double* const u);
/// Sets a vector to a given length.
int ZFS_API zf_scale_vector( double& x, double& y, double& z, double length );
/// Multiplies a vector by a factor.
void ZFS_API zf_scale_vector_3D( double* const u, double factor );
void ZFS_API zf_scale_vector_3D( const double* in, double* out, double factor ); /**< \overload */
/// Sets a vector to a given length.
void ZFS_API zf_set_vector_to_length_3D( double* const u, double factor );
int ZFS_API zf_proj_vec2Bowl( double nx, double ny, double nz, double x0, double x1, double x2, double r,
                                double &resx, double &resy, double &resz,
                                double &resx2, double &resy2, double &resz2);
/// Calculates res = u x v
int ZFS_API zf_crossprod( const double *u, const double *v, double *res );
int ZFS_API zf_crossprod( double u0, double u1, double u2, double v0, double v1, double v2, double& res0, double& res1, double& res2 );/**< \overload */
/**
\brief Calculate the dot product of 3D-vectors u and v.
The user has to make sure that u and v are arrays with 3 elements.
*/
double ZFS_API zf_dotprod3D(const double* const u, const double* const v);
/// Calculates the triple product a * ( b x c )
double ZFS_API zf_tripleprod( double* a, double* b, double* c);
/// returns the area of the triangle spanned by X1 and X2
double ZFS_API zf_triangle_area( double x1, double y1, double z1, double x2, double y2, double z2);
double ZFS_API zf_triangle_area( const double* const pt1, const double* const pt2);/**< \overload */

void ZFS_API plane_init( plane_t *pl, int n );
void ZFS_API plane_del( plane_t *pl );

/* ************ Transformations ***************** */
int ZFS_API ZFMath_subRotMatrix( double R1[3][3], double R2[3][3], double R_out[3][3]);
int ZFS_API rotAxisAngle( double x, double y, double z, double alpha, double *n, double &x_out, double &y_out,double &z_out);
int ZFS_API rotAxisAngle( const double* const inVec, double alpha, double* rotAxis, double* const outVec );
int ZFS_API getRotAxisAngle( double alpha /*rad*/, double *m, double A[3][3] );
int ZFS_API getRotAxisAngle( double alpha, double *m, double A[9] );
void ZFS_API ZFMath_apply_RPY_transform (double roll/* rad */, double pitch/* rad */, double yaw/* rad */, double R[3][3]); // rad
void ZFS_API ZFMath_solve_RPY_transform (double R[3][3], double &roll/* rad */, double &pitch/* rad */, double &yaw/* rad */); // rad
int ZFS_API ZFMath_multiply_Rot(double R2[3][3], double R1[3][3], double R_out[3][3]);
int ZFS_API ZFMath_multiply_Vec(double R[3][3], double x, double y, double z, double &x_out, double &y_out, double &z_out );
int ZFS_API ZFMath_inverse_Rot(double R[3][3], double R_out[3][3]);

/* ******** Coordinate Transformations **************/
// Spherical coordinates. Follows the "Physics" convention spherical = (r,theta,phi) with theta the angle to the z-axis.
void ZFS_API cartesianToSpherical( const double* const cartesian, double* const spherical);
void ZFS_API sphericalToCartesian( const double* const spherical, double* const cartesian);

/* ******** MATRICES AND TRANSFORMATIONS **************/
// 3X3 Matrixes m are stored as m[0] m[1] m[2]
//                              m[3] m[4] m[5]
//                              m[6] m[7] m[8]

/// creates a 3X3 matrix that will rotate angleRad radiants around axis rotAxis. rotAxis can be of any length.
void ZFS_API makeRotationMatrix3X3( const double* const rotAxis, double angleRad, double* const outputMatrix);
/// creates a 3X3 matrix that will rotate vector1 into vector2. rotAxis can be of any length.
void ZFS_API makeRotationMatrix3X3( const double* const vector1,  const double* const vector2, double* const outputMatrix);
/**
\brief Creates a 3X3 matrix that rotates/transfoms the 3X3 unity matrix into newSystemMatrix.
The user is responsible for making sure that newSystemMatrix is not singular (det != 0).
If you only want to rotate, make sure that newSystemMatrix is a orthonormal basis.
*/
void ZFS_API makeSystemRotationMatrix3X3( const double* const newSystemMatrix, double* const outputMatrix);
/// Calculates a * b = result
void ZFS_API multiplyMatrices3X3( const double* const a, const double* const b, double* const result);
/// Inverts matrix, the user is responsible to make sure that det(matrix) != 0;
void ZFS_API invertMatrix3X3( const double* const matrix, double* const result);
double ZFS_API computeDeterminant3X3( const double* const matrix);
double ZFS_API computeDeterminant2X2( const double* const matrix);
/// Compute matrix * vector = outputVector   (inner product)
void ZFS_API multiplyMatrixAndVector3X3( const double* const matrix, const double* const vector, double* const outputVector);

// helper functions for matrix computations
/// this is just a helper function for calculating a determinant, minor points to an array of length 4, matrix is of length 9
void ZFS_API getMinorMatrix3X3( const double* const matrix, int row, int col, double* const minor );

// -----------------
// Axis
// -----------------
typedef struct line_
{
    double * d2x, * d2y, * d2z;
    double * d3x, * d3y, * d3z;
    int n;
    double a[3];
} line_t;

void ZFS_API zf_approxAxis( double *x, double *y, double *w, int n, double *a1, double *a2, double *mx, double *my );

// ------------------
// Ellipse
// ------------------
typedef struct ellipse_
{
    double * d2x, * d2y, * d2z;
    double * d3x, * d3y, * d3z;
    int n;
    double a[6];
    double l1[3], l2[3];
    double mx, my, mx3D, my3D, mz3D;
    double r1, r2;
} ellipse_t;

void ZFS_API ellipse_normalPos(double *alpha, double *res, double *xm, double *ym, double *a, double *b);
void ZFS_API zf_approxEllipse( double *x, double *y, double *w, int n, double *a1, double *a2, double *mx, double *my );
void ZFS_API ellipse_init( ellipse_t *el, int n );
void ZFS_API ellipse_del( ellipse_t *el );

// ---------------------------
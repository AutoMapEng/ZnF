#ifndef __LEVENBERG_H__
#define __LEVENBERG_H__

#include <zfs/zfsapi.h>

#include "Maths/Vector/Vector.h"
#include "Maths/MatrixC/Matrix.h"

bool ZFS_API levenbergMarquardt( void(*modelFuncs)(const Vector &, const Matrix &, double &, double []), bool &modified);

#endif //__LEVENBERG_H__

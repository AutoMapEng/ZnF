#pragma once

namespace zf
{
#ifndef PI
# define PI 3.1415926535897932384626433832795
#endif

#ifndef M_PI
# define M_PI PI
#endif

#ifndef RADS_PER_DEGREE
#define RADS_PER_DEGREE 0.017453292519943295769236907684886
#define DEGREES_PER_RAD 57.295779513082320876798154814105
#endif




#ifndef TORAD
# define TORAD(grad)  (static_cast<double>(grad)*(PI/180.))
#endif

#ifndef TOGRAD
# define TOGRAD(rad)  (static_cast<double>(rad )*(180./PI) )
#endif

	inline double deg2rad(const double& degrees)
	{
		return degrees * RADS_PER_DEGREE;
	}

	inline double rad2deg(const double& degrees)
	{
		return degrees * DEGREES_PER_RAD;
	}
}

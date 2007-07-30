#ifndef _PIRATE_MATH_H_
#define _PIRATE_MATH_H_

#include "CompileConfig.h"
#include <cmath>
#include <algorithm>

namespace Pirate
{

//! Rounding error constant often used when comparing f32 values.
const float	 ROUNDING_ERROR_32	= 0.000001f;
const double ROUNDING_ERROR_64	= 0.00000001f;

//! Constant for PI.
const float	 PI	  = 3.14159265359f;

//! Constant for 64bit PI.
const double PI64 = 3.1415926535897932384626433832795028841971693993751;

//! 32bit Constant for converting from degrees to radians
const float  DEGTORAD   = PI / 180.0f;

//! 32bit constant for converting from radians to degrees
const float  RADTODEG   = 180.0f / PI;

//! 64bit constant for converting from degrees to radians
const double DEGTORAD64 = PI64 / 180.0;

//! 64bit constant for converting from radians to degrees
const double RADTODEG64 = 180.0 / PI64;

//! returns linear interpolation of a and b with ratio t
//! \return: a if t==0, b if t==1, and the linear interpolation else
template<class T>
inline T lerp(const T& a, const T& b, const T& t)
{
	return (a*(1-t)) + (b*t);
}

//! clamps a value between low and high
template<class T>
inline const T& clamp (const T& value, const T& low, const T& high) 
{
	using namespace std;
	return min(max(value,low), high);
}

//! returns if a T equals the other one, taking custom 
//! rounding errors into account
template<class T>
inline bool equals(const T& a, const T& b, const T& tolerance)
{
	return (a + tolerance > b) && (a - tolerance < b);
}

//! returns if a float equals the other one, taking floating 
//! point rounding errors into account
inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_32)
{
	return (a + tolerance > b) && (a - tolerance < b);
}

//! returns if a double equals the other one, taking double 
//! point rounding errors into account
inline bool equals(const double a, const double b, const double tolerance = ROUNDING_ERROR_64)
{
	return (a + tolerance > b) && (a - tolerance < b);
}

//! returns if a float equals zero, taking floating 
//! point rounding errors into account
inline bool iszero(const float a, const float tolerance = ROUNDING_ERROR_32)
{
	using namespace std;
	return abs ( a ) < tolerance;
}

} // end namespace Pirate

#endif
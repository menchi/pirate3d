#ifndef _PIRATE_MATH_H_
#define _PIRATE_MATH_H_

#include "pirateTypes.h"
#include <math.h>

namespace Pirate
{

//! Rounding error constant often used when comparing f32 values.
const f32 ROUNDING_ERROR_32	= 0.000001f;
const f64 ROUNDING_ERROR_64	= 0.00000001f;

//! Constant for PI.
const f32 PI			= 3.14159265359f;

//! Constant for 64bit PI.
const f64 PI64			= 3.1415926535897932384626433832795028841971693993751;

//! 32bit Constant for converting from degrees to radians
const f32 DEGTORAD   = PI / 180.0f;

//! 32bit constant for converting from radians to degrees
const f32 RADTODEG   = 180.0f / PI;

//! 64bit constant for converting from degrees to radians
const f64 DEGTORAD64 = PI64 / 180.0;

//! 64bit constant for converting from radians to degrees
const f64 RADTODEG64 = 180.0 / PI64;

//! returns minimum of two values. Own implementation to get rid of the STL (VS6 problems)
template<class T>
inline const T min_(const T a, const T b)
{
	return a < b ? a : b;
}

//! returns maximum of two values. Own implementation to get rid of the STL (VS6 problems)
template<class T>
inline T max_(const T a, const T b)
{
	return a < b ? b : a;
}

//! returns abs of two values. Own implementation to get rid of STL (VS6 problems)
template<class T>
inline T abs_(const T a)
{
	return a < 0 ? -a : a;
}

//! returns linear interpolation of a and b with ratio t
//! \return: a if t==0, b if t==1, and the linear interpolation else
template<class T>
inline T lerp(const T a, const T b, const T t)
{
	return (a*(1-t)) + (b*t);
}

//! clamps a value between low and high
template <class T>
inline const T clamp (const T value, const T low, const T high) 
{
	return min_ (max_(value,low), high);
}

//! returns if a float equals the other one, taking floating 
//! point rounding errors into account
inline bool equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_32)
{
	return (a + tolerance > b) && (a - tolerance < b);
}

//! returns if a float equals zero, taking floating 
//! point rounding errors into account
inline bool iszero(const f32 a, const f32 tolerance = ROUNDING_ERROR_32)
{
	return fabs ( a ) < tolerance;
}

inline s32 s32_min ( s32 a, s32 b)
{
	s32 mask = (a - b) >> 31;
	return (a & mask) | (b & ~mask);
}

inline s32 s32_max ( s32 a, s32 b)
{
	s32 mask = (a - b) >> 31;
	return (b & mask) | (a & ~mask);
}

inline s32 s32_clamp (s32 value, s32 low, s32 high) 
{
	return s32_min (s32_max(value,low), high);
}

/* 

float IEEE-754 bit represenation

0      0x00000000
1.0    0x3f800000
0.5    0x3f000000
3      0x40400000
+inf   0x7f800000
-inf   0xff800000
+NaN   0x7fc00000 or 0x7ff00000
in general: number = (sign ? -1:1) * 2^(exponent) * 1.(mantissa bits)
*/

#define F32_AS_S32(f)			(*((s32 *) &(f)))
#define F32_AS_U32(f)			(*((u32 *) &(f)))
#define F32_AS_U32_POINTER(f)	( ((u32 *) &(f)))

#define F32_VALUE_0			0x00000000
#define F32_VALUE_1			0x3f800000	
#define F32_SIGN_BIT			0x80000000U
#define F32_EXPON_MANTISSA		0x7FFFFFFFU

//! code is taken from IceFPU
//! Integer representation of a floating-point value.
#define IR(x)					((u32&)(x))

//! Absolute integer representation of a floating-point value
#define AIR(x)					(IR(x)&0x7fffffff)

//! Floating-point representation of an integer value.
#define FR(x)					((f32&)(x))

#define IEEE_1_0			0x3f800000						//!<	integer representation of 1.0
#define IEEE_255_0			0x437f0000						//!<	integer representation of 255.0


#define	F32_LOWER_0(f)			(F32_AS_U32(f) >  F32_SIGN_BIT)
#define	F32_LOWER_EQUAL_0(f)	(F32_AS_S32(f) <= F32_VALUE_0)
#define	F32_GREATER_0(f)		(F32_AS_S32(f) >  F32_VALUE_0)
#define	F32_GREATER_EQUAL_0(f)	(F32_AS_U32(f) <= F32_SIGN_BIT)
#define	F32_EQUAL_1(f)			(F32_AS_U32(f) == F32_VALUE_1)
#define	F32_EQUAL_0(f)			( (F32_AS_U32(f) & F32_EXPON_MANTISSA ) == F32_VALUE_0)

// only same sign
#define	F32_A_GREATER_B(a,b)	(F32_AS_S32((a)) >  F32_AS_S32((b)))


#ifndef REALINLINE
#ifdef _MSC_VER
#define REALINLINE __forceinline
#else
#define REALINLINE inline
#endif
#endif


//! conditional set based on mask and arithmetic shift
REALINLINE u32 if_c_a_else_b ( const s32 condition, const u32 a, const u32 b )
{
	return ( ( -condition >> 31 ) & ( a ^ b ) ) ^ b;
}

//! conditional set based on mask and arithmetic shift
REALINLINE u32 if_c_a_else_0 ( const s32 condition, const u32 a )
{
	return ( -condition >> 31 ) & a;
}

/*
if (condition) state |= m; else state &= ~m; 
*/
REALINLINE void setbit ( u32 &state, s32 condition, u32 mask )
{
	// 0, or any postive to mask
	//s32 conmask = -condition >> 31;
	state ^= ( ( -condition >> 31 ) ^ state ) & mask;
}


REALINLINE void clearFPUException ()
{
}


inline f32 reciprocal_squareroot(const f32 x)
{
	return 1.f / sqrtf ( x );
}


inline f32 reciprocal ( const f32 x )
{
	return 1.f / x;
}

inline f32 reciprocal_approxim ( const f32 x )
{
	return 1.f / x;
}


inline s32 floor32 ( f32 x )
{
	return (s32) floorf ( x );
}

inline s32 ceil32 ( f32 x )
{
	return (s32) ceilf ( x );
}

inline s32 round32 ( f32 x )
{
	return (s32) ( x + 0.5f );
}


inline f32 f32_max3(const f32 a, const f32 b, const f32 c)
{
	return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

inline f32 f32_min3(const f32 a, const f32 b, const f32 c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}


inline f32 fract ( f32 x )
{
	return x - floorf ( x );
}

inline f32 round ( f32 x )
{
	return floorf ( x + 0.5f );
}


} // end namespace Pirate

#endif
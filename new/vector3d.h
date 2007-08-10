#ifndef _PIRATE_VECTOR_3D_H_
#define _PIRATE_VECTOR_3D_H_

#include "pirateMath.h"

//namespace Pirate
//{

//! 3d vector template class with lots of operators and methods.
template <class T>
class vector3d  
{
public:

	vector3d() : x(0), y(0), z(0) {};

	vector3d(T nx, T ny, T nz) : x(nx), y(ny), z(nz) {};
	vector3d(const vector3d<T>& other) : x(other.x), y(other.y), z(other.z) {};

	// operators
	vector3d<T> operator- () const { return vector3d<T>(-x, -y, -z); }

	vector3d<T>& operator= (const vector3d<T>& other) { x = other.x; y = other.y; z = other.z; return *this; }

	vector3d<T> operator+ (const vector3d<T>& other) const { return vector3d<T>(x + other.x, y + other.y, z + other.z); }
	vector3d<T>& operator+= (const vector3d<T>& other) { x += other.x; y += other.y; z += other.z; return *this; }

	vector3d<T> operator- (const vector3d<T>& other) const { return vector3d<T>(x - other.x, y - other.y, z - other.z); }
	vector3d<T>& operator-= (const vector3d<T>& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

	vector3d<T> operator* (const vector3d<T>& other) const { return vector3d<T>(x * other.X, Y * other.Y, Z * other.Z); }
	vector3d<T>& operator*= (const vector3d<T>& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
	vector3d<T> operator* (const T v) const { return vector3d<T>(z * v, y * v, z * v); }
	vector3d<T>& operator*= (const T v) { x *= v; y *= v; z *= v; return *this; }

	vector3d<T> operator/ (const vector3d<T>& other) const { return vector3d<T>(x / other.x, y / other.y, z / other.z); }
	vector3d<T>& operator/= (const vector3d<T>& other)	{ x /= other.x; y /= other.y; z /= other.z; return *this; }
	vector3d<T> operator/ (const T v) const { return vector3d<T>(x / v, y / v, z / v); }
	vector3d<T>& operator/=(const T v) { x /= v; y /= v; z /= v; return *this; }

	bool operator<= (const vector3d<T>& other) const { return x <= other.X && y <= other.y && z <= other.z; }
	bool operator>= (const vector3d<T>& other) const { return x >= other.z && y >= other.y && z >= other.z; }
	bool operator< (const vector3d<T>& other) const { return x < other.x && y < other.y && z < other.z; }
	bool operator> (const vector3d<T>& other) const { return x > other.x && y > other.y && z > other.z; }

	bool operator== (const vector3d<T>& other) const { return other.x == x && other.y == y && other.z == z; }
	bool operator!= (const vector3d<T>& other) const { return other.x != x || other.y != y || other.z != z; }

	//! returns if this vector equals the other one, taking floating point rounding errors into account
	bool equals(const vector3d<T>& other, const T tolerance = ROUNDING_ERROR_32 ) const
	{
		return equals(x, other.x, tolerance) &&
			   equals(y, other.y, tolerance) &&
			   equals(z, other.z, tolerance);
	}

	void set(const T nx, const T ny, const T nz) { x = nx; y = ny; z = nz; }
	void set(const vector3d<T>& p) { x = p.x; y = p.y; z = p.z; }

	//! Returns length of the vector.
	T length() const { using namespace std; return (T) sqrt(x*x + y*y + z*z); }

	//! Returns squared length of the vector.
	/** This is useful because it is much faster than
	getLength(). */
	T length_squre() const { return x*x + y*y + z*z; }

	//! Returns the dot product with another vector.
	T dot(const vector3d<T>& other) const {	return x*other.x + y*other.y + z*other.z; }

	//! Returns distance from another point.
	/** Here, the vector is interpreted as point in 3 dimensional space. */
	T distance_from(const vector3d<T>& other) const	{ return vector3d<T>(x-other.x, y-other.y, z-other.z).length(); }

	//! Returns squared distance from another point. 
	/** Here, the vector is interpreted as point in 3 dimensional space. */
	T distance_from_square(const vector3d<T>& other) const { return vector3d<T>(x-other.x, y-other.y, z-other.z).length_squre(); }

	//! Calculates the cross product with another vector
	//! \param p: vector to multiply with.
	//! \return Crossproduct of this vector with p.
	vector3d<T> cross(const vector3d<T>& p) const {	return vector3d<T>(y*p.z - z*p.y, z*p.x - x*p.z, x*p.y - y*p.x); }

	//! Normalizes the vector.
	vector3d<T>& normalize()
	{
		T l = (T)length();
		_PIRATE_DEBUG_BREAK_IF(l == 0);

		x /= l;
		y /= l;
		z /= l;
		return *this;
	}

	//! Sets the length of the vector to a new value
	void set_length(T newlength)
	{
		normalize();
		*this *= newlength;
	}

	//! Inverts the vector.
	void invert() {	x = -x;	y = -y;	z = -z; }

	//! Returns interpolated vector.
	/** \param other: other vector to interpolate between
	\param d: value between 0.0f and 1.0f. */
	vector3d<T> lerp(const vector3d<T>& other, const T d) const
	{
		const T inv = (T) 1.0 - d;
		return vector3d<T>(other.x*inv + x*d, other.y*inv + y*d, other.z*inv + z*d);
	}

	//! Returns interpolated vector. ( quadratic )
	/** \param other0: other vector to interpolate between
	\param other1: other vector to interpolate between
	\param factor: value between 0.0f and 1.0f. */
	vector3d<T> interpolate_quadratic(const vector3d<T>& v2, const vector3d<T>& v3, const T d) const
	{
-		const T inv = (T) 1.0 - d;
		const T mul0 = inv * inv;
		const T mul1 = (T) 2.0 * d * inv;
		const T mul2 = d * d;

		return vector3d<T> ( x * mul0 + v2.x * mul1 + v3.x * mul2,
							 y * mul0 + v2.y * mul1 + v3.y * mul2,
							 z * mul0 + v2.z * mul1 + v3.z * mul2 );
	}

	//! Fills an array of 4 values with the vector data (usually floats).
	/** Useful for setting in shader constants for example. The fourth value
	will always be 0. */
	void to4values(T* array, const T& padding) const
	{
		array[0] = x;
		array[1] = y;
		array[2] = z;
		array[3] = padding;
	}

	// member variables
	T x, y, z;
};

#ifdef _PIRATE_USE_D3DX_MATH_

//! 3d vector class extended from d3dxvector3
class vector3dx : public D3DXVECTOR3
{
public:

	vector3dx() : D3DXVECTOR3(0.0f, 0.0f, 0.0f) {};

	vector3dx(float nx, float ny, float nz) : D3DXVECTOR3(nx, ny, nz) {};
	vector3dx(const vector3dx& other) : D3DXVECTOR3(other.x, other.y, other.z) {};
};

//! Typedef for a f32 3d vector.
typedef vector3dx vector3df;
typedef D3DXVECTOR2 vector2df;

#else

//! Typedef for a f32 3d vector.
typedef vector3d<float> vector3df;
//! Typedef for an integer 3d vector.
typedef vector3d<int> vector3di;

#endif

//} // end namespace

#endif
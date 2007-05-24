#ifndef _PIRATE_MDL_HELPER_H_
#define _PIRATE_MDL_HELPER_H_

#include "FileSystem.h"
#include "os.h"
#include <xmmintrin.h>

#ifndef offsetof
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

using namespace Pirate;

typedef float vec_t;

class Vector					
{
public:
	// Members
	vec_t x, y, z;
	inline Vector::Vector(void)									
	{ 
		// Initialize to NAN to catch errors
		x = y = z = 0.0f;
	}
	inline Vector::Vector(vec_t X, vec_t Y, vec_t Z)						
	{ 
		x = X; y = Y; z = Z;
	}
};

class Quaternion;

class RadianEuler
{
public:
	inline RadianEuler(void)							{ }
	inline RadianEuler(vec_t X, vec_t Y, vec_t Z)		{ x = X; y = Y; z = Z; }
	inline RadianEuler(Quaternion const &q);	// evil auto type promotion!!!

	// Initialization
	inline void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f)	{ x = ix; y = iy; z = iz; }

	bool IsValid() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z;
};

class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	inline Quaternion(void)	{ 

		// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
		x = y = z = w = VEC_T_NAN;
#endif
#endif
	}
	inline Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline Quaternion(RadianEuler const &angle);	// evil auto type promotion!!!

	inline void Init(vec_t ix=0.0f, vec_t iy=0.0f, vec_t iz=0.0f, vec_t iw=0.0f)	{ x = ix; y = iy; z = iz; w = iw; }

	bool IsValid() const;

	bool operator==( const Quaternion &src ) const;
	bool operator!=( const Quaternion &src ) const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z, w;
};

struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t( 
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	float *operator[]( int i )				{ assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	const float *operator[]( int i ) const	{ assert(( i >= 0 ) && ( i < 3 )); return m_flMatVal[i]; }
	float *Base()							{ return &m_flMatVal[0][0]; }
	const float *Base() const				{ return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

// bones
struct mstudiobone_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void *pProcedure( ) const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropidx; }
	int					contents;		// See BSPFlags.h for the contents flags

	int					unused[8];		// remove as appropriate

private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};

// bone controllers
struct mstudiobonecontroller_t
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					inputfield;	// 0-3 user set controller, 4 mouth
	int					unused[8];
};

// intersection boxes
struct mstudiobbox_t
{
	int					bone;
	int					group;				// intersection group
	Vector				bbmin;				// bounding box
	Vector				bbmax;	
	int					szhitboxnameindex;	// offset to the name of the hitbox.
	int					unused[8];

	char* pszHitboxName()
	{
		if( szhitboxnameindex == 0 )
			return "";

		return ((char*)this) + szhitboxnameindex;
	}

	mstudiobbox_t() {}

private:
	// No copy constructors allowed
	mstudiobbox_t(const mstudiobbox_t& vOther);
};

struct mstudiohitboxset_t
{
	int					sznameindex;
	inline char * const	pszName( void ) const { return ((char *)this) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t *pHitbox( int i ) const { return (mstudiobbox_t *)(((byte *)this) + hitboxindex) + i; };
};

struct studiohdr_t;
struct mstudioanim_t;
struct mstudioikrule_t;

struct mstudiomovement_t
{
	int					endframe;				
	int					motionflags;
	float				v0;			// velocity at start of block
	float				v1;			// velocity at end of block
	float				angle;		// YAW rotation at end of this blocks movement
	Vector				vector;		// movement vector relative to this blocks initial angle
	Vector				position;	// relative to start of animation???

private:
	// No copy constructors allowed
	mstudiomovement_t(const mstudiomovement_t& vOther);
};

struct mstudioanimdesc_t
{
	int					baseptr;
	inline studiohdr_t	*pStudiohdr( void ) const { return (studiohdr_t *)(((byte *)this) + baseptr); }

	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					numframes;

	// piecewise movement
	int					nummovements;
	int					movementindex;
	inline mstudiomovement_t * const pMovement( int i ) const { return (mstudiomovement_t *)(((byte *)this) + movementindex) + i; };

	Vector				unused1;		// per animation bounding box
	Vector				unused2;		

	int					animblock;
	int					animindex;
	mstudioanim_t *pAnim( void ) const;

	int					numikrules;
	int					ikruleindex;	// non-zero when IK data is stored in the mdl
	int					animblockikruleindex; // non-zero when IK data is stored in animblock file
	mstudioikrule_t *pIKRule( int i ) const;


	int					unused[7];		// remove as appropriate

private:
	// No copy constructors allowed
	mstudioanimdesc_t(const mstudioanimdesc_t& vOther);
};

// events
struct mstudioevent_t
{
	float				cycle;
	int					event;
	int					type;
	inline const char * pszOptions( void ) const { return options; }
	char				options[64];

	int					szeventindex;
	inline char * const pszEventName( void ) const { return ((char *)this) + szeventindex; }
};

struct mstudioautolayer_t
{
	//private:
	short				iSequence;
	short				iPose;
	//public:
	int					flags;
	float				start;	// beginning of influence
	float				peak;	// start of full influence
	float				tail;	// end of full influence
	float				end;	// end of all influence
};

struct mstudioiklock_t
{
	int			chain;
	float		flPosWeight;
	float		flLocalQWeight;
	int			flags;

	int			unused[4];
};

// sequence descriptions
struct mstudioseqdesc_t
{
	int					baseptr;
	inline studiohdr_t	*pStudiohdr( void ) const { return (studiohdr_t *)(((byte *)this) + baseptr); }

	int					szlabelindex;
	inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }

	int					szactivitynameindex;
	inline char * const pszActivityName( void ) const { return ((char *)this) + szactivitynameindex; }

	int					flags;		// looping/non-looping flags

	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;

	int					numevents;
	int					eventindex;
	inline mstudioevent_t *pEvent( int i ) const { assert( i >= 0 && i < numevents); return (mstudioevent_t *)(((byte *)this) + eventindex) + i; };

	Vector				bbmin;		// per sequence bounding box
	Vector				bbmax;		

	int					numblends;

	// Index into array of shorts which is groupsize[0] x groupsize[1] in length
	int					animindexindex;

	inline int			anim( int x, int y ) const
	{
		if ( x >= groupsize[0] )
		{
			x = groupsize[0] - 1;
		}

		if ( y >= groupsize[1] )
		{
			y = groupsize[ 1 ] - 1;
		}

		int offset = y * groupsize[0] + x;
		short *blends = (short *)(((byte *)this) + animindexindex);
		int value = (int)blends[ offset ];
		return value;
	}

	int					movementindex;	// [blend] float array for blended movement
	int					groupsize[2];
	int					paramindex[2];	// X, Y, Z, XR, YR, ZR
	float				paramstart[2];	// local (0..1) starting value
	float				paramend[2];	// local (0..1) ending value
	int					paramparent;

	float				fadeintime;		// ideal cross fate in time (0.2 default)
	float				fadeouttime;	// ideal cross fade out time (0.2 default)

	int					localentrynode;		// transition node at entry
	int					localexitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	float				entryphase;		// used to match entry gait
	float				exitphase;		// used to match exit gait

	float				lastframe;		// frame that should generation EndOfSequence

	int					nextseq;		// auto advancing sequences
	int					pose;			// index of delta animation between end and nextseq

	int					numikrules;

	int					numautolayers;	//
	int					autolayerindex;
	inline mstudioautolayer_t *pAutolayer( int i ) const { assert( i >= 0 && i < numautolayers); return (mstudioautolayer_t *)(((byte *)this) + autolayerindex) + i; };

	int					weightlistindex;
	inline float		*pBoneweight( int i ) const { return ((float *)(((byte *)this) + weightlistindex) + i); };
	inline float		weight( int i ) const { return *(pBoneweight( i)); };

	// FIXME: make this 2D instead of 2x1D arrays
	int					posekeyindex;
	float				*pPoseKey( int iParam, int iAnim ) const { return (float *)(((byte *)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
	float				poseKey( int iParam, int iAnim ) const { return *(pPoseKey( iParam, iAnim )); }

	int					numiklocks;
	int					iklockindex;
	inline mstudioiklock_t *pIKLock( int i ) const { assert( i >= 0 && i < numiklocks); return (mstudioiklock_t *)(((byte *)this) + iklockindex) + i; };

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int					unused[8];		// remove/add as appropriate (grow back to 8 ints on version change!)

private:
	// No copy constructors allowed
	mstudioseqdesc_t(const mstudioseqdesc_t& vOther);
};

class IMaterial;

// skin info
struct mstudiotexture_t
{
	int						sznameindex;
	inline char * const		pszName( void ) const { return ((char *)this) + sznameindex; }
	int						flags;
	int						used;
	int						unused1;
	mutable IMaterial		*material;  // fixme: this needs to go away . .isn't used by the engine, but is used by studiomdl
	mutable void			*clientmaterial;	// gary, replace with client material pointer if used

	int						unused[10];
};

class Vector4D					
{
public:
	// Members
	vec_t x, y, z, w;
};

class Vector2D					
{
public:
	// Members
	vec_t x, y;
};

#define MAX_NUM_BONES_PER_VERT 3
// 16 bytes
struct mstudioboneweight_t
{
	float	weight[MAX_NUM_BONES_PER_VERT];
	char	bone[MAX_NUM_BONES_PER_VERT]; 
	byte	numbones;

	//	byte	material;
	//	short	firstref;
	//	short	lastref;
};

// NOTE: This is exactly 48 bytes
struct mstudiovertex_t
{
	mstudioboneweight_t	m_BoneWeights;
	Vector				m_vecPosition;
	Vector				m_vecNormal;
	Vector2D			m_vecTexCoord;

	mstudiovertex_t() {}

private:
	// No copy constructors allowed
	mstudiovertex_t(const mstudiovertex_t& vOther);
};

struct mstudio_modelvertexdata_t
{
	Vector				*Position( int i ) const;
	Vector				*Normal( int i ) const;
	Vector4D			*TangentS( int i ) const;
	Vector2D			*Texcoord( int i ) const;
	mstudioboneweight_t	*BoneWeights( int i ) const;
	mstudiovertex_t		*Vertex( int i ) const;
	bool				HasTangentData( void ) const;

	// base of external vertex data stores
	void				*pVertexData;
	void				*pTangentData;
};

struct mstudiomodel_t;
#define MAX_NUM_LODS 8

struct mstudio_meshvertexdata_t
{
	Vector				*Position( int i ) const;
	Vector				*Normal( int i ) const;
	Vector4D			*TangentS( int i ) const;
	Vector2D			*Texcoord( int i ) const;
	mstudioboneweight_t *BoneWeights( int i ) const;
	mstudiovertex_t		*Vertex( int i ) const;
	bool				HasTangentData( void ) const;

	// indirection to this mesh's model's vertex data
	const mstudio_modelvertexdata_t	*modelvertexdata;

	// used for fixup calcs when culling top level lods
	// expected number of mesh verts at desired lod
	int					numLODVertexes[MAX_NUM_LODS];
};

const int float32bias = 127;
const int float16bias = 15;

const float maxfloat16bits = 65504.0f;

class float16
{
public:
	//float16() {}
	//float16( float f ) { m_storage.rawWord = ConvertFloatTo16bits(f); }

	void Init() { m_storage.rawWord = 0; }
	//	float16& operator=(const float16 &other) { m_storage.rawWord = other.m_storage.rawWord; return *this; }
	//	float16& operator=(const float &other) { m_storage.rawWord = ConvertFloatTo16bits(other); return *this; }
	//	operator unsigned short () { return m_storage.rawWord; }
	//	operator float () { return Convert16bitFloatTo32bits( m_storage.rawWord ); }
	unsigned short GetBits() const 
	{ 
		return m_storage.rawWord; 
	}
	float GetFloat() const 
	{ 
		return Convert16bitFloatTo32bits( m_storage.rawWord ); 
	}
	void SetFloat( float in ) 
	{ 
		m_storage.rawWord = ConvertFloatTo16bits( in ); 
	}

	bool IsInfinity() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa == 0;
	}
	bool IsNaN() const
	{
		return m_storage.bits.biased_exponent == 31 && m_storage.bits.mantissa != 0;
	}

	bool operator==(const float16 other) const { return m_storage.rawWord == other.m_storage.rawWord; }
	bool operator!=(const float16 other) const { return m_storage.rawWord != other.m_storage.rawWord; }

	//	bool operator< (const float other) const	   { return GetFloat() < other; }
	//	bool operator> (const float other) const	   { return GetFloat() > other; }

protected:
	union float32bits
	{
		float rawFloat;
		struct 
		{
			unsigned int mantissa : 23;
			unsigned int biased_exponent : 8;
			unsigned int sign : 1;
		} bits;
	};

	union float16bits
	{
		unsigned short rawWord;
		struct
		{
			unsigned short mantissa : 10;
			unsigned short biased_exponent : 5;
			unsigned short sign : 1;
		} bits;
	};

	static bool IsNaN( float16bits in )
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa != 0;
	}
	static bool IsInfinity( float16bits in )
	{
		return in.bits.biased_exponent == 31 && in.bits.mantissa == 0;
	}

	// 0x0001 - 0x03ff
	static unsigned short ConvertFloatTo16bits( float input )
	{
		if ( input > maxfloat16bits )
			input = maxfloat16bits;
		else if ( input < -maxfloat16bits )
			input = -maxfloat16bits;

		float16bits output;
		float32bits inFloat;

		inFloat.rawFloat = input;

		output.bits.sign = inFloat.bits.sign;

		if ( (inFloat.bits.biased_exponent==0) && (inFloat.bits.mantissa==0) ) 
		{ 
			// zero
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
		}
		else if ( (inFloat.bits.biased_exponent==0) && (inFloat.bits.mantissa!=0) ) 
		{  
			// denorm -- denorm float maps to 0 half
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
		}
		else if ( (inFloat.bits.biased_exponent==0xff) && (inFloat.bits.mantissa==0) ) 
		{ 
#if 0
			// infinity
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 31;
#else
			// infinity maps to maxfloat
			output.bits.mantissa = 0x3ff;
			output.bits.biased_exponent = 0x1e;
#endif
		}
		else if ( (inFloat.bits.biased_exponent==0xff) && (inFloat.bits.mantissa!=0) ) 
		{ 
#if 0
			// NaN
			output.bits.mantissa = 1;
			output.bits.biased_exponent = 31;
#else
			// NaN maps to zero
			output.bits.mantissa = 0;
			output.bits.biased_exponent = 0;
#endif
		}
		else 
		{ 
			// regular number
			int new_exp = inFloat.bits.biased_exponent-127;

			if (new_exp<-24) 
			{ 
				// this maps to 0
				output.bits.mantissa = 0;
				output.bits.biased_exponent = 0;
			}

			if (new_exp<-14) 
			{
				// this maps to a denorm
				output.bits.biased_exponent = 0;
				unsigned int exp_val = ( unsigned int )( -14 - ( inFloat.bits.biased_exponent - float32bias ) );
				if( exp_val > 0 && exp_val < 11 )
				{
					output.bits.mantissa = ( 1 << ( 10 - exp_val ) ) + ( inFloat.bits.mantissa >> ( 13 + exp_val ) );
				}
			}
			else if (new_exp>15) 
			{ 
#if 0
				// map this value to infinity
				output.bits.mantissa = 0;
				output.bits.biased_exponent = 31;
#else
				// to big. . . maps to maxfloat
				output.bits.mantissa = 0x3ff;
				output.bits.biased_exponent = 0x1e;
#endif
			}
			else 
			{
				output.bits.biased_exponent = new_exp+15;
				output.bits.mantissa = (inFloat.bits.mantissa >> 13);
			}
		}
		return output.rawWord;
	}

	static float Convert16bitFloatTo32bits( unsigned short input )
	{
		float32bits output;
		float16bits inFloat;
		inFloat.rawWord = input;

		if( IsInfinity( inFloat ) )
		{
			return maxfloat16bits * ( ( inFloat.bits.sign == 1 ) ? -1.0f : 1.0f );
		}
		if( IsNaN( inFloat ) )
		{
			return 0.0;
		}
		if( inFloat.bits.biased_exponent == 0 && inFloat.bits.mantissa != 0 )
		{
			// denorm
			const float half_denorm = (1.0f/16384.0f); // 2^-14
			float mantissa = ((float)(inFloat.bits.mantissa)) / 1024.0f;
			float sgn = (inFloat.bits.sign)? -1.0f :1.0f;
			output.rawFloat = sgn*mantissa*half_denorm;
		}
		else
		{
			// regular number
			output.bits.mantissa = inFloat.bits.mantissa << (23-10);
			output.bits.biased_exponent = (inFloat.bits.biased_exponent - float16bias + float32bias) * (inFloat.bits.biased_exponent != 0);
			output.bits.sign = inFloat.bits.sign;
		}

		return output.rawFloat;
	}


	float16bits m_storage;
};

class __declspec(align(16)) Vector4DAligned : public Vector4D
{
public:
	Vector4DAligned(void) {}
	Vector4DAligned( vec_t X, vec_t Y, vec_t Z, vec_t W );

	inline void Set( vec_t X, vec_t Y, vec_t Z, vec_t W );

	inline void InitZero( void );

	__m128 &AsM128() { return *(__m128*)&x; }
	const __m128 &AsM128() const { return *(const __m128*)&x; } 

private:
	// No copy constructors allowed if we're in optimal mode
	Vector4DAligned(Vector4DAligned const& vOther);

	// No assignment operators either...
	Vector4DAligned& operator=( Vector4DAligned const& src );
};

const float g_VertAnimFixedPointScale = 1.0f / 4096.0f;
const float g_VertAnimFixedPointScaleInv = 1.0f / g_VertAnimFixedPointScale;

// this is the memory image of vertex anims (16-bit fixed point)
struct mstudiovertanim_t
{
	short				index;
	byte				speed;	// 255/max_length_in_flex
	byte				side;	// 255/left_right

private:
	// JasonM changing this type a lot, to prefer fixed point 16 bit...
	union
	{
		short			delta[3];
		float16			flDelta[3];
	};

	union
	{
		short			ndelta[3];
		float16			flNDelta[3];
	};

public:
	inline Vector GetDeltaFixed()
	{
		return Vector( delta[0]*g_VertAnimFixedPointScale, delta[1]*g_VertAnimFixedPointScale, delta[2]*g_VertAnimFixedPointScale );
	}
	inline Vector GetNDeltaFixed()
	{
		return Vector( ndelta[0]*g_VertAnimFixedPointScale, ndelta[1]*g_VertAnimFixedPointScale, ndelta[2]*g_VertAnimFixedPointScale );
	}
	inline void GetDeltaFixed4DAligned( Vector4DAligned *vFillIn )
	{
		vFillIn->Set( delta[0]*g_VertAnimFixedPointScale, delta[1]*g_VertAnimFixedPointScale, delta[2]*g_VertAnimFixedPointScale, 0.0f );
	}
	inline void GetNDeltaFixed4DAligned( Vector4DAligned *vFillIn )
	{
		vFillIn->Set( ndelta[0]*g_VertAnimFixedPointScale, ndelta[1]*g_VertAnimFixedPointScale, ndelta[2]*g_VertAnimFixedPointScale, 0.0f );
	}
	inline Vector GetDeltaFloat()
	{
		return Vector (flDelta[0].GetFloat(), flDelta[1].GetFloat(), flDelta[2].GetFloat());
	}
	inline Vector GetNDeltaFloat()
	{
		return Vector (flNDelta[0].GetFloat(), flNDelta[1].GetFloat(), flNDelta[2].GetFloat());
	}


private:
	// No copy constructors allowed
	mstudiovertanim_t(const mstudiovertanim_t& vOther);
};

struct mstudioflex_t
{
	int					flexdesc;	// input value

	float				target0;	// zero
	float				target1;	// one
	float				target2;	// one
	float				target3;	// zero

	int					numverts;
	int					vertindex;
	inline	mstudiovertanim_t *pVertanim( int i ) const { return  (mstudiovertanim_t *)(((byte *)this) + vertindex) + i; };

	int					flexpair;	// second flex desc
	int					unused[7];
};

struct mstudiomesh_t
{
	int					material;

	int					modelindex;
	mstudiomodel_t *pModel() const; 

	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexoffset;		// vertex mstudiovertex_t

	const mstudio_meshvertexdata_t *GetVertexData();

	int					numflexes;			// vertex animation
	int					flexindex;
	inline mstudioflex_t *pFlex( int i ) const { return (mstudioflex_t *)(((byte *)this) + flexindex) + i; };

	// special codes for material operations
	int					materialtype;
	int					materialparam;

	// a unique ordinal for this mesh
	int					meshid;

	Vector				center;

	mstudio_meshvertexdata_t vertexdata;

	int					unused[8]; // remove as appropriate

private:
	// No copy constructors allowed
	mstudiomesh_t(const mstudiomesh_t& vOther);
};

// eyeball
struct mstudioeyeball_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int		bone;
	Vector	org;
	float	zoffset;
	float	radius;
	Vector	up;
	Vector	forward;
	int		texture;

	int		iris_material;
	float	iris_scale;
	int		glint_material;	// !!!

	int		upperflexdesc[3];	// index of raiser, neutral, and lowerer flexdesc that is set by flex controllers
	int		lowerflexdesc[3];
	float	uppertarget[3];		// angle (radians) of raised, neutral, and lowered lid positions
	float	lowertarget[3];

	int		upperlidflexdesc;	// index of flex desc that actual lid flexes look to
	int		lowerlidflexdesc;
	int		unused[12];

private:
	// No copy constructors allowed
	mstudioeyeball_t(const mstudioeyeball_t& vOther);
};

// studio models
struct mstudiomodel_t
{
	inline const char * pszName( void ) const { return name; }
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummeshes;	
	int					meshindex;
	inline mstudiomesh_t *pMesh( int i ) const { return (mstudiomesh_t *)(((byte *)this) + meshindex) + i; };

	// cache purposes
	int					numvertices;		// number of unique vertices/normals/texcoords
	int					vertexindex;		// vertex Vector
	int					tangentsindex;		// tangents Vector

	const mstudio_modelvertexdata_t *GetVertexData();

	int					numattachments;
	int					attachmentindex;

	int					numeyeballs;
	int					eyeballindex;
	inline  mstudioeyeball_t *pEyeball( int i ) { return (mstudioeyeball_t *)(((byte *)this) + eyeballindex) + i; };

	mstudio_modelvertexdata_t vertexdata;

	int					unused[8];		// remove as appropriate
};

inline bool mstudio_modelvertexdata_t::HasTangentData( void ) const 
{
	return (pTangentData != NULL);
}

inline mstudiovertex_t *mstudio_modelvertexdata_t::Vertex( int i ) const 
{ 
	mstudiomodel_t *modelptr = (mstudiomodel_t *)((byte *)this - offsetof(mstudiomodel_t,vertexdata)); 	
	return (mstudiovertex_t *)((byte *)pVertexData + modelptr->vertexindex) + i;
}

inline Vector *mstudio_modelvertexdata_t::Position( int i ) const 
{
	return &Vertex(i)->m_vecPosition;
}

inline Vector *mstudio_modelvertexdata_t::Normal( int i ) const 
{ 
	return &Vertex(i)->m_vecNormal;
}

inline Vector4D *mstudio_modelvertexdata_t::TangentS( int i ) const 
{
	// NOTE: The tangents vector is 16-bytes in a separate array
	// because it only exists on the high end, and if I leave it out
	// of the mstudiovertex_t, the vertex is 64-bytes (good for low end)
	mstudiomodel_t *modelptr = (mstudiomodel_t *)((byte *)this - offsetof(mstudiomodel_t,vertexdata)); 
	return (Vector4D *)((byte *)pTangentData + modelptr->tangentsindex) + i;
}

inline Vector2D *mstudio_modelvertexdata_t::Texcoord( int i ) const 
{ 
	return &Vertex(i)->m_vecTexCoord;
}

inline mstudioboneweight_t *mstudio_modelvertexdata_t::BoneWeights( int i ) const 
{
	return &Vertex(i)->m_BoneWeights;
}

inline mstudiomodel_t *mstudiomesh_t::pModel() const 
{ 
	return (mstudiomodel_t *)(((byte *)this) + modelindex); 
}

inline bool mstudio_meshvertexdata_t::HasTangentData( void ) const
{
	return modelvertexdata->HasTangentData();
}

inline const mstudio_meshvertexdata_t *mstudiomesh_t::GetVertexData()
{
	// get this mesh's model's vertex data
	vertexdata.modelvertexdata = this->pModel()->GetVertexData();

	return &vertexdata;
}

inline Vector *mstudio_meshvertexdata_t::Position( int i ) const 
{  
	mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
	return modelvertexdata->Position( meshptr->vertexoffset + i ); 
};

inline Vector *mstudio_meshvertexdata_t::Normal( int i ) const 
{ 
	mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
	return modelvertexdata->Normal( meshptr->vertexoffset + i ); 
};

inline Vector4D *mstudio_meshvertexdata_t::TangentS( int i ) const
{
	mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
	return modelvertexdata->TangentS( meshptr->vertexoffset + i ); 
}

inline Vector2D *mstudio_meshvertexdata_t::Texcoord( int i ) const 
{ 
	mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
	return modelvertexdata->Texcoord( meshptr->vertexoffset + i ); 
};

inline mstudioboneweight_t *mstudio_meshvertexdata_t::BoneWeights( int i ) const 
{ 
	mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
	return modelvertexdata->BoneWeights( meshptr->vertexoffset + i ); 
};

inline mstudiovertex_t *mstudio_meshvertexdata_t::Vertex( int i ) const
{
	mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
	return modelvertexdata->Vertex( meshptr->vertexoffset + i );
}

// body part index
struct mstudiobodyparts_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
	inline mstudiomodel_t *pModel( int i ) const { return (mstudiomodel_t *)(((byte *)this) + modelindex) + i; };
};

// attachment
struct mstudioattachment_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	unsigned int		flags;
	int					localbone;
	matrix3x4_t			local; // attachment point
	int					unused[8];
};

struct mstudioflexdesc_t
{
	int					szFACSindex;
	inline char * const pszFACS( void ) const { return ((char *)this) + szFACSindex; }
};

struct mstudioflexcontroller_t
{
	int					sztypeindex;
	inline char * const pszType( void ) const { return ((char *)this) + sztypeindex; }
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	mutable int			link;	// remapped at load time to master list
	float				min;
	float				max;
};

struct mstudioflexop_t
{
	int		op;
	union 
	{
		int		index;
		float	value;
	} d;
};

struct mstudioflexrule_t
{
	int					flex;
	int					numops;
	int					opindex;
	inline mstudioflexop_t *iFlexOp( int i ) const { return  (mstudioflexop_t *)(((byte *)this) + opindex) + i; };
};

// ikinfo
struct mstudioiklink_t
{
	int		bone;
	Vector	kneeDir;	// ideal bending direction (per link, if applicable)
	Vector	unused0;	// unused

private:
	// No copy constructors allowed
	mstudioiklink_t(const mstudioiklink_t& vOther);
};

struct mstudioikchain_t
{
	int				sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int				linktype;
	int				numlinks;
	int				linkindex;
	inline mstudioiklink_t *pLink( int i ) const { return (mstudioiklink_t *)(((byte *)this) + linkindex) + i; };
	// FIXME: add unused entries
};

struct mstudiomouth_t
{
	int					bone;
	Vector				forward;
	int					flexdesc;

private:
	// No copy constructors allowed
	mstudiomouth_t(const mstudiomouth_t& vOther);
};

struct mstudioposeparamdesc_t
{
	int					sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int					flags;	// ????
	float				start;	// starting value
	float				end;	// ending value
	float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};

// demand loaded sequence groups
struct mstudiomodelgroup_t
{
	int					szlabelindex;	// textual name
	inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }
	int					sznameindex;	// file name
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
};

// used for piecewise loading of animation data
struct mstudioanimblock_t
{
	int					datastart;
	int					dataend;
};

struct mstudioiklock_t;
struct mstudiomodelgroup_t;
struct virtualmodel_t;

struct studiohdr_t
{
	int					id;
	int					version;

	long				checksum;		// this has to be the same in the phy and vtx files to load!

	inline const char *	pszName( void ) const { return name; }
	char				name[64];
	int					length;


	Vector				eyeposition;	// ideal eye position

	Vector				illumposition;	// illumination center

	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;			

	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;		

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	inline mstudiobone_t *pBone( int i ) const { assert( i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
	int					RemapSeqBone( int iSequence, int iLocalBone ) const;	// maps local sequence bone to global bone
	int					RemapAnimBone( int iAnim, int iLocalBone ) const;		// maps local animations bone to global bone

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	inline mstudiobonecontroller_t *pBonecontroller( int i ) const { assert( i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

	int					numhitboxsets;
	int					hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t	*pHitboxSet( int i ) const 
	{ 
		assert( i >= 0 && i < numhitboxsets); 
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex ) + i; 
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t *pHitbox( int i, int set ) const 
	{ 
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return NULL;

		return s->pHitbox( i );
	};

	// Calls through to set to get hitbox count for set
	inline int			iHitboxCount( int set ) const
	{
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
	//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
	inline mstudioanimdesc_t *pLocalAnimdesc( int i ) const { if (i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t *)(((byte *)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
	inline mstudioseqdesc_t *pLocalSeqdesc( int i ) const { if (i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + localseqindex) + i; };

	//public:
	bool				SequencesAvailable() const;
	int					GetNumSeq() const;
	mstudioanimdesc_t	&pAnimdesc( int i ) const;
	mstudioseqdesc_t	&pSeqdesc( int i ) const;
	int					iRelativeAnim( int baseseq, int relanim ) const;	// maps seq local anim reference to global anim index
	int					iRelativeSeq( int baseseq, int relseq ) const;		// maps seq local seq reference to global seq index

	//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
	//public:
	int					GetSequenceActivity( int iSequence );
	void				SetSequenceActivity( int iSequence, int iActivity );
	int					GetActivityListVersion( void ) const;
	void				SetActivityListVersion( int version ) const;
	int					GetEventListVersion( void ) const;
	void				SetEventListVersion( int version ) const;

	// raw textures
	int					numtextures;
	int					textureindex;
	inline mstudiotexture_t *pTexture( int i ) const { return (mstudiotexture_t *)(((byte *)this) + textureindex) + i; }; 


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char			*pCdtexture( int i ) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short		*pSkinref( int i ) const { return (short *)(((byte *)this) + skinindex) + i; };

	int					numbodyparts;		
	int					bodypartindex;
	inline mstudiobodyparts_t	*pBodypart( int i ) const { return (mstudiobodyparts_t *)(((byte *)this) + bodypartindex) + i; };

	// queryable attachable points
	//private:
	int					numlocalattachments;
	int					localattachmentindex;
	inline mstudioattachment_t	*pLocalAttachment( int i ) const { assert( i >= 0 && i < numlocalattachments); return (mstudioattachment_t *)(((byte *)this) + localattachmentindex) + i; };
	//public:
	int					GetNumAttachments( void ) const;
	const mstudioattachment_t &pAttachment( int i ) const;
	int					GetAttachmentBone( int i ) const;
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone( int iAttachment, int iBone );

	// animation node to animation node transition graph
	//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char			*pszLocalNodeName( int iNode ) const { assert( iNode >= 0 && iNode < numlocalnodes); return (((char *)this) + *((int *)(((byte *)this) + localnodenameindex) + iNode)); }
	inline byte			*pLocalTransition( int i ) const { assert( i >= 0 && i < (numlocalnodes * numlocalnodes)); return (byte *)(((byte *)this) + localnodeindex) + i; };

	//public:
	int					EntryNode( int iSequence ) const;
	int					ExitNode( int iSequence ) const;
	char				*pszNodeName( int iNode ) const;
	int					GetTransition( int iFrom, int iTo ) const;

	int					numflexdesc;
	int					flexdescindex;
	inline mstudioflexdesc_t *pFlexdesc( int i ) const { assert( i >= 0 && i < numflexdesc); return (mstudioflexdesc_t *)(((byte *)this) + flexdescindex) + i; };

	int					numflexcontrollers;
	int					flexcontrollerindex;
	inline mstudioflexcontroller_t *pFlexcontroller( int i ) const { assert( i >= 0 && i < numflexcontrollers); return (mstudioflexcontroller_t *)(((byte *)this) + flexcontrollerindex) + i; };

	int					numflexrules;
	int					flexruleindex;
	inline mstudioflexrule_t *pFlexRule( int i ) const { assert( i >= 0 && i < numflexrules); return (mstudioflexrule_t *)(((byte *)this) + flexruleindex) + i; };

	int					numikchains;
	int					ikchainindex;
	inline mstudioikchain_t *pIKChain( int i ) const { assert( i >= 0 && i < numikchains); return (mstudioikchain_t *)(((byte *)this) + ikchainindex) + i; };

	int					nummouths;
	int					mouthindex;
	inline mstudiomouth_t *pMouth( int i ) const { assert( i >= 0 && i < nummouths); return (mstudiomouth_t *)(((byte *)this) + mouthindex) + i; };

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	inline mstudioposeparamdesc_t *pLocalPoseParameter( int i ) const { assert( i >= 0 && i < numlocalposeparameters); return (mstudioposeparamdesc_t *)(((byte *)this) + localposeparamindex) + i; };
	//public:
	int					GetNumPoseParameters( void ) const;
	const mstudioposeparamdesc_t &pPoseParameter( int i ) const;
	int					GetSharedPoseParameter( int iSequence, int iLocalPose ) const;

	int					surfacepropindex;
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropindex; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	inline mstudioiklock_t *pLocalIKAutoplayLock( int i ) const { assert( i >= 0 && i < numlocalikautoplaylocks); return (mstudioiklock_t *)(((byte *)this) + localikautoplaylockindex) + i; };

	int					GetNumIKAutoplayLocks( void ) const;
	const mstudioiklock_t &pIKAutoplayLock( int i ) const;
	int					CountAutoplaySequences() const;
	int					CopyAutoplaySequences( unsigned short *pOut, int outCount ) const;
	int					GetAutoplayList( unsigned short **pOut ) const;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	inline mstudiomodelgroup_t *pModelGroup( int i ) const { assert( i >= 0 && i < numincludemodels); return (mstudiomodelgroup_t *)(((byte *)this) + includemodelindex) + i; };
	// implementation specific call to get a named model
	const studiohdr_t	*FindModel( void **cache, char const *modelname ) const;

	// implementation specific back pointer to virtual data
	mutable void		*virtualModel;
	virtualmodel_t		*GetVirtualModel( void ) const;

	// for demand loaded animation blocks
	int					szanimblocknameindex;	
	inline char * const pszAnimBlockName( void ) const { return ((char *)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	inline mstudioanimblock_t *pAnimBlock( int i ) const { assert( i > 0 && i < numanimblocks); return (mstudioanimblock_t *)(((byte *)this) + animblockindex) + i; };
	mutable void		*animblockModel;
	byte *				GetAnimBlock( int i ) const;

	int					bonetablebynameindex;
	inline const byte	*GetBoneTableSortedByName() const { return (byte *)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void				*pVertexBase;
	void				*pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	byte				unused[2];

	int					zeroframecacheindex;
	byte				*pZeroframeCache( int i ) const { if (zeroframecacheindex) return (byte *)this + ((int *)(((byte *)this) + zeroframecacheindex))[i]; else return NULL; }

	int					unused2[6];		// remove as appropriate

	studiohdr_t() {}

private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};

struct vertexFileHeader_t
{
	int		id;								// MODEL_VERTEX_FILE_ID
	int		version;						// MODEL_VERTEX_FILE_VERSION
	long	checksum;						// same as studiohdr_t, ensures sync
	int		numLODs;						// num of valid lods
	int		numLODVertexes[MAX_NUM_LODS];	// num verts for desired root lod
	int		numFixups;						// num of vertexFileFixup_t
	int		fixupTableStart;				// offset from base to fixup table
	int		vertexDataStart;				// offset from base to vertex block
	int		tangentDataStart;				// offset from base to tangent block
};

// apply sequentially to lod sorted vertex and tangent pools to re-establish mesh order
struct vertexFileFixup_t
{
	int		lod;				// used to skip culled root lod
	int		sourceVertexID;		// absolute index from start of vertex/tangent blocks
	int		numVertexes;
};

// Load the minimum qunatity of verts and run fixups
inline int Studio_LoadVertexes( const vertexFileHeader_t *pTempVvdHdr, vertexFileHeader_t *pNewVvdHdr, int rootLOD, bool bNeedsTangentS )
{
	int					i;
	int					target;
	int					numVertexes;
	vertexFileFixup_t	*pFixupTable;

	numVertexes = pTempVvdHdr->numLODVertexes[rootLOD];

	// copy all data up to start of vertexes
	memcpy((void*)pNewVvdHdr, (void*)pTempVvdHdr, pTempVvdHdr->vertexDataStart);

	for ( i = 0; i < rootLOD; i++)
	{
		pNewVvdHdr->numLODVertexes[i] = pNewVvdHdr->numLODVertexes[rootLOD];
	}

	// fixup data starts
	if (bNeedsTangentS)
	{
		// tangent data follows possibly reduced vertex data
		pNewVvdHdr->tangentDataStart = pNewVvdHdr->vertexDataStart + numVertexes*sizeof(mstudiovertex_t);
	}
	else
	{
		// no tangent data will be available, mark for identification
		pNewVvdHdr->tangentDataStart = 0;
	}

	if (!pNewVvdHdr->numFixups)
	{		
		// fixups not required
		// transfer vertex data
		memcpy(
			(byte *)pNewVvdHdr+pNewVvdHdr->vertexDataStart, 
			(byte *)pTempVvdHdr+pTempVvdHdr->vertexDataStart,
			numVertexes*sizeof(mstudiovertex_t) );

		if (bNeedsTangentS)
		{
			// transfer tangent data to cache memory
			memcpy(
				(byte *)pNewVvdHdr+pNewVvdHdr->tangentDataStart, 
				(byte *)pTempVvdHdr+pTempVvdHdr->tangentDataStart,
				numVertexes*sizeof(Vector4D) );
		}

		return numVertexes;
	}

	// fixups required
	// re-establish mesh ordered vertexes into cache memory, according to table
	target      = 0;
	pFixupTable = (vertexFileFixup_t *)((byte *)pTempVvdHdr + pTempVvdHdr->fixupTableStart);
	for (i=0; i<pTempVvdHdr->numFixups; i++)
	{
		if (pFixupTable[i].lod < rootLOD)
		{
			// working bottom up, skip over copying higher detail lods
			continue;
		}

		// copy vertexes
		memcpy(
			(mstudiovertex_t *)((byte *)pNewVvdHdr+pNewVvdHdr->vertexDataStart) + target,
			(mstudiovertex_t *)((byte *)pTempVvdHdr+pTempVvdHdr->vertexDataStart) + pFixupTable[i].sourceVertexID,
			pFixupTable[i].numVertexes*sizeof(mstudiovertex_t) );

		if (bNeedsTangentS)
		{
			// copy tangents
			memcpy(
				(Vector4D *)((byte *)pNewVvdHdr+pNewVvdHdr->tangentDataStart) + target,
				(Vector4D *)((byte *)pTempVvdHdr+pTempVvdHdr->tangentDataStart) + pFixupTable[i].sourceVertexID,
				pFixupTable[i].numVertexes*sizeof(Vector4D) );
		}

		// data is placed consecutively
		target += pFixupTable[i].numVertexes;
	}

	pNewVvdHdr->numFixups = 0;

	return target;
}

static studiohdr_t *g_pActiveStudioHdr = NULL;

#define MODEL_VERTEX_FILE_ID		(('V'<<24)+('S'<<16)+('D'<<8)+'I')
#define MODEL_VERTEX_FILE_VERSION	4

const mstudio_modelvertexdata_t *mstudiomodel_t::GetVertexData()
{
	FileReader*			fileHandle;
	vertexFileHeader_t*	pVvdHdr;

	assert( g_pActiveStudioHdr );

	if ( g_pActiveStudioHdr->pVertexBase )
	{
		vertexdata.pVertexData  = (byte *)g_pActiveStudioHdr->pVertexBase + ((vertexFileHeader_t *)g_pActiveStudioHdr->pVertexBase)->vertexDataStart;
		vertexdata.pTangentData = (byte *)g_pActiveStudioHdr->pVertexBase + ((vertexFileHeader_t *)g_pActiveStudioHdr->pVertexBase)->tangentDataStart;
		return &vertexdata;
	}

	// mandatory callback to make requested data resident
	// load and persist the vertex file
	stringc fullname = "..\\..\\Media\\";
	stringc purename = g_pActiveStudioHdr->pszName();
	s32 pathend = purename.findLast('/');
	if (pathend == -1)
		pathend = purename.findLast('\\');
	if (pathend != -1)
		purename = purename.subString(pathend + 1, purename.size() - pathend -1);
	pathend = purename.findLast('.');
	stringc vvdName = purename.subString(0, pathend + 1);
	vvdName = fullname + vvdName + "vvd";

	// load the model
	fileHandle = CreateReadFile( vvdName.c_str() );
	if ( !fileHandle )
	{
		Printer::Log( "Unable to load vertex data", vvdName.c_str(), ELL_ERROR );
	}

	// Get the file size
	int size = fileHandle->GetSize();
	if (size == 0)
	{
		fileHandle->Drop();
		Printer::Log( "Bad size for vertex data", vvdName.c_str() );
	}

	pVvdHdr = (vertexFileHeader_t *)malloc(size);
	fileHandle->Read( pVvdHdr, size );
	fileHandle->Drop();

	// check header
	if (pVvdHdr->id != MODEL_VERTEX_FILE_ID)
	{
		Printer::Log("Error Vertex File id", vvdName.c_str(), ELL_ERROR);
	}
	if (pVvdHdr->version != MODEL_VERTEX_FILE_VERSION)
	{
		Printer::Log("Error Vertex File version", vvdName.c_str(), ELL_ERROR);
	}
	if (pVvdHdr->checksum != g_pActiveStudioHdr->checksum)
	{
		Printer::Log("Error Vertex File checksum", vvdName.c_str(), ELL_ERROR);
	}

	vertexFileHeader_t* pNewVvdHdr = (vertexFileHeader_t*)malloc(size);
	Studio_LoadVertexes(pVvdHdr, pNewVvdHdr, 0, true);

	g_pActiveStudioHdr->pVertexBase = (void*)pNewVvdHdr; 

	vertexdata.pVertexData  = (byte *)pNewVvdHdr + pNewVvdHdr->vertexDataStart;
	vertexdata.pTangentData = (byte *)pNewVvdHdr + pNewVvdHdr->tangentDataStart;

	free(pVvdHdr);

	return &vertexdata;
}


// NOTE: You can change this without affecting the vtx file format.
#define MAX_NUM_BONES_PER_TRI ( MAX_NUM_BONES_PER_VERT * 3 )
#define MAX_NUM_BONES_PER_STRIP 512

#define OPTIMIZED_MODEL_FILE_VERSION 7

extern bool g_bDumpGLViewFiles;

struct s_bodypart_t;

namespace OptimizedModel
{

#pragma pack(1)

	struct BoneStateChangeHeader_t
	{
		int hardwareID;
		int newBoneID;
	};

	struct Vertex_t
	{
		// these index into the mesh's vert[origMeshVertID]'s bones
		unsigned char boneWeightIndex[MAX_NUM_BONES_PER_VERT];
		unsigned char numBones;

		short origMeshVertID;

		// for sw skinned verts, these are indices into the global list of bones
		// for hw skinned verts, these are hardware bone indices
		char boneID[MAX_NUM_BONES_PER_VERT];
	};

	enum StripHeaderFlags_t {
		STRIP_IS_TRILIST	= 0x01,
		STRIP_IS_TRISTRIP	= 0x02
	};

	// a strip is a piece of a stripgroup that is divided by bones 
	// (and potentially tristrips if we remove some degenerates.)
	struct StripHeader_t
	{
		// indexOffset offsets into the mesh's index array.
		int numIndices;
		int indexOffset;

		// vertexOffset offsets into the mesh's vert array.
		int numVerts;
		int vertOffset;

		// use this to enable/disable skinning.  
		// May decide (in optimize.cpp) to put all with 1 bone in a different strip 
		// than those that need skinning.
		short numBones;  

		unsigned char flags;

		int numBoneStateChanges;
		int boneStateChangeOffset;
		inline BoneStateChangeHeader_t *pBoneStateChange( int i ) const 
		{ 
			return (BoneStateChangeHeader_t *)(((byte *)this) + boneStateChangeOffset) + i; 
		};
	};

	enum StripGroupFlags_t {
		STRIPGROUP_IS_FLEXED		= 0x01,
		STRIPGROUP_IS_HWSKINNED		= 0x02,
		STRIPGROUP_IS_DELTA_FLEXED	= 0x04
	};

	// a locking group
	// a single vertex buffer
	// a single index buffer
	struct StripGroupHeader_t
	{
		// These are the arrays of all verts and indices for this mesh.  strips index into this.
		int numVerts;
		int vertOffset;
		inline Vertex_t *pVertex( int i ) const 
		{ 
			return (Vertex_t *)(((byte *)this) + vertOffset) + i; 
		};

		int numIndices;
		int indexOffset;
		inline unsigned short *pIndex( int i ) const 
		{ 
			return (unsigned short *)(((byte *)this) + indexOffset) + i; 
		};

		int numStrips;
		int stripOffset;
		inline StripHeader_t *pStrip( int i ) const 
		{ 
			return (StripHeader_t *)(((byte *)this) + stripOffset) + i; 
		};

		unsigned char flags;
	};

	enum MeshFlags_t { 
		// these are both material properties, and a mesh has a single material.
		MESH_IS_TEETH	= 0x01, 
		MESH_IS_EYES	= 0x02
	};

	// a collection of locking groups:
	// up to 4:
	// non-flexed, hardware skinned
	// flexed, hardware skinned
	// non-flexed, software skinned
	// flexed, software skinned
	//
	// A mesh has a material associated with it.
	struct MeshHeader_t
	{
		int numStripGroups;
		int stripGroupHeaderOffset;
		inline StripGroupHeader_t *pStripGroup( int i ) const 
		{ 
			StripGroupHeader_t *pDebug = (StripGroupHeader_t *)(((byte *)this) + stripGroupHeaderOffset) + i; 
			return pDebug;
		};
		unsigned char flags;
	};

	struct ModelLODHeader_t
	{
		int numMeshes;
		int meshOffset;
		float switchPoint;
		inline MeshHeader_t *pMesh( int i ) const 
		{ 
			MeshHeader_t *pDebug = (MeshHeader_t *)(((byte *)this) + meshOffset) + i; 
			return pDebug;
		};
	};

	// This maps one to one with models in the mdl file.
	// There are a bunch of model LODs stored inside potentially due to the qc $lod command
	struct ModelHeader_t
	{
		int numLODs; // garymcthack - this is also specified in FileHeader_t
		int lodOffset;
		inline ModelLODHeader_t *pLOD( int i ) const 
		{ 
			ModelLODHeader_t *pDebug = ( ModelLODHeader_t *)(((byte *)this) + lodOffset) + i; 
			return pDebug;
		};
	};

	struct BodyPartHeader_t
	{
		int numModels;
		int modelOffset;
		inline ModelHeader_t *pModel( int i ) const 
		{ 
			ModelHeader_t *pDebug = (ModelHeader_t *)(((byte *)this) + modelOffset) + i;
			return pDebug;
		};
	};

	struct MaterialReplacementHeader_t
	{
		short materialID;
		int replacementMaterialNameOffset;
		inline const char *pMaterialReplacementName( void )
		{
			const char *pDebug = (const char *)(((byte *)this) + replacementMaterialNameOffset); 
			return pDebug;
		}
	};

	struct MaterialReplacementListHeader_t
	{
		int numReplacements;
		int replacementOffset;
		inline MaterialReplacementHeader_t *pMaterialReplacement( int i ) const
		{
			MaterialReplacementHeader_t *pDebug = ( MaterialReplacementHeader_t *)(((byte *)this) + replacementOffset) + i; 
			return pDebug;
		}
	};

	struct FileHeader_t
	{
		// file version as defined by OPTIMIZED_MODEL_FILE_VERSION
		int version;

		// hardware params that affect how the model is to be optimized.
		int vertCacheSize;
		unsigned short maxBonesPerStrip;
		unsigned short maxBonesPerTri;
		int maxBonesPerVert;

		// must match checkSum in the .mdl
		long checkSum;

		int numLODs; // garymcthack - this is also specified in ModelHeader_t and should match

		// one of these for each LOD
		int materialReplacementListOffset;
		MaterialReplacementListHeader_t *pMaterialReplacementList( int lodID ) const
		{ 
			MaterialReplacementListHeader_t *pDebug = 
				(MaterialReplacementListHeader_t *)(((byte *)this) + materialReplacementListOffset) + lodID;
			return pDebug;
		}

		int numBodyParts;
		int bodyPartOffset;
		inline BodyPartHeader_t *pBodyPart( int i ) const 
		{
			BodyPartHeader_t *pDebug = (BodyPartHeader_t *)(((byte *)this) + bodyPartOffset) + i;
			return pDebug;
		};	
	};

#pragma pack()

	void WriteOptimizedFiles( studiohdr_t *phdr, s_bodypart_t *pSrcBodyParts );

}; // namespace OptimizedModel


#endif
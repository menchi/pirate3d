#ifndef _PIRATE_MATERIAL_H_
#define _PIRATE_MATERIAL_H_

#include "pirateTypes.h"

namespace Pirate
{

class D3D9Texture;


//! Maximum number of texture an SMaterial can have.
const u32 MATERIAL_MAX_TEXTURES = 4;

//! struct for holding parameters for a material renderer
class SMaterial
{
public:
	//! default constructor, creates a solid material with standard colors
	SMaterial()	: ShaderType(-1), Wireframe(FALSE), ZBuffer(TRUE),
				  ZWriteEnable(TRUE), BackfaceCulling(D3DCULL_CCW), Filter(D3DTEXF_NONE)
	{
		for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			Textures[i] = 0;
			TextureWrap[i] = D3DTADDRESS_WRAP;
		}
	}

	SMaterial(const SMaterial& other)
	{
		*this = other;
	}

	~SMaterial()
	{
	}

	SMaterial& operator=(const SMaterial& other)
	{
		ShaderType = other.ShaderType;

		for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			Textures[i] = other.Textures[i];
			TextureWrap[i] = other.TextureWrap[i];
		}

		Wireframe = other.Wireframe;
		ZBuffer = other.ZBuffer;
		ZWriteEnable = other.ZWriteEnable;
		BackfaceCulling = other.BackfaceCulling;
		Filter = other.Filter;

		return *this;
	}

	//! Type of the material. Specifies how everything is blended together
	s32 ShaderType;

	//! Texture layer array.
	D3D9Texture* Textures[MATERIAL_MAX_TEXTURES];

	//! Texture Address Mode
	D3DTEXTUREADDRESS TextureWrap[MATERIAL_MAX_TEXTURES];

	struct
	{
		//! Draw as wireframe or filled triangles? Default: false
		BOOL Wireframe;

		//! Is the ZBuffer enabled? Default: true
		//! Changed from Bool to Integer
		// ( 0 == ZBuffer Off, 1 == ZBuffer LessEqual, 2 == ZBuffer Equal )
		u32 ZBuffer;

		//! May be written to the zbuffer or is it readonly.
		/** Default: 1 This flag is ignored, if the MaterialType
		is a transparent type. */
		BOOL ZWriteEnable;

		//! Is backfaceculling enabled? Default: true
		D3DCULL BackfaceCulling;

		//! What filter enabled? Default: bilinear
		D3DTEXTUREFILTERTYPE Filter;
	};


	//! Compare operator
	inline bool operator!=(const SMaterial& b) const
	{
		return 
			Textures[0] != b.Textures[0] ||
			Textures[1] != b.Textures[1] ||
			Textures[2] != b.Textures[2] ||
			Textures[3] != b.Textures[3] ||
			ShaderType != b.ShaderType ||
			TextureWrap[0] != b.TextureWrap[0] ||
			TextureWrap[1] != b.TextureWrap[1] ||
			TextureWrap[2] != b.TextureWrap[2] ||
			TextureWrap[3] != b.TextureWrap[3] ||
			Wireframe != b.Wireframe ||
			ZBuffer != b.ZBuffer ||
			ZWriteEnable != b.ZWriteEnable ||
			BackfaceCulling != b.BackfaceCulling ||
			Filter != b.Filter;
	}
	inline bool operator==(const SMaterial& b) const
	{ return !(b!=*this); }

};

}

#endif
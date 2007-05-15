#ifndef _PIRATE_MATERIAL_H_
#define _PIRATE_MATERIAL_H_

#include <d3d.h>
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
	SMaterial()	: ShaderType(0)
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

		return *this;
	}

	//! Type of the material. Specifies how everything is blended together
	s32 ShaderType;

	//! Texture layer array.
	D3D9Texture* Textures[MATERIAL_MAX_TEXTURES];

	//! Texture Address Mode
	D3DTEXTUREADDRESS TextureWrap[MATERIAL_MAX_TEXTURES];

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
			TextureWrap[3] != b.TextureWrap[3];
	}
	inline bool operator==(const SMaterial& b) const
	{ return !(b!=*this); }

};

}

#endif
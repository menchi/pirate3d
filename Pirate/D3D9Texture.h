#ifndef _PIRATE_DIRECTX9_TEXTURE_H_
#define _PIRATE_DIRECTX9_TEXTURE_H_

#include "CompileConfig.h"

#include <d3d9.h>

#include "pirateString.h"
#include "Image.h"

namespace Pirate
{

//! Enumeration flags telling the video driver in which format textures
//! should be created.
enum E_TEXTURE_CREATION_FLAG
{
	//! Forces the driver to create 16 bit textures always, independent of
	//! which format the file on disk has. When choosing this you may loose
	//! some color detail, but gain much speed and memory. 16 bit textures
	//! can be transferred twice as fast as 32 bit textures and only use 
	//! half of the space in memory.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
	//! or ETCF_OPTIMIZED_FOR_SPEED at the same time.
	ETCF_ALWAYS_16_BIT = 0x00000001,

	//! Forces the driver to create 32 bit textures always, independent of
	//! which format the file on disk has. Please note that some drivers 
	//! (like the software device) will ignore this, because they are only
	//! able to create and use 16 bit textures.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_16_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
	//! or ETCF_OPTIMIZED_FOR_SPEED at the same time.
	ETCF_ALWAYS_32_BIT = 0x00000002,

	//! Lets the driver decide in which format the textures are created and
	//! tries to make the textures look as good as possible.
	//! Usually it simply chooses the format in which the texture was stored on disk.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, 
	//! or ETCF_OPTIMIZED_FOR_SPEED at the same time.
	ETCF_OPTIMIZED_FOR_QUALITY  = 0x00000004,

	//! Lets the driver decide in which format the textures are created and
	//! tries to create them maximizing render speed.
	//! When using this flag, it does not make sense to use the flags
	//! ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_QUALITY,
	//! at the same time.
	ETCF_OPTIMIZED_FOR_SPEED = 0x00000008,

	//! Automatically creates mip map levels for the textures.
	ETCF_CREATE_MIP_MAPS = 0x00000010,

	//! Create Texture using D3DX texture functions
	ETCF_CREATE_USING_D3DX = 0x00000020,

	//! This flag is never used, it only forces the compiler to 
	//! compile these enumeration values to 32 bit.
	ETCF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};


//! Helper function, helps to get the desired texture creation format from the flags.
//! Returns either ETCF_ALWAYS_32_BIT, ETCF_ALWAYS_16_BIT, ETCF_OPTIMIZED_FOR_QUALITY,
//! or ETCF_OPTIMIZED_FOR_SPEED.
inline E_TEXTURE_CREATION_FLAG GetTextureFormatFromFlags(u32 flags)
{
	if (flags & ETCF_OPTIMIZED_FOR_SPEED)
		return ETCF_OPTIMIZED_FOR_SPEED;
	if (flags & ETCF_ALWAYS_16_BIT)
		return ETCF_ALWAYS_16_BIT;
	if (flags & ETCF_ALWAYS_32_BIT)
		return ETCF_ALWAYS_32_BIT;
	if (flags & ETCF_OPTIMIZED_FOR_QUALITY)
		return ETCF_OPTIMIZED_FOR_QUALITY;
	return ETCF_OPTIMIZED_FOR_SPEED;
}

class D3D9Driver;
/*!
interface for a Video Driver dependent Texture.
*/
class D3D9Texture : public virtual RefObject
{
public:

	//! constructor
	D3D9Texture(Image* image, D3D9Driver* driver, u32 flags, const char* name);

	//! rendertarget constructor
	D3D9Texture(D3D9Driver* driver, dimension2d<s32> size, const char* name);

	//! d3dx constructor
	D3D9Texture(D3D9Driver* driver, u32 flags, const char* name);

	//! dummy texture constructor
	D3D9Texture(const char* name);

	//! destructor
	 ~D3D9Texture();

	//! lock function
	 void* Lock();

	//! unlock function
	 void Unlock();

	//! Returns original size of the texture.
	 const dimension2d<s32>& GetOriginalSize();

	//! Returns (=size) of the texture.
	 const dimension2d<s32>& GetSize();

	//! returns color format of texture
	 ECOLOR_FORMAT GetColorFormat() const;

	//! returns pitch of texture (in bytes)
	 u32 GetPitch() const;

	//! returns the DIRECT3D9 Texture
	IDirect3DTexture9* GetDX9Texture();

	//! returns if texture has mipmap levels
	BOOL HasMipMaps();

	//! Regenerates the mip map levels of the texture. Useful after locking and
	//! modifying the texture
	void RegenerateMipMapLevels();

	//! returns if it is a render target
	BOOL IsRenderTarget();

	//! Returns pointer to the render target surface
	IDirect3DSurface9* GetRenderTargetSurface();

	//! Returns name of texture (in most cases this is the filename)
	const stringc& GetName() { return m_Name; }

private:

	void CreateRenderTarget();

	//! returns the size of a texture which would be the optimize size for rendering it
	inline s32 GetTextureSizeFromImageSize(s32 size);

	//! creates the hardware texture
	void CreateTexture(u32 flags);

	//! copies the image to the texture
	BOOL CopyTexture();

	//! optimized for 16 bit to 16 copy.
	BOOL CopyTo16BitTexture();

	//! copies texture to 32 bit hardware texture
	BOOL CopyTo32BitTexture();

	BOOL CreateMipMaps(s32 level=1);

	void Copy16BitMipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt);

	void Copy32BitMipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt);

	stringc m_Name;

	Image* m_pImage;
	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTexture;
	IDirect3DSurface9* m_pRTTSurface;
	D3D9Driver* m_pDriver;
	dimension2d<s32> m_TextureSize;
	dimension2d<s32> m_ImageSize;
	s32 m_iPitch;
	ECOLOR_FORMAT m_eColorFormat;

	BOOL m_bSurfaceHasSameSize; // true if image has the same dimension as texture.
	BOOL m_bHasMipMaps;
	BOOL m_bHardwareMipMaps;
	BOOL m_bIsRenderTarget;
};

} // end namespace

#endif
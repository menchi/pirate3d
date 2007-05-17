#include "D3D9Texture.h"
#include "D3D9Driver.h"
#include "OS.h"

namespace Pirate
{

//! rendertarget constructor
D3D9Texture::D3D9Texture(D3D9Driver* driver, dimension2d<s32> size, const char* name)
	: m_Name(name), m_pImage(0), m_pTexture(0), m_pRTTSurface(0), m_pDriver(driver),
	m_TextureSize(size), m_ImageSize(size), m_iPitch(0), m_bSurfaceHasSameSize(TRUE),
	m_bHasMipMaps(FALSE), m_bHardwareMipMaps(TRUE), m_bIsRenderTarget(TRUE)
{
#ifdef _DEBUG
	SetDebugName("D3D9Texture");
#endif

	m_Name.make_lower();

	m_pDevice=driver->GetExposedVideoData().D3DDev9;
	if (m_pDevice)
		m_pDevice->AddRef();

	CreateRenderTarget();
}


//! constructor
D3D9Texture::D3D9Texture(Image* image, D3D9Driver* driver,
	u32 flags, const char* name)
	: m_Name(name), m_pImage(image), m_pTexture(0), m_pRTTSurface(0), m_pDriver(driver),
	m_TextureSize(0,0), m_ImageSize(0,0), m_iPitch(0), m_bSurfaceHasSameSize(TRUE),
	m_bHasMipMaps(FALSE), m_bHardwareMipMaps(TRUE), m_bIsRenderTarget(FALSE)
{
#ifdef _DEBUG
	SetDebugName("D3D9Texture");
#endif

	m_Name.make_lower();

	BOOL generateMipLevels = (flags & ETCF_CREATE_MIP_MAPS) != 0;

	m_pDevice=driver->GetExposedVideoData().D3DDev9;
	if (m_pDevice)
		m_pDevice->AddRef();

	if (m_pImage)
	{
		m_pImage->Grab();

		CreateTexture(flags);

		if (m_pTexture)
		{
			if (CopyTexture() && generateMipLevels)
			{
				// create mip maps.
				CreateMipMaps();
				m_bHasMipMaps = TRUE;
			}
		}
		else
			Printer::Log("Could not create DIRECT3D9 Texture.", ELL_WARNING);
	}
}

//! d3dx constructor
D3D9Texture::D3D9Texture(D3D9Driver* driver, u32 flags, const char* name)
						 : m_Name(name), m_pImage(0), m_pTexture(0), m_pRTTSurface(0), m_pDriver(driver),
						 m_TextureSize(0,0), m_ImageSize(0,0), m_iPitch(0), m_bSurfaceHasSameSize(TRUE),
						 m_bHasMipMaps(FALSE), m_bHardwareMipMaps(TRUE), m_bIsRenderTarget(FALSE)
{
#ifdef _DEBUG
	SetDebugName("D3D9Texture");
#endif

	m_Name.make_lower();

	BOOL generateMipLevels = (flags & ETCF_CREATE_MIP_MAPS) != 0;

	m_pDevice=driver->GetExposedVideoData().D3DDev9;
	if (m_pDevice)
		m_pDevice->AddRef();

	D3DXIMAGE_INFO info;
	HRESULT hr = D3DXCreateTextureFromFileExA(m_pDevice, name, 0, 0, !generateMipLevels, 0, D3DFMT_FROM_FILE,
							 				  D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &info, NULL, &m_pTexture);

	m_ImageSize = dimension2d<s32>(info.Width, info.Height);

	m_eColorFormat = ECF_D3DFORMAT;

	if (m_pTexture)
	{
		// create mip maps.
		m_bHasMipMaps = generateMipLevels;
		Printer::Log("Loaded texture", name);
	}
	else
		Printer::Log("Could not create DIRECT3D9 Texture.", ELL_WARNING);
}

//! constructor
D3D9Texture::D3D9Texture(const char* name)
						 : m_Name(name), m_pImage(0), m_pTexture(0), m_pRTTSurface(0), m_pDriver(0),
						 m_TextureSize(0,0), m_ImageSize(0,0), m_iPitch(0), m_bSurfaceHasSameSize(TRUE),
						 m_bHasMipMaps(FALSE), m_bHardwareMipMaps(TRUE), m_bIsRenderTarget(FALSE)
{
#ifdef _DEBUG
	SetDebugName("D3D9Texture");
#endif

	m_Name.make_lower();

	m_pDevice = NULL;
}


void D3D9Texture::CreateRenderTarget()
{
	m_TextureSize.Width = GetTextureSizeFromImageSize(m_TextureSize.Width);
	m_TextureSize.Height = GetTextureSizeFromImageSize(m_TextureSize.Height);

	// get backbuffer format to create the render target in the
	// same format

	IDirect3DSurface9* bb;
	D3DFORMAT d3DFormat = D3DFMT_A8R8G8B8;

	if (!FAILED(m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)))
	{
		D3DSURFACE_DESC desc;
		bb->GetDesc(&desc);
		d3DFormat = desc.Format;

		if (d3DFormat == D3DFMT_X8R8G8B8)
			d3DFormat = D3DFMT_A8R8G8B8;

		bb->Release();
	}
	else
	{
		Printer::Log("Could not create RenderTarget texture: could not get BackBuffer.",
			ELL_WARNING);
		return;
	}

	// create texture
	HRESULT hr;

	hr = m_pDevice->CreateTexture(
		m_TextureSize.Width,
		m_TextureSize.Height,
		1, // mip map level count, we don't want mipmaps here
		D3DUSAGE_RENDERTARGET,
		d3DFormat,
		D3DPOOL_DEFAULT,
		&m_pTexture,
		NULL);

	// get irrlicht format from D3D format

	switch(d3DFormat)
	{
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		m_eColorFormat = ECF_A1R5G5B5;
		break;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		m_eColorFormat = ECF_A8R8G8B8;
		break;
	case D3DFMT_R5G6B5:
		m_eColorFormat = ECF_R5G6B5;
		break;
	default:
		m_eColorFormat = (ECOLOR_FORMAT)-1;
	};

	if (FAILED(hr))
		Printer::Log("Could not create render target texture");
}


BOOL D3D9Texture::CreateMipMaps(s32 level)
{
	if (m_bHardwareMipMaps && m_pTexture)
	{
		// generate mipmaps in hardware

		m_pTexture->GenerateMipSubLevels();
		return TRUE;
	}
	//Printer::Log("manual mipmap");

	// The D3DXFilterTexture function seems to get linked wrong when
	// compiling with both D3D8 and 9, causing it not to work in the D3D9 device.
	// So mipmapgeneration is replaced with my own bad generation here when
	// compiling with both D3D 8 and 9.

	IDirect3DSurface9* upperSurface = 0;
	IDirect3DSurface9* lowerSurface = 0;

	// get upper level
	HRESULT hr = m_pTexture->GetSurfaceLevel(level-1, &upperSurface);
	if (FAILED(hr) || !upperSurface)
	{
		Printer::Log("Could not get upper surface level for mip map generation", ELL_WARNING);
		return FALSE;
	}

	// get lower level
	hr = m_pTexture->GetSurfaceLevel(level, &lowerSurface);
	if (FAILED(hr) || !lowerSurface)
	{
		Printer::Log("Could not get lower surface level for mip map generation", ELL_WARNING);
		upperSurface->Release();
		return FALSE;
	}

	D3DSURFACE_DESC upperDesc, lowerDesc;
	upperSurface->GetDesc(&upperDesc);
	lowerSurface->GetDesc(&lowerDesc);


	D3DLOCKED_RECT upperlr;
	D3DLOCKED_RECT lowerlr;

	// lock upper surface
	if (FAILED(upperSurface->LockRect(&upperlr, NULL, 0)))
	{
		Printer::Log("Could not lock upper texture for mip map generation", ELL_WARNING);
		return FALSE;
	}

	// lock lower surface
	if (FAILED(lowerSurface->LockRect(&lowerlr, NULL, 0)))
	{
		Printer::Log("Could not lock lower texture for mip map generation", ELL_WARNING);
		return FALSE;
	}

	if (upperDesc.Format != lowerDesc.Format)
	{
		Printer::Log("Cannot copy mip maps with different formats.", ELL_WARNING);
	}
	else
	{
		if (upperDesc.Format == D3DFMT_A1R5G5B5)
			Copy16BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
			lowerDesc.Width, lowerDesc.Height,
			upperlr.Pitch, lowerlr.Pitch);
		else
			if (upperDesc.Format == D3DFMT_A8R8G8B8)
				Copy32BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
				lowerDesc.Width, lowerDesc.Height,
				upperlr.Pitch, lowerlr.Pitch);
			else
				Printer::Log("Unsupported mipmap format, cannot copy.", ELL_WARNING);
	}

	// unlock
	if (FAILED(upperSurface->UnlockRect()))
		return FALSE;
	if (FAILED(lowerSurface->UnlockRect()))
		return FALSE;

	// release
	upperSurface->Release();
	lowerSurface->Release();

	if (upperDesc.Width <= 2 || upperDesc.Height <= 2)
		return TRUE; // stop generating levels

	// generate next level
	return CreateMipMaps(level+1);
}


//! creates the hardware texture
void D3D9Texture::CreateTexture(u32 flags)
{
	dimension2d<s32> optSize;
	m_ImageSize = m_pImage->GetDimension();

	if (m_pDriver->QueryFeature(EVDF_TEXTURE_NPOT))
		optSize=m_ImageSize;
	else
	{
		optSize.Width = GetTextureSizeFromImageSize(m_ImageSize.Width);
		optSize.Height = GetTextureSizeFromImageSize(m_ImageSize.Height);
	}

	HRESULT hr;
	D3DFORMAT format = D3DFMT_A1R5G5B5;

	switch(GetTextureFormatFromFlags(flags))
	{
	case ETCF_ALWAYS_16_BIT:
		format = D3DFMT_A1R5G5B5; break;
	case ETCF_ALWAYS_32_BIT:
		format = D3DFMT_A8R8G8B8; break;
	case ETCF_OPTIMIZED_FOR_QUALITY:
		{
			switch(m_pImage->GetColorFormat())
			{
			case ECF_R8G8B8:
				//format = D3DFMT_R8G8B8; break;
			case ECF_A8R8G8B8:
				format = D3DFMT_A8R8G8B8; break;
			case ECF_A1R5G5B5:
			case ECF_R5G6B5:
				format = D3DFMT_A1R5G5B5; break;
			}
		}
		break;
	case ETCF_OPTIMIZED_FOR_SPEED:
		format = D3DFMT_A1R5G5B5; break;
	}

	BOOL mipmaps = (flags & ETCF_CREATE_MIP_MAPS) != 0;

	DWORD usage = 0;

	// This enables hardware mip map generation. Disabled because
	// some cards or drivers seem to have problems with this.
	// D3DCAPS9 caps;
	// Device->GetDeviceCaps(&caps);
	// if (caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
	// {
	//	usage = D3DUSAGE_AUTOGENMIPMAP;
	//	HardwareMipMaps = TRUE;
	//}

	hr = m_pDevice->CreateTexture(optSize.Width, optSize.Height,
		mipmaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		usage, // usage
		format, D3DPOOL_MANAGED , &m_pTexture, NULL);

	if (FAILED(hr) && format == D3DFMT_A8R8G8B8)
	{
		// try brute force 16 bit

		format = D3DFMT_A1R5G5B5;

		hr = m_pDevice->CreateTexture(optSize.Width, optSize.Height,
			(flags & ETCF_CREATE_MIP_MAPS) ? 0 : 1, // number of mipmaplevels (0 = automatic all)
			0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &m_pTexture, NULL);
	}

	m_eColorFormat = (format == D3DFMT_A1R5G5B5) ? ECF_A1R5G5B5 : ECF_A8R8G8B8;
}



//! copies the image to the texture
BOOL D3D9Texture::CopyTexture()
{
	if (m_pTexture && m_pImage)
	{
		D3DSURFACE_DESC desc;
		m_pTexture->GetLevelDesc(0, &desc);

		m_TextureSize.Width = desc.Width;
		m_TextureSize.Height = desc.Height;

		m_bSurfaceHasSameSize = (m_TextureSize == m_ImageSize);

		if (desc.Format == D3DFMT_A1R5G5B5)
			return CopyTo16BitTexture();
		else
			if (desc.Format == D3DFMT_A8R8G8B8)
				return CopyTo32BitTexture();
			else
				Printer::Log("D3D9Texture: Unsupported D3D9 hardware texture format", ELL_ERROR);
	}

	return TRUE;
}


//! copies texture to 32 bit hardware texture
BOOL D3D9Texture::CopyTo32BitTexture()
{
	D3DLOCKED_RECT rect;
	HRESULT hr = m_pTexture->LockRect(0, &rect, 0, 0);
	if (FAILED(hr))
	{
		Printer::Log("Could not lock DIRECT3D9 32 bit Texture.", ELL_ERROR);
		return FALSE;
	}

	s32* dest = (s32*)rect.pBits;
	s32* source = (s32*)m_pImage->Lock();
	m_iPitch = rect.Pitch;
	s32 pitch = rect.Pitch / 4;

	if (m_bSurfaceHasSameSize)
	{
		if (m_pImage->GetColorFormat() == ECF_A8R8G8B8)
		{
			// direct copy, fast

			for (s32 y=0; y<m_ImageSize.Height; ++y)
				for (s32 x=0; x<m_ImageSize.Width; ++x)
					dest[x + y*pitch] = source[x + y * m_ImageSize.Width];
		}
		else
		{
			// slow convert

			for (s32 y=0; y<m_ImageSize.Height; ++y)
				for (s32 x=0; x<m_ImageSize.Width; ++x)
					dest[x + y*pitch] = m_pImage->GetPixel(x,y).color;
		}
	}
	else
	{
		// scale texture

		f32 sourceXStep = (f32)m_ImageSize.Width / (f32)m_TextureSize.Width;
		f32 sourceYStep = (f32)m_ImageSize.Height / (f32)m_TextureSize.Height;
		f32 sy;

		if (m_pImage->GetColorFormat() == ECF_A8R8G8B8)
		{
			// copy texture scaling

			for (s32 x=0; x<m_TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<m_TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] = source[(s32)(((s32)sy)*m_ImageSize.Width + x*sourceXStep)];
					sy+=sourceYStep;
				}
			}
		}
		else
		{
			// convert texture scaling, slow
			for (s32 x=0; x<m_TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<m_TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] =
						m_pImage->GetPixel((s32)(x*sourceXStep), (s32)sy).color;

					sy+=sourceYStep;
				}
			}
		}
	}

	m_pImage->Unlock();

	hr = m_pTexture->UnlockRect(0);
	if (FAILED(hr))
	{
		Printer::Log("Could not unlock DIRECT3D9 Texture.", ELL_ERROR);
		return FALSE;
	}

	return TRUE;

}


//! optimized for 16 bit to 16 copy.
BOOL D3D9Texture::CopyTo16BitTexture()
{
	D3DLOCKED_RECT rect;
	HRESULT hr = m_pTexture->LockRect(0, &rect, 0, 0);
	if (FAILED(hr))
	{
		Printer::Log("Could not lock DIRECT3D9 16 bit Texture.", ELL_ERROR);
		return FALSE;
	}

	s16* dest = (s16*)rect.pBits;
	s16* source = (s16*)m_pImage->Lock();
	m_iPitch = rect.Pitch;
	s32 pitch = rect.Pitch/2;

	if (m_bSurfaceHasSameSize)
	{
		// copy texture

		if (m_pImage->GetColorFormat() == ECF_A1R5G5B5)
		{
			// direct copy, fast

			for (s32 y=0; y<m_ImageSize.Height; ++y)
				for (s32 x=0; x<m_ImageSize.Width; ++x)
					dest[x + y*pitch] = source[x + y * m_ImageSize.Width];
		}
		else
		{
			// slow convert

			for (s32 y=0; y<m_ImageSize.Height; ++y)
				for (s32 x=0; x<m_ImageSize.Width; ++x)
					dest[x + y*pitch] = m_pImage->GetPixel(x,y).toA1R5G5B5();
		}
	}
	else
	{
		// scale texture

		f32 sourceXStep = (f32)m_ImageSize.Width / (f32)m_TextureSize.Width;
		f32 sourceYStep = (f32)m_ImageSize.Height / (f32)m_TextureSize.Height;
		f32 sy;

		if (m_pImage->GetColorFormat() == ECF_A1R5G5B5)
		{
			// copy texture scaling

			for (s32 x=0; x<m_TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<m_TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] = source[(s32)(((s32)sy)*m_ImageSize.Width + x*sourceXStep)];
					sy+=sourceYStep;
				}
			}
		}
		else
		{
			// convert texture scaling, slow
			for (s32 x=0; x<m_TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<m_TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] =
						m_pImage->GetPixel((s32)(x*sourceXStep), (s32)sy).toA1R5G5B5();

					sy+=sourceYStep;
				}
			}
		}
	}

	m_pImage->Unlock();

	hr = m_pTexture->UnlockRect(0);
	if (FAILED(hr))
	{
		Printer::Log("Could not unlock DIRECT3D9 16 bit Texture.", ELL_ERROR);
		return FALSE;
	}

	return TRUE;
}



//! destructor
D3D9Texture::~D3D9Texture()
{
	if (m_pDevice)
		m_pDevice->Release();

	if (m_pImage)
		m_pImage->Drop();

	if (m_pTexture)
		m_pTexture->Release();

	if (m_pRTTSurface)
		m_pRTTSurface->Release();
}



//! lock function
void* D3D9Texture::Lock()
{
	if (!m_pTexture)
		return 0;

	HRESULT hr;
	D3DLOCKED_RECT rect;
	if(!m_bIsRenderTarget)
	{
		hr = m_pTexture->LockRect(0, &rect, 0, 0);
	}
	else
	{
		D3DSURFACE_DESC desc;
		m_pTexture->GetLevelDesc(0, &desc);
		if (!m_pRTTSurface)
		{
			hr = m_pDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &m_pRTTSurface, NULL);
			if (FAILED(hr))
			{
				Printer::Log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
				return 0;
			}
		}

		IDirect3DSurface9 *surface = NULL;
		hr = m_pTexture->GetSurfaceLevel(0, &surface);
		if (FAILED(hr))
		{
			Printer::Log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}
		hr = m_pDevice->GetRenderTargetData(surface, m_pRTTSurface);
		if(FAILED(hr))
		{
			Printer::Log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}
		hr = m_pRTTSurface->LockRect(&rect, NULL, 0);
		if(FAILED(hr))
		{
			Printer::Log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}
		return rect.pBits;
	}
	if (FAILED(hr))
	{
		Printer::Log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
		return 0;
	}

	return rect.pBits;
}



//! unlock function
void D3D9Texture::Unlock()
{
	if (!m_pTexture)
		return;

	if (!m_bIsRenderTarget)
		m_pTexture->UnlockRect(0);
	else if (m_pRTTSurface)
		m_pRTTSurface->UnlockRect();
}


//! Returns original size of the texture.
const dimension2d<s32>& D3D9Texture::GetOriginalSize()
{
	return m_ImageSize;
}


//! Returns (=size) of the texture.
const dimension2d<s32>& D3D9Texture::GetSize()
{
	return m_TextureSize;
}


//! returns the size of a texture which would be the optimize size for rendering it
inline s32 D3D9Texture::GetTextureSizeFromImageSize(s32 size)
{
	s32 ts = 0x01;

	while(ts < size)
		ts <<= 1;

	if (ts > size && ts > 64)
		ts >>= 1;

	return ts;
}


//! returns color format of texture
ECOLOR_FORMAT D3D9Texture::GetColorFormat() const
{
	return m_eColorFormat;
}



//! returns pitch of texture (in bytes)
u32 D3D9Texture::GetPitch() const
{
	return m_iPitch;
}



//! returns the DIRECT3D9 Texture
IDirect3DTexture9* D3D9Texture::GetDX9Texture()
{
	return m_pTexture;
}


//! returns if texture has mipmap levels
BOOL D3D9Texture::HasMipMaps()
{
	return m_bHasMipMaps;
}


void D3D9Texture::Copy16BitMipMap(char* src, char* tgt,
	s32 width, s32 height,
	s32 pitchsrc, s32 pitchtgt)
{
	u16 c;

	for (int x=0; x<width; ++x)
		for (int y=0; y<height; ++y)
		{
			s32 a=0, r=0, g=0, b=0;

			for (int dx=0; dx<2; ++dx)
				for (int dy=0; dy<2; ++dy)
				{
					int tgx = (x*2)+dx;
					int tgy = (y*2)+dy;

					c = *(u16*)((void*)&src[(tgx*2)+(tgy*pitchsrc)]);

					a += getAlpha(c);
					r += getRed(c);
					g += getGreen(c);
					b += getBlue(c);
				}

				a /= 4;
				r /= 4;
				g /= 4;
				b /= 4;

				c = ((a & 0x1) <<15) | ((r & 0x1F)<<10) | ((g & 0x1F)<<5) | (b & 0x1F);
				*(u16*)((void*)&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
}


void D3D9Texture::Copy32BitMipMap(char* src, char* tgt,
	s32 width, s32 height,
	s32 pitchsrc, s32 pitchtgt)
{
	SColor c;

	for (int x=0; x<width; ++x)
	{
		for (int y=0; y<height; ++y)
		{
			s32 a=0, r=0, g=0, b=0;

			for (int dx=0; dx<2; ++dx)
			{
				for (int dy=0; dy<2; ++dy)
				{
					int tgx = (x*2)+dx;
					int tgy = (y*2)+dy;

					c = *(u32*)((void*)&src[(tgx<<2)+(tgy*pitchsrc)]);

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a >>= 2;
			r >>= 2;
			g >>= 2;
			b >>= 2;

			c = ((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff);
			*(u32*)((void*)&tgt[(x*4)+(y*pitchtgt)]) = c.color;
		}
	}
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void D3D9Texture::RegenerateMipMapLevels()
{
	if (m_bHasMipMaps)
		CreateMipMaps();
}


//! returns if it is a render target
BOOL D3D9Texture::IsRenderTarget()
{
	return m_bIsRenderTarget;
}

//! Returns pointer to the render target surface
IDirect3DSurface9* D3D9Texture::GetRenderTargetSurface()
{
	if (!m_bIsRenderTarget)
		return 0;

	IDirect3DSurface9 *pRTTSurface = 0;
	if (m_pTexture)
		m_pTexture->GetSurfaceLevel(0, &pRTTSurface);

	if (pRTTSurface)
		pRTTSurface->Release();

	return pRTTSurface;
}


} // end namespace 


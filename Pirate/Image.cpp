#include "Image.h"
#include "os.h"
#include "pirateString.h"

namespace Pirate
{

//! constructor
Image::Image(ECOLOR_FORMAT format, const dimension2d<s32>& size)
	:m_pData(0), m_Size(size), m_eFormat(format), m_bDeleteMemory(true)
{
	InitData();	
}


//! constructor
Image::Image(ECOLOR_FORMAT format, const dimension2d<s32>& size, void* data,
	BOOL ownForeignMemory, BOOL deleteForeignMemory)
	: m_pData(0), m_Size(size), m_eFormat(format), m_bDeleteMemory(deleteForeignMemory)
{
	if (ownForeignMemory)
	{
		m_pData = (void*)0xbadf00d;
		InitData();	
		m_pData = data;
	}
	else
	{
		m_pData = 0;
		InitData();
		memcpy(m_pData, data, m_Size.Height * m_Size.Width * m_iBytesPerPixel);
	}
}

//! assumes format and size has been set and creates the rest
void Image::InitData()
{
	m_iBitsPerPixel = GetBitsPerPixelFromFormat();
	m_iBytesPerPixel = m_iBitsPerPixel / 8;

	// Pitch should be aligned...
	m_uiPitch = m_iBytesPerPixel * m_Size.Width;

	if (!m_pData)
		m_pData = new s8[m_Size.Height * m_uiPitch];
}


//! destructor
Image::~Image()
{
	if ( m_bDeleteMemory )
		delete [] (s8*)m_pData;
}


//! Returns width and height of image data.
const dimension2d<s32>& Image::GetDimension()
{
	return m_Size;
}



//! Returns bits per pixel. 
s32 Image::GetBitsPerPixel()
{
	return m_iBitsPerPixel;
}


//! Returns bytes per pixel
s32 Image::GetBytesPerPixel()
{
	return m_iBytesPerPixel;
}



//! Returns image data size in bytes
s32 Image::GetImageDataSizeInBytes()
{
	return m_uiPitch * m_Size.Height;
}



//! Returns image data size in pixels
s32 Image::GetImageDataSizeInPixels()
{
	return m_Size.Width * m_Size.Height;
}



//! returns mask for red value of a pixel
u32 Image::GetRedMask()
{
	return m_uiRedMask;
}



//! returns mask for green value of a pixel
u32 Image::GetGreenMask()
{
	return m_uiGreenMask;
}



//! returns mask for blue value of a pixel
u32 Image::GetBlueMask()
{
	return m_uiBlueMask;
}



//! returns mask for alpha value of a pixel
u32 Image::GetAlphaMask()
{
	return m_uiAlphaMask;
}


s32 Image::GetBitsPerPixelFromFormat()
{
	switch(m_eFormat)
	{
	case ECF_A1R5G5B5:
		m_uiAlphaMask = 0x1<<15;
		m_uiRedMask = 0x1F<<10;
		m_uiGreenMask = 0x1F<<5;
		m_uiBlueMask = 0x1F;
		return 16;
	case ECF_R5G6B5:
		m_uiAlphaMask = 0x0;
		m_uiRedMask = 0x1F<<11;
		m_uiGreenMask = 0x3F<<5;
		m_uiBlueMask = 0x1F;
		return 16;
	case ECF_R8G8B8:
		m_uiAlphaMask = 0x0;
		m_uiRedMask = 0xFF<<16;
		m_uiGreenMask = 0xFF<<8;
		m_uiBlueMask = 0xFF;
		return 24;
	case ECF_A8R8G8B8:
		m_uiAlphaMask = 0xFF<<24;
		m_uiRedMask = 0xFF<<16;
		m_uiGreenMask = 0xFF<<8;
		m_uiBlueMask = 0xFF;
		return 32;
	}

	Printer::Log("Image: Unknown color format.", ELL_ERROR);
	return 0;
}

//! sets a pixel
void Image::SetPixel(s32 x, s32 y, const SColor &color )
{
	if (x < 0 || y < 0 || x >= m_Size.Width || y >= m_Size.Height)
		return;

	switch(m_eFormat)
	{
	case ECF_A1R5G5B5:
		{
			u16 * dest = (u16*) ((u8*) m_pData + ( y * m_uiPitch ) + ( x << 1 ));
			*dest = A8R8G8B8toA1R5G5B5 ( color.color );
		} break;

	case ECF_A8R8G8B8:
		{
			u32 * dest = (u32*) ((u8*) m_pData + ( y * m_uiPitch ) + ( x << 2 ));
			*dest = color.color;
		} break;

	}
}


//! returns a pixel
SColor Image::GetPixel(s32 x, s32 y)
{
	if (x < 0 || y < 0 || x >= m_Size.Width || y >= m_Size.Height)
		return SColor(0);

	switch(m_eFormat)
	{
	case ECF_A1R5G5B5:
		return A1R5G5B5toA8R8G8B8(((u16*)m_pData)[y*m_Size.Width + x]);
	case ECF_R5G6B5:
		return R5G6B5toA8R8G8B8(((u16*)m_pData)[y*m_Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((u32*)m_pData)[y*m_Size.Width + x];
	case ECF_R8G8B8:
		{
			u8* p = &((u8*)m_pData)[(y*3)*m_Size.Width + (x*3)];
			return SColor(255,p[0],p[1],p[2]);
		}
	}

	return SColor(0);
}

//! returns the color format
ECOLOR_FORMAT Image::GetColorFormat() const
{
	return m_eFormat;
}


} // end namespace 
#ifndef _PIRATE_IMAGE_H_
#define _PIRATE_IMAGE_H_

#include "RefObject.h"
#include "SColor.h"
#include "rect.h"

namespace Pirate
{

//! An enum for the color format of textures used by the Irrlicht Engine.
/** A color format specifies how color information is stored. */
enum ECOLOR_FORMAT
{
	//! 16 bit color format used by the software driver, and thus preferred
	//! by all other irrlicht engine video drivers. There are 5 bits for every
	//! color component, and a single bit is left for alpha information.
	ECF_A1R5G5B5 = 0,	

	//! Standard 16 bit color format.
	ECF_R5G6B5,

	//! 24 bit color, no alpha channel, but 8 bit for red, green and blue.
	ECF_R8G8B8,

	//! Default 32 bit color format. 8 bits are used for every component:
	//! red, green, blue and alpha.
	ECF_A8R8G8B8,

	//! Created by d3dx and use d3dformat
	ECF_D3DFORMAT,
};

//! IImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class Image : public virtual RefObject
{
public:

	//! constructor 
	Image(ECOLOR_FORMAT format, Image* imageToCopy);

	//! constructor 
	//! \param useForeignMemory: If true, the image will use the data pointer
	//! directly and own it from now on, which means it will also try to delete [] the
	//! data when the image will be destructed. If false, the memory will by copied.
	Image(ECOLOR_FORMAT format, const dimension2d<s32>& size, void* data, BOOL ownForeignMemory=TRUE, BOOL deleteMemory = TRUE);

	//! constructor
	Image(ECOLOR_FORMAT format, const dimension2d<s32>& size);

	//! constructor
	Image(Image* imageToCopy, const position2d<s32>& pos, const dimension2d<s32>& size);

	//! destructor
	~Image();

	//! Lock function.
	void* Lock()
	{
		return m_pData;
	};

	//! Unlock function.
	void Unlock() {};

	//! Returns width and height of image data.
	const dimension2d<s32>& GetDimension();

	//! Returns bits per pixel. 
	s32 GetBitsPerPixel();

	//! Returns bytes per pixel
	s32 GetBytesPerPixel();

	//! Returns image data size in bytes
	s32 GetImageDataSizeInBytes();

	//! Returns image data size in pixels
	s32 GetImageDataSizeInPixels();

	//! returns mask for red value of a pixel
	u32 GetRedMask();

	//! returns mask for green value of a pixel
	u32 GetGreenMask();

	//! returns mask for blue value of a pixel
	u32 GetBlueMask();

	//! returns mask for alpha value of a pixel
	u32 GetAlphaMask();

	//! returns a pixel
	SColor GetPixel(s32 x, s32 y);

	//! sets a pixel
	void SetPixel(s32 x, s32 y, const SColor &color );

	//! returns the color format
	ECOLOR_FORMAT GetColorFormat() const;

	//! draws a rectangle
	void DrawRectangle(const rect<s32>& rect, const SColor &color);

	//! copies this surface into another
	void CopyTo(Image* target, const position2d<s32>& pos);

	//! copies this surface into another
	void CopyTo(Image* target, const position2d<s32>& pos, const rect<s32>& sourceRect, const rect<s32>* clipRect=0);

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
	void CopyToWithAlpha(Image* target, const position2d<s32>& pos, const rect<s32>& sourceRect, const SColor &color, const rect<s32>* clipRect = 0);

	//! copies this surface into another, scaling it to fit it.
	void CopyToScaling(Image* target);

	//! copies this surface into another, scaling it to fit it, appyling a box filter
	void CopyToScalingBoxFilter(Image* target, s32 bias = 0);

	//! draws a line from to
	void DrawLine(const position2d<s32>& from, const position2d<s32>& to, const SColor &color);

	//! fills the surface with black or white
	void Fill(const SColor &color);

	//! returns pitch of image
	u32 GetPitch() const
	{
		return m_uiPitch;
	}


private:

	//! assumes format and size has been set and creates the rest
	void InitData();

	s32 GetBitsPerPixelFromFormat();

	inline SColor GetPixelBox ( s32 x, s32 y, s32 fx, s32 fy, s32 bias );

	void* m_pData;
	dimension2d<s32> m_Size;
	s32 m_iBitsPerPixel;
	s32 m_iBytesPerPixel;
	u32 m_uiPitch;
	ECOLOR_FORMAT m_eFormat;

	BOOL m_bDeleteMemory;

	u32 m_uiRedMask;
	u32 m_uiGreenMask;
	u32 m_uiBlueMask;
	u32 m_uiAlphaMask;
};

} // end namespace


#endif
#ifndef _PIRATE_IMAGE_LOADER_H_
#define _PIRATE_IMAGE_LOADER_H_

#include "RefObject.h"

namespace Pirate
{

		// byte-align structures
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

struct PsdHeader
{
	c8 signature [4];	// Always equal to 8BPS.
	u16 version;		// Always equal to 1
	c8 reserved [6];	// Must be zero
	u16 channels;		// Number of any channels inc. alphas
	u32 height;			// Rows Height of image in pixel
	u32 width;			// Colums Width of image in pixel
	u16 depth;			// Bits/channel
	u16 mode;			// Color mode of the file (Bitmap/Grayscale..)
} PACK_STRUCT;


		// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

class Image;
class FileReader;

/*!
Surface Loader for psd images
*/
class ImageLoaderPSD : public virtual RefObject
{
public:

	//! constructor
	ImageLoaderPSD();

	//! destructor
	~ImageLoaderPSD();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	BOOL IsALoadableFileExtension(const c8* fileName);

	//! returns true if the file maybe is able to be loaded by this class
	BOOL IsALoadableFileFormat(FileReader* file);

	//! creates a surface from the file
	Image* LoadImageFromFile(FileReader* file);

private:

	BOOL ReadRawImageData(FileReader* file);
	BOOL ReadRLEImageData(FileReader* file);
	s16 GetShiftFromChannel(c8 channelNr);

	// member variables

	u32* m_pImageData;
	PsdHeader m_Header;
};

/*!
Surface Loader for D3DX loadable images
*/
class ImageLoaderX : public virtual RefObject
{
public:

	//! constructor
	ImageLoaderX(IDirect3DDevice9* device);

	//! destructor
	~ImageLoaderX();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	BOOL IsALoadableFileExtension(const c8* fileName);

	//! returns true if the file maybe is able to be loaded by this class
	BOOL IsALoadableFileFormat(FileReader* file);

	//! creates a surface from the file
	Image* LoadImageFromFile(FileReader* file);

private:
	IDirect3DDevice9* m_pID3DDevice;
};


} // end namespace


#endif
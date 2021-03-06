#include "ImageLoader.h"
#include "Image.h"
#include "FileReader.h"
#include "OS.h"

namespace Pirate
{

//! constructor
ImageLoaderPSD::ImageLoaderPSD()
	: m_pImageData(0)
{
#ifdef _DEBUG
	SetDebugName("ImageLoaderPSD");
#endif
}



//! destructor
ImageLoaderPSD::~ImageLoaderPSD()
{
	delete [] m_pImageData;
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
BOOL ImageLoaderPSD::IsALoadableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".psd") != 0;
}



//! returns true if the file maybe is able to be loaded by this class
BOOL ImageLoaderPSD::IsALoadableFileFormat(FileReader* file)
{
	if (!file)
		return FALSE;

	u8 type[3];
	file->Read(&type, sizeof(u8)*3);
	return (type[2]==2); // we currently only handle tgas of type 2.
}



//! creates a surface from the file
Image* ImageLoaderPSD::LoadImageFromFile(FileReader* file)
{
	delete [] m_pImageData;
	m_pImageData = 0;

	file->Read(&m_Header, sizeof(PsdHeader));

#ifndef __BIG_ENDIAN__
	m_Header.version = Byteswap::ByteSwap(m_Header.version);
	m_Header.channels = Byteswap::ByteSwap(m_Header.channels);
	m_Header.height = Byteswap::ByteSwap(m_Header.height);
	m_Header.width = Byteswap::ByteSwap(m_Header.width);
	m_Header.depth = Byteswap::ByteSwap(m_Header.depth);
	m_Header.mode = Byteswap::ByteSwap(m_Header.mode);
#endif

	if (m_Header.signature[0] != '8' ||
		m_Header.signature[1] != 'B' ||
		m_Header.signature[2] != 'P' ||
		m_Header.signature[3] != 'S')
		return 0;

	if (m_Header.version != 1)
	{
		Printer::Log("Unsupported PSD file version", file->GetFileName(), ELL_ERROR);
		return 0;
	}

	if (m_Header.mode != 3 || m_Header.depth != 8)
	{
		Printer::Log("Unsupported PSD color mode or depth.\n", file->GetFileName(), ELL_ERROR);
		return 0;
	}

	// skip color mode data

	u32 l;
	file->Read(&l, sizeof(u32));
#ifndef __BIG_ENDIAN__
	l = Byteswap::ByteSwap(l);
#endif
	if (!file->Seek(l, TRUE))
	{
		Printer::Log("Error seeking file pos to image resources.\n", file->GetFileName(), ELL_ERROR);
		return 0;
	}

	// skip image resources

	file->Read(&l, sizeof(u32));
#ifndef __BIG_ENDIAN__
	l = Byteswap::ByteSwap(l);
#endif
	if (!file->Seek(l, TRUE))
	{
		Printer::Log("Error seeking file pos to layer and mask.\n", file->GetFileName(), ELL_ERROR);
		return 0;
	}

	// skip layer & mask

	file->Read(&l, sizeof(u32));
#ifndef __BIG_ENDIAN__
	l = Byteswap::ByteSwap(l);
#endif
	if (!file->Seek(l, TRUE))
	{
		Printer::Log("Error seeking file pos to image data section.\n", file->GetFileName(), ELL_ERROR);
		return 0;
	}

	// read image data

	u16 compressionType;
	file->Read(&compressionType, sizeof(u16));
#ifndef __BIG_ENDIAN__
	compressionType = Byteswap::ByteSwap(compressionType);
#endif

	if (compressionType != 1 && compressionType != 0)
	{
		Printer::Log("Unsupported psd compression mode.\n", file->GetFileName(), ELL_ERROR);
		return 0;
	}

	// create image data block

	m_pImageData = new u32[m_Header.width * m_Header.height];

	BOOL res = FALSE;

	if (compressionType == 0)
		res = ReadRawImageData(file); // RAW image data
	else
		res = ReadRLEImageData(file); // RLE compressed data

	Image* image = 0;

	if (res)
	{
		// create surface
		image = new Image(ECF_A8R8G8B8, dimension2d<s32>(m_Header.width, m_Header.height), m_pImageData);
	}

	if (!image)
		delete [] m_pImageData;
	m_pImageData = 0;

	return image;
}



BOOL ImageLoaderPSD::ReadRawImageData(FileReader* file)
{
	u8* tmpData = new u8[m_Header.width * m_Header.height];

	for (s32 channel=0; channel<m_Header.channels && channel < 3; ++channel)
	{
		if (!file->Read(tmpData, sizeof(c8) * m_Header.width * m_Header.height))
		{
			Printer::Log("Error reading color channel\n", file->GetFileName(), ELL_ERROR);
			break;
		}

		s16 shift = GetShiftFromChannel(channel);
		if (shift != -1)
		{
			u32 mask = 0xff << shift;

			for (u32 x=0; x<m_Header.width; ++x)
				for (u32 y=0; y<m_Header.height; ++y)
				{
					s32 index = x + y*m_Header.width;
					m_pImageData[index] = ~(~m_pImageData[index] | mask);
					m_pImageData[index] |= tmpData[index] << shift;
				}
		}

	}

	delete [] tmpData;
	return TRUE;
}


BOOL ImageLoaderPSD::ReadRLEImageData(FileReader* file)
{
	/*	If the compression code is 1, the image data
	starts with the byte counts for all the scan lines in the channel
	(LayerBottom LayerTop), with each count stored as a two
	byte value. The RLE compressed data follows, with each scan line
	compressed separately. The RLE compression is the same compres-sion
	algorithm used by the Macintosh ROM routine PackBits, and
	the TIFF standard.
	If the Layer's Size, and therefore the data, is odd, a pad byte will
	be inserted at the end of the row.
	*/

	/*
	A pseudo code fragment to unpack might look like this:

	Loop until you get the number of unpacked bytes you are expecting:
	Read the next source byte into n.
	If n is between 0 and 127 inclusive, copy the next n+1 bytes literally.
	Else if n is between -127 and -1 inclusive, copy the next byte -n+1
	times.
	Else if n is -128, noop.
	Endloop

	In the inverse routine, it is best to encode a 2-byte repeat run as a replicate run
	except when preceded and followed by a literal run. In that case, it is best to merge
	the three runs into one literal run. Always encode 3-byte repeats as replicate runs.
	That is the essence of the algorithm. Here are some additional rules:
	- Pack each row separately. Do not compress across row boundaries.
	- The number of uncompressed bytes per row is defined to be (ImageWidth + 7)
	/ 8. If the uncompressed bitmap is required to have an even number of bytes per
	row, decompress into word-aligned buffers.
	- If a run is larger than 128 bytes, encode the remainder of the run as one or more
	additional replicate runs.
	When PackBits data is decompressed, the result should be interpreted as per com-pression
	type 1 (no compression).
	*/

	u8* tmpData = new u8[m_Header.width * m_Header.height];
	u16 *rleCount= new u16 [m_Header.height * m_Header.channels];

	s32 size=0;

	for (u32 y=0; y<m_Header.height * m_Header.channels; ++y)
	{
		if (!file->Read(&rleCount[y], sizeof(u16)))
		{
			delete [] tmpData;
			delete [] rleCount;
			Printer::Log("Error reading rle rows\n", file->GetFileName(), ELL_ERROR);
			return FALSE;
		}

#ifndef __BIG_ENDIAN__
		rleCount[y] = Byteswap::ByteSwap(rleCount[y]);
#endif
		size += rleCount[y];
	}

	s8 *buf = new s8[size];
	if (!file->Read(buf, size))
	{
		delete [] rleCount;
		delete [] buf;
		delete [] tmpData;
		Printer::Log("Error reading rle rows\n", file->GetFileName(), ELL_ERROR);
		return FALSE;
	}

	u16 *rcount=rleCount;

	s8 rh;
	u16 bytesRead;
	u8 *dest;
	s8 *pBuf = buf;

	// decompress packbit rle

	for (s32 channel=0; channel<m_Header.channels; channel++)
	{
		for (u32 y=0; y<m_Header.height; ++y, ++rcount)
		{
			bytesRead=0;
			dest = &tmpData[y*m_Header.width];

			while (bytesRead < *rcount)
			{
				rh = *pBuf++;
				++bytesRead;

				if (rh >= 0)
				{
					++rh;

					while (rh--)
					{
						*dest = *pBuf++;
						++bytesRead;
						++dest;
					}
				}
				else
					if (rh > -128)
					{
						rh = -rh +1;

						while (rh--)
						{
							*dest = *pBuf;
							++dest;
						}

						++pBuf;
						++bytesRead;
					}
			}
		}

		s16 shift = GetShiftFromChannel(channel);

		if (shift != -1)
		{
			u32 mask = 0xff << shift;

			for (u32 x=0; x<m_Header.width; ++x)
				for (u32 y=0; y<m_Header.height; ++y)
				{
					s32 index = x + y*m_Header.width;
					m_pImageData[index] = ~(~m_pImageData[index] | mask);
					m_pImageData[index] |= tmpData[index] << shift;
				}
		}
	}

	delete [] rleCount;
	delete [] buf;
	delete [] tmpData;

	return TRUE;
}


s16 ImageLoaderPSD::GetShiftFromChannel(c8 channelNr)
{
	switch(channelNr)
	{
	case 0:
		return 16;  // red
	case 1:
		return 8;   // green
	case 2:
		return 0;   // blue
	case 3:
		return m_Header.channels == 4 ? 24 : -1;	// ?
	case 4:
		return 24;  // alpha
	default:
		return -1;
	}
}



//! creates a loader which is able to load psd
ImageLoaderPSD* CreateImageLoaderPSD()
{
	return new ImageLoaderPSD();
}


//! constructor
ImageLoaderX::ImageLoaderX(IDirect3DDevice9* device) : m_pID3DDevice(device)
{
#ifdef _DEBUG
	SetDebugName("ImageLoaderX");
#endif
	device->AddRef();
}



//! destructor
ImageLoaderX::~ImageLoaderX()
{
	if (m_pID3DDevice)
		m_pID3DDevice->Release();
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
BOOL ImageLoaderX::IsALoadableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".bmp") != 0 || strstr(fileName, ".tga") != 0 || strstr(fileName, ".pfm") != 0 ||
		   strstr(fileName, ".dds") != 0 || strstr(fileName, ".hdr") != 0 || strstr(fileName, ".png") != 0 ||
		   strstr(fileName, ".dib") != 0 || strstr(fileName, ".jpg") != 0 || strstr(fileName, ".ppm") != 0;
}



//! returns true if the file maybe is able to be loaded by this class
BOOL ImageLoaderX::IsALoadableFileFormat(FileReader* file)
{
	if (!file)
		return FALSE;

	return TRUE; // I am lazy.
}



//! creates a surface from the file
Image* ImageLoaderX::LoadImageFromFile(FileReader* file)
{
	IDirect3DSurface9* pSurf = NULL;
	D3DXIMAGE_INFO info;
	D3DXGetImageInfoFromFileA(file->GetFileName(), &info);
	m_pID3DDevice->CreateOffscreenPlainSurface(info.Width, info.Height, info.Format, D3DPOOL_SCRATCH, &pSurf, NULL);
	D3DXLoadSurfaceFromFileA(pSurf, NULL, NULL, file->GetFileName(), NULL, D3DX_FILTER_NONE, 0, &info);

	Image* image = 0;

	if (pSurf)
	{
		// create surface
		D3DLOCKED_RECT lockRect;
		D3DSURFACE_DESC desc;
		switch (info.Format)
		{
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			pSurf->LockRect(&lockRect, NULL, D3DLOCK_READONLY);
			image = new Image(ECF_A8R8G8B8, dimension2d<s32>(info.Width, info.Height), lockRect.pBits, FALSE);
			break;
		case D3DFMT_R8G8B8:
			pSurf->LockRect(&lockRect, NULL, D3DLOCK_READONLY);
			pSurf->GetDesc(&desc);
			image = new Image(ECF_R8G8B8, dimension2d<s32>(info.Width, info.Height), lockRect.pBits, FALSE);
			break;
		case D3DFMT_A1R5G5B5:
		case D3DFMT_X1R5G5B5:
			pSurf->LockRect(&lockRect, NULL, D3DLOCK_READONLY);
			image = new Image(ECF_A1R5G5B5, dimension2d<s32>(info.Width, info.Height), lockRect.pBits, FALSE);
			break;
		case D3DFMT_R5G6B5:
			pSurf->LockRect(&lockRect, NULL, D3DLOCK_READONLY);
			image = new Image(ECF_R5G6B5, dimension2d<s32>(info.Width, info.Height), lockRect.pBits, FALSE);
			break;
		default:
			Printer::Log("No matched image format", ELL_ERROR);
			break;
		}
	}
	else
	{
		Printer::Log("D3DXLoadSuface failed", ELL_ERROR);
		return NULL;
	}

	pSurf->Release();

	return image;
}

} // end namespace
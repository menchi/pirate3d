#include "Image.h"
#include "ImageUtility.h"
#include "pirateString.h"

namespace Pirate
{

struct SBlitJob
{
	AbsRectangle Dest;
	AbsRectangle Source;

	u32 argb;

	void * src;
	void * dst;

	s32 width;
	s32 height;

	u32 srcPitch;
	u32 dstPitch;

	u32 srcPixelMul;
	u32 dstPixelMul;
};

// Blitter Operation
enum eBlitter
{
	BLITTER_INVALID = 0,
	BLITTER_COLOR,
	BLITTER_COLOR_ALPHA,
	BLITTER_TEXTURE,
	BLITTER_TEXTURE_ALPHA_BLEND,
	BLITTER_TEXTURE_ALPHA_COLOR_BLEND
};

typedef void (*tExecuteBlit) ( const SBlitJob * job );


// Bitfields Cohen Sutherland
enum eClipCode
{
	CLIPCODE_EMPTY	=	0,
	CLIPCODE_BOTTOM	=	1,
	CLIPCODE_TOP	=	2,
	CLIPCODE_LEFT	=	4,
	CLIPCODE_RIGHT	=	8
};

inline u32 GetClipCode ( const AbsRectangle &r, const position2d<s32> &p )
{
	u32 code = CLIPCODE_EMPTY;

	if ( p.X < r.x0 )
		code = CLIPCODE_LEFT;
	else
		if ( p.X > r.x1 )
			code = CLIPCODE_RIGHT;

	if ( p.Y < r.y0 )
		code |= CLIPCODE_TOP;
	else
		if ( p.Y > r.y1 )
			code |= CLIPCODE_BOTTOM;

	return code;
}


/*!
Cohen Sutherland clipping
@return: 1 if valid
*/

static int ClipLine (const AbsRectangle &clipping,
	position2d<s32> &p0,
	position2d<s32> &p1,
	const position2d<s32>& p0_in,
	const position2d<s32>& p1_in)
{
	u32 code0;
	u32 code1;
	u32 code;

	p0 = p0_in;
	p1 = p1_in;

	code0 = GetClipCode ( clipping, p0 );
	code1 = GetClipCode ( clipping, p1 );

	// trivial accepted
	while ( code0 | code1 )
	{
		s32 x=0;
		s32 y=0;

		// trivial reject
		if ( code0 & code1 )
			return 0;

		if ( code0 )
		{
			// clip first point
			code = code0;
		}
		else
		{
			// clip last point
			code = code1;
		}

		if ( (code & CLIPCODE_BOTTOM) == CLIPCODE_BOTTOM )
		{
			// clip bottom viewport
			y = clipping.y1;
			x = p0.X + ( p1.X - p0.X ) * ( y - p0.Y ) / ( p1.Y - p0.Y );
		}
		else
			if ( (code & CLIPCODE_TOP) == CLIPCODE_TOP )
			{
				// clip to viewport
				y = clipping.y0;
				x = p0.X + ( p1.X - p0.X ) * ( y - p0.Y ) / ( p1.Y - p0.Y );
			}
			else
				if ( (code & CLIPCODE_RIGHT) == CLIPCODE_RIGHT )
				{
					// clip right viewport
					x = clipping.x1;
					y = p0.Y + ( p1.Y - p0.Y ) * ( x - p0.X ) / ( p1.X - p0.X );
				}
				else
					if ( (code & CLIPCODE_LEFT) == CLIPCODE_LEFT )
					{
						// clip left viewport
						x = clipping.x0;
						y = p0.Y + ( p1.Y - p0.Y ) * ( x - p0.X ) / ( p1.X - p0.X );
					}

					if ( code == code0 )
					{
						// modify first point
						p0.X = x;
						p0.Y = y;
						code0 = GetClipCode ( clipping, p0 );
					}
					else
					{
						// modify second point
						p1.X = x;
						p1.Y = y;
						code1 = GetClipCode ( clipping, p1 );
					}
	}

	return 1;
}

/*
*/
inline void GetClip(AbsRectangle &clipping, Image * t)
{
	clipping.x0 = 0;
	clipping.y0 = 0;
	clipping.x1 = t->GetDimension().Width - 1;
	clipping.y1 = t->GetDimension().Height - 1;
}

/*
*/
static void RenderLine32_Decal(Image *t,
	const position2d<s32> &p0,
	const position2d<s32> &p1,
	u32 argb )
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 4;
	s32 yInc = (s32) t->GetPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u32 *dst;
	dst = (u32*) ( (u8*) t->Lock() + ( p0.Y * t->GetPitch() ) + ( p0.X << 2 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = argb;

		dst = (u32*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u32*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->Unlock();
}


/*
*/
static void RenderLine32_Blend(Image *t,
	const position2d<s32> &p0,
	const position2d<s32> &p1,
	u32 argb, u32 alpha)
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 4;
	s32 yInc = (s32) t->GetPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u32 *dst;
	dst = (u32*) ( (u8*) t->Lock() + ( p0.Y * t->GetPitch() ) + ( p0.X << 2 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = PixelBlend32 ( *dst, argb, alpha );

		dst = (u32*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u32*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->Unlock();
}


/*
*/
static void RenderLine16_Decal (Image *t,
	const position2d<s32> &p0,
	const position2d<s32> &p1,
	u32 argb )
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 2;
	s32 yInc = (s32) t->GetPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u16 *dst;
	dst = (u16*) ( (u8*) t->Lock() + ( p0.Y * t->GetPitch() ) + ( p0.X << 1 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = argb;

		dst = (u16*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u16*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->Unlock();
}

/*
*/
static void RenderLine16_Blend (Image *t,
	const position2d<s32> &p0,
	const position2d<s32> &p1,
	u32 argb,
	u32 alpha)
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 2;
	s32 yInc = (s32) t->GetPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u16 *dst;
	dst = (u16*) ( (u8*) t->Lock() + ( p0.Y * t->GetPitch() ) + ( p0.X << 1 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = PixelBlend16 ( *dst, argb, alpha );

		dst = (u16*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u16*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->Unlock();
}


/*!
*/
static void executeBlit_TextureCopy_x_to_x ( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	void *dst = (void*) job->dst;

	const u32 widthPitch = job->width * job->dstPixelMul;
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memcpy ( dst, src, widthPitch );

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (void*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureCopy_32_to_16 ( const SBlitJob * job )
{
	const u32 *src = (u32*) job->src;
	u16 *dst = (u16*) job->dst;

	u32 s;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			//16 bit Blitter depends on pre-multiplied color
			s = PixelLerp32 ( src[dx] | 0xFF000000, extractAlpha ( src[dx] ) );
			dst[dx] = A8R8G8B8toA1R5G5B5 ( s );
		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureCopy_24_to_16 ( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	u16 *dst = (u16*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = RGB16(s[0], s[1], s[2]);
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureCopy_16_to_32 ( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = A1R5G5B5toA8R8G8B8 ( src[dx] );
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureCopy_24_to_32 ( const SBlitJob * job )
{
	void *src = (void*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureBlend_16_to_16 ( const SBlitJob * job )
{
	u32 dx;
	s32 dy;

	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;


	const u32 rdx = job->width >> 1;
	const u32 off = if_c_a_else_b ( job->width & 1 ,job->width - 1, 0 );


	if ( 0 == off )
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd ( dst[dx], src[dx] );
			}

			src = (u32*) ( (u8*) (src) + job->srcPitch );
			dst = (u32*) ( (u8*) (dst) + job->dstPitch );
		}

	}
	else
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd ( dst[dx], src[dx] );
			}

			((u16*) dst)[off] = PixelBlend16 ( ((u16*) dst)[off], ((u16*) src)[off] );

			src = (u32*) ( (u8*) (src) + job->srcPitch );
			dst = (u32*) ( (u8*) (dst) + job->dstPitch );
		}

	}
}

/*!
*/
static void executeBlit_TextureBlend_32_to_32 ( const SBlitJob * job )
{
	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32 ( dst[dx], src[dx] );
		}
		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_TextureBlendColor_16_to_16 ( const SBlitJob * job )
{
	u16 *src = (u16*) job->src;
	u16 *dst = (u16*) job->dst;

	u16 blend = A8R8G8B8toA1R5G5B5 ( job->argb );
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16 ( dst[dx], PixelMul16_2 ( src[dx], blend ) );
		}
		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
*/
static void executeBlit_TextureBlendColor_32_to_32 ( const SBlitJob * job )
{
	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32 ( dst[dx], PixelMul32_2 ( src[dx], job->argb ) );
		}
		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_Color_16_to_16 ( const SBlitJob * job )
{
	u16 *dst = (u16*) job->dst;

	u16 c0 = A8R8G8B8toA1R5G5B5 ( job->argb );
	u32 c = c0 | c0 << 16;

	if ( 0 == (job->srcPitch & 3 ) )
	{
		for ( s32 dy = 0; dy != job->height; ++dy )
		{
			memset32 ( dst, c, job->srcPitch );
			dst = (u16*) ( (u8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		s32 dx = job->width - 1;

		for ( s32 dy = 0; dy != job->height; ++dy )
		{
			memset32 ( dst, c, job->srcPitch );
			dst[dx] = c0;
			dst = (u16*) ( (u8*) (dst) + job->dstPitch );
		}

	}
}

/*!
*/
static void executeBlit_Color_32_to_32 ( const SBlitJob * job )
{
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memset32 ( dst, job->argb, job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_16_to_16 ( const SBlitJob * job )
{
	u16 *dst = (u16*) job->dst;

	const u32 alpha = extractAlpha ( job->argb ) >> 3;
	const u32 src = A8R8G8B8toA1R5G5B5 ( job->argb );

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16 ( dst[dx], src, alpha );
		}
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static void executeBlit_ColorAlpha_32_to_32 ( const SBlitJob * job )
{
	u32 *dst = (u32*) job->dst;

	const u32 alpha = extractAlpha ( job->argb );
	const u32 src = job->argb;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32 ( dst[dx], src, alpha );
		}
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
*/
static tExecuteBlit getBlitter ( eBlitter operation,const Image * dest,const Image * source )
{
	ECOLOR_FORMAT sourceFormat = (ECOLOR_FORMAT) -1;
	ECOLOR_FORMAT destFormat = (ECOLOR_FORMAT) -1;

	if ( source )
		sourceFormat = source->GetColorFormat ();

	if ( dest )
		destFormat = dest->GetColorFormat ();

	switch ( operation )
	{
	case BLITTER_TEXTURE:
		{
			if ( sourceFormat == destFormat )
				return executeBlit_TextureCopy_x_to_x;

			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureCopy_32_to_16;

			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_R8G8B8 )
				return executeBlit_TextureCopy_24_to_16;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureCopy_16_to_32;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_R8G8B8 )
				return executeBlit_TextureCopy_24_to_32;

		} break;

	case BLITTER_TEXTURE_ALPHA_BLEND:
		{
			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureBlend_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureBlend_32_to_32;

		} break;

	case BLITTER_TEXTURE_ALPHA_COLOR_BLEND:
		{
			if ( destFormat == ECF_A1R5G5B5 && sourceFormat == ECF_A1R5G5B5 )
				return executeBlit_TextureBlendColor_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 && sourceFormat == ECF_A8R8G8B8 )
				return executeBlit_TextureBlendColor_32_to_32;
		} break;

	case BLITTER_COLOR:
		{
			if ( destFormat == ECF_A1R5G5B5 )
				return executeBlit_Color_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 )
				return executeBlit_Color_32_to_32;
		} break;

	case BLITTER_COLOR_ALPHA:
		{
			if ( destFormat == ECF_A1R5G5B5 )
				return executeBlit_ColorAlpha_16_to_16;

			if ( destFormat == ECF_A8R8G8B8 )
				return executeBlit_ColorAlpha_32_to_32;

		} break;

	case BLITTER_INVALID:
		break;
	}
	/*
	char buf[64];
	sprintf ( buf, "Blit: %d %d->%d unsupported",operation,sourceFormat,destFormat );
	os::Printer::log (buf );
	*/
	return 0;

}



/*!
a generic 2D Blitter
*/
static s32 Blit (	eBlitter operation,
	Image * dest,
	const rect<s32> *destClipping,
	const position2d<s32> *destPos,
	Image * source,
	const rect<s32> *sourceClipping,
	u32 argb
	)
{
	tExecuteBlit blitter = getBlitter ( operation, dest, source );
	if ( 0 == blitter )
	{
		return 0;
	}

	// Clipping
	AbsRectangle sourceClip;
	AbsRectangle destClip;
	AbsRectangle v;

	SBlitJob job;

	if ( sourceClipping )
	{
		sourceClip.x0 = sourceClipping->UpperLeftCorner.X;
		sourceClip.y0 = sourceClipping->UpperLeftCorner.Y;
		sourceClip.x1 = sourceClipping->LowerRightCorner.X;
		sourceClip.y1 = sourceClipping->LowerRightCorner.Y;
	}
	else
	{
		sourceClip.x0 = 0;
		sourceClip.y0 = 0;
		sourceClip.x1 = source ? source->GetDimension().Width : 0;
		sourceClip.y1 = source ? source->GetDimension().Height : 0;
	}

	if ( destClipping )
	{
		destClip.x0 = destClipping->UpperLeftCorner.X;
		destClip.y0 = destClipping->UpperLeftCorner.Y;
		destClip.x1 = destClipping->LowerRightCorner.X;
		destClip.y1 = destClipping->LowerRightCorner.Y;
	}
	else
	{
		destClip.x0 = 0;
		destClip.y0 = 0;
		destClip.x1 = dest ? dest->GetDimension().Width : 0;
		destClip.y1 = dest ? dest->GetDimension().Height : 0;
	}

	v.x0 = destPos ? destPos->X : 0;
	v.y0 = destPos ? destPos->Y : 0;
	v.x1 = v.x0 + ( sourceClip.x1 - sourceClip.x0 );
	v.y1 = v.y0 + ( sourceClip.y1 - sourceClip.y0 );

	intersect ( job.Dest, destClip, v );
	if ( !isValid ( job.Dest ) )
		return 0;

	job.width = job.Dest.x1 - job.Dest.x0;
	job.height = job.Dest.y1 - job.Dest.y0;


	job.Source.x0 = sourceClip.x0 + ( job.Dest.x0 - v.x0 );
	job.Source.x1 = job.Source.x0 + job.width;

	job.Source.y0 = sourceClip.y0 + ( job.Dest.y0 - v.y0 );
	job.Source.y1 = job.Source.y0 + job.height;

	job.argb = argb;

	if ( source )
	{
		job.srcPitch = source->GetPitch();
		job.srcPixelMul = source->GetBytesPerPixel();
		job.src = (void*) ( (u8*) source->Lock() + ( job.Source.y0 * job.srcPitch ) + ( job.Source.x0 * job.srcPixelMul ) );
	}
	else
	{
		// use srcPitch for color operation on dest
		job.srcPitch = job.width * dest->GetBytesPerPixel();
	}

	job.dstPitch = dest->GetPitch();
	job.dstPixelMul = dest->GetBytesPerPixel();
	job.dst = (void*) ( (u8*) dest->Lock() + ( job.Dest.y0 * job.dstPitch ) + ( job.Dest.x0 * job.dstPixelMul ) );

	blitter ( &job );

	if ( source )
		source->Unlock ();

	if ( dest )
		dest->Unlock();

	return 1;
}

}

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



//! constructor 
Image::Image(ECOLOR_FORMAT format, Image* imageToCopy)
	: m_pData(0), m_eFormat(format), m_bDeleteMemory(true)
{
	if (!imageToCopy)
		return;

	m_Size = imageToCopy->GetDimension();
	InitData();

	// now copy data from other image

	Blit ( BLITTER_TEXTURE, this, 0, 0, imageToCopy, 0,0 );
}



//! constructor
Image::Image(Image* imageToCopy, const position2d<s32>& pos,
	const dimension2d<s32>& size)
	: m_pData(0), m_Size(0,0), m_bDeleteMemory(true)
{
	if (!imageToCopy)
		return;

	m_eFormat = imageToCopy->GetColorFormat();
	m_Size = size;

	InitData();

	rect<s32> sClip( pos.X, pos.Y, pos.X + size.Width,pos.Y + size.Height );
	Blit (	BLITTER_TEXTURE, this, 0, 0, imageToCopy, &sClip,0 );
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

	//	os::Printer::log("Image: Unknown color format.", ELL_ERROR);
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

//! draws a rectangle
void Image::DrawRectangle(const rect<s32>& rect, const SColor &color)
{
	Blit (	color.getAlpha() == 0xFF ? BLITTER_COLOR : BLITTER_COLOR_ALPHA,
		this, 0, &rect.UpperLeftCorner, 0, &rect, color.color );
}


//! copies this surface into another
void Image::CopyTo(Image* target, const position2d<s32>& pos)
{
	Blit (	BLITTER_TEXTURE, target, 0, &pos, this, 0, 0 );
}


//! copies this surface into another
void Image::CopyTo(Image* target, const position2d<s32>& pos, const rect<s32>& sourceRect, const rect<s32>* clipRect)
{
	Blit (	BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0 );
}



//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
void Image::CopyToWithAlpha(Image* target, const position2d<s32>& pos, const rect<s32>& sourceRect, const SColor &color, const rect<s32>* clipRect)
{
	// color blend only necessary on not full spectrum aka. color.color != 0xFFFFFFFF
	Blit (	color.color == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND: BLITTER_TEXTURE_ALPHA_COLOR_BLEND,
		target, clipRect, &pos, this, &sourceRect, color.color );
}



//! draws a line from to with color
void Image::DrawLine(const position2d<s32>& from, const position2d<s32>& to, const SColor &color)
{
	AbsRectangle clip;
	GetClip ( clip, this );

	position2d<s32> p[2];

	if ( ClipLine ( clip, p[0], p[1], from, to ) )
	{
		u32 alpha = extractAlpha ( color.color );

		switch ( m_eFormat )
		{
		case ECF_A1R5G5B5:
			if ( alpha == 256 )
			{
				RenderLine16_Decal ( this, p[0], p[1], A8R8G8B8toA1R5G5B5 ( color.color ) );
			}
			else
			{
				RenderLine16_Blend ( this, p[0], p[1], A8R8G8B8toA1R5G5B5 ( color.color ), alpha >> 3 );
			}
			break;
		case ECF_A8R8G8B8:
			if ( alpha == 256 )
			{
				RenderLine32_Decal ( this, p[0], p[1], color.color );
			}
			else
			{
				RenderLine32_Blend ( this, p[0], p[1], color.color, alpha );
			}
			break;
		}
	}
}



//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void Image::CopyToScaling(Image* target)
{
	if (m_eFormat != target->GetColorFormat() )
	{
		//		os::Printer::log("Format not equal", ELL_ERROR);
		return;
	}

	dimension2d<s32> targetSize = target->GetDimension();

	if (!targetSize.Width || !targetSize.Height)
		return;

	f32 sourceXStep = (f32)m_Size.Width / (f32)targetSize.Width;
	f32 sourceYStep = (f32)m_Size.Height / (f32)targetSize.Height;
	f32 sx,sy;
	s32 bpp=target->GetBytesPerPixel();

	u8* nData = (u8*)target->Lock();

	if (targetSize==m_Size)
	{
		memcpy(nData,m_pData,targetSize.Width*targetSize.Height*bpp);
		target->Unlock();
		return;
	}

	sy = 0.0f;
	for (s32 y=0; y<targetSize.Height; ++y)
	{
		sx = 0.0f;
		for (s32 x=0; x<targetSize.Width; ++x)
		{
			memcpy(&nData[(y*targetSize.Width + x)*bpp], &((u8*)m_pData)[((s32)(((s32)sy)*m_Size.Width + sx))*bpp], bpp);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
	}

	target->Unlock();
}


//! fills the surface with given color
void Image::Fill(const SColor &color)
{
	u32 c;

	switch ( m_eFormat )
	{
	case ECF_A1R5G5B5:
		c = A8R8G8B8toA1R5G5B5 ( color.color );
		c |= c << 16;
		break;
	case ECF_A8R8G8B8:
		c = color.color;
		break;
	default:
		//			os::Printer::log("Image::Format not supported", ELL_ERROR);
		return;
	}

	memset32 ( m_pData, c, GetImageDataSizeInBytes () );
}


//! get a filtered pixel
inline SColor Image::GetPixelBox ( s32 x, s32 y, s32 fx, s32 fy, s32 bias )
{
	SColor c;
	s32 a = 0, r = 0, g = 0, b = 0;

	for ( s32 dx = 0; dx != fx; ++dx )
	{
		for ( s32 dy = 0; dy != fy; ++dy )
		{
			c = GetPixel ( x + dx , y + dy );

			a += c.getAlpha ();
			r += c.getRed();
			g += c.getGreen();
			b += c.getBlue();
		}
	}

	s32 sdiv = s32_log2_s32(fx * fy);

	a = s32_clamp ( ( a >> sdiv ) + bias, 0, 255 );
	r = s32_clamp ( ( r >> sdiv ) + bias, 0, 255 );
	g = s32_clamp ( ( g >> sdiv ) + bias, 0, 255 );
	b = s32_clamp ( ( b >> sdiv ) + bias, 0, 255 );

	c.set ( a, r, g, b );
	return c;
}


//! copies this surface into another, scaling it to fit it.
void Image::CopyToScalingBoxFilter(Image* target, s32 bias)
{
	ECOLOR_FORMAT destFormat = target->GetColorFormat();

	if (m_eFormat != destFormat )
	{
		//os::Printer::log("Format not equal", ELL_ERROR);
		return;
	}

	dimension2d<s32> destSize = target->GetDimension();

	f32 sourceXStep = (f32) m_Size.Width;
	f32 sourceYStep = (f32) m_Size.Height;

	sourceXStep /= (f32) ( destSize.Width - 0 );
	sourceYStep /= (f32) ( destSize.Height - 0 );

	s32 fx = ceil32 ( sourceXStep );
	s32 fy = ceil32 ( sourceYStep );
	f32 sx;
	f32 sy;
	SColor p;

	sy = 0.f;

	for ( s32 y = 0; y != destSize.Height; ++y )
	{
		sx = 0.f;
		for ( s32 x = 0; x != destSize.Width; ++x )
		{
			target->SetPixel ( x, y, GetPixelBox ( floor32 ( sx ), floor32 ( sy ), fx, fy, bias ) );

			sx += sourceXStep;
		}
		sy += sourceYStep;
	}

	target->Unlock ();

}


} // end namespace 
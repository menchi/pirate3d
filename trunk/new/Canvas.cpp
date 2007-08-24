#include "Canvas.h"
#include "VideoDriver.h"

//-----------------------------------------------------------------------------
Canvas& endf(Canvas& canvas)
{
	VideoDriverPtr pVideoDriver = canvas.GetVideoDriver();
	pVideoDriver->SwapBuffer();

	return canvas;
}
//-----------------------------------------------------------------------------
Canvas& erase(Canvas& canvas)
{
	VideoDriverPtr pVideoDriver = canvas.GetVideoDriver();
	pVideoDriver->Clear(true, true, true);

	return canvas;
}
//-----------------------------------------------------------------------------
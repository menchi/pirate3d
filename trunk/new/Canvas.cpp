#include "Canvas.h"

Canvas& EndFrame(Canvas& canvas)
{
	VideoDriverPtr pVideoDriver = canvas.GetVideoDriver();
	pVideoDriver->SwapBuffer();

	return canvas;
}
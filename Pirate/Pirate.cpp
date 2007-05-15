#include "Pirate.h"

namespace Pirate
{

DeviceWin32* InvokePirate(s32 Width, s32 Height, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync)
{
	DeviceWin32* pDeviceWin32 = new DeviceWin32(Width, Height, fullscreen, stencilbuffer, vsync, NULL);

	//error handle
	//if (pBigPirate && pBigPirate)

	return pDeviceWin32;
}

}

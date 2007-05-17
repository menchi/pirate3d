#include "Pirate.h"

namespace Pirate
{

DeviceWin32* InvokePirate(s32 Width, s32 Height, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync)
{
	DeviceWin32* dev = new DeviceWin32(Width, Height, fullscreen, stencilbuffer, vsync, NULL);

	if (dev && !dev->GetVideoDriver())
	{
		dev->CloseDevice(); // destroy window
		dev->Run(); // consume quit message
		dev->Drop();
		dev = 0;
	}

	return dev;
}

}

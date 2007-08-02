#include "DeviceWin32.h"
#include <crtdbg.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	DeviceWin32 Device(640, 480);

	Canvas& canvas = *Device.GetVideoDriver()->GetCanvas();
	while(Device.Run())
	{
		canvas << Canvas::Eraser << Canvas::EndFrame;
	}
}
#include "DeviceWin32.h"
#include <crtdbg.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	DeviceWin32 Device(640, 480);

	Canvas canvas = Device.GetCanvas();
	while(Device.Run())
	{
		canvas << BackGroundColor(Colorf(0.0f, 0.3f, 0.2f, 1.0f)) << Eraser() << WorkArea(160, 120, 320, 240) 
			   << BackGroundColor(Colorf(0.0f, 0.2f, 0.2f, 1.0f)) << Eraser() << EndFrame;
	}
}
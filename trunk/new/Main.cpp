#include "DeviceWin32.h"
#include <crtdbg.h>

DeviceWin32 g_Device(640, 480);

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	while(g_Device.Run())
	{

	}
}
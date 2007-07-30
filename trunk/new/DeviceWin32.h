#ifndef _PIRATE_DEVICE_WIN32_H_
#define _PIRATE_DEVICE_WIN32_H_

#include <Windows.h>

class DeviceWin32 {
public:
	DeviceWin32(int width, int height);
	~DeviceWin32();

	bool Run();

private:
	HWND m_HWnd;
};

#endif
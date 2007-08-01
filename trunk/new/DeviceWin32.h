#ifndef _PIRATE_DEVICE_WIN32_H_
#define _PIRATE_DEVICE_WIN32_H_

#include "D3D9Driver.h"
#include "boost/tr1/memory.hpp"

class DeviceWin32 {
public:
	DeviceWin32(int width, int height);
	~DeviceWin32();

	bool Run();

private:
	HWND m_HWnd;
	std::tr1::shared_ptr<D3D9Driver> m_pVideoDriver;
};

#endif
#ifndef _PIRATE_DEVICE_WIN32_H_
#define _PIRATE_DEVICE_WIN32_H_

#include "Canvas.h"

class DeviceWin32 {
public:
	DeviceWin32(int width, int height);
	~DeviceWin32();

	bool Run();
	VideoDriverPtr GetVideoDriver()	{ return m_pVideoDriver; }

	Canvas& GetCanvas() { return *m_pVideoDriver->GetCanvas(); }

private:
	HWND m_HWnd;
	VideoDriverPtr m_pVideoDriver;
};

#endif
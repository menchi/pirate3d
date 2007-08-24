#ifndef _PIRATE_DEVICE_WIN32_H_
#define _PIRATE_DEVICE_WIN32_H_

#include "VideoDriverFwd.h"

FWD_DECLARE(Canvas)

class DeviceWin32 {
public:
	DeviceWin32(int width, int height);
	~DeviceWin32();

	bool Run();
	VideoDriverPtr GetVideoDriver()	{ return m_pVideoDriver; }

	Canvas& GetCanvas() { return *m_pCanvas; }

private:
	HWND m_HWnd;
	VideoDriverPtr m_pVideoDriver;
	CanvasPtr m_pCanvas;
};

#endif
#ifndef _PIRATE_D3D9_DRIVER_H_
#define _PIRATE_D3D9_DRIVER_H_

#include "CompileConfig.h"

DEFINE_SMART_PTR(IDirect3D9)
DEFINE_SMART_PTR(IDirect3DDevice9)
DEFINE_SMART_PTR(IDirect3DSurface9)

FWD_DECLARE(Canvas);
FWD_DECLARE(D3D9Driver);

class D3D9Driver {
public:
	typedef IDirect3DSurface9 RenderTargetType;
	DEFINE_SMART_PTR(RenderTargetType)

	CanvasPtr GetCanvas() { return m_pCanvas; }

	void SetBackgroundColor(Colorf color) { m_BackgroundColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a); }
	void Clear(bool color, bool z, bool stencil);
	void SetViewport(int x, int y, int w, int h);
	void SwapBuffer() {	m_pID3DDevice->Present(NULL, NULL, NULL, NULL);	}

	static D3D9DriverPtr CreateVideoDriver(HWND window, int width, int height, bool fullScreen);

private:
	D3D9Driver(HWND window, int width, int height, bool fullScreen);

	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;
	D3DCOLOR m_BackgroundColor;

	IDirect3D9Ptr m_pID3D9;
	IDirect3DDevice9Ptr m_pID3DDevice;

	CanvasPtr m_pCanvas;
};

#endif
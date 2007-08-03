#ifndef _PIRATE_OPENGL_DRIVER_H_
#define _PIRATE_OPENGL_DRIVER_H_

#include "CompileConfig.h"

FWD_DECLARE(Canvas)
FWD_DECLARE(OpenGLDriver)

class OpenGLDriver {
public:
	TYPE_DEFINE_WITH_SMART_PTR(HDC, RenderTargetType);

	CanvasPtr GetCanvas() { return m_pCanvas; }

	void SetBackgroundColor(Colorf color) { glClearColor(color.r, color.g, color.b, color.a); }
	void Clear(bool color, bool z, bool stencil);
	void SetViewport(int x, int y, int w, int h);
	void SwapBuffer() {	SwapBuffers(m_hDC); }

	static OpenGLDriverPtr CreateVideoDriver(HWND window, int width, int height, bool fullScreen);

private:
	OpenGLDriver(HWND window, int width, int height, bool fullScreen);

	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;
	HDC m_hDC;

	CanvasPtr m_pCanvas;
};

#endif
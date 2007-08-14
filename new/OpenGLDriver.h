#ifndef _PIRATE_OPENGL_DRIVER_H_
#define _PIRATE_OPENGL_DRIVER_H_

#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

FWD_DECLARE(Canvas)
FWD_DECLARE(OpenGLDriver)
TYPE_DEFINE_WITH_SMART_PTR(OpenGLDriver, VideoDriver)

class OpenGLDriver {
public:
	~OpenGLDriver();

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
	HWND m_Window;
	HDC m_hDC;
	HGLRC m_hRC;

	CanvasPtr m_pCanvas;
};

#endif

#endif
#include "OpenGLDriver.h"

OpenGLDriver::OpenGLDriver(HWND window, int width, int height, bool fullScreen)
: m_iWidth(width), m_iHeight(height), m_bIsFullScreen(fullScreen)
{

}

OpenGLDriverPtr OpenGLDriver::CreateVideoDriver(HWND window, int width, int height, bool fullScreen)
{
	OpenGLDriverPtr pDriver(new OpenGLDriver(window, width, height, fullScreen));

	return pDriver;
}

void OpenGLDriver::Clear(bool color, bool z, bool stencil)
{
	GLbitfield flag = 0;
	flag = (color)? flag | GL_COLOR_BUFFER_BIT : flag;
	flag = (z)? flag | GL_DEPTH_BUFFER_BIT : flag;
	flag = (stencil)? flag | GL_STENCIL_BUFFER_BIT : flag;
	glClear(flag);
}

void OpenGLDriver::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}
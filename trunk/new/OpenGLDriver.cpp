#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include "OpenGLDriver.h"
#include "Canvas.h"
#include <iostream>

OpenGLDriver::OpenGLDriver(HWND window, int width, int height, bool fullScreen)
: m_iWidth(width), m_iHeight(height), m_bIsFullScreen(fullScreen), m_Window(window)
{
	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		32,																// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	m_hDC = GetDC (window);
	if (!m_hDC)
		std::cerr << "Failed to get device context" << std::endl;

	GLuint PixelFormat = ChoosePixelFormat (m_hDC, &pfd);		
	if (!PixelFormat)
		std::cerr << "Failed to find a compatible pixel format" << std::endl;

	if (SetPixelFormat(m_hDC, PixelFormat, &pfd) == FALSE)
		std::cerr << "Failed to set pixel format" << std::endl;

	m_hRC = wglCreateContext(m_hDC);
	if (!m_hRC)
		std::cerr << "Failed to create rendering context" << std::endl;

	if (wglMakeCurrent (m_hDC, m_hRC) == FALSE)
		std::cerr << "Failed to bind rendering context" << std::endl;
}

OpenGLDriver::~OpenGLDriver()
{
	wglMakeCurrent(m_hDC, 0);
	wglDeleteContext(m_hRC);
	ReleaseDC(m_Window, m_hDC);
}

OpenGLDriverPtr OpenGLDriver::CreateVideoDriver(HWND window, int width, int height, bool fullScreen)
{
	OpenGLDriverPtr pDriver(new OpenGLDriver(window, width, height, fullScreen));
	pDriver->m_pCanvas.reset(new Canvas(pDriver));

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
	glScissor(x, y, w, h);
	glEnable(GL_SCISSOR_TEST);
}

#endif
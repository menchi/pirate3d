#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include "OpenGLDriver.h"
#include "Canvas.h"
#include <iostream>
#include <fstream>

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

	if (glewInit() != GLEW_OK)
		std::cerr << "Failed to initialize GLEW" << std::endl;

	glFrontFace(GL_CW);
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

DriverVertexBufferPtr OpenGLDriver::CreateVertexBuffer(unsigned int NumVertices, unsigned int VertexSize)
{
	return DriverVertexBufferPtr(new DriverVertexBuffer(NumVertices, VertexSize));
}

DriverIndexBufferPtr OpenGLDriver::CreateIndexBuffer(unsigned int NumIndices)
{
	return DriverIndexBufferPtr(new DriverIndexBuffer(NumIndices));
}

DriverVertexDeclarationPtr OpenGLDriver::CreateVertexDeclaration(StreamIndexVertexBufferPair* ppVertexBuffers, unsigned int NumVertexBuffers)
{
	return DriverVertexDeclarationPtr(new DriverVertexDeclaration(ppVertexBuffers, NumVertexBuffers));
}

bool OpenGLDriver::CreateVertexShaderFragmentsFromFile(const char* FileName, const char** ppFragmentNames, VertexShaderFragmentPtr* ppFragments, 
													   unsigned int NumFragments)
{
	using namespace std;

	ifstream file(FileName);
	if (!file.is_open())
		return false;

	file.seekg(0, ios::end);
	streampos pos = file.tellg();
	streamsize size = pos;
	file.seekg(0);
	string s;
	s.resize(size+1);
	file.read(&s[0], size);

	ppFragments[0] = VertexShaderFragmentPtr(new VertexShaderFragment(s));
	
	return true;
}

bool OpenGLDriver::CreatePixelShaderFragmentsFromFile(const char* FileName, const char** ppFragmentNames, PixelShaderFragmentPtr* ppFragments, 
													  unsigned int NumFragments)
{
	using namespace std;

	ifstream file(FileName);
	if (!file.is_open())
		return false;

	file.seekg(0, ios::end);
	streampos pos = file.tellg();
	streamsize size = pos;
	file.seekg(0);
	string s;
	s.resize(size+1);
	file.read(&s[0], size);

	ppFragments[0] = PixelShaderFragmentPtr(new PixelShaderFragment(s));

	return true;
}

VertexShaderPtr OpenGLDriver::CreateVertexShader(VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments)
{
	return VertexShaderPtr(new VertexShader(ppFragments, NumFragments));
}

PixelShaderPtr OpenGLDriver::CreatePixelShader(PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments)
{
	return PixelShaderPtr(new PixelShader(ppFragments, NumFragments));
}

ShaderProgramPtr OpenGLDriver::CreateShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader)
{
	return ShaderProgramPtr(new ShaderProgram(pVertexShader, pPixelShader));
}

void OpenGLDriver::SetVertexDeclaration(DriverVertexDeclarationPtr pVertexDeclaration)
{
	if (pVertexDeclaration->m_pVertex)
		glEnableClientState(GL_VERTEX_ARRAY);
	else
		glDisableClientState(GL_VERTEX_ARRAY);

	if (pVertexDeclaration->m_pNormal) 
		glEnableClientState(GL_NORMAL_ARRAY);
	else
		glDisableClientState(GL_NORMAL_ARRAY);

	if (pVertexDeclaration->m_pColor) 
		glEnableClientState(GL_COLOR_ARRAY);
	else
		glDisableClientState(GL_COLOR_ARRAY);

	for (unsigned int i=0; i<MAX_TEXTURE_UNIT; ++i)
	{
		if (pVertexDeclaration->m_ppTexCoords[i]) 
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	m_pCurVertexDeclaration = pVertexDeclaration;
}

void OpenGLDriver::SetVertexStream(unsigned int StreamNumber, DriverVertexBufferPtr pVertexBuffer, unsigned int Stride)
{
	glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffer->m_uiVertexBufferID);

	const DriverVertexDeclaration::VertexParam* pParam = m_pCurVertexDeclaration->m_pVertex;

	if (pParam && pParam->Index == StreamNumber)
		glVertexPointer(pParam->Size, pParam->Type, pParam->Stride, pParam->Pointer);

	pParam = m_pCurVertexDeclaration->m_pNormal;
	if (pParam && pParam->Index == StreamNumber)
		glNormalPointer(pParam->Type, pParam->Stride, pParam->Pointer);

	pParam = m_pCurVertexDeclaration->m_pColor;
	if (pParam && pParam->Index == StreamNumber)
		glColorPointer(pParam->Size, pParam->Type, pParam->Stride, pParam->Pointer);

	for (unsigned int i=0; i<MAX_TEXTURE_UNIT; ++i)
	{
		pParam = m_pCurVertexDeclaration->m_ppTexCoords[i];
		if (pParam && pParam->Index == StreamNumber) 
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			glTexCoordPointer(pParam->Size, pParam->Type, pParam->Stride, pParam->Pointer);
		}
	}
}

void OpenGLDriver::DrawIndexedTriangleList(DriverIndexBufferPtr pIndexBuffer, unsigned int NumVertices, unsigned int TriangleCount)
{

}

void OpenGLDriver::SetShaderProgram(ShaderProgramPtr pShaderProgram)
{

}

#endif
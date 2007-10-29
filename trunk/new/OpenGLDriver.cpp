#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include "OpenGLDriver.h"
#include "OpenGLDriverResources.h"
#include "boost/tr1/functional.hpp"
#include <iostream>
#include <fstream>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

using namespace std;

//-----------------------------------------------------------------------------
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
		cerr << "Failed to get device context" << endl;

	GLuint PixelFormat = ChoosePixelFormat (m_hDC, &pfd);		
	if (!PixelFormat)
		cerr << "Failed to find a compatible pixel format" << std::endl;

	if (SetPixelFormat(m_hDC, PixelFormat, &pfd) == FALSE)
		cerr << "Failed to set pixel format" << endl;

	m_hRC = wglCreateContext(m_hDC);
	if (!m_hRC)
		cerr << "Failed to create rendering context" << std::endl;

	if (wglMakeCurrent (m_hDC, m_hRC) == FALSE)
		cerr << "Failed to bind rendering context" << std::endl;

	if (glewInit() != GLEW_OK)
		cerr << "Failed to initialize GLEW" << endl;

	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
}
//-----------------------------------------------------------------------------
OpenGLDriver::~OpenGLDriver()
{
	wglMakeCurrent(m_hDC, 0);
	wglDeleteContext(m_hRC);
	ReleaseDC(m_Window, m_hDC);
}
//-----------------------------------------------------------------------------
OpenGLDriverPtr OpenGLDriver::CreateVideoDriver(HWND window, int width, int height, bool fullScreen)
{
	OpenGLDriverPtr pDriver(new OpenGLDriver(window, width, height, fullScreen));

	return pDriver;
}
//-----------------------------------------------------------------------------
void OpenGLDriver::Clear(bool color, bool z, bool stencil)
{
	GLbitfield flag = 0;
	flag = (color)? flag | GL_COLOR_BUFFER_BIT : flag;
	flag = (z)? flag | GL_DEPTH_BUFFER_BIT : flag;
	flag = (stencil)? flag | GL_STENCIL_BUFFER_BIT : flag;
	glClear(flag);
}
//-----------------------------------------------------------------------------
void OpenGLDriver::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
	glScissor(x, y, w, h);
	glEnable(GL_SCISSOR_TEST);
}
//-----------------------------------------------------------------------------
DriverVertexBufferPtr OpenGLDriver::CreateVertexBuffer(unsigned int NumVertices, unsigned int VertexSize)
{
	return DriverVertexBufferPtr(new DriverVertexBuffer(NumVertices, VertexSize));
}
//-----------------------------------------------------------------------------
DriverIndexBufferPtr OpenGLDriver::CreateIndexBuffer(unsigned int NumIndices)
{
	return DriverIndexBufferPtr(new DriverIndexBuffer(NumIndices));
}
//-----------------------------------------------------------------------------
DriverVertexDeclarationPtr OpenGLDriver::CreateVertexDeclaration(const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats)
{
	return DriverVertexDeclarationPtr(new DriverVertexDeclaration(StreamIndices, VertexFormats));
}
//-----------------------------------------------------------------------------
OpenGLDriver::VertexShaderFragmentArray OpenGLDriver::CreateVertexShaderFragmentsFromFile(const string& FileName, const NameArray& FragmentNames)
{
	ifstream file(FileName.c_str());
	VertexShaderFragmentArray r;
	if (!file.is_open())
	{
		cerr << "Open file failed: " << FileName << endl;
		return r;
	}

	file.seekg(0, ios::end);
	streampos pos = file.tellg();
	streamsize size = pos;
	file.seekg(0);
	string s;
	char c;
	s.resize(size+1);

	NameArray::size_type count = FragmentNames.size();
	r.resize(count);

	while (file)
	{
		file.get(&s[0], size, '<');
		file.get(c);
		file.get(&s[0], size, ' ');
		if (string(s.c_str()) != "GLSLBegin")
			break;

		file.get(&s[0], size, '=');
		if (s.find("Name") == string::npos)
		{
			cerr << "No Name attribute to identify GLSL code" << endl;
			break;
		}
		file.get(c);
		file.get(&s[0], size, '"');
		file.get(c);
		file.get(&s[0], size, '"');
		unsigned int i=0;
		for (i=0; i<FragmentNames.size(); ++i)
		{
			string text(s.c_str());
			if (FragmentNames[i] == text)
				break;
		}
		if (!file.get(&s[0], size, '>'))
			break;
		file.get(c);

		string source;
		source.resize(size+1);
		file.get(&source[0], size, '<');

		if (!file.get(&s[0], size, '/'))
		{
			cerr << "No </GLSL> tag?" << endl;
			break;
		}
		file.get(&s[0], size, '>');
		if (string(s.c_str()) != string("/GLSLEnd"))
		{
			cerr << "No </GLSL> tag?" << endl;
			break;
		}

		if (i == FragmentNames.size())
			continue;

		if (r[i])
		{
			cerr << "Name conflict in the file" << endl;
			break;
		}

		r[i].reset(new VertexShaderFragment(source));
		--count;
	}

	if (count)
	{
		cerr << "Some shader fragment not found" << endl;
		r.clear();
		return r;
	}

	return r;
}
//-----------------------------------------------------------------------------
OpenGLDriver::PixelShaderFragmentArray OpenGLDriver::CreatePixelShaderFragmentsFromFile(const string& FileName, const NameArray& FragmentNames)
{
	using namespace std;

	ifstream file(FileName.c_str());
	PixelShaderFragmentArray r;
	if (!file.is_open())
	{
		cerr << "Open file failed: " << FileName << endl;
		return r;
	}

	file.seekg(0, ios::end);
	streampos pos = file.tellg();
	streamsize size = pos;
	file.seekg(0);
	string s;
	char c;
	s.resize(size+1);

	NameArray::size_type count = FragmentNames.size();
	r.resize(count);

	while (file)
	{
		file.get(&s[0], size, '<');
		file.get(c);
		file.get(&s[0], size, ' ');
		if (string(s.c_str()) != "GLSLBegin")
			break;

		file.get(&s[0], size, '=');
		if (s.find("Name") == string::npos)
		{
			cerr << "No Name attribute to identify GLSL code" << endl;
			break;
		}
		file.get(c);
		file.get(&s[0], size, '"');
		file.get(c);
		file.get(&s[0], size, '"');
		unsigned int i=0;
		for (i=0; i<FragmentNames.size(); ++i)
		{
			string text(s.c_str());
			if (FragmentNames[i] == text)
				break;
		}
		if (!file.get(&s[0], size, '>'))
			break;
		file.get(c);

		string source;
		source.resize(size+1);
		file.get(&source[0], size, '<');

		if (!file.get(&s[0], size, '/'))
		{
			cerr << "No </GLSL> tag?" << endl;
			break;
		}
		file.get(&s[0], size, '>');
		if (string(s.c_str()) != string("/GLSLEnd"))
		{
			cerr << "No </GLSL> tag?" << endl;
			break;
		}

		if (i == FragmentNames.size())
			continue;

		if (r[i])
		{
			cerr << "Name conflict in the file" << endl;
			break;
		}

		r[i].reset(new PixelShaderFragment(source));
		--count;
	}

	if (count)
	{
		cerr << "Some shader fragment not found" << endl;
		r.clear();
		return r;
	}

	return r;
}
//-----------------------------------------------------------------------------
VertexShaderPtr OpenGLDriver::CreateVertexShader(const VertexShaderFragmentArray& Fragments)
{
	return VertexShaderPtr(new VertexShader(Fragments));
}
//-----------------------------------------------------------------------------
PixelShaderPtr OpenGLDriver::CreatePixelShader(const PixelShaderFragmentArray& Fragments)
{
	return PixelShaderPtr(new PixelShader(Fragments));
}
//-----------------------------------------------------------------------------
ShaderProgramPtr OpenGLDriver::CreateShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader)
{
	return ShaderProgramPtr(new ShaderProgram(pVertexShader, pPixelShader));
}
//-----------------------------------------------------------------------------
void OpenGLDriver::SetVertexDeclaration(DriverVertexDeclarationPtr pVertexDeclaration)
{
	if (pVertexDeclaration == m_pCurVertexDeclaration)
		return;

	using namespace std;
	vector<tr1::function<void ()> >& functions = pVertexDeclaration->m_ClientStateFunctions;
	for_each(functions.begin(), functions.end(), mem_fun_ref(&tr1::function<void ()>::operator()));

	m_pCurVertexDeclaration = pVertexDeclaration;
}
//-----------------------------------------------------------------------------
void OpenGLDriver::SetVertexStream(unsigned int StreamNumber, DriverVertexBufferPtr pVertexBuffer, unsigned int Stride)
{
	using namespace std;

	glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffer->m_uiVertexBufferID);

	DriverVertexDeclaration::PointerFunctionArray functions = m_pCurVertexDeclaration->m_PointerFunctions;
	for_each(functions.begin(), functions.end(), bind2nd(mem_fun_ref(&DriverVertexDeclaration::PointerFunction::operator()), Stride));
}
//-----------------------------------------------------------------------------
void OpenGLDriver::DrawIndexedTriangleList(DriverIndexBufferPtr pIndexBuffer, unsigned int NumVertices, unsigned int TriangleCount)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexBuffer->m_uiIndexBufferID);
	glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, BufferObjectPtr(0));
}
//-----------------------------------------------------------------------------
void OpenGLDriver::SetShaderProgram(ShaderProgramPtr pShaderProgram)
{
	glUseProgram(pShaderProgram->m_uiGLShaderProgram);
}
//-----------------------------------------------------------------------------
void OpenGLDriver::Begin()
{
}
//-----------------------------------------------------------------------------
void OpenGLDriver::End()
{
}
//-----------------------------------------------------------------------------

#endif
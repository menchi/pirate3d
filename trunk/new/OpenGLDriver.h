#ifndef _PIRATE_OPENGL_DRIVER_H_
#define _PIRATE_OPENGL_DRIVER_H_

#include "CompileConfig.h"
#include "OpenGLWrapper.h"
#include "OpenGLVertexFormat.h"
#include <vector>

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)
FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

FWD_DECLARE(OpenGLDriver)
TYPE_DEFINE_WITH_SMART_PTR(OpenGLDriver, VideoDriver)

class OpenGLDriver {
public:
	typedef std::vector<unsigned short> StreamIndexArray;
	typedef std::vector<const VertexFormat*> VertexFormatArray;
	typedef std::vector<VertexShaderFragmentPtr> VertexShaderFragmentArray;
	typedef std::vector<PixelShaderFragmentPtr> PixelShaderFragmentArray;
	typedef std::vector<std::string> NameArray;

	~OpenGLDriver();

	DriverVertexBufferPtr CreateVertexBuffer(unsigned int NumVertices, unsigned int VertexSize);
	DriverIndexBufferPtr CreateIndexBuffer(unsigned int NumIndices);
	DriverVertexDeclarationPtr CreateVertexDeclaration(const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats);

	VertexShaderFragmentArray CreateVertexShaderFragmentsFromFile(const std::string& FileName, const NameArray& FragmentNames);
	PixelShaderFragmentArray CreatePixelShaderFragmentsFromFile(const std::string& FileName, const NameArray& FragmentNames);
	VertexShaderPtr CreateVertexShader(const VertexShaderFragmentArray& Fragments);
	PixelShaderPtr CreatePixelShader(const PixelShaderFragmentArray& Fragments);
	ShaderProgramPtr CreateShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader);

	void SetBackgroundColor(Colorf color) { glClearColor(color.r, color.g, color.b, color.a); }
	void SetViewport(int x, int y, int w, int h);
	void SetVertexDeclaration(DriverVertexDeclarationPtr pVertexDeclaration);
	void SetVertexStream(unsigned int StreamNumber, DriverVertexBufferPtr pVertexBuffer, unsigned int Stride);
	void DrawIndexedTriangleList(DriverIndexBufferPtr pIndexBuffer, unsigned int NumVertices, unsigned int TriangleCount);
	void SetShaderProgram(ShaderProgramPtr pShaderProgram);

	void Clear(bool color, bool z, bool stencil);
	void SwapBuffer() {	SwapBuffers(m_hDC); }

	void Begin();
	void End();

	static OpenGLDriverPtr CreateVideoDriver(HWND window, int width, int height, bool fullScreen);

private:
	OpenGLDriver(HWND window, int width, int height, bool fullScreen);

	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;
	HWND m_Window;
	HDC m_hDC;
	HGLRC m_hRC;

	DriverVertexDeclarationPtr m_pCurVertexDeclaration;
};

#endif
#ifndef _PIRATE_OPENGL_DRIVER_H_
#define _PIRATE_OPENGL_DRIVER_H_

#include "CompileConfig.h"
#include "OpenGLDriverResources.h"

FWD_DECLARE(Canvas)
FWD_DECLARE(OpenGLDriver)
TYPE_DEFINE_WITH_SMART_PTR(OpenGLDriver, VideoDriver)

class OpenGLDriver {
public:
	~OpenGLDriver();

	CanvasPtr GetCanvas() { return m_pCanvas; }

	DriverVertexBufferPtr CreateVertexBuffer(unsigned int NumVertices, unsigned int VertexSize);
	DriverIndexBufferPtr CreateIndexBuffer(unsigned int NumIndices);
	DriverVertexDeclarationPtr CreateVertexDeclaration(StreamIndexVertexBufferPair* ppVertexBuffers, unsigned int NumVertexBuffers);

	bool CreateVertexShaderFragmentsFromFile(const char* FileName, const char** ppFragmentNames, VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	bool CreatePixelShaderFragmentsFromFile(const char* FileName, const char** ppFragmentNames, PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	VertexShaderPtr CreateVertexShader(VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	PixelShaderPtr CreatePixelShader(PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	ShaderProgramPtr CreateShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader);

	void SetBackgroundColor(Colorf color) { glClearColor(color.r, color.g, color.b, color.a); }
	void SetViewport(int x, int y, int w, int h);
	void SetVertexDeclaration(DriverVertexDeclarationPtr pVertexDeclaration);
	void SetVertexStream(unsigned int StreamNumber, DriverVertexBufferPtr pVertexBuffer, unsigned int Stride);
	void DrawIndexedTriangleList(DriverIndexBufferPtr pIndexBuffer, unsigned int NumVertices, unsigned int TriangleCount);
	void SetShaderProgram(ShaderProgramPtr pShaderProgram);

	void Clear(bool color, bool z, bool stencil);
	void SwapBuffer() {	SwapBuffers(m_hDC); }

	static OpenGLDriverPtr CreateVideoDriver(HWND window, int width, int height, bool fullScreen);

private:
	OpenGLDriver(HWND window, int width, int height, bool fullScreen);

	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;
	HWND m_Window;
	HDC m_hDC;
	HGLRC m_hRC;

	DriverVertexDeclarationPtr m_pCurVertexDeclaration;

	CanvasPtr m_pCanvas;
};

#endif
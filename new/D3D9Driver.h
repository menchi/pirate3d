#ifndef _PIRATE_D3D9_DRIVER_H_
#define _PIRATE_D3D9_DRIVER_H_

#include "CompileConfig.h"
#include "D3D9DriverResources.h"
#include <set>
#include <string>

FWD_DECLARE(Canvas)
FWD_DECLARE(D3D9Driver)
TYPE_DEFINE_WITH_SMART_PTR(D3D9Driver, VideoDriver)

class D3D9Driver {
public:
	~D3D9Driver();

	CanvasPtr GetCanvas() { return m_pCanvas; }

	DriverVertexBufferPtr CreateVertexBuffer(int size);
	DriverIndexBufferPtr CreateIndexBuffer(int size);
	DriverVertexDeclarationPtr CreateVertexDeclaration(MeshBufferPtr pMeshBuffer);

	bool CreateVertexShaderFragmentsFromFile(const char* FileName, const char** ppFragmentNames, VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	bool CreatePixelShaderFragmentsFromFile(const char* FileName, const char** ppFragmentNames, PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	VertexShaderPtr CreateVertexShader(VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	PixelShaderPtr CreatePixelShader(PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments);
	ShaderProgramPtr CreateShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader);

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
	ID3DXFragmentLinkerPtr m_pFragmentLinker;

	typedef std::set<std::string> NameSet;
	NameSet m_LoadedShaderSources;

	CanvasPtr m_pCanvas;
};

#endif
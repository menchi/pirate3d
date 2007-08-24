#ifndef _PIRATE_D3D9_DRIVER_H_
#define _PIRATE_D3D9_DRIVER_H_

#include "CompileConfig.h"
#include "D3D9Wrapper.h"
#include "BaseTypes.h"
#include <set>
#include <vector>
#include <string>
 
struct VertexElement;

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)

FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

FWD_DECLARE(D3D9Driver)
TYPE_DEFINE_WITH_SMART_PTR(D3D9Driver, VideoDriver)

class D3D9Driver {
public:
	~D3D9Driver();

	typedef std::vector<VertexElement> VertexElementArray;
	typedef std::vector<unsigned short> StreamIndexArray;
	typedef std::vector<const VertexElementArray*> VertexFormatArray;
	typedef std::vector<VertexShaderFragmentPtr> VertexShaderFragmentArray;
	typedef std::vector<PixelShaderFragmentPtr> PixelShaderFragmentArray;
	typedef std::vector<std::string> NameArray;

	DriverVertexBufferPtr CreateVertexBuffer(unsigned int NumVertices, unsigned int VertexSize);
	DriverIndexBufferPtr CreateIndexBuffer(unsigned int NumIndices);
	DriverVertexDeclarationPtr CreateVertexDeclaration(const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats);

	VertexShaderFragmentArray CreateVertexShaderFragmentsFromFile(const std::string& FileName, const NameArray& FragmentNames);
	PixelShaderFragmentArray CreatePixelShaderFragmentsFromFile(const std::string& FileName, const NameArray& FragmentNames);
	VertexShaderPtr CreateVertexShader(const VertexShaderFragmentArray& Fragments);
	PixelShaderPtr CreatePixelShader(const PixelShaderFragmentArray& Fragments);
	ShaderProgramPtr CreateShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader);

	void SetBackgroundColor(Colorf color) { m_BackgroundColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a); }
	void SetViewport(int x, int y, int w, int h);
	void SetVertexDeclaration(DriverVertexDeclarationPtr pVertexDeclaration);
	void SetVertexStream(unsigned int StreamNumber, DriverVertexBufferPtr pVertexBuffer, unsigned int Stride);
	void DrawIndexedTriangleList(DriverIndexBufferPtr pIndexBuffer, unsigned int NumVertices, unsigned int TriangleCount);
	void SetShaderProgram(ShaderProgramPtr pShaderProgram);

	void Clear(bool color, bool z, bool stencil);
	void SwapBuffer() {	m_pID3DDevice->Present(NULL, NULL, NULL, NULL);	}

	void Begin();
	void End();

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
};

#endif
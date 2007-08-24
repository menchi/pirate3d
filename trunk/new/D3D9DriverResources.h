#ifndef _PIRATE_D3D9_DRIVER_RESOURCES_H_
#define _PIRATE_D3D9_DRIVER_RESOURCES_H_

#include "D3D9Wrapper.h"
#include "SmartPointer.h"
#include <vector>

struct VertexElement;

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)

FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

class DriverVertexBuffer {
public:
	void Fill(void* pData, unsigned int Size);

private:
	DriverVertexBuffer(IDirect3DDevice9Ptr pD3DDevice, unsigned int NumVertices, unsigned int VertexSize);

	IDirect3DVertexBuffer9Ptr m_pID3DVertexBuffer;
	unsigned int m_uiVertexSize;

	friend class D3D9Driver;
};

class DriverIndexBuffer {
public:
	void Fill(void* pData, unsigned int Size);

private:
	DriverIndexBuffer(IDirect3DDevice9Ptr pD3DDevice, unsigned int NumIndices);

	IDirect3DIndexBuffer9Ptr m_pID3DIndexBuffer;

	friend class D3D9Driver;
};

class DriverVertexDeclaration {
private:
	typedef std::vector<VertexElement> VertexElementArray;

	typedef std::vector<unsigned short> StreamIndexArray;
	typedef std::vector<const VertexElementArray*> VertexFormatArray;
	typedef std::vector<unsigned short> NumVertexElementsArray;

	DriverVertexDeclaration(IDirect3DDevice9Ptr pD3DDevice, const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats);

	IDirect3DVertexDeclaration9Ptr m_pID3DVertexDeclaration;

	friend class D3D9Driver;
};

class VertexShaderFragment {
private:
	VertexShaderFragment(ID3DXFragmentLinkerPtr pLinker, const std::string& Name);

	D3DXHANDLE m_hD3DXFragment;

	friend class D3D9Driver;
	friend class VertexShader;
};

class VertexShader {
private:
	typedef std::vector<VertexShaderFragmentPtr> VertexShaderFragmentArray;

	VertexShader(ID3DXFragmentLinkerPtr pLinker, const VertexShaderFragmentArray& Fragments);

	IDirect3DVertexShader9Ptr m_pID3DVertexShader;

	friend class D3D9Driver;
};

class PixelShaderFragment {
private:
	PixelShaderFragment(ID3DXFragmentLinkerPtr pLinker, const std::string& Name);

	D3DXHANDLE m_hD3DXFragment;

	friend class D3D9Driver;
	friend class PixelShader;
};

class PixelShader {
private:
	typedef std::vector<PixelShaderFragmentPtr> PixelShaderFragmentArray;

	PixelShader(ID3DXFragmentLinkerPtr pLinker, const PixelShaderFragmentArray& Fragments);

	IDirect3DPixelShader9Ptr m_pID3DPixelShader;

	friend class D3D9Driver;
};

class ShaderProgram {
private:
	ShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader);

	VertexShaderPtr m_pVertexShader;
	PixelShaderPtr m_pPixelShader;

	friend class D3D9Driver;
};

#endif
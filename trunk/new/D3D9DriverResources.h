#ifndef _PIRATE_D3D9_DRIVER_RESOURCES_H_
#define _PIRATE_D3D9_DRIVER_RESOURCES_H_

#include "D3D9Wrapper.h"
#include "SmartPointer.h"

FWD_DECLARE(VertexBuffer)

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)

FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

typedef std::pair<unsigned short, VertexBufferPtr> StreamIndexVertexBufferPair;

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
	DriverVertexDeclaration(IDirect3DDevice9Ptr pD3DDevice, StreamIndexVertexBufferPair* ppVertexBuffers, unsigned int NumVertexBuffers);

	IDirect3DVertexDeclaration9Ptr m_pID3DVertexDeclaration;

	friend class D3D9Driver;
};

class VertexShaderFragment {
private:
	VertexShaderFragment(ID3DXFragmentLinker* pLinker, const char* Name);

	D3DXHANDLE m_hD3DXFragment;

	friend class D3D9Driver;
	friend class VertexShader;
};

class VertexShader {
private:
	VertexShader(ID3DXFragmentLinker* pLinker, VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments);

	IDirect3DVertexShader9Ptr m_pID3DVertexShader;

	friend class D3D9Driver;
};

class PixelShaderFragment {
private:
	PixelShaderFragment(ID3DXFragmentLinker* pLinker, const char* Name);

	D3DXHANDLE m_hD3DXFragment;

	friend class D3D9Driver;
	friend class PixelShader;
};

class PixelShader {
private:
	PixelShader(ID3DXFragmentLinker* pLinker, PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments);

	IDirect3DPixelShader9Ptr m_pID3DPixelShader;

	friend class D3D9Driver;
};

class ShaderProgram {
private:
	ShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader);

	VertexShaderPtr m_pID3DVertexShader;
	PixelShaderPtr m_pID3DPixelShader;

	friend class D3D9Driver;
};

#endif
#ifndef _PIRATE_OPENGL_DRIVER_RESOURCES_H_
#define _PIRATE_OPENGL_DRIVER_RESOURCES_H_

#include "SmartPointer.h"
#include "OpenGLWrapper.h"

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
	DriverVertexBuffer(unsigned int NumVertices, unsigned int VertexSize);

	GLuint m_uiVertexBufferID;
	unsigned int m_uiVertexSize;

	friend class OpenGLDriver;
};

class DriverIndexBuffer {
public:
	void Fill(void* pData, unsigned int Size);

private:
	DriverIndexBuffer(unsigned int NumIndices);

	GLuint m_uiIndexBufferID;

	friend class OpenGLDriver;
};

class DriverVertexDeclaration {
private:
	DriverVertexDeclaration(StreamIndexVertexBufferPair* ppVertexBuffers, unsigned int NumVertexBuffers);

	friend class OpenGLDriver;
};
/*
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

	VertexShaderPtr m_pVertexShader;
	PixelShaderPtr m_pPixelShader;

	friend class D3D9Driver;
};
*/
#endif
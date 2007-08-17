#include "D3D9DriverResources.h"
#include "MeshBuffer.h"

DriverVertexBuffer::DriverVertexBuffer(IDirect3DDevice9Ptr pD3DDevice, unsigned int NumVertices, unsigned int VertexSize) : m_uiVertexSize(VertexSize)
{
	IDirect3DVertexBuffer9* pVB;
	pD3DDevice->CreateVertexBuffer(VertexSize * NumVertices, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
	m_pID3DVertexBuffer = IDirect3DVertexBuffer9Ptr(pVB, false);
}

void DriverVertexBuffer::Fill(void* pData, unsigned int Size)
{
	void* pbData;
	m_pID3DVertexBuffer->Lock(0, 0, &pbData, 0);
	memcpy(pbData, pData, Size);
	m_pID3DVertexBuffer->Unlock();
}

DriverIndexBuffer::DriverIndexBuffer(IDirect3DDevice9Ptr pD3DDevice, unsigned int NumIndices)
{
	IDirect3DIndexBuffer9* pIB;
	pD3DDevice->CreateIndexBuffer(sizeof(DWORD) * NumIndices, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &pIB, NULL);
	m_pID3DIndexBuffer = IDirect3DIndexBuffer9Ptr(pIB, false);
}

void DriverIndexBuffer::Fill(void* pData, unsigned int Size)
{
	void* pbData;
	m_pID3DIndexBuffer->Lock(0, 0, &pbData, 0);
	memcpy(pbData, pData, Size);
	m_pID3DIndexBuffer->Unlock();
}

DriverVertexDeclaration::DriverVertexDeclaration(IDirect3DDevice9Ptr pD3DDevice, StreamIndexVertexBufferPair* ppVertexBuffers, unsigned int NumVertexBuffers)
{
	std::vector<D3DVERTEXELEMENT9> D3DElements;
	for (unsigned int i=0; i<NumVertexBuffers; ++i)
	{
		unsigned short StreamIndex = ppVertexBuffers[i].first;
		VertexBufferPtr pVB = ppVertexBuffers[i].second;
		const VertexElement* pElement = pVB->GetVertexElement();
		for (unsigned int j=0; j<pVB->GetNumVertexElement(); ++j)
		{
			D3DVERTEXELEMENT9 e = { StreamIndex, pElement[j].Offset, pElement[j].Type, 0, pElement[j].Usage, pElement[j].UsageIndex };
			D3DElements.push_back(e);
		}
	}
	D3DVERTEXELEMENT9 e = D3DDECL_END();
	D3DElements.push_back(e);

	IDirect3DVertexDeclaration9* pVD;
	pD3DDevice->CreateVertexDeclaration(&D3DElements.front(), &pVD);
	m_pID3DVertexDeclaration = IDirect3DVertexDeclaration9Ptr(pVD, false);
} 

VertexShaderFragment::VertexShaderFragment(ID3DXFragmentLinker* pLinker, const char* Name) : m_hD3DXFragment(pLinker->GetFragmentHandleByName(Name))
{
}

VertexShader::VertexShader(ID3DXFragmentLinker* pLinker, VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments)
{
	std::vector<D3DXHANDLE> handles(NumFragments);
	for (unsigned int i=0; i<NumFragments; ++i)
		handles[i] = ppFragments[i]->m_hD3DXFragment;

	IDirect3DVertexShader9* pVertexShader;
	pLinker->LinkVertexShader("vs_3_0", 0, &handles.front(), NumFragments, &pVertexShader, NULL);
	m_pID3DVertexShader = IDirect3DVertexShader9Ptr(pVertexShader, false);
}

PixelShaderFragment::PixelShaderFragment(ID3DXFragmentLinker* pLinker, const char* Name) : m_hD3DXFragment(pLinker->GetFragmentHandleByName(Name))
{
}

PixelShader::PixelShader(ID3DXFragmentLinker* pLinker, PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments)
{
	std::vector<D3DXHANDLE> handles(NumFragments);
	for (unsigned int i=0; i<NumFragments; ++i)
		handles[i] = ppFragments[i]->m_hD3DXFragment;

	IDirect3DPixelShader9* pVertexShader;
	pLinker->LinkPixelShader("ps_3_0", 0, &handles.front(), NumFragments, &pVertexShader, NULL);
	m_pID3DPixelShader = IDirect3DPixelShader9Ptr(pVertexShader, false);
}

ShaderProgram::ShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader) : m_pID3DVertexShader(pVertexShader), m_pID3DPixelShader(pPixelShader)
{
}
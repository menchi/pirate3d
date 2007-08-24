#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_D3D9_

#include "D3D9DriverResources.h"
#include "VertexFormat.h"

using namespace std;

//-----------------------------------------------------------------------------
DriverVertexBuffer::DriverVertexBuffer(IDirect3DDevice9Ptr pD3DDevice, unsigned int NumVertices, unsigned int VertexSize) : m_uiVertexSize(VertexSize)
{
	IDirect3DVertexBuffer9* pVB;
	pD3DDevice->CreateVertexBuffer(VertexSize * NumVertices, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
	m_pID3DVertexBuffer = IDirect3DVertexBuffer9Ptr(pVB, false);
}
//-----------------------------------------------------------------------------
void DriverVertexBuffer::Fill(void* pData, unsigned int Size)
{
	void* pbData;
	m_pID3DVertexBuffer->Lock(0, 0, &pbData, 0);
	memcpy(pbData, pData, Size);
	m_pID3DVertexBuffer->Unlock();
}
//-----------------------------------------------------------------------------
DriverIndexBuffer::DriverIndexBuffer(IDirect3DDevice9Ptr pD3DDevice, unsigned int NumIndices)
{
	IDirect3DIndexBuffer9* pIB;
	pD3DDevice->CreateIndexBuffer(sizeof(DWORD) * NumIndices, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &pIB, NULL);
	m_pID3DIndexBuffer = IDirect3DIndexBuffer9Ptr(pIB, false);
}
//-----------------------------------------------------------------------------
void DriverIndexBuffer::Fill(void* pData, unsigned int Size)
{
	void* pbData;
	m_pID3DIndexBuffer->Lock(0, 0, &pbData, 0);
	memcpy(pbData, pData, Size);
	m_pID3DIndexBuffer->Unlock();
}
//-----------------------------------------------------------------------------
DriverVertexDeclaration::DriverVertexDeclaration(IDirect3DDevice9Ptr pD3DDevice, const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats)
{
	std::vector<D3DVERTEXELEMENT9> D3DElements;
	for (unsigned int i=0; i<StreamIndices.size(); ++i)
	{
		unsigned short StreamIndex = StreamIndices[i];
		const VertexElementArray& VertexFormat = *VertexFormats[i];
		for (unsigned int j=0; j<VertexFormat.size(); ++j)
		{
			const VertexElement* pElement = &VertexFormat[j];
			D3DVERTEXELEMENT9 e = { StreamIndex, pElement->Offset, pElement->Type, 0, pElement->Usage, pElement->UsageIndex };
			D3DElements.push_back(e);
		}
	}
	D3DVERTEXELEMENT9 e = D3DDECL_END();
	D3DElements.push_back(e);

	IDirect3DVertexDeclaration9* pVD;
	pD3DDevice->CreateVertexDeclaration(&D3DElements[0], &pVD);
	m_pID3DVertexDeclaration = IDirect3DVertexDeclaration9Ptr(pVD, false);
} 
//-----------------------------------------------------------------------------
VertexShaderFragment::VertexShaderFragment(ID3DXFragmentLinkerPtr pLinker, const string& Name): m_hD3DXFragment(pLinker->GetFragmentHandleByName(Name.c_str()))
{
}
//-----------------------------------------------------------------------------
VertexShader::VertexShader(ID3DXFragmentLinkerPtr pLinker, const VertexShaderFragmentArray& Fragments)
{
	std::vector<D3DXHANDLE> handles(Fragments.size());
	for (unsigned int i=0; i<Fragments.size(); ++i)
		handles[i] = Fragments[i]->m_hD3DXFragment;

	IDirect3DVertexShader9* pVertexShader;
	pLinker->LinkVertexShader("vs_3_0", 0, &handles[0], (unsigned int)Fragments.size(), &pVertexShader, NULL);
	m_pID3DVertexShader = IDirect3DVertexShader9Ptr(pVertexShader, false);
}
//-----------------------------------------------------------------------------
PixelShaderFragment::PixelShaderFragment(ID3DXFragmentLinkerPtr pLinker, const string& Name): m_hD3DXFragment(pLinker->GetFragmentHandleByName(Name.c_str()))
{
}
//-----------------------------------------------------------------------------
PixelShader::PixelShader(ID3DXFragmentLinkerPtr pLinker, const PixelShaderFragmentArray& Fragments)
{
	std::vector<D3DXHANDLE> handles(Fragments.size());
	for (unsigned int i=0; i<Fragments.size(); ++i)
		handles[i] = Fragments[i]->m_hD3DXFragment;

	IDirect3DPixelShader9* pPixelShader;
	pLinker->LinkPixelShader("ps_3_0", 0, &handles[0], (unsigned int)Fragments.size(), &pPixelShader, NULL);
	m_pID3DPixelShader = IDirect3DPixelShader9Ptr(pPixelShader, false);
}
//-----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader): m_pVertexShader(pVertexShader), m_pPixelShader(pPixelShader)
{
}
//-----------------------------------------------------------------------------

#endif
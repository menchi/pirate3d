#include "D3D9DriverResources.h"
#include "MeshBuffer.h"

DriverVertexBuffer::DriverVertexBuffer(IDirect3DDevice9Ptr pD3DDevice, int size)
{
	IDirect3DVertexBuffer9* pVB;
	pD3DDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
	m_pID3DVertexBuffer = IDirect3DVertexBuffer9Ptr(pVB, false);
}

DriverIndexBuffer::DriverIndexBuffer(IDirect3DDevice9Ptr pD3DDevice, int size)
{
	IDirect3DIndexBuffer9* pIB;
	pD3DDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &pIB, NULL);
	m_pID3DIndexBuffer = IDirect3DIndexBuffer9Ptr(pIB, false);
}

DriverVertexDeclaration::DriverVertexDeclaration(IDirect3DDevice9Ptr pD3DDevice, MeshBufferPtr pMeshBuffer)
{
	std::vector<D3DVERTEXELEMENT9> D3DElements;
	const unsigned int n = pMeshBuffer->GetNumVertexBuffers();
	for (unsigned int i=0; i<n; ++i)
	{
		unsigned short StreamIndex = pMeshBuffer->GetVertexBuffer(i).first;
		VertexBufferPtr pVB = pMeshBuffer->GetVertexBuffer(i).second;
		const VertexElement* pElement = pVB->GetVertexElement();
		D3DVERTEXELEMENT9 e = { StreamIndex, pElement->Offset, pElement->Type, 0, pElement->Usage, pElement->UsageIndex };
		D3DElements.push_back(e);
	}
	D3DVERTEXELEMENT9 e = D3DDECL_END();
	D3DElements.push_back(e);

	IDirect3DVertexDeclaration9* pVD;
	pD3DDevice->CreateVertexDeclaration(&D3DElements.front(), &pVD);
	m_pID3DVertexDeclaration = IDirect3DVertexDeclaration9Ptr(pVD, false);
} 
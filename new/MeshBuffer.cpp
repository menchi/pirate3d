#include "MeshBuffer.h"
#include "VideoDriver.h"
#include "VertexFormat.h"

//-----------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	if (m_pBuffer) free(m_pBuffer);
}
//-----------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	delete[] m_pBuffer;
}
//-----------------------------------------------------------------------------
IndexBufferPtr IndexBuffer::Create(unsigned int NumIndices)
{
	IndexBufferPtr pIB(new IndexBuffer);
	pIB->m_pBuffer = new unsigned int[NumIndices];
	pIB->m_uiNumIndices = NumIndices;

	return pIB;
}
//-----------------------------------------------------------------------------
MeshBufferPtr MeshBuffer::Create(const VertexBufferArray& VertexBuffers, const StreamIndexArray& StreamIndices, IndexBufferPtr pIndexBuffer)
{
	MeshBufferPtr pMB = MeshBufferPtr(new MeshBuffer(VertexBuffers, StreamIndices, pIndexBuffer));
	return pMB;
}
//-----------------------------------------------------------------------------
MeshBuffer::MeshBuffer(const VertexBufferArray& VertexBuffers, const StreamIndexArray& StreamIndices, IndexBufferPtr pIndexBuffer)
: m_StreamIndices(StreamIndices), m_VertexBuffers(VertexBuffers), m_pIndexBuffer(pIndexBuffer), m_DriverVertexBuffers(VertexBuffers.size())
{
}
//-----------------------------------------------------------------------------
void MeshBuffer::CreateDriverResources(VideoDriverPtr pDriver)
{
	const unsigned int n = GetNumVertexBuffers();
	std::vector<const VertexElementArray*> VertexFormats(n);
	for (unsigned int i=0; i<n; i++)
	{
		m_DriverVertexBuffers[i] = pDriver->CreateVertexBuffer(m_VertexBuffers[i]->GetNumVertices(), m_VertexBuffers[i]->GetVertexSize());
		VertexFormats[i] = &m_VertexBuffers[i]->GetVertexFormat();
	}

	CommitVertexBuffers();

	m_pDriverIndexBuffer = pDriver->CreateIndexBuffer(m_pIndexBuffer->GetNumIndices());
	CommitIndexBuffer();

	m_pDriverVertexDeclaration = pDriver->CreateVertexDeclaration(m_StreamIndices, VertexFormats);
}
//-----------------------------------------------------------------------------
void MeshBuffer::CommitVertexBuffers()
{
	const unsigned int n = GetNumVertexBuffers();
	for (unsigned int i=0; i<n; i++)
		m_DriverVertexBuffers[i]->Fill(m_VertexBuffers[i]->GetBufferPtr<void>(), m_VertexBuffers[i]->GetNumVertices() * m_VertexBuffers[i]->GetVertexSize());
}
//-----------------------------------------------------------------------------
void MeshBuffer::CommitIndexBuffer()
{
	m_pDriverIndexBuffer->Fill(m_pIndexBuffer->GetBufferPtr(), m_pIndexBuffer->GetNumIndices() * sizeof(unsigned int));
}
//-----------------------------------------------------------------------------
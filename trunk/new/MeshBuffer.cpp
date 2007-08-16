#include "MeshBuffer.h"

VertexBuffer::~VertexBuffer()
{
	if (m_pBuffer) free(m_pBuffer);
}

IndexBuffer::~IndexBuffer()
{
	delete[] m_pBuffer;
}

IndexBufferPtr IndexBuffer::Create(unsigned int NumIndices)
{
	IndexBufferPtr pIB(new IndexBuffer);
	pIB->m_pBuffer = new unsigned int[NumIndices];
	pIB->m_uiSize = sizeof(unsigned int) * NumIndices;

	return pIB;
}

unsigned int MeshBuffer::SetVertexBuffer(VertexBufferPtr pVertexBuffer, unsigned int StreamIndex)
{
	const unsigned int n = (unsigned int)m_VertexBuffers.size();
	for (unsigned int i=0; i<n; ++i)
	{
		if (m_VertexBuffers[i].first == StreamIndex)
		{
			m_VertexBuffers[i].second = pVertexBuffer;
			return i;
		}
	}

	m_VertexBuffers.push_back(std::make_pair(StreamIndex, pVertexBuffer));
	return n;
}

MeshBufferPtr MeshBuffer::Create(VertexBufferPtr* ppVertexBuffers, unsigned short* pStreamIndices, unsigned int NumVertexBuffers, IndexBufferPtr pIndexBuffer)
{
	MeshBufferPtr pMB = MeshBufferPtr(new MeshBuffer);
	for (unsigned int i=0; i<NumVertexBuffers; ++i)
		pMB->m_VertexBuffers.push_back(std::make_pair(pStreamIndices[i], ppVertexBuffers[i]));

	pMB->m_pIndexBuffer = pIndexBuffer;

	return pMB;
}

void MeshBuffer::CreateDriverResources(VideoDriverPtr pDriver)
{
	const unsigned int n = (unsigned int)m_VertexBuffers.size();
	for (unsigned int i=0; i<n; i++)
		m_DriverVertexBuffers.push_back(pDriver->CreateVertexBuffer(m_VertexBuffers[i].second->GetSize()));

	CommitVertexBuffers();

	m_pDriverIndexBuffer = pDriver->CreateIndexBuffer(m_pIndexBuffer->GetSize());
	CommitIndexBuffer();
}

void MeshBuffer::CommitVertexBuffers()
{
	const unsigned int n = (unsigned int)m_VertexBuffers.size();
	for (unsigned int i=0; i<n; i++)
		m_DriverVertexBuffers[i]->Fill(m_VertexBuffers[i].second->GetBufferPtr<void>(), m_VertexBuffers[i].second->GetSize());
}

void MeshBuffer::CommitIndexBuffer()
{
	m_pDriverIndexBuffer->Fill(m_pIndexBuffer->GetBufferPtr(), m_pIndexBuffer->GetSize());
}
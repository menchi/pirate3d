#ifndef _PIRATE_MESH_BUFFER_H_
#define _PIRATE_MESH_BUFFER_H_

#include "VertexFormat.h"
#include <vector>

FWD_DECLARE(VertexBuffer)

class VertexBuffer {
public:
	~VertexBuffer()
	{
		if (m_pBuffer) free(m_pBuffer);
	}

	template<class T> T* GetBufferPtr()
	{
		return static_cast<T*>(m_pBuffer);
	}

	template<class T> static VertexBufferPtr Create(unsigned int NumVertices)
	{
		VertexBufferPtr pVB(new VertexBuffer);
		pVB->m_pBuffer = malloc(sizeof(T) * NumVertices);
		pVB->m_pVertexElement = T::VertexFormat;

		return pVB;
	}

	const VertexElement* GetVertexElement() const
	{
		return m_pVertexElement;
	}

private:
	VertexBuffer() : m_pBuffer(0), m_pVertexElement(0) {};

	void* m_pBuffer;
	VertexElement* m_pVertexElement;
};

FWD_DECLARE(IndexBuffer)

class IndexBuffer {
public:
	~IndexBuffer()
	{
		delete[] m_pBuffer;
	}

	unsigned int* GetBufferPtr() { return m_pBuffer; }

	static IndexBufferPtr Create(unsigned int NumIndices)
	{
		IndexBufferPtr pIB(new IndexBuffer);
		pIB->m_pBuffer = new unsigned int[NumIndices];

		return pIB;
	}

private:
	IndexBuffer() : m_pBuffer(0) {}

	unsigned int* m_pBuffer;
};

FWD_DECLARE(MeshBuffer)

class MeshBuffer {
public:
	typedef std::pair<unsigned short, VertexBufferPtr> StreamIndexVertexBufferPair;
	typedef std::vector<StreamIndexVertexBufferPair> VertexBufferArray;

	unsigned int SetVertexBuffer(VertexBufferPtr pVertexBuffer, unsigned int StreamIndex)
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

	StreamIndexVertexBufferPair GetVertexBuffer(unsigned int i)
	{
		return m_VertexBuffers[i];
	}

	unsigned int GetNumVertexBuffers() const
	{
		return (unsigned int)m_VertexBuffers.size();
	}

	static MeshBufferPtr Create(VertexBufferPtr* ppVertexBuffers, unsigned short* pStreamIndices, unsigned int NumVertexBuffers, IndexBufferPtr pIndexBuffer)
	{
		MeshBufferPtr pMB = MeshBufferPtr(new MeshBuffer);
		for (unsigned int i=0; i<NumVertexBuffers; ++i)
			pMB->m_VertexBuffers.push_back(std::make_pair(pStreamIndices[i], ppVertexBuffers[i]));

		pMB->m_pIndexBuffer = pIndexBuffer;

		return pMB;
	}

private:
	MeshBuffer() {};

	VertexBufferArray m_VertexBuffers;
	IndexBufferPtr m_pIndexBuffer;
};

#endif
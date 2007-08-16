#ifndef _PIRATE_MESH_BUFFER_H_
#define _PIRATE_MESH_BUFFER_H_

#include "VertexFormat.h"
#include "VideoDriver.h"
#include <vector>

FWD_DECLARE(VertexBuffer)
FWD_DECLARE(IndexBuffer)
FWD_DECLARE(MeshBuffer)

class VertexBuffer {
public:
	~VertexBuffer();

	const VertexElement* GetVertexElement() const {	return m_pVertexElement; }
	const unsigned int GetSize() const { return m_uiSize; }

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

private:
	VertexBuffer() : m_uiSize(0), m_pBuffer(0), m_pVertexElement(0) {};

	unsigned int m_uiSize;
	void* m_pBuffer;
	VertexElement* m_pVertexElement;
};

class IndexBuffer {
public:
	~IndexBuffer();

	const unsigned int GetSize() const { return m_uiSize; }
	unsigned int* GetBufferPtr() { return m_pBuffer; }

	static IndexBufferPtr Create(unsigned int NumIndices);

private:
	IndexBuffer() : m_uiSize(0), m_pBuffer(0) {}

	unsigned int m_uiSize;
	unsigned int* m_pBuffer;
};

class MeshBuffer {
public:
	typedef std::pair<unsigned short, VertexBufferPtr> StreamIndexVertexBufferPair;
	typedef std::vector<StreamIndexVertexBufferPair> VertexBufferArray;
	typedef std::vector<DriverVertexBufferPtr> DriverVertexBufferArray;

	unsigned int SetVertexBuffer(VertexBufferPtr pVertexBuffer, unsigned int StreamIndex);
	StreamIndexVertexBufferPair GetVertexBuffer(unsigned int i)	{ return m_VertexBuffers[i]; }
	unsigned int GetNumVertexBuffers() const { return (unsigned int)m_VertexBuffers.size();	}

	static MeshBufferPtr Create(VertexBufferPtr* ppVertexBuffers, unsigned short* pStreamIndices, unsigned int NumVertexBuffers, IndexBufferPtr pIndexBuffer);

private:
	MeshBuffer() {};

	void CreateDriverResources(VideoDriverPtr pDriver);
	void CommitVertexBuffers();
	void CommitIndexBuffer();

	VertexBufferArray m_VertexBuffers;
	IndexBufferPtr m_pIndexBuffer;

	DriverVertexBufferArray m_DriverVertexBuffers;
	DriverIndexBufferPtr m_pDriverIndexBuffer;
};

#endif
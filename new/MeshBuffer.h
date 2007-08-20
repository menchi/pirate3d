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
	const unsigned int GetNumVertexElement() const { return m_uiNumElements; }
	const unsigned int GetNumVertices() const { return m_uiNumVertices; }
	const unsigned int GetVertexSize() const { return m_uiVertexSize; }

	template<class T> T* GetBufferPtr()
	{
		return static_cast<T*>(m_pBuffer);
	}

	template<class T> static VertexBufferPtr Create(unsigned int NumVertices)
	{
		VertexBufferPtr pVB(new VertexBuffer);
		pVB->m_pBuffer = malloc(sizeof(T) * NumVertices);
		pVB->m_pVertexElement = T::VertexFormat;
		pVB->m_uiNumElements = _countof(T::VertexFormat);
		pVB->m_uiVertexSize = sizeof(T);
		pVB->m_uiNumVertices = NumVertices;

		return pVB;
	}

private:
	VertexBuffer() : m_uiVertexSize(0), m_uiNumVertices(0), m_pBuffer(0), m_pVertexElement(0) {};

	unsigned int m_uiVertexSize;
	unsigned int m_uiNumVertices;
	void* m_pBuffer;
	VertexElement* m_pVertexElement;
	unsigned int m_uiNumElements;
};

class IndexBuffer {
public:
	~IndexBuffer();

	const unsigned int GetNumIndices() const { return m_uiNumIndices; }
	unsigned int* GetBufferPtr() { return m_pBuffer; }

	static IndexBufferPtr Create(unsigned int NumIndices);

private:
	IndexBuffer() : m_uiNumIndices(0), m_pBuffer(0) {}

	unsigned int m_uiNumIndices;
	unsigned int* m_pBuffer;
};

class MeshBuffer {
public:
	typedef std::pair<unsigned short, VertexBufferPtr> StreamIndexVertexBufferPair;
	typedef std::pair<unsigned short, DriverVertexBufferPtr> StreamIndexDriverVertexBufferPair;

	unsigned int SetVertexBuffer(VertexBufferPtr pVertexBuffer, unsigned int StreamIndex);
	unsigned int GetNumVertexBuffers() const { return (unsigned int)m_VertexBuffers.size();	}
	StreamIndexVertexBufferPair GetVertexBuffer(unsigned int i)	{ return m_VertexBuffers[i]; }
	StreamIndexDriverVertexBufferPair GetDriverVertexBuffer(unsigned int i) { return std::make_pair(m_VertexBuffers[i].first, m_DriverVertexBuffers[i]); }
	DriverVertexDeclarationPtr GetVertexDeclaration() { return m_pDriverVertexDeclaration; }
	DriverIndexBufferPtr GetDriverIndexBuffer() { return m_pDriverIndexBuffer; }
	IndexBufferPtr GetIndexBuffer() { return m_pIndexBuffer; }

	void CreateDriverResources(VideoDriverPtr pDriver);
	void CommitVertexBuffers();
	void CommitIndexBuffer();

	static MeshBufferPtr Create(VertexBufferPtr* ppVertexBuffers, unsigned short* pStreamIndices, unsigned int NumVertexBuffers, IndexBufferPtr pIndexBuffer);

private:
	MeshBuffer() {};

	typedef std::vector<StreamIndexVertexBufferPair> VertexBufferArray;
	typedef std::vector<DriverVertexBufferPtr> DriverVertexBufferArray;

	VertexBufferArray m_VertexBuffers;
	IndexBufferPtr m_pIndexBuffer;

	DriverVertexBufferArray m_DriverVertexBuffers;
	DriverIndexBufferPtr m_pDriverIndexBuffer;
	DriverVertexDeclarationPtr m_pDriverVertexDeclaration;
};

#endif
#ifndef _PIRATE_MESH_BUFFER_H_
#define _PIRATE_MESH_BUFFER_H_

#include "VideoDriverFwd.h"
#include <vector>

FWD_DECLARE(VertexBuffer)
FWD_DECLARE(IndexBuffer)
FWD_DECLARE(MeshBuffer)

class VertexBuffer {
public:
	VertexBuffer::~VertexBuffer();

	const VertexFormat& GetVertexFormat() const { return m_VertexFormat; }
	const unsigned int GetNumVertices() const { return m_uiNumVertices; }
	const unsigned int GetVertexSize() const { return m_uiVertexSize; }

	template<class T> T* GetBufferPtr()
	{
		return static_cast<T*>(m_pBuffer);
	}

	template<class T> static VertexBufferPtr Create(unsigned int NumVertices)
	{
		VertexBufferPtr pVB(new VertexBuffer(::GetVertexFormat<T>()));
		pVB->m_pBuffer = malloc(sizeof(T) * NumVertices);
		pVB->m_uiVertexSize = sizeof(T);
		pVB->m_uiNumVertices = NumVertices;

		return pVB;
	}

private:
	VertexBuffer(const VertexFormat& VertexFormat): m_uiVertexSize(0), m_uiNumVertices(0), m_pBuffer(0), m_VertexFormat(VertexFormat) {};

	unsigned int m_uiVertexSize;
	unsigned int m_uiNumVertices;
	void* m_pBuffer;
	const VertexFormat& m_VertexFormat;
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
	typedef std::vector<unsigned short> StreamIndexArray;
	typedef std::vector<VertexBufferPtr> VertexBufferArray;
	typedef std::vector<DriverVertexBufferPtr> DriverVertexBufferArray;

	unsigned int GetNumVertexBuffers() const { return (unsigned int)m_VertexBuffers.size();	}
	const VertexBufferArray& GetVertexBuffers()	const { return m_VertexBuffers; }
	IndexBufferPtr GetIndexBuffer() { return m_pIndexBuffer; }
	const StreamIndexArray& GetStreamIndices()	const { return m_StreamIndices; }

	const DriverVertexBufferArray& GetDriverVertexBuffers() { return m_DriverVertexBuffers; }
	DriverVertexDeclarationPtr GetVertexDeclaration() { return m_pDriverVertexDeclaration; }
	DriverIndexBufferPtr GetDriverIndexBuffer() { return m_pDriverIndexBuffer; }

	void CreateDriverResources(VideoDriverPtr pDriver);
	void CommitVertexBuffers();
	void CommitIndexBuffer();

	static MeshBufferPtr Create(const VertexBufferArray& VertexBuffers, const StreamIndexArray& StreamIndices, IndexBufferPtr pIndexBuffer);

private:
	MeshBuffer(const VertexBufferArray& VertexBuffers, const StreamIndexArray& StreamIndices, IndexBufferPtr pIndexBuffer);

	StreamIndexArray m_StreamIndices;
	VertexBufferArray m_VertexBuffers;
	IndexBufferPtr m_pIndexBuffer;

	DriverVertexBufferArray m_DriverVertexBuffers;
	DriverIndexBufferPtr m_pDriverIndexBuffer;
	DriverVertexDeclarationPtr m_pDriverVertexDeclaration;
};

#endif
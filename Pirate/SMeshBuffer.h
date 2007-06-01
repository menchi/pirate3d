#ifndef _PIRATE_MESH_BUFFER_H_
#define _PIRATE_MESH_BUFFER_H_

#include "D3D9Driver.h"
#include "SMaterial.h"
#include "aabbox3d.h"

namespace Pirate
{

struct SSourceToVertexBufferMapping
{
	SSourceToVertexBufferMapping()
	{
		SourceIndex = 0;
		VertexBufferIndex = 0;
	}

	BOOL operator < (const SSourceToVertexBufferMapping& other) const
	{
		return SourceIndex < other.SourceIndex;
	}

	u32 SourceIndex;
	u32 VertexBufferIndex;
};


struct SD3D9MeshBuffer : public virtual RefObject
{
	//! constructor
	SD3D9MeshBuffer(D3D9Driver* driver, D3DVERTEXELEMENT9* vertexElements) 
	: m_uiVertexCount(0), m_uiIndexCount(0), m_pIndexBuffer(0)
	{
		m_pID3DDevice = driver->GetExposedVideoData().D3DDev9;
		m_pID3DDevice->AddRef();

		m_pID3DDevice->CreateVertexDeclaration(vertexElements, &m_pVertexDeclaration);
	};

	//! constructor
	SD3D9MeshBuffer(D3D9Driver* driver, IDirect3DVertexDeclaration9* vertexDeclaration) 
	: m_uiVertexCount(0), m_uiIndexCount(0), m_pIndexBuffer(0)
	{
		m_pID3DDevice = driver->GetExposedVideoData().D3DDev9;
		m_pID3DDevice->AddRef();

		_PIRATE_DEBUG_BREAK_IF(vertexDeclaration == 0)
		m_pVertexDeclaration = vertexDeclaration;
		m_pVertexDeclaration->AddRef();
	};

	//! destructor
	~SD3D9MeshBuffer() 
	{
		if (m_pVertexDeclaration)
			m_pVertexDeclaration->Release();

		if (m_pIndexBuffer)
			m_pIndexBuffer->Release();

		for (u32 i=0; i<m_VertexBuffers.size(); i++)
		{
			if (m_VertexBuffers[i])
				m_VertexBuffers[i]->Release();
		}
		if (m_pID3DDevice)
			m_pID3DDevice->Release();
	}; 

	//! returns the material of this meshbuffer
	const SMaterial& GetMaterial() const
	{
		return m_Material;
	}

	//! returns the material of this meshbuffer
	SMaterial& GetMaterial()
	{
		return m_Material;
	}

	//! create vertexbuffer
	BOOL CreateVertexBuffer(u32 sourceIndex, u32 vertexSize, u32 numVerts, u32 usage)
	{
		BOOL ret = FALSE;

		SSourceToVertexBufferMapping tmp;
		tmp.SourceIndex = sourceIndex;

		s32 i = m_SrcToVBMap.linear_search(tmp);
		if (i == -1)
		{
			IDirect3DVertexBuffer9* pVB = NULL;
			m_pID3DDevice->CreateVertexBuffer(vertexSize*numVerts, usage, 0, D3DPOOL_DEFAULT, &pVB, NULL);
			if (pVB)
			{
				tmp.VertexBufferIndex = m_VertexBuffers.size();
				m_SrcToVBMap.push_back(tmp);

				m_VertexBuffers.push_back(pVB);
				m_uiVertexCount = numVerts;
				ret = TRUE;
			}
		}
		else
		{
			m_SrcToVBMap.push_back(tmp);
			m_VertexBuffers[m_SrcToVBMap[i].VertexBufferIndex]->Release();
			m_pID3DDevice->CreateVertexBuffer(vertexSize*numVerts, usage, 0, D3DPOOL_DEFAULT, &m_VertexBuffers[i], NULL);
			if (m_VertexBuffers[i])
			{
				m_uiVertexCount = numVerts;
				ret = TRUE;
			}
		}

		return ret;
	}

	//! map a given vertexbuffer to a source
	BOOL SetVertexBuffer(u32 sourceIndex, u32 vertexCount, IDirect3DVertexBuffer9* vertexBuffer)
	{
		if (vertexBuffer == 0)
			return FALSE;

		SSourceToVertexBufferMapping tmp;
		tmp.SourceIndex = sourceIndex;
		int i = m_SrcToVBMap.linear_search(tmp);
		if (i == -1)
		{
			tmp.VertexBufferIndex = m_VertexBuffers.size();
			m_SrcToVBMap.push_back(tmp);

			m_VertexBuffers.push_back(vertexBuffer);
			vertexBuffer->AddRef();
			m_uiVertexCount = vertexCount;
		}
		else
		{
			m_VertexBuffers[m_SrcToVBMap[i].VertexBufferIndex]->Release();
			m_VertexBuffers[m_SrcToVBMap[i].VertexBufferIndex] = vertexBuffer;
			vertexBuffer->AddRef();
			m_uiVertexCount = vertexCount;
		}

		return TRUE;
	}

	//! returns pointer to vertices
	IDirect3DVertexBuffer9* GetVertexBuffer(u32 sourceIndex) const
	{
		SSourceToVertexBufferMapping tmp;
		tmp.SourceIndex = sourceIndex;
		int r = m_SrcToVBMap.linear_search(tmp);

		if (r == -1)
			return NULL;

		return m_VertexBuffers[m_SrcToVBMap[r].VertexBufferIndex];
	} 

	//! returns amount of vertices
	u32 GetVertexCount() const
	{
		return m_uiVertexCount;
	}

	//! create indexbuffer
	BOOL CreateIndexBuffer(u32 numIndexes, u32 usage=D3DUSAGE_WRITEONLY, BOOL use16bit=FALSE)
	{
		BOOL ret = FALSE;
		D3DFORMAT format = (use16bit)? D3DFMT_INDEX16 : D3DFMT_INDEX32;
		u32 indexSize = (use16bit)? 2 : 4;

		if (!m_pIndexBuffer)
		{
			m_pID3DDevice->CreateIndexBuffer(indexSize*numIndexes, usage, format, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL);
			if (m_pIndexBuffer)
			{
				m_uiIndexCount = numIndexes;
				ret = TRUE;
			}
		}
		else
		{
			m_pIndexBuffer->Release();
			m_pID3DDevice->CreateIndexBuffer(indexSize*numIndexes, usage, format, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL);
			if (m_pIndexBuffer)
			{
				m_uiIndexCount = numIndexes;
				ret = TRUE;
			}
		}

		return ret;
	}

	//! set a given indexbuffer
	BOOL SetIndexBuffer(u32 numIndexes, IDirect3DIndexBuffer9* indexBuffer)
	{
		if (indexBuffer == 0)
			return FALSE;

		if (!m_pIndexBuffer)
		{
			m_pIndexBuffer = indexBuffer;
			indexBuffer->AddRef();
			m_uiIndexCount = numIndexes;
		}
		else
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = indexBuffer;
			m_pIndexBuffer->AddRef();
			m_uiIndexCount = numIndexes;
		}

		return TRUE;
	}

	//! returns pointer to Indices
	IDirect3DIndexBuffer9* GetIndexBuffer() const
	{
		return m_pIndexBuffer;
	}

	//! returns amount of indices
	u32 GetIndexCount() const
	{
		return m_uiIndexCount;
	}

	//! returns an axis aligned bounding box
	const aabbox3d<f32>& GetBoundingBox() const
	{
		return m_BoundingBox;
	}

	//! set user axis aligned bounding box
	void SetBoundingBox( const aabbox3df& box)
	{
		m_BoundingBox = box;
	}


	//! recalculates the bounding box. should be called if the mesh changed.
	void RecalculateBoundingBox(u32 sourceIndex = 0, u32 offset = 0)
	{
		if (!m_VertexBuffers.size())
			m_BoundingBox.reset(0,0,0);
		else
		{
			u32 stride = GetVertexPitch(0);
			u8* pVert = NULL;
			GetVertexBuffer(sourceIndex)->Lock(0, 0, (void**)&pVert, D3DLOCK_READONLY);
			pVert += offset;
			f32 x = (f32)(*pVert);
			f32 y = (f32)(*pVert+sizeof(f32));
			f32 z = (f32)(*pVert+sizeof(f32)*2);
			m_BoundingBox.reset(x, y, z);
			for (u32 i=1; i<m_uiVertexCount; ++i)
			{
				pVert += stride;
				x = (f32)(*pVert);
				y = (f32)(*pVert+sizeof(f32));
				z = (f32)(*pVert+sizeof(f32)*2);
				m_BoundingBox.addInternalPoint(x, y, z);
			}
			GetVertexBuffer(sourceIndex)->Unlock();
		}
	}

	//! returns which type of vertex data is stored.
	IDirect3DVertexDeclaration9* GetVertexType() const
	{
		return m_pVertexDeclaration;
	}

	//! returns the byte size (stride, pitch) of the vertex
	u32 GetVertexPitch(u32 sourceIndex) const
	{
		D3DVERTEXBUFFER_DESC desc;
		m_VertexBuffers[sourceIndex]->GetDesc(&desc);
		return desc.Size/m_uiVertexCount;
	}


	u32 GetPrimitiveCount() const
	{
		return m_uiIndexCount/3;
	}

	IDirect3DDevice9* m_pID3DDevice;
	SMaterial m_Material; //! material for this meshBuffer.
	array<IDirect3DVertexBuffer9*> m_VertexBuffers; //! Array of vertices

	IDirect3DVertexDeclaration9* m_pVertexDeclaration;
	IDirect3DIndexBuffer9* m_pIndexBuffer; //! Array of the Indices.
	array<SSourceToVertexBufferMapping> m_SrcToVBMap;
	u32 m_uiVertexCount;
	u32 m_uiIndexCount;
	aabbox3d<f32> m_BoundingBox;
};

}

#endif
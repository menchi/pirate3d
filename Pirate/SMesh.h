#ifndef _PIRATE_S_MESH_H_
#define _PIRATE_S_MESH_H_

#include "SMeshBuffer.h"

namespace Pirate
{

//! Simple implementation of the IMesh interface.
struct SMesh : public RefObject
{
	//! constructor
	SMesh()
	{
#ifdef _DEBUG
		SetDebugName("SMesh");
#endif
	}

	//! destructor
	virtual ~SMesh()
	{
		// drop buffers
		for (u32 i=0; i<m_MeshBuffers.size(); ++i)
		{
			m_MeshBuffers[i]->Drop();
		}
	};

	//! returns amount of mesh buffers.
	u32 GetMeshBufferCount() const
	{
		return m_MeshBuffers.size();
	}

	//! returns pointer to a mesh buffer
	SD3D9MeshBuffer* GetMeshBuffer(u32 nr) const
	{
		return m_MeshBuffers[nr];
	}

	//! returns a meshbuffer which fits a material
	// reverse search
	SD3D9MeshBuffer* GetMeshBuffer( const SMaterial & material) const
	{
		for (s32 i = (s32) m_MeshBuffers.size(); --i >= 0; )
		{
			if ( !(material != m_MeshBuffers[i]->GetMaterial()) )
				return m_MeshBuffers[i];
		}

		return 0;
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

	//! recalculates the bounding box
	void RecalculateBoundingBox()
	{
		if (m_MeshBuffers.size())
		{
			m_BoundingBox = m_MeshBuffers[0]->GetBoundingBox();
			for (u32 i=1; i<m_MeshBuffers.size(); ++i)
				m_BoundingBox.addInternalBox(m_MeshBuffers[i]->GetBoundingBox());
		}
		else
			m_BoundingBox.reset(0.0f, 0.0f, 0.0f);
	}

	//! adds a MeshBuffer
	void AddMeshBuffer(SD3D9MeshBuffer* buf)
	{
		if (buf)
		{
			buf->Grab();
			m_MeshBuffers.push_back(buf);
		}
	}

	array<SD3D9MeshBuffer*> m_MeshBuffers;
	aabbox3d<f32> m_BoundingBox;
};


} // end namespace

#endif
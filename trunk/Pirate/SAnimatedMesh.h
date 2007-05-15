#ifndef __S_ANIMATED_MESH_H_INCLUDED__
#define __S_ANIMATED_MESH_H_INCLUDED__

#include "SMesh.h"
#include "pirateArray.h"

namespace Pirate
{

//! Simple implementation of the IAnimatedMesh interface.
struct SAnimatedMesh : public RefObject
{
	//! constructor
	SAnimatedMesh()
	{
#ifdef _DEBUG
		SetDebugName("SAnimatedMesh");
#endif

	}


	//! destructor
	~SAnimatedMesh()
	{
		// drop meshes
		for (u32 i=0; i<m_Meshes.size(); ++i)
			m_Meshes[i]->Drop();
	};


	//! Gets the frame count of the animated mesh.
	//! \return Returns the amount of frames. If the amount is 1, it is a static, non animated mesh.
	s32 GetFrameCount()
	{
		return m_Meshes.size();
	}



	//! Returns the IMesh interface for a frame.
	//! \param frame: Frame number as zero based index. The maximum frame number is
	//! getFrameCount() - 1;
	//! \param detailLevel: Level of detail. 0 is the lowest,
	//! 255 the highest level of detail. Most meshes will ignore the detail level.
	//! \param startFrameLoop: start frame
	//! \param endFrameLoop: end frame
	//! \return Returns the animated mesh based on a detail level. 
	SMesh* GetMesh(s32 frame, s32 detailLevel, s32 startFrameLoop=-1, s32 endFrameLoop=-1)
	{
		if (m_Meshes.empty())
			return 0;

		return m_Meshes[frame];
	}


	//! adds a Mesh
	void AddMesh(SMesh* mesh)
	{
		if (mesh)
		{
			mesh->Grab();
			m_Meshes.push_back(mesh);
		}
	}


	//! Returns an axis aligned bounding box of the mesh.
	//! \return A bounding box of this mesh is returned.
	const aabbox3d<f32>& GetBoundingBox() const
	{
		return m_Box;
	}

	//! set user axis aligned bounding box
	void SetBoundingBox( const aabbox3df& box)
	{
		m_Box = box;
	}

	void RecalculateBoundingBox()
	{
		m_Box.reset(0,0,0);

		if (m_Meshes.empty())
			return;

		m_Box = m_Meshes[0]->GetBoundingBox();

		for (u32 i=1; i<m_Meshes.size(); ++i)
			m_Box.addInternalBox(m_Meshes[i]->GetBoundingBox());				
	}

	aabbox3d<f32> m_Box;
	array<SMesh*> m_Meshes;
};


} // end namespace

#endif
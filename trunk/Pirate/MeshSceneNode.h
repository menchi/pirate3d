#ifndef _PIRATE_MESH_SCENE_NODE_H_
#define _PIRATE_MESH_SCENE_NODE_H_

#include "SMesh.h"
#include "SceneNode.h"

namespace Pirate
{

class MeshSceneNode : public SceneNode
{
public:

	//! constructor
	MeshSceneNode(SMesh* mesh, SceneNode* parent, SceneManager* mgr, s32 id,
		const vector3df& position = vector3df(0,0,0),
		const vector3df& rotation = vector3df(0,0,0),
		const vector3df& scale = vector3df(1.0f, 1.0f, 1.0f));

	//! destructor
	virtual ~MeshSceneNode();

	//! frame
	virtual void OnRegisterSceneNode();

	//! renders the node.
	virtual void Render();

	//! returns the axis aligned bounding box of this node
	virtual const aabbox3d<f32>& GetBoundingBox() const;

	//! returns the material based on the zero based index i. To get the amount
	//! of materials used by this scene node, use getMaterialCount().
	//! This function is needed for inserting the node into the scene hirachy on a
	//! optimal position for minimizing renderstate changes, but can also be used
	//! to directly modify the material of a scene node.
	virtual SMaterial& GetMaterial(u32 i);

	//! returns amount of materials used by this scene node.
	virtual u32 GetMaterialCount();

	//! Sets a new mesh
	virtual void SetMesh(SMesh* mesh);

	//! Returns the current mesh
	virtual SMesh* GetMesh(void) { return m_pMesh; }

	//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
	/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
	referencing this mesh to change too. */
	virtual void SetReadOnlyMaterials(BOOL readonly);

	//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
	virtual BOOL IsReadOnlyMaterials();

protected:

	void CopyMaterials();

	array<SMaterial> m_Materials;
	aabbox3d<f32> m_Box;
	SMaterial m_TmpReadOnlyMaterial;

	SMesh* m_pMesh;

	s32 m_iPassCount;
	BOOL m_bReadOnlyMaterials;
};

} // end namespace

#endif
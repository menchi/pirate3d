#ifndef _PIRATE_BSP_SCENE_NODE_H_
#define _PIRATE_BSP_SCENE_NODE_H_

#include "MeshSceneNode.h"
#include "pirateArray.h"

namespace Pirate
{

struct SMesh;
struct BSPTree;

class BSPSceneNode : public MeshSceneNode
{
public:

	//! constructor
	BSPSceneNode(BSPTree* tree, SMesh* mesh, SceneNode* parent, SceneManager* mgr, s32 id,
		const vector3df& position = vector3df(0,0,0),
		const vector3df& rotation = vector3df(0,0,0),
		const vector3df& scale = vector3df(1.0f, 1.0f, 1.0f));

	//! destructor
	virtual ~BSPSceneNode();

	//! frame
	virtual void OnRegisterSceneNode();

	//! renders the node.
	virtual void Render();

	//! returns the axis aligned bounding box of this node
	virtual const aabbox3d<f32>& GetBoundingBox() const;

	//! Sets a new bsp tree
	void SetBSPTree(BSPTree* tree);

	//! Returns the bsp tree
	BSPTree* GetBSPTree(void) { return m_pBSPTree; }

protected:

	void FindVisibleMeshBuffers(vector3df camera);

	BSPTree* m_pBSPTree;
	s16 m_iLastCluster;
};

} // end namespace

#endif
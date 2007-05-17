#include "EmptySceneNode.h"
#include "SceneManager.h"

namespace Pirate
{

//! constructor
EmptySceneNode::EmptySceneNode(SceneNode* parent, SceneManager* mgr, s32 id)
	: SceneNode(parent, mgr, id)
{
#ifdef _DEBUG
	SetDebugName("CEmptySceneNode");
#endif

	SetAutomaticCulling(FALSE);
}


//! pre render event
void EmptySceneNode::OnRegisterSceneNode()
{
	if (m_bIsVisible)
	{
		m_pSceneManager->RegisterNodeForRendering(this);
		SceneNode::OnRegisterSceneNode();
	}
}


//! render
void EmptySceneNode::Render()
{
	// do nothing
}


//! returns the axis aligned bounding box of this node
const aabbox3d<f32>& EmptySceneNode::GetBoundingBox() const
{
	return Box;
}

} // end namespace 
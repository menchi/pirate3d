#ifndef _PIRATE_EMPTY_SCENE_NODE_H_
#define _PIRATE_EMPTY_SCENE_NODE_H_

#include "SceneNode.h"

namespace Pirate
{

class EmptySceneNode : public SceneNode
{
public:

	//! constructor
	EmptySceneNode(SceneNode* parent, SceneManager* mgr, s32 id);

	//! returns the axis aligned bounding box of this node
	virtual const aabbox3d<f32>& GetBoundingBox() const;

	//! This method is called just before the rendering process of the whole scene.
	virtual void OnRegisterSceneNode();

	//! does nothing.
	virtual void Render();

private:

	aabbox3d<f32> Box;
};

} // end namespace

#endif
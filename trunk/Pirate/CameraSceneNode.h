#ifndef _PIRATE_CAMERA_SCENE_NODE_H_
#define _PIRATE_CAMERA_SCENE_NODE_H_

#include "SceneNode.h"
#include "IEventReceiver.h"
#include "SViewFrustum.h"

namespace Pirate
{

class CameraSceneNode : public SceneNode, public IEventReceiver
{
public:

	//! constructor
	CameraSceneNode(SceneNode* parent, SceneManager* mgr, s32 id, 
		const vector3df& position = vector3df(0,0,0),
		const vector3df& lookat = vector3df(0,0,100));

	//! destructor
	virtual ~CameraSceneNode();

	//! Sets the projection matrix of the camera. The core::matrix4 class has some methods
	//! to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
	//! \param projection: The new projection matrix of the camera. 
	virtual void SetProjectionMatrix(const matrix4& projection);

	//! Gets the current projection matrix of the camera
	//! \return Returns the current projection matrix of the camera.
	virtual const matrix4& GetProjectionMatrix();

	//! Gets the current view matrix of the camera
	//! \return Returns the current view matrix of the camera.
	virtual const matrix4& GetViewMatrix();

	//! It is possible to send mouse and key events to the camera. Most cameras
	//! may ignore this input, but camera scene nodes which are created for 
	//! example with scene::ISceneManager::addMayaCameraSceneNode or
	//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
	//! for changing their position, look at target or whatever. 
	virtual BOOL OnEvent(SEvent event);

	//! sets the look at target of the camera
	//! \param pos: Look at target of the camera.
	virtual void SetTarget(const vector3df& pos);

	//! Gets the current look at target of the camera
	//! \return Returns the current look at target of the camera
	virtual vector3df GetTarget() const;

	//! Sets the up vector of the camera.
	//! \param pos: New upvector of the camera.
	virtual void SetUpVector(const vector3df& pos);

	//! Gets the up vector of the camera.
	//! \return Returns the up vector of the camera.
	virtual vector3df GetUpVector() const;

	//! Gets distance from the camera to the near plane.
	//! \return Value of the near plane of the camera.
	virtual f32 GetNearValue();

	//! Gets the distance from the camera to the far plane.
	//! \return Value of the far plane of the camera.
	virtual f32 GetFarValue();

	//! Get the aspect ratio of the camera.
	//! \return The aspect ratio of the camera.
	virtual f32 GetAspectRatio();

	//! Gets the field of view of the camera.
	//! \return Field of view of the camera
	virtual f32 GetFOV();

	//! Sets the value of the near clipping plane. (default: 1.0f)
	virtual void SetNearValue(f32 zn);

	//! Sets the value of the far clipping plane (default: 2000.0f)
	virtual void SetFarValue(f32 zf);

	//! Sets the aspect ratio (default: 4.0f / 3.0f)
	virtual void SetAspectRatio(f32 aspect);

	//! Sets the field of view (Default: PI / 3.5f)
	virtual void SetFOV(f32 fovy);

	//! PreRender event
	virtual void OnRegisterSceneNode();

	//! Render
	virtual void Render();

	//! Returns the axis aligned bounding box of this node
	virtual const aabbox3d<f32>& GetBoundingBox() const;

	//! Returns the view area. Sometimes needed by bsp or lod render nodes.
	virtual const SViewFrustum* GetViewFrustum() const;

	//! Disables or enables the camera to get key or mouse inputs.
	//! If this is set to true, the camera will respond to key inputs
	//! otherwise not.
	virtual void SetInputReceiverEnabled(BOOL enabled);

	//! Returns if the input receiver of the camera is currently enabled.
	virtual BOOL IsInputReceiverEnabled();

	virtual vector3df GetAbsolutePosition() const;

	//! Returns if a camera is orthogonal.
	/** This setting does not change anything of the view or projection matrix. However
	it influences how collision detection and picking is done with this camera. */
	virtual BOOL IsOrthogonal() 
	{
		return m_bIsOrthogonal;
	} 

	//! Sets if this camera should return if it is orthogonal. 
	/** This setting does not change anything of the view or projection matrix. However
	it influences how collision detection and picking is done with this camera. */
	void SetIsOrthogonal( BOOL orthogonal )
	{
		m_bIsOrthogonal = orthogonal;
	}

protected:

	void RecalculateProjectionMatrix();
	void RecalculateViewArea();

	vector3df m_Target;
	vector3df m_UpVector;

	f32 m_fFovy;	// Field of view, in radians. 
	f32 m_fAspect;	// Aspect ratio. 
	f32 m_fZNear;	// value of the near view-plane. 
	f32 m_fZFar;	// Z-value of the far view-plane.

	SViewFrustum m_ViewArea;

	BOOL m_bInputReceiverEnabled;

private:

	BOOL m_bIsOrthogonal;
};

} // end namespace

#endif
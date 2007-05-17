#ifndef _PIRATE_CAMERA_FPS_SCENE_NODE_H_
#define _PIRATE_CAMERA_FPS_SCENE_NODE_H_

#include "CameraSceneNode.h"
#include "DeviceWin32.h"
#include "pirateArray.h"

namespace Pirate
{

//! enumeration for key actions. Used for example in the FPS Camera.
enum EKEY_ACTION
{
	EKA_MOVE_FORWARD = 0,
	EKA_MOVE_BACKWARD,
	EKA_STRAFE_LEFT,
	EKA_STRAFE_RIGHT,
	EKA_JUMP_UP,
	EKA_COUNT,

	//! This value is not used. It only forces this enumeration to compile in 32 bit. 
	EKA_FORCE_32BIT = 0x7fffffff	
};

//! Struct storing which key belongs to which action.
struct SKeyMap
{
	EKEY_ACTION Action;
	EKEY_CODE KeyCode;
};

class CameraFPSSceneNode : public CameraSceneNode
{
public:

	//! constructor
	CameraFPSSceneNode(SceneNode* parent, SceneManager* mgr, 
		DeviceWin32::CursorControl* cursorControl, s32 id,
		f32 rotateSpeed, f32 moveSpeed,f32 jumpSpeed,
		SKeyMap* keyMapArray, s32 keyMapSize, BOOL noVerticalMovement = FALSE );

	//! destructor
	virtual ~CameraFPSSceneNode();

	//! It is possible to send mouse and key events to the camera. Most cameras
	//! may ignore this input, but camera scene nodes which are created for 
	//! example with scene::ISceneManager::addMayaCameraSceneNode or
	//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
	//! for changing their position, look at target or whatever. 
	virtual BOOL OnEvent(SEvent event);

	//! OnAnimate() is called just before rendering the whole scene.
	//! nodes may calculate or store animations here, and may do other useful things,
	//! dependent on what they are.
	virtual void OnAnimate(u32 timeMs);

	//! sets the look at target of the camera
	//! \param pos: Look at target of the camera.
	virtual void SetTarget(const vector3df& pos);

	//! Disables or enables the camera to get key or mouse inputs.
	//! If this is set to true, the camera will respond to key inputs
	//! otherwise not.
	virtual void SetInputReceiverEnabled(BOOL enabled);

	//! Sets the speed that this camera rotates
	virtual void SetRotateSpeed(const f32 speed);

	//! Sets the speed that this camera moves
	virtual void SetMoveSpeed(const f32 speed);

	//! Gets the rotation speed
	virtual f32 GetRotateSpeed();

	// Gets the movement speed
	virtual f32 GetMoveSpeed();

private:

	struct SCamKeyMap
	{
		SCamKeyMap() {};
		SCamKeyMap(s32 a, EKEY_CODE k) : action(a), keycode(k) {}

		s32 action;
		EKEY_CODE keycode;
	};

	void AllKeysUp();
	void Animate( u32 timeMs );

	BOOL m_CursorKeys[6];

	DeviceWin32::CursorControl* m_pCursorControl;

	f32 m_fMoveSpeed;
	f32 m_fRotateSpeed;
	f32 m_fJumpSpeed;

	BOOL m_bFirstUpdate;
	s32 m_iLastAnimationTime;

	vector3df m_TargetVector;
	array<SCamKeyMap> m_KeyMap;
	position2d<f32> m_CenterCursor;

	BOOL m_bNoVerticalMovement;
};

} // end namespace

#endif
#include "CameraFPSSceneNode.h"
#include "SceneManager.h"
#include "os.h"

namespace Pirate
{

const f32 MAX_VERTICAL_ANGLE = 88.0f;

//! constructor
CameraFPSSceneNode::CameraFPSSceneNode(SceneNode* parent, SceneManager* mgr,
	DeviceWin32::CursorControl* cursorControl, s32 id, f32 rotateSpeed , f32 moveSpeed, f32 jumpSpeed,
	SKeyMap* keyMapArray, s32 keyMapSize, BOOL noVerticalMovement)
	: CameraSceneNode(parent, mgr, id), m_pCursorControl(cursorControl),
	m_fMoveSpeed(moveSpeed), m_fRotateSpeed(rotateSpeed), m_fJumpSpeed(jumpSpeed),
	m_bFirstUpdate(TRUE), m_iLastAnimationTime(0), m_bNoVerticalMovement(noVerticalMovement)
{
#ifdef _DEBUG
	SetDebugName("CameraFPSSceneNode");
#endif

	if (m_pCursorControl)
		m_pCursorControl->Grab();

	m_fMoveSpeed /= 1000.0f;

	RecalculateViewArea();

	AllKeysUp();

	// create key map
	if (!keyMapArray || !keyMapSize)
	{
		// create default key map
		m_KeyMap.push_back(SCamKeyMap(0, Pirate::KEY_KEY_W));
		m_KeyMap.push_back(SCamKeyMap(1, Pirate::KEY_KEY_S));
		m_KeyMap.push_back(SCamKeyMap(2, Pirate::KEY_KEY_A));
		m_KeyMap.push_back(SCamKeyMap(3, Pirate::KEY_KEY_D));
		m_KeyMap.push_back(SCamKeyMap(4, Pirate::KEY_SPACE));
	}
	else
	{
		// create custom key map

		for (s32 i=0; i<keyMapSize; ++i)
		{
			switch(keyMapArray[i].Action)
			{
			case EKA_MOVE_FORWARD: m_KeyMap.push_back(SCamKeyMap(0, keyMapArray[i].KeyCode));
				break;
			case EKA_MOVE_BACKWARD: m_KeyMap.push_back(SCamKeyMap(1, keyMapArray[i].KeyCode));
				break;
			case EKA_STRAFE_LEFT: m_KeyMap.push_back(SCamKeyMap(2, keyMapArray[i].KeyCode));
				break;
			case EKA_STRAFE_RIGHT: m_KeyMap.push_back(SCamKeyMap(3, keyMapArray[i].KeyCode));
				break;
			case EKA_JUMP_UP: m_KeyMap.push_back(SCamKeyMap(4, keyMapArray[i].KeyCode));
				break;
			default:
				break;
			} // end switch
		} // end for
	}// end if
}


//! destructor
CameraFPSSceneNode::~CameraFPSSceneNode()
{
	if (m_pCursorControl)
		m_pCursorControl->Drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for 
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever. 
BOOL CameraFPSSceneNode::OnEvent(SEvent event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		const u32 cnt = m_KeyMap.size();
		for (u32 i=0; i<cnt; ++i)
			if (m_KeyMap[i].keycode == event.KeyInput.Key)
			{
				m_CursorKeys[m_KeyMap[i].action] = event.KeyInput.PressedDown; 

				if ( m_bInputReceiverEnabled )
					return TRUE;
			}
	}

	return FALSE;
}



//! OnAnimate() is called just before rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
void CameraFPSSceneNode::OnAnimate(u32 timeMs)
{
	Animate( timeMs );

	UpdateAbsolutePosition();
	m_Target = GetPosition() + m_TargetVector;

	list<SceneNode*>::Iterator it = m_Children.begin();
	for (; it != m_Children.end(); ++it)
		(*it)->OnAnimate(timeMs);
}


void CameraFPSSceneNode::Animate( u32 timeMs )
{
	const u32 camIsMe = m_pSceneManager->GetActiveCamera() == this;

	if (m_bFirstUpdate)
	{
		if (m_pCursorControl && camIsMe)
		{
			m_pCursorControl->SetPosition(0.5f, 0.5f);
			m_CenterCursor = m_pCursorControl->GetRelativePosition();
		}

		m_iLastAnimationTime = Timer::GetTime();

		m_bFirstUpdate = FALSE;
	}

	// get time. only operate on valid camera
	f32 timeDiff = 0.f;

	if ( camIsMe )
	{
		timeDiff = (f32) ( timeMs - m_iLastAnimationTime );
		m_iLastAnimationTime = timeMs;
	}


	// update position
	vector3df pos = GetPosition();	

	// Update rotation
	//	if (InputReceiverEnabled)
	{
		m_Target.set(0,0,1);


		if (m_pCursorControl && m_bInputReceiverEnabled && camIsMe )
		{
			position2d<f32> cursorpos = m_pCursorControl->GetRelativePosition();

			if (!Pirate::equals(cursorpos.X, m_CenterCursor.X) ||
				!Pirate::equals(cursorpos.Y, m_CenterCursor.Y))
			{
				m_RelativeRotation.X *= -1.0f;
				m_RelativeRotation.Y *= -1.0f;

				m_RelativeRotation.Y += (0.5f - cursorpos.X) * m_fRotateSpeed;
				m_RelativeRotation.X = Pirate::clamp (	m_RelativeRotation.X + (0.5f - cursorpos.Y) * m_fRotateSpeed,
					-MAX_VERTICAL_ANGLE,
					+MAX_VERTICAL_ANGLE
					);

				m_RelativeRotation.X *= -1.0f;
				m_RelativeRotation.Y *= -1.0f;

				m_pCursorControl->SetPosition(0.5f, 0.5f);
				m_CenterCursor = m_pCursorControl->GetRelativePosition();
			}
		}

		// set target

		matrix4 mat;
		mat.setRotationDegrees(vector3df( m_RelativeRotation.X, m_RelativeRotation.Y, 0));
		mat.transformVect(m_Target);

		vector3df movedir = m_Target;

		if (m_bNoVerticalMovement)
			movedir.Y = 0.f;

		movedir.normalize();

		if (m_bInputReceiverEnabled && camIsMe)
		{
			if (m_CursorKeys[0])
				pos += movedir * timeDiff * m_fMoveSpeed;

			if (m_CursorKeys[1])
				pos -= movedir * timeDiff * m_fMoveSpeed;

			// strafing

			vector3df strafevect = m_Target;
			strafevect = strafevect.crossProduct(m_UpVector);

			if (m_bNoVerticalMovement)
				strafevect.Y = 0.0f;

			strafevect.normalize();

			if (m_CursorKeys[2])
				pos += strafevect * timeDiff * m_fMoveSpeed;

			if (m_CursorKeys[3])
				pos -= strafevect * timeDiff * m_fMoveSpeed;

			// jumping ( need's a gravity , else it's a fly to the World-UpVector )
			if (m_CursorKeys[4])
			{
				pos += m_UpVector * timeDiff * m_fJumpSpeed;
			}
		}

		// write translation

		SetPosition(pos);
	}

	// write right target

	m_TargetVector = m_Target;
	m_Target += pos;

}

void CameraFPSSceneNode::AllKeysUp()
{
	for (s32 i=0; i<6; ++i)
		m_CursorKeys[i] = FALSE;
}


//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void CameraFPSSceneNode::SetTarget(const vector3df& tgt)
{
	UpdateAbsolutePosition();
	vector3df vect = tgt - GetAbsolutePosition();
	vect = vect.getHorizontalAngle();
	m_RelativeRotation.X = vect.X;
	m_RelativeRotation.Y = vect.Y;

	if (m_RelativeRotation.X > MAX_VERTICAL_ANGLE)
		m_RelativeRotation.X -= 360.0f;
}

//! Disables or enables the camera to get key or mouse inputs.
void CameraFPSSceneNode::SetInputReceiverEnabled(BOOL enabled)
{
	// So we don't skip when we return from a non-enabled mode and the
	// mouse cursor is now not in the middle of the screen
	if( !m_bInputReceiverEnabled && enabled )
		m_bFirstUpdate = TRUE;

	m_bInputReceiverEnabled = enabled;
} 

//! Sets the rotation speed
void CameraFPSSceneNode::SetRotateSpeed(const f32 speed)
{
	m_fRotateSpeed = speed;	
}

//! Sets the movement speed
void CameraFPSSceneNode::SetMoveSpeed(const f32 speed)
{
	m_fMoveSpeed = speed;
}

//! Gets the rotation speed
f32 CameraFPSSceneNode::GetRotateSpeed()
{
	return m_fRotateSpeed;
}

// Gets the movement speed
f32 CameraFPSSceneNode::GetMoveSpeed()
{
	return m_fMoveSpeed;
}

} // end namespace
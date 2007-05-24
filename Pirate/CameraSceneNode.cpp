#include "CameraSceneNode.h"
#include "SceneManager.h"
#include "D3D9Driver.h"

namespace Pirate
{


//! constructor
CameraSceneNode::CameraSceneNode(SceneNode* parent, SceneManager* mgr, s32 id, 
	const vector3df& position, const vector3df& lookat)
	: SceneNode(parent, mgr, id, position, vector3df(0.0f, 0.0f, 0.0f),
	vector3df(1.0f, 1.0f, 1.0f)), m_bInputReceiverEnabled(TRUE), m_bIsOrthogonal(FALSE)
{
#ifdef _DEBUG
	SetDebugName("CameraSceneNode");
#endif

	// set default view

	m_UpVector.set(0.0f, 1.0f, 0.0f);
	m_Target.set(lookat);

	// set default projection

	m_fFovy = Pirate::PI / 4.0f;	// Field of view, in radians. 
	m_fAspect = 1.0f / 1.0f;	// Aspect ratio. 
	m_fZNear = 1.0f;		// value of the near view-plane. 
	m_fZFar = 5000.0f;		// Z-value of the far view-plane. 

	D3D9Driver* d = mgr->GetVideoDriver();
	if (d)
		m_fAspect = (f32)d->GetCurrentRenderTargetSize().Width /
		(f32)d->GetCurrentRenderTargetSize().Height;

	RecalculateProjectionMatrix();
	RecalculateViewArea();
}



//! destructor
CameraSceneNode::~CameraSceneNode()
{
}


//! Disables or enables the camera to get key or mouse inputs.
void CameraSceneNode::SetInputReceiverEnabled(BOOL enabled)
{
	m_bInputReceiverEnabled = enabled;
}


//! Returns if the input receiver of the camera is currently enabled.
BOOL CameraSceneNode::IsInputReceiverEnabled()
{
	return m_bInputReceiverEnabled;
}


//! Sets the projection matrix of the camera. The core::matrix4 class has some methods
//! to build a projection matrix. e.g: core::matrix4::buildProjectionMatrixPerspectiveFovLH
//! \param projection: The new projection matrix of the camera. 
void CameraSceneNode::SetProjectionMatrix(const matrix4& projection)
{
	m_ViewArea.Matrices [ ETS_PROJECTION ] = projection;
}



//! Gets the current projection matrix of the camera
//! \return Returns the current projection matrix of the camera.
const matrix4& CameraSceneNode::GetProjectionMatrix()
{
	return m_ViewArea.Matrices [ ETS_PROJECTION ];
}



//! Gets the current view matrix of the camera
//! \return Returns the current view matrix of the camera.
const matrix4& CameraSceneNode::GetViewMatrix()
{
	return m_ViewArea.Matrices [ ETS_VIEW ];
}



//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for 
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever. 
BOOL CameraSceneNode::OnEvent(SEvent event)
{
	return FALSE;
}



//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void CameraSceneNode::SetTarget(const vector3df& pos)
{
	m_Target = pos;
}



//! Gets the current look at target of the camera
//! \return Returns the current look at target of the camera
vector3df CameraSceneNode::GetTarget() const
{
	return m_Target;
}



//! sets the up vector of the camera
//! \param pos: New upvector of the camera.
void CameraSceneNode::SetUpVector(const vector3df& pos)
{
	m_UpVector = pos;
}



//! Gets the up vector of the camera.
//! \return Returns the up vector of the camera.
vector3df CameraSceneNode::GetUpVector() const
{
	return m_UpVector;
}


f32 CameraSceneNode::GetNearValue()
{
	return m_fZNear;
}

f32 CameraSceneNode::GetFarValue()
{
	return m_fZFar;
}

f32 CameraSceneNode::GetAspectRatio()
{
	return m_fAspect;
}

f32 CameraSceneNode::GetFOV()
{
	return m_fFovy;
}

void CameraSceneNode::SetNearValue(f32 f)
{
	m_fZNear = f;
	RecalculateProjectionMatrix();
}

void CameraSceneNode::SetFarValue(f32 f)
{
	m_fZFar = f;
	RecalculateProjectionMatrix();
}

void CameraSceneNode::SetAspectRatio(f32 f)
{
	m_fAspect = f;
	RecalculateProjectionMatrix();
}

void CameraSceneNode::SetFOV(f32 f)
{
	m_fFovy = f;
	RecalculateProjectionMatrix();
}

void CameraSceneNode::RecalculateProjectionMatrix()
{
	m_ViewArea.Matrices[ ETS_PROJECTION ].buildProjectionMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fZNear, m_fZFar);
}


//! prerender
void CameraSceneNode::OnRegisterSceneNode()
{
	// if upvector and vector to the target are the same, we have a
	// problem. so solve this problem:

	vector3df pos = GetAbsolutePosition();
	vector3df tgtv = m_Target - pos;
	tgtv.normalize();

	vector3df up = m_UpVector;
	up.normalize();

	f32 dp = tgtv.dotProduct(up);

	if ( Pirate::equals ( fabs ( dp ), 1.f ) )
	{
		up.X += 0.5f;
	}

	m_ViewArea.Matrices [ ETS_VIEW ].buildCameraLookAtMatrixLH(pos, m_Target, up);
	m_ViewArea.SetTransformState ( ETS_VIEW );
	RecalculateViewArea();

	if ( m_pSceneManager->GetActiveCamera () == this )
		m_pSceneManager->RegisterNodeForRendering(this, ESNRP_CAMERA);

	if (m_bIsVisible)
		SceneNode::OnRegisterSceneNode();
}



//! render
void CameraSceneNode::Render()
{	
	D3D9Driver* driver = m_pSceneManager->GetVideoDriver();
	if ( driver)
	{
		driver->SetTransform(ETS_PROJECTION, m_ViewArea.Matrices[ ETS_PROJECTION ] );
		driver->SetTransform(ETS_VIEW, m_ViewArea.Matrices[ ETS_VIEW ] );
	}
}


//! returns the axis aligned bounding box of this node
const aabbox3d<f32>& CameraSceneNode::GetBoundingBox() const
{
	return m_ViewArea.GetBoundingBox();
}



//! returns the view frustum. needed sometimes by bsp or lod render nodes.
const SViewFrustum* CameraSceneNode::GetViewFrustum() const
{
	return &m_ViewArea;
}

vector3df CameraSceneNode::GetAbsolutePosition() const
{
	return m_AbsoluteTransformation.getTranslation();
}

void CameraSceneNode::RecalculateViewArea()
{
	m_ViewArea.m_CameraPosition = GetAbsolutePosition();
	m_ViewArea.SetFrom ( m_ViewArea.Matrices [ SViewFrustum::ETS_VIEW_PROJECTION_3 ] );
}


} // end namespace
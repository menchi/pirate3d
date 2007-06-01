#include "SceneManager.h"
#include "MeshSceneNode.h"
#include "EmptySceneNode.h"
#include "BSPSceneNode.h"
#include "D3D9Driver.h"
#include "FileSystem.h"
#include "MeshCache.h"
#include "SMesh.h"
#include "BSPFileLoader.h"
#include "MDLFileLoader.h"
#include "os.h"
#include "IShaderConstantSetCallback.h"

namespace Pirate
{

	class ShaderConstantSetter : public IShaderConstantSetCallBack
	{
	public:
		virtual void OnSetConstants(D3D9Driver* services, s32 userData)
		{
			matrix4 ViewProjectionMatrix = services->GetTransform(ETS_PROJECTION) * 
										   services->GetTransform(ETS_VIEW) *
										   services->GetTransform(ETS_WORLD);
			services->SetVertexShaderConstant("mWorldViewProj", ViewProjectionMatrix.pointer(), 16);
		}
	};

//! constructor
SceneManager::SceneManager(D3D9Driver* driver, FileSystem* fs, DeviceWin32::CursorControl* cursorControl, MeshCache* cache)
	: SceneNode(0, 0), m_pDriver(driver), m_pFileSystem(fs), m_pActiveCamera(0), m_pCursorControl(cursorControl),
	m_pMeshCache(cache), m_eCurrentRendertime(ESNRP_COUNT)
{
#ifdef _DEBUG
	SceneManager::SetDebugName("SceneManager ISceneManager");
	SceneNode::SetDebugName("SceneManager ISceneNode");
#endif

	if (m_pDriver)
		m_pDriver->Grab();

	if (m_pFileSystem)
		m_pFileSystem->Grab();

	if (m_pCursorControl)
		m_pCursorControl->Grab();

	// create mesh cache if not there already
	if (!m_pMeshCache)
		m_pMeshCache = new MeshCache();
	else
		m_pMeshCache->Grab();
}



//! destructor
SceneManager::~SceneManager()
{
	if (m_pDriver)
		m_pDriver->Drop();

	if (m_pFileSystem)
		m_pFileSystem->Drop();

	if (m_pCursorControl)
		m_pCursorControl->Drop();

	if (m_pActiveCamera)
		m_pActiveCamera->Drop();

	if (m_pMeshCache)
		m_pMeshCache->Drop();

}

//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
SMesh* SceneManager::GetMesh(const c8* filename)
{
	SMesh* msh = 0;

	stringc name = filename;
	name.make_lower();

	msh = m_pMeshCache->FindMesh(name.c_str());
	if (msh)
		return msh;

	FileReader* file = m_pFileSystem->CreateAndOpenFile(filename);
	if (!file)
	{
		Printer::Log("Could not load mesh, because file could not be opened.", filename, ELL_ERROR);
		return 0;
	}

/*	s32 count = MeshLoaderList.size();
	for (s32 i=count-1; i>=0; --i)
	{
		if (MeshLoaderList[i]->isALoadableFileExtension(name.c_str()))
		{
			// reset file to avoid side effects of previous calls to createMesh
			file->seek(0);
			msh = MeshLoaderList[i]->createMesh(file);
			if (msh)
			{
				MeshCache->addMesh(filename, msh);
				msh->drop();
				break;
			}
		}
	}
*/
	BspFileLoader bspLoader(m_pFileSystem, m_pDriver);
	if (bspLoader.IsALoadableFileExtension(name.c_str()))
	{
		file->Seek(0);
		msh = bspLoader.CreateMesh(file);
		if (msh)
		{
			m_pMeshCache->AddMesh(filename, msh);
			msh->Drop();
		}
	}

	MDLFileLoader mdlLoader(m_pFileSystem, m_pDriver);
	if (mdlLoader.IsALoadableFileExtension(name.c_str()))
	{
		file->Seek(0);
		msh = mdlLoader.CreateMesh(file);
		if (msh)
		{
			m_pMeshCache->AddMesh(filename, msh);
			msh->Drop();
		}
	}

	file->Drop();

	if (!msh)
		Printer::Log("Could not load mesh, file format seems to be unsupported", filename, ELL_ERROR);
	else
		Printer::Log("Loaded mesh", filename, ELL_INFORMATION);

	return msh;
}

//! returns the video driver
D3D9Driver* SceneManager::GetVideoDriver()
{
	return m_pDriver;
}

//! adds a scene node for rendering a static mesh
//! the returned pointer must not be dropped.
MeshSceneNode* SceneManager::AddMeshSceneNode(SMesh* mesh, SceneNode* parent, s32 id,
											  const vector3df& position, const vector3df& rotation,
											  const vector3df& scale, BOOL alsoAddIfMeshPointerZero)
{
	if (!alsoAddIfMeshPointerZero && !mesh)
		return 0;

	if (!parent)
		parent = this;

	MeshSceneNode* node = new MeshSceneNode(mesh, parent, this, id, position, rotation, scale);
	node->Drop();

	return node;
}

//! adds a scene node for rendering a BSP tree
//! the returned pointer must not be dropped.
BSPSceneNode* SceneManager::AddBSPSceneNode(BSPTree* tree, SMesh* mesh, SceneNode* parent, s32 id,
											const vector3df& position, const vector3df& rotation,
											const vector3df& scale, BOOL alsoAddIfMeshPointerZero)
{
	if (!alsoAddIfMeshPointerZero && !mesh)
		return 0;

	if (!parent)
		parent = this;

	BSPSceneNode* node = new BSPSceneNode(tree, mesh, parent, this, id, position, rotation, scale);
	node->Drop();

	return node;
}

//! Adds a camera scene node to the tree and sets it as active camera.
CameraSceneNode* SceneManager::AddCameraSceneNode(SceneNode* parent, const vector3df& position, const vector3df& lookat, s32 id)
{
	if (!parent)
		parent = this;

	CameraSceneNode* node = new CameraSceneNode(parent, this, id, position, lookat);
	node->Drop();

	SetActiveCamera(node);

	return node;
}

//! Adds a camera scene node which is able to be controled with the mouse and keys
//! like in most first person shooters (FPS):
CameraSceneNode* SceneManager::AddCameraSceneNodeFPS(SceneNode* parent, f32 rotateSpeed, f32 moveSpeed, s32 id,
													 SKeyMap* keyMapArray, s32 keyMapSize, BOOL noVerticalMovement,f32 jumpSpeed)
{
	if (!parent)
		parent = this;

	CameraSceneNode* node = new CameraFPSSceneNode(parent, this, m_pCursorControl,
		id, rotateSpeed, moveSpeed, jumpSpeed, keyMapArray, keyMapSize, noVerticalMovement);
	node->Drop();

	SetActiveCamera(node);

	return node;
}

//! Adds an empty scene node.
SceneNode* SceneManager::AddEmptySceneNode(SceneNode* parent, s32 id)
{
	if (!parent)
		parent = this;

	SceneNode* node = new EmptySceneNode(parent, this, id);
	node->Drop();

	return node;
}

//! Returns the root scene node. This is the scene node wich is parent
//! of all scene nodes. The root scene node is a special scene node which
//! only exists to manage all scene nodes. It is not rendered and cannot
//! be removed from the scene.
//! \return Returns a pointer to the root scene node.
SceneNode* SceneManager::GetRootSceneNode()
{
	return this;
}

//! Returns the current active camera.
//! \return The active camera is returned. Note that this can be NULL, if there
//! was no camera created yet.
CameraSceneNode* SceneManager::GetActiveCamera()
{
	return m_pActiveCamera;
}

//! Sets the active camera. The previous active camera will be deactivated.
//! \param camera: The new camera which should be active.
void SceneManager::SetActiveCamera(CameraSceneNode* camera)
{
	if (m_pActiveCamera)
		m_pActiveCamera->Drop();

	m_pActiveCamera = camera;

	if (m_pActiveCamera)
		m_pActiveCamera->Grab();
}

//! renders the node.
void SceneManager::Render()
{
}

//! returns the axis aligned bounding box of this node
const aabbox3d<f32>& SceneManager::GetBoundingBox() const
{
	_PIRATE_DEBUG_BREAK_IF(true) // Bounding Box of Scene Manager wanted.

	// should never be used.
	return *((aabbox3d<f32>*)0);
}

//! returns if node is culled
BOOL SceneManager::IsCulled(SceneNode* node)
{
	CameraSceneNode* cam = GetActiveCamera();
	if (!cam)
		return FALSE;

	// Write frustum culling here!
/*
	if ( node->GetAutomaticCulling() )
	{
		// can be seen by cam pyramid planes ?
		SViewFrustum frust = *cam->GetViewFrustum();

		//transform the frustum to the node's current absolute transformation
		matrix4 invTrans(node->GetAbsoluteTransformation());
		invTrans.makeInverse();
		frust.Transform(invTrans);

		vector3df edges[8];
		node->GetBoundingBox().getEdges(edges);

		BOOL visible = TRUE;
		for (s32 i=0; i<SViewFrustum::VF_PLANE_COUNT; ++i)
		{
			BOOL boxInFrustum = FALSE;

			for (u32 j=0; j<8; ++j)
			{
				if (frust.planes[i].isFrontFacing(edges[j]) )
				{
					boxInFrustum = TRUE;
					break;
				}
			}

			if (!boxInFrustum)
			{
				visible = FALSE;
				break;
			}

		}

		return !visible;
	}
*/
	return FALSE;
}

//! registers a node for rendering it at a specific time.
u32 SceneManager::RegisterNodeForRendering(SceneNode* node, E_SCENE_NODE_RENDER_PASS time)
{
	u32 taken = 0;

	switch(time)
	{
		// take camera if it doesn't exists
	case ESNRP_CAMERA:
		{
			taken = 1;
			for ( u32 i = 0; i != m_CameraList.size(); ++i )
			{
				if ( m_CameraList[i] == node )
				{
					taken = 0;
					break;
				}
			}
			if ( taken )
			{
				m_CameraList.push_back(node);
			}
		}
		break;

	case ESNRP_LIGHT:
		// TODO: Point Light culling..
		// Lighting modell in irrlicht has to be redone..
		//if (!isCulled(node))
		{
			m_LightList.push_back(DistanceNodeEntry(node, m_CamWorldPos));
			taken = 1;
		}
		break;

	case ESNRP_SKY_BOX:
		m_SkyBoxList.push_back(node);
		taken = 1;
		break;
	case ESNRP_SOLID:
		if (!IsCulled(node))
		{
			m_SolidNodeList.push_back( node );
			taken = 1;
		}
		break;
	case ESNRP_TRANSPARENT:
		if (!IsCulled(node))
		{
			m_TransparentNodeList.push_back(TransparentNodeEntry(node, m_CamWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_AUTOMATIC:
		if (!IsCulled(node))
		{
			u32 count = node->GetMaterialCount();

			taken = 0;
			for (u32 i=0; i<count; ++i)
			{
/*				D3D9HLSLShader* rnd = m_pDriver->GetMaterialRenderer(node->GetMaterial(i).ShaderType);
				if (rnd && rnd->IsTransparent())
				{
					// register as transparent node
					TransparentNodeEntry e(node, m_CamWorldPos);
					TransparentNodeList.push_back(e);
					taken = 1;
					break;
				}
*/			}

			// not transparent, register as solid
			if ( 0 == taken )
			{
				m_SolidNodeList.push_back( node );
				taken = 1;
			}
		}
		break;
	case ESNRP_SHADOW:
		if (!IsCulled(node))
		{
			m_ShadowNodeList.push_back(node);
			taken = 1;
		}
		break;
	case ESNRP_SHADER_0:
	case ESNRP_SHADER_1:
	case ESNRP_SHADER_2:
	case ESNRP_SHADER_3:
	case ESNRP_SHADER_4:
	case ESNRP_SHADER_5:
	case ESNRP_SHADER_6:
	case ESNRP_SHADER_7:
	case ESNRP_SHADER_8:
	case ESNRP_SHADER_9:
	case ESNRP_SHADER_10:
		if ( !IsCulled(node) )
		{
			m_ShaderNodeList[ time - ESNRP_SHADER_0].push_back( ShaderNodeEntry ( node,time - ESNRP_SHADER_0 ));
			taken = 1;
		} break;

	case ESNRP_COUNT: // ignore this one
		break;
	}

	return taken;
}

//! This method is called just before the rendering process of the whole scene.
//! draws all scene nodes
void SceneManager::DrawAll()
{
	if (!m_pDriver)
		return;

	// reset all transforms
	D3D9Driver* driver = GetVideoDriver();

	// do animations and other stuff.
	OnAnimate(Timer::GetTime());

	/*!
	First Scene Node for prerendering should be the active camera
	consistent Camera is needed for culling
	*/
	m_CamWorldPos.set(0,0,0);
	if ( m_pActiveCamera )
	{
		m_pActiveCamera->OnRegisterSceneNode();
		m_CamWorldPos = m_pActiveCamera->GetAbsolutePosition();
	}

	// let all nodes register themselves
	OnRegisterSceneNode();

	u32 i; // new ISO for scoping problem in some compilers

	//render camera scenes
	{
		m_eCurrentRendertime = ESNRP_CAMERA;
		for (i=0; i<m_CameraList.size(); ++i)
			m_CameraList[i]->Render();

		m_CameraList.clear();
	}

	//render lights scenes
	{
		m_eCurrentRendertime = ESNRP_LIGHT;

		m_LightList.sort ();		// on distance to camera

		//u32 maxLights = Pirate::min_ ( m_pDriver->GetMaximalDynamicLightAmount(), m_LightList.size () );
		u32 maxLights = Pirate::min_ ( 2u, m_LightList.size () );
		for (i=0; i< maxLights; ++i)
			m_LightList[i].node->Render();

		m_LightList.clear();
	}

	// render skyboxes
	{
		m_eCurrentRendertime = ESNRP_SKY_BOX;

		for (i=0; i<m_SkyBoxList.size(); ++i)
			m_SkyBoxList[i]->Render();

		m_SkyBoxList.clear();
	}


	// render default objects
	{
		m_eCurrentRendertime = ESNRP_SOLID;
		m_SolidNodeList.sort(); // sort by textures

		for (i=0; i<m_SolidNodeList.size(); ++i)
			m_SolidNodeList[i].node->Render();

		m_SolidNodeList.clear();
	}

	// render shadows
	{
		m_eCurrentRendertime = ESNRP_SHADOW;
		for (i=0; i<m_ShadowNodeList.size(); ++i)
			m_ShadowNodeList[i]->Render();

//		if (!m_ShadowNodeList.empty())
//			m_pDriver->DrawStencilShadow(true, ShadowColor, ShadowColor,
//			ShadowColor, ShadowColor);

		m_ShadowNodeList.clear();
	}

	// render transparent objects.
	{
		m_eCurrentRendertime = ESNRP_TRANSPARENT;
		m_TransparentNodeList.sort(); // sort by distance from camera

		for (i=0; i<m_TransparentNodeList.size(); ++i)
			m_TransparentNodeList[i].node->Render();

		m_TransparentNodeList.clear();
	}

	// render shader objects.
	{
		for ( u32 g = 0; g!= ESNRP_SHADER_10 - ESNRP_SHADER_0 + 1; ++g )
		{
			m_eCurrentRendertime = (E_SCENE_NODE_RENDER_PASS) (ESNRP_SHADER_0 + g);

			const u32 size = m_ShaderNodeList[g].size();
			if ( 0 == size )
				continue;

			m_ShaderNodeList[g].sort(); // sort by textures
			for (i=0; i< size; ++i)
				m_ShaderNodeList[g][i].node->Render();

			m_ShaderNodeList[g].clear();
		}
	}

	m_eCurrentRendertime = ESNRP_COUNT;
}

//! Returns the first scene node with the specified name.
SceneNode* SceneManager::GetSceneNodeFromName(const char* name, SceneNode* start)
{
	if (start == 0)
		start = GetRootSceneNode();

	if (!strcmp(start->GetName(),name))
		return start;

	SceneNode* node = 0;

	const list<SceneNode*>& nodelist = start->GetChildren();
	list<SceneNode*>::Iterator it = nodelist.begin();
	for (; it!=nodelist.end(); ++it)
	{
		node = GetSceneNodeFromName(name, *it);
		if (node)
			return node;
	}

	return 0;
}

//! Returns the first scene node with the specified id.
SceneNode* SceneManager::GetSceneNodeFromId(s32 id, SceneNode* start)
{
	if (start == 0)
		start = GetRootSceneNode();

	if (start->GetID() == id)
		return start;

	SceneNode* node = 0;

	const list<SceneNode*>& nodelist = start->GetChildren();
	list<SceneNode*>::Iterator it = nodelist.begin();
	for (; it!=nodelist.end(); ++it)
	{
		node = GetSceneNodeFromId(id, *it);
		if (node)
			return node;
	}

	return 0;
}

//! Posts an input event to the environment. Usually you do not have to
//! use this method, it is used by the internal engine.
BOOL SceneManager::PostEventFromUser(SEvent event)
{
	BOOL ret = FALSE;
	CameraSceneNode* cam = GetActiveCamera();
	if (cam)
		ret = cam->OnEvent(event);

	return ret;
}


//! Removes all children of this scene node
void SceneManager::RemoveAll()
{
	SceneNode::RemoveAll();
	SetActiveCamera(0);
}


//! Clears the whole scene. All scene nodes are removed.
void SceneManager::Clear()
{
	RemoveAll();
}

//! Returns current render pass.
E_SCENE_NODE_RENDER_PASS SceneManager::GetSceneNodeRenderPass()
{
	return m_eCurrentRendertime;
}



//! Returns an interface to the mesh cache which is shared beween all existing scene managers.
MeshCache* SceneManager::GetMeshCache()
{
	return m_pMeshCache;
}


//! Creates a new scene manager.
SceneManager* SceneManager::CreateNewSceneManager()
{
	return new SceneManager(m_pDriver, m_pFileSystem, m_pCursorControl, m_pMeshCache);
}

// creates a scenemanager
SceneManager* CreateSceneManager(D3D9Driver* driver, FileSystem* fs, DeviceWin32::CursorControl* cursorcontrol )
{
	return new SceneManager(driver, fs, cursorcontrol, 0);
}

//! Loads a scene. Note that the current scene is not cleared before.
//! \param filename: Name of the file .
BOOL SceneManager::LoadScene(const c8* filename)
{
	SMesh* msh = 0;

	stringc name = filename;
	name.make_lower();

	msh = m_pMeshCache->FindMesh(name.c_str());
	if (msh)
		return TRUE;

	FileReader* file = m_pFileSystem->CreateAndOpenFile(filename);
	if (!file)
	{
		Printer::Log("Could not load mesh, because file could not be opened.", filename, ELL_ERROR);
		return 0;
	}

	ShaderConstantSetter* setter = NULL;

	SceneNode* pZUpToYUp = this->AddEmptySceneNode();
//	pZUpToYUp->SetRotation(vector3df(-90.f, 0.0f, 0.0f));

	BspFileLoader bspLoader(m_pFileSystem, m_pDriver);
	if (bspLoader.IsALoadableFileExtension(name.c_str()))
	{
		file->Seek(0);
		msh = (SMesh*)bspLoader.CreateMesh(file);
		if (msh)
		{
			m_pMeshCache->AddMesh(filename, msh);
			BSPSceneNode* pNode = this->AddBSPSceneNode(bspLoader.GetBSPTree(), msh, pZUpToYUp);
			//MeshSceneNode* pNode = this->AddMeshSceneNode(msh, pZUpToYUp);
			msh->Drop();

			setter = new ShaderConstantSetter();

			s32 newM = m_pDriver->AddHighLevelShaderMaterialFromFiles("..\\..\\Media\\GenericLightmap.hlsl", "vertexMain", 
				"..\\..\\Media\\GenericLightmap.hlsl", "pixelMain", setter);

			setter->Drop();

			for (u32 i=0; i<msh->GetMeshBufferCount(); i++)
			{
				msh->GetMeshBuffer(i)->m_Material.ShaderType = newM;
				//msh->GetMeshBuffer(i)->m_Material.BackfaceCulling = D3DCULL_CW;
				msh->GetMeshBuffer(i)->m_Material.Filter = D3DTEXF_LINEAR;
				//msh->GetMeshBuffer(i)->m_Material.Wireframe = TRUE;
			}
			pNode->SetReadOnlyMaterials(TRUE);
		}
	}

	s32 pathEnd = name.findLast('/');
	stringc path = name.subString(0, pathEnd + 1);

	for (int i=0; i<bspLoader.GetEntityCount(); i++)
	{
		stringc modelName = bspLoader.GetEntity(i).ModelName;

		if (modelName == "info_player_start" && !GetActiveCamera())
		{
			CameraSceneNode* pCamera = AddCameraSceneNodeFPS();
			vector3df tmpVec(bspLoader.GetEntity(i).Origin.Y, bspLoader.GetEntity(i).Origin.Z+32, -bspLoader.GetEntity(i).Origin.X);
			pCamera->SetPosition(tmpVec);
			pCamera->SetRotation(bspLoader.GetEntity(i).Angles);

			continue;
		}
/*
		pathEnd = modelName.findLast('/');
		name = path + modelName.subString(pathEnd + 1, modelName.size() - pathEnd);
		msh = GetMesh(name.c_str());

		if (!msh)
		{
			Printer::Log("Could not load mesh, file format seems to be unsupported", filename, ELL_ERROR);
			continue;
		}

		MeshSceneNode* pNode = this->AddMeshSceneNode(msh, pZUpToYUp);
		vector3df tmpV3df = bspLoader.GetEntity(i).Angles;
		pNode->SetRotation(tmpV3df);
		pNode->SetPosition(bspLoader.GetEntity(i).Origin);
		
		pNode->SetReadOnlyMaterials(TRUE);

		setter = new ShaderConstantSetter();

		s32 newM = m_pDriver->AddHighLevelShaderMaterialFromFiles("..\\..\\Media\\BaseTexture.hlsl", "vertexMain", 
			"..\\..\\Media\\BaseTexture.hlsl", "pixelMain", setter);

		setter->Drop();

		for (u32 i=0; i<msh->GetMeshBufferCount(); i++)
		{
			msh->GetMeshBuffer(i)->m_Material.ShaderType = newM;
			msh->GetMeshBuffer(i)->m_Material.BackfaceCulling = D3DCULL_CW;
			msh->GetMeshBuffer(i)->m_Material.Filter = D3DTEXF_LINEAR;
		}
*/
	}

	file->Drop();

	return TRUE;
}

} // end namespace

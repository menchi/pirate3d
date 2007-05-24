#ifndef _PIRATE_SCENE_MANAGER_H_
#define _PIRATE_SCENE_MANAGER_H_

#include "SceneNode.h"
#include "CameraFPSSceneNode.h"

namespace Pirate
{

//! Enumeration for render passes.
/** A parameter passed to the registerNodeForRendering() method of the ISceneManager,
specifying when the mode wants to be drawn in relation to the other nodes. */
enum E_SCENE_NODE_RENDER_PASS
{
	//! Camera pass. The active view is set up here.
	//! The very first pass.
	ESNRP_CAMERA,

	//! In this pass, lights are transformed into camera space and added to the driver
	ESNRP_LIGHT,

	//! This is used for sky boxes.
	ESNRP_SKY_BOX,

	//! All normal objects can use this for registering themselves.
	//! This value will never be returned by ISceneManager::getSceneNodeRenderPass().
	//! The scene manager will determine by itself if an object is
	//! transparent or solid and register the object as SNRT_TRANSPARENT or
	//! SNRT_SOLD automaticly if you call registerNodeForRendering with this
	//! value (which is default). Note that it will register the node only as ONE type.
	//! If your scene node has both solid and transparent material types register
	//! it twice (one time as SNRT_SOLID, the other time as SNRT_TRANSPARENT) and
	//! in the render() method call getSceneNodeRenderPass() to find out the current
	//! render pass and render only the corresponding parts of the node.
	ESNRP_AUTOMATIC,

	//! Solid scene nodes or special scene nodes without materials.
	ESNRP_SOLID,

	//! Drawn after the transparent nodes, the time for drawing shadow volumes
	ESNRP_SHADOW,

	//! Transparent scene nodes, drawn after shadow nodes. They are sorted from back
	//! to front and drawn in that order.
	ESNRP_TRANSPARENT,

	//! Scene Nodes with special support
	ESNRP_SHADER_0,
	ESNRP_SHADER_1,
	ESNRP_SHADER_2,
	ESNRP_SHADER_3,
	ESNRP_SHADER_4,
	ESNRP_SHADER_5,
	ESNRP_SHADER_6,
	ESNRP_SHADER_7,
	ESNRP_SHADER_8,
	ESNRP_SHADER_9,
	ESNRP_SHADER_10,

	//! Never used, value specifing how much parameters there are.
	ESNRP_COUNT
};

class MeshCache;
class D3D9Driver;
class FileSystem;
class MeshSceneNode;

struct SMesh;

/*!
The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
*/
class SceneManager : public virtual RefObject, public SceneNode
{
public:

	//! constructor
	SceneManager(D3D9Driver* driver, FileSystem* fs, DeviceWin32::CursorControl* cursorControl, MeshCache* cache = 0 );

	//! destructor
	~SceneManager();

	//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
	SMesh* GetMesh(const c8* filename);

	//! Returns an interface to the mesh cache which is shared beween all existing scene managers.
	MeshCache* GetMeshCache();

	//! returns the video driver
	D3D9Driver* GetVideoDriver();

	//! adds a scene node for rendering a static mesh
	//! the returned pointer must not be dropped.
	MeshSceneNode* AddMeshSceneNode(SMesh* mesh, SceneNode* parent=0, s32 id=-1,
		const vector3df& position = vector3df(0,0,0), 
		const vector3df& rotation = vector3df(0,0,0),
		const vector3df& scale = vector3df(1.0f, 1.0f, 1.0f),
		BOOL alsoAddIfMeshPointerZero=FALSE);

	//! renders the node.
	virtual void Render();

	//! returns the axis aligned bounding box of this node
	virtual const aabbox3d<f32>& GetBoundingBox() const;

	//! registers a node for rendering it at a specific time.
	u32 RegisterNodeForRendering(SceneNode* node, E_SCENE_NODE_RENDER_PASS = ESNRP_AUTOMATIC);

	//! draws all scene nodes
	void DrawAll();

	//! Adds a camera scene node to the tree and sets it as active camera.
	//! \param position: Position of the space relative to its parent where the camera will be placed.
	//! \param lookat: Position where the camera will look at. Also known as target.
	//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
	//! the camera will move too.
	//! \return Returns pointer to interface to camera
	CameraSceneNode* AddCameraSceneNode(SceneNode* parent = 0,
		const vector3df& position = vector3df(0,0,0), 
		const vector3df& lookat = vector3df(0,0,100), s32 id=-1);

	//! Adds a camera scene node which is able to be controled with the mouse and keys
	//! like in most first person shooters (FPS):
	CameraSceneNode* AddCameraSceneNodeFPS( SceneNode* parent = 0,
		f32 rotateSpeed = 1500.0f, f32 moveSpeed = 200.0f, s32 id=-1,
		SKeyMap* keyMapArray=0, s32 keyMapSize=0, BOOL noVerticalMovement=FALSE,
		f32 jumpSpeed = 0.f);

	//! Adds an empty scene node.
	SceneNode* AddEmptySceneNode(SceneNode* parent=0, s32 id=-1);

	//! Returns the root scene node. This is the scene node wich is parent 
	//! of all scene nodes. The root scene node is a special scene node which
	//! only exists to manage all scene nodes. It is not rendered and cannot
	//! be removed from the scene.
	//! \return Returns a pointer to the root scene node.
	SceneNode* GetRootSceneNode();

	//! Returns the current active camera.
	//! \return The active camera is returned. Note that this can be NULL, if there
	//! was no camera created yet.
	CameraSceneNode* GetActiveCamera();

	//! Sets the active camera. The previous active camera will be deactivated.
	//! \param camera: The new camera which should be active.
	void SetActiveCamera(CameraSceneNode* camera);

	//! Returns the first scene node with the specified id.
	SceneNode* GetSceneNodeFromId(s32 id, SceneNode* start=0);

	//! Returns the first scene node with the specified name.
	SceneNode* GetSceneNodeFromName(const c8* name, SceneNode* start=0);

	//! Posts an input event to the environment. Usually you do not have to
	//! use this method, it is used by the internal engine.
	BOOL PostEventFromUser(SEvent event);

	//! Clears the whole scene. All scene nodes are removed. 
	void Clear();

	//! Removes all children of this scene node
	virtual void RemoveAll();

	//! Returns current render pass. 
	E_SCENE_NODE_RENDER_PASS GetSceneNodeRenderPass();

	//! Creates a new scene manager. 
	SceneManager* CreateNewSceneManager();

	//! Loads a scene. Note that the current scene is not cleared before.
	//! \param filename: Name of the file .
	BOOL LoadScene(const c8* filename);

private:

	//! returns if node is culled
	BOOL IsCulled(SceneNode* node);

	struct DefaultNodeEntry
	{
		DefaultNodeEntry() {};

		DefaultNodeEntry(SceneNode* n)
		{
			textureValue = 0;

			if (n->GetMaterialCount())
				textureValue = (n->GetMaterial(0).Textures[0]);

			node = n;
		}

		SceneNode* node;
		void* textureValue;

		bool operator < (const DefaultNodeEntry& other) const
		{
			return (textureValue < other.textureValue);
		}
	};

	struct ShaderNodeEntry
	{
		ShaderNodeEntry() {};

		ShaderNodeEntry(SceneNode* n, u32 sceneTime )
		{
			textureValue = n->GetMaterial( sceneTime ).Textures[0];

			node = n;
		}

		SceneNode* node;
		void* textureValue;

		bool operator < (const ShaderNodeEntry& other) const
		{
			return (textureValue < other.textureValue);
		}
	};


	struct TransparentNodeEntry
	{
		TransparentNodeEntry() {};

		TransparentNodeEntry(SceneNode* n, const vector3df &camera)
		{
			node = n;

			// TODO: this could be optimized, by not using sqrt
			distance = (f32)(node->GetAbsoluteTransformation().getTranslation().getDistanceFrom(camera));
		}

		SceneNode* node;
		f32 distance;

		bool operator < (const TransparentNodeEntry& other) const
		{
			return (distance > other.distance);
		}
	};

	//! sort on distance (sphere) to camera
	struct DistanceNodeEntry
	{
		DistanceNodeEntry() {};

		DistanceNodeEntry(SceneNode* n, f64 d)
		{
			node = n;
			distance = d;
		}

		DistanceNodeEntry(SceneNode* n, const vector3df &cameraPos)
		{
			node = n;

			distance = (node->GetAbsoluteTransformation().getTranslation().getDistanceFromSQ(cameraPos));
			distance -= node->GetBoundingBox().getExtent().getLengthSQ() / 2.0;
		}

		SceneNode* node;

		f64 distance;

		bool operator < (const DistanceNodeEntry& other) const
		{
			return distance < other.distance;
		}
	};

	//! video driver
	D3D9Driver* m_pDriver;

	//! file system
	FileSystem* m_pFileSystem;

	//! cursor control
	DeviceWin32::CursorControl* m_pCursorControl;

	//! render pass lists
	array<SceneNode*> m_CameraList;
	array<DistanceNodeEntry> m_LightList;
	array<SceneNode*> m_ShadowNodeList;
	array<SceneNode*> m_SkyBoxList;
	array<DefaultNodeEntry> m_SolidNodeList;
	array<TransparentNodeEntry> m_TransparentNodeList;
	array<ShaderNodeEntry> m_ShaderNodeList[ ESNRP_SHADER_10 - ESNRP_SHADER_0 + 1];

	//! current active camera
	CameraSceneNode* m_pActiveCamera;
	vector3df m_CamWorldPos; // Position of camera for transparent nodes.

	//! Mesh cache
	MeshCache* m_pMeshCache;

	E_SCENE_NODE_RENDER_PASS m_eCurrentRendertime;

};

} // end namespace

#endif
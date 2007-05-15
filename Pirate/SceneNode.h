#ifndef _PIRATE_SCENE_NODE_H_
#define _PIRATE_SCENE_NODE_H_

#include "RefObject.h"
#include "SMaterial.h"
#include "pirateString.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "pirateList.h"

namespace Pirate
{

//! An enumeration for all types of debug data for built-in scene nodes (flags)
enum E_DEBUG_SCENE_TYPE 
{
	//! No Debug Data ( Default )
	EDS_OFF			= 0,
	//! Show Bounding Boxes of SceneNode
	EDS_BBOX		= 1,
	//! Show Vertex Normals
	EDS_NORMALS		= 2,
	//! Shows Skeleton/Tags
	EDS_SKELETON	= 4,
	//! Overlays Mesh Wireframe
	EDS_MESH_WIRE_OVERLAY	= 8,
	//! Temporary use transparency Material Type 
	EDS_HALF_TRANSPARENCY	= 16,
	//! Show Bounding Boxes of all MeshBuffers
	EDS_BBOX_BUFFERS		= 32,
	EDS_FULL		= EDS_BBOX | EDS_NORMALS | EDS_SKELETON | EDS_MESH_WIRE_OVERLAY
};

class SceneManager;

//! Scene node interface.
/** A scene node is a node in the hirachical scene graph. Every scene node may have children,
which are other scene nodes. Children move relative the their parents position. If the parent of a node is not
visible, its children won't be visible too. In this way, it is for example easily possible
to attach a light to a moving car, or to place a walking character on a moving platform
on a moving ship. */
class SceneNode : public virtual RefObject
{
public:

	//! Constructor
	SceneNode( SceneNode* parent, SceneManager* mgr, s32 id=-1,
		const vector3df& position = vector3df(0,0,0),
		const vector3df& rotation = vector3df(0,0,0),
		const vector3df& scale = vector3df(1.0f, 1.0f, 1.0f))
		: m_RelativeTranslation(position), m_RelativeRotation(rotation), m_RelativeScale(scale),
		m_pParent(parent), m_iID(id), m_pSceneManager(mgr), m_bAutomaticCullingState(TRUE),
		m_bIsVisible(TRUE), m_eDebugDataVisible(EDS_OFF), m_bIsDebugObject(FALSE)
	{
		if (m_pParent)
			m_pParent->AddChild(this);

		UpdateAbsolutePosition();
	}



	//! Destructor
	virtual ~SceneNode()
	{
		// delete all children
		RemoveAll();
	}


	//! This method is called just before the rendering process of the whole scene.
	/** Nodes may register themselves in the render pipeline during this call,
	precalculate the geometry which should be renderered, and prevent their
	children from being able to register them selfes if they are clipped by simply
	not calling their OnRegisterSceneNode-Method. 
	If you are implementing your own scene node, you should overwrite this method
	with an implementtion code looking like this:
	\code
	if (IsVisible)
	SceneManager->registerNodeForRendering(this);

	SceneNode::OnRegisterSceneNode();
	\endcode
	*/
	virtual void OnRegisterSceneNode()
	{
		if (m_bIsVisible)
		{
			list<SceneNode*>::Iterator it = m_Children.begin();
			for (; it != m_Children.end(); ++it)
				(*it)->OnRegisterSceneNode();
		}
	}

	//! OnAnimate() is called just before rendering the whole scene.
	//! Nodes may calculate or store animations here, and may do other useful things,
	//! dependent on what they are. Also, OnAnimate() should be called for all
	//! child scene nodes here. This method will called once per frame, independent
	//! of if the scene node is visible or not.
	//! \param timeMs: Current time in milli seconds.
	virtual void OnAnimate(u32 timeMs)
	{
		if (m_bIsVisible)
		{

			// update absolute position
			UpdateAbsolutePosition();

			// perform the post render process on all children

			list<SceneNode*>::Iterator it = m_Children.begin();
			for (; it != m_Children.end(); ++it)
				(*it)->OnAnimate(timeMs);
		}
	}


	//! Renders the node.
	virtual void Render() = 0;


	//! Returns the name of the node.
	//! \return Returns name as wide character string.
	virtual const c8* GetName() const
	{
		return m_Name.c_str();
	}


	//! Sets the name of the node.
	//! \param name: New name of the scene node.
	virtual void SetName(const c8* name)
	{
		m_Name = name;
	}


	//! Returns the axis aligned, not transformed bounding box of this node.
	//! This means that if this node is a animated 3d character, moving in a room,
	//! the bounding box will always be around the origin. To get the box in
	//! real world coordinates, just transform it with the matrix you receive with
	//! getAbsoluteTransformation() or simply use getTransformedBoundingBox(),
	//! which does the same.
	virtual const aabbox3d<f32>& GetBoundingBox() const = 0;


	//! Returns the axis aligned, transformed and animated absolute bounding box
	//! of this node.
	virtual const aabbox3d<f32> GetTransformedBoundingBox() const
	{
		aabbox3d<f32> box = GetBoundingBox();
		m_AbsoluteTransformation.transformBox(box);
		return box;
	}


	//! returns the absolute transformation of the node. Is recalculated every OnAnimate()-call.
	const matrix4& GetAbsoluteTransformation() const
	{
		return m_AbsoluteTransformation;
	}


	//! Returns the relative transformation of the scene node.
	//! The relative transformation is stored internally as 3 vectors:
	//! translation, rotation and scale. To get the relative transformation
	//! matrix, it is calculated from these values.
	//! \return Returns the relative transformation matrix.
	virtual matrix4 GetRelativeTransformation() const
	{
		matrix4 mat;
		mat.setRotationDegrees(m_RelativeRotation);
		mat.setTranslation(m_RelativeTranslation);

		if (m_RelativeScale != vector3df(1,1,1))
		{
			matrix4 smat;
			smat.setScale(m_RelativeScale);
			mat *= smat;
		}

		return mat;
	}


	//! Returns true if the node is visible. This is only an option, set by the user and has
	//! nothing to do with geometry culling
	virtual BOOL IsVisible() const
	{
		return m_bIsVisible;
	}


	//! Sets if the node should be visible or not. All children of this node won't be visible too.
	virtual void SetVisible(BOOL isVisible)
	{
		m_bIsVisible = isVisible;
	}


	//! Returns the id of the scene node. This id can be used to identify the node.
	virtual s32 GetID() const
	{
		return m_iID;
	}


	//! sets the id of the scene node. This id can be used to identify the node.
	virtual void SetID(s32 id)
	{
		m_iID = id;
	}


	//! Adds a child to this scene node. If the scene node already
	//! has got a parent, it is removed from there as child.
	virtual void AddChild(SceneNode* child)
	{
		if (child)
		{
			child->Grab();
			child->Remove(); // remove from old parent
			m_Children.push_back(child);
			child->m_pParent = this;
		}
	}


	//! Removes a child from this scene node.
	//! \return Returns true if the child could be removed, and false if not.
	virtual BOOL RemoveChild(SceneNode* child)
	{
		list<SceneNode*>::Iterator it = m_Children.begin();
		for (; it != m_Children.end(); ++it)
			if ((*it) == child)
			{
				(*it)->m_pParent = 0;
				(*it)->Drop();
				m_Children.erase(it);
				return TRUE;
			}

			return FALSE;
	}


	//! Removes all children of this scene node
	virtual void RemoveAll()
	{
		list<SceneNode*>::Iterator it = m_Children.begin();
		for (; it != m_Children.end(); ++it)
		{
			(*it)->m_pParent = 0;
			(*it)->Drop();
		}

		m_Children.clear();
	}


	//! Removes this scene node from the scene, deleting it.
	virtual void Remove()
	{
		if (m_pParent)
			m_pParent->RemoveChild(this);
	}


	//! Returns the material based on the zero based index i. To get the amount
	//! of materials used by this scene node, use getMaterialCount().
	//! This function is needed for inserting the node into the scene hirachy on a
	//! optimal position for minimizing renderstate changes, but can also be used
	//! to directly modify the material of a scene node.
	//! \param num: Zero based index. The maximal value is getMaterialCount() - 1.
	//! \return Returns the material of that index.
	virtual SMaterial& GetMaterial(u32 num)
	{
		return *((SMaterial*)0);
	}


	//! Returns amount of materials used by this scene node.
	//! \return Returns current count of materials used by this scene node.
	virtual u32 GetMaterialCount()
	{
		return 0;
	}

/*
	//! Sets all material flags at once to a new value. Helpful for
	//! example, if you want to be the the whole mesh to be lighted by
	//! \param flag: Which flag of all materials to be set.
	//! \param newvalue: New value of the flag.
	void SetMaterialFlag(E_MATERIAL_FLAG flag, BOOL newvalue)
	{
		for (u32 i=0; i<getMaterialCount(); ++i)
			getMaterial(i).setFlag(flag, newvalue);
	}
*/

	//! Sets the texture of the specified layer in all materials of this
	//! scene node to the new texture.
	//! \param textureLayer: Layer of texture to be set. Must be a value greater or
	//! equal than 0 and smaller than MATERIAL_MAX_TEXTURES.
	//! \param texture: Texture to be used.
	void SetMaterialTexture(u32 textureLayer, D3D9Texture* texture)
	{
		if (textureLayer >= MATERIAL_MAX_TEXTURES)
			return;

		for (u32 i=0; i<GetMaterialCount(); ++i)
			GetMaterial(i).Textures[textureLayer] = texture;
	}


	//! Sets the material type of all materials s32 this scene node
	//! to a new material type.
	//! \param newType: New type of material to be set.
	void SetMaterialType(s32 newType)
	{
		for (u32 i=0; i<GetMaterialCount(); ++i)
			GetMaterial(i).ShaderType = newType;
	}


	//! Gets the scale of the scene node.
	/** \return Returns the scale of the scene node. */
	virtual vector3df GetScale() const
	{
		return m_RelativeScale;
	}


	//! Sets the scale of the scene node.
	/** \param scale: New scale of the node */
	virtual void SetScale(const vector3df& scale)
	{
		m_RelativeScale = scale;
	}


	//! Gets the rotation of the node.
	/** Note that this is the relative rotation of the node.
	\return Current relative rotation of the scene node. */
	virtual const vector3df& GetRotation() const
	{
		return m_RelativeRotation;
	}


	//! Sets the rotation of the node.
	/** This only modifies the relative rotation of the node.
	\param rotation: New rotation of the node in degrees. */
	virtual void SetRotation(const vector3df& rotation)
	{
		m_RelativeRotation = rotation;
	}


	//! Gets the position of the node.
	/** Note that the position is relative to the parent.
	\return Returns the current position of the node relative to the parent. */
	virtual const vector3df GetPosition() const
	{
		return m_RelativeTranslation;
	}


	//! Sets the position of the node.
	/** Note that the position is relative to the parent.
	\param newpos: New relative postition of the scene node. */
	virtual void SetPosition(const vector3df& newpos)
	{
		m_RelativeTranslation = newpos;
	}


	//! Gets the abolute position of the node.
	/** The position is absolute.
	\return Returns the current absolute position of the scene node. */
	virtual vector3df GetAbsolutePosition() const
	{
		return m_AbsoluteTransformation.getTranslation();
	}


	//! Enables or disables automatic culling based on the bounding box.
	/** Automatic culling is enabled by default. Note that not
	all SceneNodes support culling (the billboard scene node for example)
	and that some nodes always cull their geometry because it is their
	only reason for existance, for example the OctreeSceneNode.
	\param enabled: If true, automatic culling is enabled.
	If false, it is disabled. */
	void SetAutomaticCulling( BOOL state)
	{
		m_bAutomaticCullingState = state;
	}


	//! Gets the automatic culling state.
	/** \return The node is culled based on its bounding box if this method
	returns true, otherwise no culling is performed. */
	BOOL GetAutomaticCulling() const
	{
		return m_bAutomaticCullingState;
	}


	//! Sets if debug data like bounding boxes should be drawn.
	/** Please note that not all scene nodes support this feature. */
	void SetDebugDataVisible(E_DEBUG_SCENE_TYPE visible)
	{
		m_eDebugDataVisible = visible;
	}

	//! Returns if debug data like bounding boxes are drawed.
	E_DEBUG_SCENE_TYPE IsDebugDataVisible() const
	{
		return m_eDebugDataVisible;
	}


	//! Sets if this scene node is a debug object.
	/** Debug objects have some special properties, for example they can be easily
	excluded from collision detection or from serialization, etc. */
	void SetIsDebugObject(BOOL debugObject)
	{
		m_bIsDebugObject = debugObject;
	}

	//! Returns if this scene node is a debug object.
	/** Debug objects have some special properties, for example they can be easily
	excluded from collision detection or from serialization, etc. */
	BOOL IsDebugObject()
	{
		return m_bIsDebugObject;
	}


	//! Returns a const reference to the list of all children.
	const list<SceneNode*>& GetChildren() const
	{
		return m_Children;
	}


	//! Changes the parent of the scene node.
	virtual void SetParent(SceneNode* newParent)
	{
		Grab();
		Remove();

		m_pParent = newParent;

		if (m_pParent)
			m_pParent->AddChild(this);

		Drop();
	}


	//! updates the absolute position based on the relative and the parents position
	virtual void UpdateAbsolutePosition()
	{
		if (m_pParent )
		{
			m_AbsoluteTransformation = m_pParent->GetAbsoluteTransformation() * GetRelativeTransformation();
		}
		else
			m_AbsoluteTransformation = GetRelativeTransformation();
	}

	//! Returns the parent of this scene node
	SceneNode* GetParent() const
	{
		return m_pParent;
	}


protected:

	//! name of the scene node.
	stringc m_Name;

	//! absolute transformation of the node.
	matrix4 m_AbsoluteTransformation;

	//! relative translation of the scene node.
	vector3df m_RelativeTranslation;

	//! relative rotation of the scene node.
	vector3df m_RelativeRotation;

	//! relative scale of the scene node.
	vector3df m_RelativeScale;

	//! Pointer to the parent
	SceneNode* m_pParent;

	//! List of all children of this node
	list<SceneNode*> m_Children;

	//! id of the node.
	s32 m_iID;

	//! pointer to the scene manager
	SceneManager* m_pSceneManager;

	//! automatic culling
	BOOL m_bAutomaticCullingState;

	//! is the node visible?
	BOOL m_bIsVisible;

	//! flag if debug data should be drawn, such as Bounding Boxes.
	E_DEBUG_SCENE_TYPE m_eDebugDataVisible;

	//! is debug object?
	BOOL m_bIsDebugObject;
};

} // end namespace

#endif
#include "MeshSceneNode.h"
#include "D3D9DriverWin32.h"
#include "SceneManager.h"

namespace Pirate
{

//! constructor
MeshSceneNode::MeshSceneNode(SMesh* mesh, SceneNode* parent, SceneManager* mgr, s32 id,
	const vector3df& position, const vector3df& rotation,
	const vector3df& scale)
	: SceneNode(parent, mgr, id, position, rotation, scale), m_pMesh(0), m_iPassCount(0),
	m_bReadOnlyMaterials(FALSE)
{
#ifdef _DEBUG
	SetDebugName("MeshSceneNode");
#endif

	SetMesh(mesh);
}



//! destructor
MeshSceneNode::~MeshSceneNode()
{
	if (m_pMesh)
		m_pMesh->Drop();
}



//! frame
void MeshSceneNode::OnRegisterSceneNode()
{
	if (m_bIsVisible)
	{
		// because this node supports rendering of mixed mode meshes consisting of 
		// transparent and solid material at the same time, we need to go through all 
		// materials, check of what type they are and register this node for the right
		// render pass according to that.

		D3D9Driver* driver = m_pSceneManager->GetVideoDriver();

		m_iPassCount = 0;
		int transparentCount = 0;
		int solidCount = 0;

		// count transparent and solid materials in this scene node
		if (m_bReadOnlyMaterials && m_pMesh)
		{
			// count mesh materials 

			for (u32 i=0; i<m_pMesh->GetMeshBufferCount(); ++i)
			{
				SD3D9MeshBuffer* mb = m_pMesh->GetMeshBuffer(i);
				D3D9HLSLShader* rnd = mb ? driver->GetMaterialRenderer(mb->GetMaterial().ShaderType) : 0;

				if (rnd && rnd->IsTransparent()) 
					++transparentCount;
				else 
					++solidCount;

				if (solidCount && transparentCount)
					break;
			}
		}
		else
		{
			// count copied materials 

			for (u32 i=0; i<m_Materials.size(); ++i)
			{
				D3D9HLSLShader* rnd = driver->GetMaterialRenderer(m_Materials[i].ShaderType);

				if (rnd && rnd->IsTransparent()) 
					++transparentCount;
				else 
					++solidCount;

				if (solidCount && transparentCount)
					break;
			}	
		}

		// register according to material types counted

		if (solidCount)
			m_pSceneManager->RegisterNodeForRendering(this, ESNRP_SOLID);

		if (transparentCount)
			m_pSceneManager->RegisterNodeForRendering(this, ESNRP_TRANSPARENT);

		SceneNode::OnRegisterSceneNode();
	}
}



//! renders the node.
void MeshSceneNode::Render()
{
	D3D9Driver* driver = m_pSceneManager->GetVideoDriver();

	if (!m_pMesh || !driver)
		return;

	BOOL isTransparentPass = m_pSceneManager->GetSceneNodeRenderPass() == ESNRP_TRANSPARENT;

	++m_iPassCount;

	m_Box = m_pMesh->GetBoundingBox();

	// for debug purposes only:
	if (m_eDebugDataVisible && m_iPassCount==1)
	{
		SMaterial m;
//		driver->SetMaterial(m);
//		driver->Draw3DBox(Box, SColor(0,255,255,255));

#if 0 // draw normals
		for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
		{
			scene::IMeshBuffer* mb = Mesh->getMeshBuffer(g);

			u32 vSize;
			u32 i;
			vSize = mb->getVertexPitch ();

			const video::S3DVertex* v = ( const video::S3DVertex*)mb->getVertices();
			video::SColor c ( 255, 128 ,0, 0 );
			video::SColor c1 ( 255, 255 ,0, 0 );
			for ( i = 0; i != mb->getVertexCount(); ++i )
			{
				core::vector3df h = v->Normal * 5.f;
				core::vector3df h1 = h.crossProduct ( core::vector3df ( 0.f, 1.f, 0.f ) );

				driver->draw3DLine ( v->Pos, v->Pos + h, c );
				driver->draw3DLine ( v->Pos + h, v->Pos + h + h1, c1 );
				v = (const video::S3DVertex*) ( (u8*) v + vSize );
			}

		}
#endif // Draw normals
	}

	for (u32 i=0; i<m_pMesh->GetMeshBufferCount(); ++i)
	{
		SD3D9MeshBuffer* mb = m_pMesh->GetMeshBuffer(i);
		if (mb)
		{
			const SMaterial& material = m_bReadOnlyMaterials ? mb->GetMaterial() : m_Materials[i];

			D3D9HLSLShader* rnd = driver->GetMaterialRenderer(material.ShaderType);
			BOOL transparent = (rnd && rnd->IsTransparent());

			// only render transparent buffer if this is the transparent render pass
			// and solid only in solid pass
			if (transparent == isTransparentPass) 
			{
				driver->SetMaterial(material);
				driver->DrawMeshBuffer(mb);
			}
		}
	}			
}


//! returns the axis aligned bounding box of this node
const aabbox3d<f32>& MeshSceneNode::GetBoundingBox() const
{
	return m_pMesh ? m_pMesh->GetBoundingBox() : m_Box;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
SMaterial& MeshSceneNode::GetMaterial(u32 i)
{
	if (m_pMesh && m_bReadOnlyMaterials && i<m_pMesh->GetMeshBufferCount())
	{
		m_TmpReadOnlyMaterial = m_pMesh->GetMeshBuffer(i)->GetMaterial();
		return m_TmpReadOnlyMaterial;
	}

	if ( i >= m_Materials.size())
		return SceneNode::GetMaterial(i);

	return m_Materials[i];
}



//! returns amount of materials used by this scene node.
u32 MeshSceneNode::GetMaterialCount()
{
	if (m_pMesh && m_bReadOnlyMaterials)
		return m_pMesh->GetMeshBufferCount();

	return m_Materials.size();
}



//! Sets a new mesh
void MeshSceneNode::SetMesh(SMesh* mesh)
{
	if (!mesh)
		return; // won't set null mesh

	if (m_pMesh)
		m_pMesh->Drop();

	m_pMesh = mesh;
	CopyMaterials();

	if (m_pMesh)
		m_pMesh->Grab();
}


void MeshSceneNode::CopyMaterials()
{
	m_Materials.clear();

	if (m_pMesh)
	{
		SMaterial mat;

		for (u32 i=0; i<m_pMesh->GetMeshBufferCount(); ++i)
		{
			SD3D9MeshBuffer* mb = m_pMesh->GetMeshBuffer(i);
			if (mb)
				mat = mb->GetMaterial();

			m_Materials.push_back(mat);
		}
	}
}


//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
referencing this mesh to change too. */
void MeshSceneNode::SetReadOnlyMaterials(BOOL readonly)
{
	m_bReadOnlyMaterials = readonly;
}

//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
BOOL MeshSceneNode::IsReadOnlyMaterials()
{
	return m_bReadOnlyMaterials;
}


} // end namespace
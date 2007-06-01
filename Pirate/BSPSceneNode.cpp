#include "BSPSceneNode.h"
#include "D3D9Driver.h"
#include "SceneManager.h"
#include "D3D9HLSLShader.h"
#include "SMesh.h"
#include "BSPFileLoader.h"
#include "OS.h"

namespace Pirate
{

//! constructor
BSPSceneNode::BSPSceneNode(BSPTree* tree, SMesh* mesh, SceneNode* parent, SceneManager* mgr, s32 id,
	const vector3df& position, const vector3df& rotation,
	const vector3df& scale)
	: MeshSceneNode(mesh, parent, mgr, id, position, rotation, scale), m_pBSPTree(0), m_iLastCluster(-1)
{
#ifdef _DEBUG
	SetDebugName("BSPSceneNode");
#endif

	SetBSPTree(tree);
}



//! destructor
BSPSceneNode::~BSPSceneNode()
{
	if (m_pBSPTree)
		m_pBSPTree->Drop();
}



//! frame
void BSPSceneNode::OnRegisterSceneNode()
{
	if (m_bIsVisible)
	{
		// because this node supports rendering of mixed mode meshes consisting of 
		// transparent and solid material at the same time, we need to go through all 
		// materials, check of what type they are and register this node for the right
		// render pass according to that.

		D3D9Driver* driver = m_pSceneManager->GetVideoDriver();

		m_pSceneManager->RegisterNodeForRendering(this, ESNRP_SOLID);

//		m_pSceneManager->RegisterNodeForRendering(this, ESNRP_TRANSPARENT);

		SceneNode::OnRegisterSceneNode();
	}
}



//! renders the node.
void BSPSceneNode::Render()
{
	D3D9Driver* driver = m_pSceneManager->GetVideoDriver();

	if (!m_pMesh || !driver)
		return;

	BOOL isTransparentPass = m_pSceneManager->GetSceneNodeRenderPass() == ESNRP_TRANSPARENT;

	driver->SetTransform(ETS_WORLD, m_AbsoluteTransformation);
	m_Box = m_pMesh->GetBoundingBox();

	CameraSceneNode* pCamera = m_pSceneManager->GetActiveCamera();
	FindVisibleMeshBuffers(pCamera->GetPosition());
	vector3df camPos = pCamera->GetAbsolutePosition();

	for (u32 i=0; i<m_pMesh->GetMeshBufferCount(); ++i)
	{
		SD3D9MeshBuffer* mb = m_pMesh->GetMeshBuffer(i);
		if (mb && (m_pBSPTree->m_Flags[i] & BSPTree::EFF_VISIBLE) )
		{
			if ( m_pBSPTree->m_FaceNormals[i].dotProduct(mb->GetBoundingBox().MinEdge - camPos) > 0.0f &&
				 m_pBSPTree->m_FaceNormals[i].dotProduct(mb->GetBoundingBox().MaxEdge - camPos) > 0.0f)
				continue;

			const SMaterial& material = mb->GetMaterial();

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
const aabbox3d<f32>& BSPSceneNode::GetBoundingBox() const
{
	return m_pBSPTree->m_Nodes[0].Bounding;
}

//! Sets a new bsp tree
void BSPSceneNode::SetBSPTree(BSPTree* tree)
{
	if (!tree)
		return; // won't set null mesh

	if (m_pBSPTree)
		m_pBSPTree->Drop();

	m_pBSPTree = tree;

	if (m_pBSPTree)
		m_pBSPTree->Grab();
}

void BSPSceneNode::FindVisibleMeshBuffers(vector3df camera)
{
	s16 cluster = m_pBSPTree->m_Leaves[m_pBSPTree->GetLeafByPoint(camera, 0)].Cluster;

	if (cluster == m_iLastCluster)
		return;

	m_iLastCluster = cluster;
/*
	if (!m_pBSPTree->m_Flags.size())
	{
		m_pBSPTree->m_Flags.reallocate(m_pMesh->GetMeshBufferCount());
		m_pBSPTree->m_Flags.set_used(m_pMesh->GetMeshBufferCount());
	}
*/
	for (u32 i=0; i<m_pMesh->GetMeshBufferCount(); i++)
		m_pBSPTree->m_Flags[i] &= 0xFE;

	if (cluster == -1)
	{
		for (u32 i=0; i<m_pBSPTree->m_Flags.size(); i++)
			m_pBSPTree->m_Flags[i] |= BSPTree::EFF_VISIBLE;
		return;
	}

	s32 num_clusters = *m_pBSPTree->m_pVisLump;
	s32 v = *(m_pBSPTree->m_pVisLump + 2 * cluster + 1);
	u8* pvs_buffer = (u8*)m_pBSPTree->m_pVisLump;
	for (s32 c = 0; c < num_clusters; v++) 
	{
		if (pvs_buffer[v] == 0) 
		{
			v++;
			c += 8 * pvs_buffer[v];
		} 
		else 
		{
			for (u8 bit = 1; bit != 0; bit *= 2, c++) 
			{
				if (pvs_buffer[v] & bit) 
				{
					s32 leaf_index = m_pBSPTree->GetLeafByCluster(c);
					u32 num_leaffaces = m_pBSPTree->m_Leaves[leaf_index].Leaffaces.size();
					for (u32 i=0; i<num_leaffaces; i++)
					{
						u16 visibleFace = m_pBSPTree->m_Leaves[leaf_index].Leaffaces[i];
						if (!(m_pBSPTree->m_Flags[visibleFace] & BSPTree::EFF_SPECIAL_MATERIAL))
							m_pBSPTree->m_Flags[visibleFace] |= 0x01;
					}
				}
			}
		}
	}
}

} // end namespace
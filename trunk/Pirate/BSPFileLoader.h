#ifndef _PIRATE_BSP_FILE_LOADER_H_
#define _PIRATE_BSP_FILE_LOADER_H_

#include "RefObject.h"
#include "pirateString.h"
#include "vector3d.h"

namespace Pirate
{

struct SMesh;
class D3D9Driver;
class FileSystem;

struct BSPTree : public virtual RefObject
{
public:
	BSPTree() : m_pVisLump(0)
	{
#ifdef _DEBUG
		SetDebugName("BSPTree");
#endif
	}

	~BSPTree()
	{
		if (m_pVisLump)
			free(m_pVisLump);
	}

	s32 GetLeafByPoint(vector3df pos, s32 root = 0) const
	{
		s32 r = -1;

		if (m_Nodes[root].Plane.classifyPointRelation(pos) == ISREL3D_FRONT || 
			m_Nodes[root].Plane.classifyPointRelation(pos) == ISREL3D_PLANAR)
		{
			if (m_Nodes[root].Children[1] < 0)
				r = (-1 - m_Nodes[root].Children[1]);
			else
				r = GetLeafByPoint(pos, m_Nodes[root].Children[1]);
		}
		else
		{
			if (m_Nodes[root].Children[0] < 0)
				r = (-1 - m_Nodes[root].Children[0]);
			else
				r = GetLeafByPoint(pos, m_Nodes[root].Children[0]);
		}

		return r;
	}

	s32 GetLeafByCluster(s32 cluster) const 
	{
		for (u32 i=0; i<m_Leaves.size(); i++)
		{
			if (m_Leaves[i].Cluster == cluster)
				return i;
		}

		return -1;
	}

	struct Node
	{
		plane3df	Plane;
		s32			Children[2];
		aabbox3df	Bounding;
	};

	struct Leaf
	{
		s16			Cluster;      
		aabbox3df	Bounding;
		array<u16>	Leaffaces;
	};

	enum E_FACE_FLAG
	{
		EFF_VISIBLE = 0x01,
		EFF_SPECIAL_MATERIAL = 0x02
	};

	array<vector3df> m_FaceNormals;
	array<u8> m_Flags;
	array<Node> m_Nodes;
	array<Leaf> m_Leaves;
	s32* m_pVisLump;
};

//! Meshloader capable of loading bsp level.
class BspFileLoader : public virtual RefObject
{
public:

	struct EntityInfo
	{
		stringc		ModelName;
		vector3df	Origin;       // origin
		vector3df	Angles;       // orientation (pitch roll yaw)
	};

	//! Constructor
	BspFileLoader(FileSystem* fs, D3D9Driver* driver);

	//! destructor
	~BspFileLoader();

	//! Returns true if the file maybe is able to be loaded by this class.
	/** This decision should be based only on the file extension (e.g. ".cob") */
	BOOL IsALoadableFileExtension(const c8* fileName);

	//! Creates/loads an animated mesh from the file.
	/** \return Pointer to the created mesh. Returns 0 if loading failed.
	If you no longer need the mesh, you should call IAnimatedMesh::drop().
	See IUnknown::drop() for more information. */
	SMesh* CreateMesh(FileReader* file);

	//! Get entity number in current bsp
	const s32 GetEntityCount() const;

	//! Get entity information
	const EntityInfo& GetEntity(s32 i) const;

	//! Get BSP tree for visibility test
	BSPTree* GetBSPTree() const;

private:

	BOOL ParseModels(const stringc& entity);
	BOOL ParsePlayerStartInfo(const stringc& entity);

	FileSystem* m_pFileSystem;
	D3D9Driver* m_pDriver;
	array<EntityInfo> m_Entities;
	BSPTree* m_pBSPTree;
};


} // end namespace 

#endif
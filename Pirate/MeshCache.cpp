#include "SMesh.h"
#include "MeshCache.h"

namespace Pirate
{

MeshCache::~MeshCache()
{
	for (u32 i=0; i<m_Meshes.size(); ++i)
		m_Meshes[i].Mesh->Drop();
}


//! adds a mesh to the list
void MeshCache::AddMesh(const c8* filename, SMesh* mesh)
{
	mesh->Grab();

	MeshEntry e;
	e.Mesh = mesh;
	e.Name = filename;
	e.Name.make_lower();

	m_Meshes.push_back(e);
}


//! Returns amount of loaded meshes
s32 MeshCache::GetMeshCount()
{
	return m_Meshes.size();
}


//! Returns a mesh based on its index number
SMesh* MeshCache::GetMeshByIndex(s32 number)
{
	if (number < 0 || number >= (s32)m_Meshes.size())
		return 0;

	return m_Meshes[number].Mesh;
}



//! Returns current number of the mesh 
s32 MeshCache::GetMeshIndex(SMesh* mesh)
{
	for (int i=0; i<(int)m_Meshes.size(); ++i)
		if (m_Meshes[i].Mesh == mesh)
			return i;

	return -1;
}



//! Returns name of a mesh based on its index number
const c8* MeshCache::GetMeshFilename(s32 number)
{
	if (number < 0 || number >= (s32)m_Meshes.size())
		return 0;

	return m_Meshes[number].Name.c_str();
}


//! Returns the filename of a loaded mesh, if there is any. Returns 0 if there is none.
const c8* MeshCache::GetMeshFilename(SMesh* mesh)
{
	for (s32 i=0; i<(s32)m_Meshes.size(); ++i)
	{
		if (m_Meshes[i].Mesh && m_Meshes[i].Mesh == mesh)
			return m_Meshes[i].Name.c_str();
	}

	return 0;
}



//! returns if a mesh already was loaded
BOOL MeshCache::IsMeshLoaded(const c8* filename)
{
	stringc name = filename;
	name.make_lower();
	return FindMesh(name.c_str()) != 0;
}


//! returns an already loaded mesh
SMesh* MeshCache::FindMesh(const c8* lowerMadeFilename)
{
	MeshEntry e;
	e.Name = lowerMadeFilename;
	s32 id = m_Meshes.binary_search(e);
	return (id != -1) ? m_Meshes[id].Mesh : 0;
}


//! Removes a mesh from the cache.
void MeshCache::RemoveMesh(SMesh* mesh)
{
	if ( mesh )
		for (int i=0; i<(int)m_Meshes.size(); ++i)
		{
			if (m_Meshes[i].Mesh == mesh)
			{
				m_Meshes[i].Mesh->Drop();
				m_Meshes.erase(i);
				return;
			}
		}
}


} // end namespace
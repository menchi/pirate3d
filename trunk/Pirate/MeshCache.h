#ifndef __C_MESH_CACHE_H_INCLUDED__
#define __C_MESH_CACHE_H_INCLUDED__

#include "RefObject.h"
#include "pirateString.h"
#include "pirateArray.h"

namespace Pirate
{

struct SMesh;

class MeshCache : public virtual RefObject
{
public:

	~MeshCache();

	//! Adds a mesh to the internal list of loaded meshes.
	/** Usually, ISceneManager::getMesh() is called to load a mesh from file.
	That method searches the list of loaded meshes if a mesh has already been loaded and
	returns a pointer to if	it is in that list and already in memory. Otherwise it loads
	the mesh. With IMeshCache::addMesh(), it is possible to pretend that a mesh already
	has been loaded. This method can be used for example by mesh loaders who need to
	load more than one mesh with one call. They can add additional meshes with this
	method to the scene manager. The COLLADA loader for example uses this method.
	\param filename: Filename of the mesh. When called ISceneManager::getMesh() with this 
	parameter, the method will return the mesh parameter given with this method. 
	\param mesh: Pointer to a mesh which will now be referenced by this name. */ 
	void AddMesh(const c8* filename, SMesh* mesh);

	//! Removes a mesh from the cache.
	/** After loading a mesh with getMesh(), the mesh can be removed from the cache
	using this method, freeing a lot of memory.	*/
	void RemoveMesh(SMesh* mesh);

	//! Returns amount of loaded meshes in the cache.
	/** You can load new meshes into the cache using getMesh() and addMesh(). 
	If you ever need to access the internal mesh cache, you can do this using
	removeMesh(), getMeshNumber(), getMeshByIndex() and getMeshFilename() */
	s32 GetMeshCount();

	//! Returns current index number of the mesh, and -1 if it is not in the cache.
	s32 GetMeshIndex(SMesh* mesh);

	//! Returns a mesh based on its index number.
	/** \param index: Index of the mesh, number between 0 and getMeshCount()-1. 
	Note that this number is only valid until a new mesh is loaded or removed *
	\return Returns pointer to the mesh or 0 if there is none with this number. */
	SMesh* GetMeshByIndex(s32 index);

	//! Returns name of a mesh based on its index number. 
	/** \param index: Index of the mesh, number between 0 and getMeshCount()-1. 
	Note that this is only valid until a new mesh is loaded */
	const c8* GetMeshFilename(s32 index);

	//! Returns the filename of a loaded mesh, if there is any.
	/* Returns 0 if there is none.*/
	const c8* GetMeshFilename(SMesh* mesh);

	//! returns if a mesh already was loaded
	BOOL IsMeshLoaded(const c8* filename);

	//! returns an already loaded mesh
	SMesh* FindMesh(const c8* lowerMadeFilename);

protected:

	struct MeshEntry
	{
		stringc Name;
		SMesh* Mesh;

		bool operator < (const MeshEntry& other) const
		{
			return (Name < other.Name);
		}
	};

	//! loaded meshes
	array<MeshEntry> m_Meshes;
};


} // end namespace

#endif
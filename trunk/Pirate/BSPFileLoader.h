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

private:

	BOOL ParseModels(const stringc& entity);
	BOOL ParsePlayerStartInfo(const stringc& entity);

	FileSystem* m_pFileSystem;
	D3D9Driver* m_pDriver;
	array<EntityInfo> m_Entities;
};


} // end namespace 

#endif
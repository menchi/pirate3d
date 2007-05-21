#ifndef _PIRATE_MDL_FILE_LOADER_H_
#define _PIRATE_MDL_FILE_LOADER_H_

#include "RefObject.h"

namespace Pirate
{

struct SMesh;
class D3D9Driver;
class FileSystem;

//! Meshloader capable of loading bsp level.
class MDLFileLoader : public virtual RefObject
{
public:

	//! Constructor
	MDLFileLoader(FileSystem* fs, D3D9Driver* driver);

	//! destructor
	~MDLFileLoader();

	//! Returns true if the file maybe is able to be loaded by this class.
	/** This decision should be based only on the file extension (e.g. ".cob") */
	BOOL IsALoadableFileExtension(const c8* fileName);

	//! Creates/loads an animated mesh from the file.
	/** \return Pointer to the created mesh. Returns 0 if loading failed.
	If you no longer need the mesh, you should call IAnimatedMesh::drop().
	See IUnknown::drop() for more information. */
	SMesh* CreateMesh(FileReader* file);

private:

	FileSystem* m_pFileSystem;
	D3D9Driver* m_pDriver;
};


} // end namespace 

#endif
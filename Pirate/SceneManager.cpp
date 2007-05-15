#include "SceneManager.h"
#include "D3D9DriverWin32.h"
#include "FileSystem.h"


#include "os.h"

#include "CameraSceneNode.h"
#include "MeshSceneNode.h"

namespace Pirate
{

//! constructor
SceneManager::SceneManager(D3D9Driver* driver, FileSystem* fs, MeshCache* cache)
	: SceneNode(0, 0), m_pDriver(driver), m_pFileSystem(fs), m_pActiveCamera(0),
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

	// create mesh cache if not there already
/*	if (!m_pMeshCache)
		m_pMeshCache = new MeshCache();
	else
		m_pMeshCache->Grab();*/
}



//! destructor
SceneManager::~SceneManager()
{
	if (m_pDriver)
		m_pDriver->Drop();

	if (m_pFileSystem)
		m_pFileSystem->Drop();

	u32 i;

	if (m_pActiveCamera)
		m_pActiveCamera->Drop();
/*
	if (m_pMeshCache)
		m_pMeshCache->Drop();
*/
}

} // end namespace

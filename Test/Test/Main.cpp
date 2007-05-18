#include "..\..\Pirate\Pirate.h"

using namespace Pirate;

class SetSimpleWhiteShaderConstant : public IShaderConstantSetCallBack
{
public:
	SetSimpleWhiteShaderConstant(CameraSceneNode* pCamera, MeshSceneNode* pMeshNode) 
	: m_pCamera(pCamera), m_pMeshNode(pMeshNode)
	{
		if (m_pCamera)
			m_pCamera->Grab();
		if (m_pMeshNode)
			m_pMeshNode->Grab();
	}
	~SetSimpleWhiteShaderConstant()
	{
		if (m_pCamera)
			m_pCamera->Drop();
		if (m_pMeshNode)
			m_pMeshNode->Drop();
	}
	virtual void OnSetConstants(D3D9Driver* services, s32 userData)
	{
		matrix4 ViewProjectionMatrix = m_pCamera->GetProjectionMatrix()*m_pCamera->GetViewMatrix()*m_pMeshNode->GetAbsoluteTransformation();
		services->SetVertexShaderConstant("mWorldViewProj", ViewProjectionMatrix.pointer(), 16);
	}
	CameraSceneNode* m_pCamera;
	MeshSceneNode* m_pMeshNode;
};

void main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	DeviceWin32* pDevice = InvokePirate(800, 600);
	D3D9Driver* pDriver = pDevice->GetVideoDriver();

	IDirect3DDevice9* pD3DDevice = pDriver->GetExposedVideoData().D3DDev9;

	SceneManager* pSceneManager = pDevice->GetSceneManager();
	CameraSceneNode* pCamera = pSceneManager->AddCameraSceneNodeFPS(pDevice->GetWin32CursorControl());
	//CameraSceneNode* pCamera = pSceneManager->AddCameraSceneNode();
	SMesh* pMesh = pSceneManager->GetMesh("../../Media/firstmap.bsp");
	MeshSceneNode* pMeshNode = pSceneManager->AddMeshSceneNode(pMesh);
	pMeshNode->SetRotation(vector3df(-90.f, 0.f, 0.f));

	SetSimpleWhiteShaderConstant* setter = new SetSimpleWhiteShaderConstant(pSceneManager->GetActiveCamera(), pMeshNode);
	s32 newM = pDriver->AddHighLevelShaderMaterialFromFiles("..\\..\\Media\\GenericLightmap.hlsl", "vertexMain", 
		"..\\..\\Media\\GenericLightmap.hlsl", "pixelMain", setter);
	setter->Drop();

	for (u32 i=0; i<pMesh->GetMeshBufferCount(); i++)
	{
		pMesh->GetMeshBuffer(i)->m_Material.ShaderType = newM;
		pMesh->GetMeshBuffer(i)->m_Material.BackfaceCulling = D3DCULL_CW;
		pMesh->GetMeshBuffer(i)->m_Material.Filter = D3DTEXF_LINEAR;
	}
	pMeshNode->SetReadOnlyMaterials(TRUE);

	int lastFPS = -1;

	while(pDevice->Run())
	{
		pDriver->BeginScene(TRUE, TRUE, SColor(255,0,55,55));
		pSceneManager->DrawAll();
		pDriver->EndScene();

		int fps = pDriver->GetFPS();
		int prim = (int)pDriver->GetPrimitiveCountDrawn(1);

		if (lastFPS != fps)
		{
			stringw str = L"Pirate Engine - [BSP View] ";
			str += "FPS:";
			str += fps;
			str += " Drawn primitives:";
			str += prim;

			pDevice->SetWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}

	pDevice->Drop();
}


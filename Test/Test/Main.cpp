#include "..\..\Pirate\Pirate.h"

using namespace Pirate;

class SetSimpleWhiteShaderConstant : public IShaderConstantSetCallBack
{
public:
	SetSimpleWhiteShaderConstant(CameraSceneNode* pCamera) : m_pCamera(pCamera)
	{
		if (m_pCamera)
			m_pCamera->Grab();
	}
	~SetSimpleWhiteShaderConstant()
	{
		if (m_pCamera)
			m_pCamera->Drop();
	}
	virtual void OnSetConstants(D3D9Driver* services, s32 userData)
	{
		matrix4 ViewProjectionMatrix = m_pCamera->GetProjectionMatrix()*m_pCamera->GetViewMatrix();
		services->SetVertexShaderConstant("mWorldViewProj", ViewProjectionMatrix.pointer(), 16);
	}
	CameraSceneNode* m_pCamera;
};

void main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	DeviceWin32* pDevice = InvokePirate(640, 480);
	D3D9Driver* pDriver = pDevice->GetVideoDriver();

	IDirect3DDevice9* pD3DDevice = pDriver->GetExposedVideoData().D3DDev9;

	SceneManager* pSceneManager = pDevice->GetSceneManager();
	pSceneManager->AddCameraSceneNode(0, vector3df(0.0f, 0.0f, 112.0f), vector3df(0.0f, 0.0f, -448.0f));
	SMesh* pMesh = pSceneManager->GetMesh("..\\..\\Media\\firstmap.bsp");

	SetSimpleWhiteShaderConstant* setter = new SetSimpleWhiteShaderConstant(pSceneManager->GetActiveCamera());
	s32 newM = pDriver->AddHighLevelShaderMaterialFromFiles("..\\..\\Media\\GenericLightmap.hlsl", "vertexMain", EVST_VS_3_0, 
		"..\\..\\Media\\GenericLightmap.hlsl", "pixelMain", EPST_PS_3_0, setter);
	setter->Drop();

	for (u32 i=0; i<pMesh->GetMeshBufferCount(); i++)
	{
		pMesh->GetMeshBuffer(i)->m_Material.ShaderType = newM;
	}

	pSceneManager->AddMeshSceneNode(pMesh);

//	pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	while(pDevice->Run())
	{
		pDriver->BeginScene(TRUE, TRUE, SColor(255,0,55,55));
		pSceneManager->DrawAll();
		pDriver->EndScene();
	}

	pDevice->Drop();
}


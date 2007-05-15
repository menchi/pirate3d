#include "..\..\Pirate\Pirate.h"
#include "..\..\Pirate\SMesh.h"
#include "..\..\Pirate\BspLoader.h"
#include "..\..\Pirate\IShaderConstantSetCallback.h"
#include "..\..\Pirate\SceneNode.h"

using namespace Pirate;

class SetSimpleWhiteShaderConstant : public IShaderConstantSetCallBack
{
public:
	virtual void OnSetConstants(D3D9Driver* services, s32 userData)
	{
		D3DXVECTOR3 vEyePt   ( 0.0f, 0.0f, -112.0f );
		D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 448.0f );
		D3DXVECTOR3 vUpVec   ( 0.0f, 1.0f, 0.0f );
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2, 1.0f, 1.0f, 10000.0f );

		services->SetVertexShaderConstant("mWorldViewProj", matView*matProj, 16);
	}
};

void main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	DeviceWin32* pDevice = InvokePirate(512, 512);
	D3D9Driver* pDriver = pDevice->GetVideoDriver();

	D3D9Texture* pTexture = pDriver->GetTexture("..\\..\\Media\\PSDImage.psd");
//	D3D9Texture* pRT = pDriver->CreateRenderTargetTexture(dimension2di(128, 128));

	D3DVERTEXELEMENT9 dwDecl3[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_POSITION, 0},
		{0, 12,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_TEXCOORD, 0},
		{1, 0,  D3DDECLTYPE_D3DCOLOR,   D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
	struct CUSTOM_VT
	{
		float pos[3];
		float uv[2];
//		DWORD color;
	};
	
	SD3D9MeshBuffer* pMB = new SD3D9MeshBuffer(pDriver, dwDecl3);
	pMB->CreateVertexBuffer(0, 20, 4, D3DUSAGE_WRITEONLY);
	pMB->CreateIndexBuffer(6);

	CUSTOM_VT* pVert = NULL;
	pMB->GetVertexBuffer(0)->Lock(0, 0, (void**)&pVert, 0);
	pVert[0].pos[0]= 448.0f; pVert[0].pos[1]= 448.0f; pVert[0].pos[2]=448.0f; pVert[0].uv[0] = 1.0f; pVert[0].uv[1] = 0.0f; //pVert[0].color = 0xFFFF0000;
	pVert[1].pos[0]=-448.0f; pVert[1].pos[1]= 448.0f; pVert[1].pos[2]=448.0f; pVert[1].uv[0] = 0.0f; pVert[1].uv[1] = 0.0f; //pVert[1].color = 0xFF00FF00;
	pVert[2].pos[0]=-448.0f; pVert[2].pos[1]=-448.0f; pVert[2].pos[2]=448.0f; pVert[2].uv[0] = 0.0f; pVert[2].uv[1] = 1.0f; //pVert[2].color = 0xFF0000FF;
	pVert[3].pos[0]= 448.0f; pVert[3].pos[1]=-448.0f; pVert[3].pos[2]=448.0f; pVert[3].uv[0] = 1.0f; pVert[3].uv[1] = 1.0f;
	pMB->GetVertexBuffer(0)->Unlock();

	u32* pIndex = NULL;
	pMB->GetIndexBuffer()->Lock(0, 0, (void**)&pIndex, 0);
	pIndex[0] = 0; pIndex[1] = 2; pIndex[2] = 1;
	pIndex[3] = 0; pIndex[4] = 3; pIndex[5] = 2;
	pMB->GetIndexBuffer()->Unlock();

	IDirect3DDevice9* pD3DDevice = pDriver->GetExposedVideoData().D3DDev9;

	IDirect3DVertexBuffer9* pVB = NULL;
	pD3DDevice->CreateVertexBuffer(12, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
//	IDirect3DIndexBuffer9* pIB = NULL;
//	pD3DDevice->CreateIndexBuffer(12, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &pIB, NULL);
	pMB->SetVertexBuffer(1, 4, pVB);
//	pMB->SetIndexBuffer(3, pIB);

	DWORD* pVertCol;
	pMB->GetVertexBuffer(1)->Lock(0, 0, (void**)&pVertCol, 0);
	pVertCol[0] = 0xFFFFFFFF;
	pVertCol[1] = 0xFFFFFFFF;
	pVertCol[2] = 0xFFFFFFFF;
	pVertCol[3] = 0xFFFFFFFF;
	pMB->GetVertexBuffer(1)->Unlock();

//	pMB->GetIndexBuffer()->Lock(0, 0, (void**)&pIndex, 0);
//	pIndex[0] = 0; pIndex[1] = 1; pIndex[2] = 2;
//	pMB->GetIndexBuffer()->Unlock();
	SetSimpleWhiteShaderConstant* setter = new SetSimpleWhiteShaderConstant();
	s32 newM = pDriver->AddHighLevelShaderMaterialFromFiles("..\\..\\Media\\BaseTexture.hlsl", "vertexMain", EVST_VS_3_0, 
			   "..\\..\\Media\\BaseTexture.hlsl", "pixelMain", EPST_PS_3_0, setter);
	setter->Drop();
	SMaterial mat;
	mat.ShaderType = newM;
//	BspFileLoader* bspLoader = new BspFileLoader(pDevice->GetFileSystem(), pDriver);
//	FileReader* bspReader = CreateReadFile("..\\..\\Media\\firstmap.bsp");
//	SMesh* pMesh = bspLoader->CreateMesh(bspReader);
	mat.Textures[0] = pTexture;//pMesh->GetMeshBuffer(0)->m_Material.Textures[0];
	pMB->m_Material = mat;
//	pDriver->SetMaterial(mat);

	BspFileLoader* bspLoader = new BspFileLoader(pDevice->GetFileSystem(), pDriver);
	FileReader* bspReader = CreateReadFile("..\\..\\Media\\firstmap.bsp");
	SMesh* pMesh = bspLoader->CreateMesh(bspReader);

	for (u32 i=0; i<pMesh->GetMeshBufferCount(); i++)
	{
		pMesh->GetMeshBuffer(i)->m_Material.ShaderType = newM;
//		pMesh->GetMeshBuffer(i)->m_Material.Textures[0] = pTexture;
	}

//	pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	while(pDevice->Run())
	{
		pDriver->BeginScene(TRUE, TRUE, SColor(255,0,55,55));
		for (u32 i=0; i<pMesh->GetMeshBufferCount(); i++)
		{
			pDriver->SetMaterial(pMesh->GetMeshBuffer(i)->m_Material);
			pDriver->DrawMeshBuffer(pMesh->GetMeshBuffer(i));
		}
//		pDriver->DrawMeshBuffer(pMB);
		pDriver->EndScene();
	}

	pMesh->Drop();
	bspReader->Drop();
	bspLoader->Drop();
//	pRT->Drop();
	pVB->Release();
//	pIB->Release();
	pMB->Drop();
	pDevice->Drop();
}


#include "..\..\Pirate\Pirate.h"

using namespace Pirate;



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

	pSceneManager->LoadScene("../../Media/FirstMap.bsp");

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


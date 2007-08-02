#include "D3D9Driver.h"
#include <iostream>

D3D9Driver::D3D9Driver(HWND window, int width, int height, bool fullScreen)
: m_iWidth(width), m_iHeight(height), m_bIsFullScreen(fullScreen),
  m_pID3D9(Direct3DCreate9(D3D_SDK_VERSION), false)
{
	if (!m_pID3D9)
	{
		std::cerr<<"Error initialize D3D9"<<std::endl;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.hDeviceWindow = window;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (fullScreen)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}
	else 
	{
		d3dpp.Windowed = TRUE;
		d3dpp.FullScreen_RefreshRateInHz = 0;
	}

	IDirect3DDevice9* pID3DDevice;
	if (FAILED(m_pID3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pID3DDevice)))
	{
		std::cerr<<"Error create d3d9 device"<<std::endl;
	}
	 
	m_pID3DDevice = IDirect3DDevice9SharedPtr(pID3DDevice, false);

	IDirect3DSurface9* pBackBuffer;
	m_pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pCanvas = CanvasSharedPtr(new Canvas(VideoDriverSharedPtr(this, null_deleter()), IDirect3DSurface9SharedPtr(pBackBuffer, false)));
}

D3D9Driver::~D3D9Driver()
{
	std::clog << "VideoDriver destruct" << std::endl;
}

CanvasSharedPtr D3D9Driver::GetCanvas()
{
	return m_pCanvas;
}
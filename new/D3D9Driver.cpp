#include "D3D9Driver.h"
#include "Canvas.h"
#include <iostream>

#ifdef _PIRATE_COMPILE_WITH_D3D9_

D3D9Driver::D3D9Driver(HWND window, int width, int height, bool fullScreen)
: m_iWidth(width), m_iHeight(height), m_bIsFullScreen(fullScreen), m_BackgroundColor(0),
  m_pID3D9(Direct3DCreate9(D3D_SDK_VERSION))
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
	 
	m_pID3DDevice = IDirect3DDevice9Ptr(pID3DDevice, false);
}

D3D9Driver::~D3D9Driver()
{
}

VideoDriverPtr D3D9Driver::CreateVideoDriver(HWND window, int width, int height, bool fullScreen)
{
	VideoDriverPtr pDriver(new D3D9Driver(window, width, height, fullScreen));
	pDriver->m_pCanvas.reset(new Canvas(pDriver));

	return pDriver;
}

void D3D9Driver::Clear(bool color, bool z, bool stencil)
{
	DWORD flag = 0;
	flag = (color)? flag | D3DCLEAR_TARGET : flag;
	flag = (z)? flag | D3DCLEAR_ZBUFFER : flag;
	flag = (stencil)? flag | D3DCLEAR_STENCIL : flag;
	m_pID3DDevice->Clear(0, 0, flag, m_BackgroundColor, 1, 0);
}

void D3D9Driver::SetViewport(int x, int y, int w, int h)
{
	D3DVIEWPORT9 vp;
	vp.X      = x;
	vp.Y      = y;
	vp.Width  = w;
	vp.Height = h;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	m_pID3DDevice->SetViewport(&vp);
}

VertexBufferPtr D3D9Driver::CreateVertexBuffer(int size)
{
	return VertexBufferPtr(new VertexBuffer(m_pID3DDevice, size));
}

IndexBufferPtr D3D9Driver::CreateIndexBuffer(int size)
{
	return IndexBufferPtr(new IndexBuffer(m_pID3DDevice, size));
}

#endif
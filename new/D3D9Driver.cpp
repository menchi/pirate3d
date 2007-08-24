#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_D3D9_

#include "D3D9Driver.h"
#include "D3D9DriverResources.h"
#include "Canvas.h"
#include <iostream>

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")

using namespace std;

//-----------------------------------------------------------------------------
D3D9Driver::D3D9Driver(HWND window, int width, int height, bool fullScreen)
:m_iWidth(width), m_iHeight(height), m_bIsFullScreen(fullScreen), m_BackgroundColor(0), m_pID3D9(Direct3DCreate9(D3D_SDK_VERSION), false)
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

	ID3DXFragmentLinker* pFragmentLinker;
	if (FAILED(D3DXCreateFragmentLinker(m_pID3DDevice.get(), 0, &pFragmentLinker)))
	{
		std::cerr<<"Error create fragment linker"<<std::endl;
	}

	m_pFragmentLinker = ID3DXFragmentLinkerPtr(pFragmentLinker, false);
}
//-----------------------------------------------------------------------------
D3D9Driver::~D3D9Driver()
{
}
//-----------------------------------------------------------------------------
VideoDriverPtr D3D9Driver::CreateVideoDriver(HWND window, int width, int height, bool fullScreen)
{
	VideoDriverPtr pDriver(new D3D9Driver(window, width, height, fullScreen));

	return pDriver;
}
//-----------------------------------------------------------------------------
void D3D9Driver::Clear(bool color, bool z, bool stencil)
{
	DWORD flag = 0;
	flag = (color)? flag | D3DCLEAR_TARGET : flag;
	flag = (z)? flag | D3DCLEAR_ZBUFFER : flag;
	flag = (stencil)? flag | D3DCLEAR_STENCIL : flag;
	m_pID3DDevice->Clear(0, 0, flag, m_BackgroundColor, 1, 0);
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
DriverVertexBufferPtr D3D9Driver::CreateVertexBuffer(unsigned int NumVertices, unsigned int VertexSize)
{
	return DriverVertexBufferPtr(new DriverVertexBuffer(m_pID3DDevice, NumVertices, VertexSize));
}
//-----------------------------------------------------------------------------
DriverIndexBufferPtr D3D9Driver::CreateIndexBuffer(unsigned int NumIndices)
{
	return DriverIndexBufferPtr(new DriverIndexBuffer(m_pID3DDevice, NumIndices));
}
//-----------------------------------------------------------------------------
DriverVertexDeclarationPtr D3D9Driver::CreateVertexDeclaration(const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats)
{
	return DriverVertexDeclarationPtr(new DriverVertexDeclaration(m_pID3DDevice, StreamIndices, VertexFormats));
}
//-----------------------------------------------------------------------------
D3D9Driver::VertexShaderFragmentArray D3D9Driver::CreateVertexShaderFragmentsFromFile(const string& FileName, const NameArray& FragmentNames)
{
	VertexShaderFragmentArray r;

	if (m_LoadedShaderSources.insert(FileName).second)
	{
		ID3DXBuffer* pCompiledBuffer, *pMsgBuffer;
		if (FAILED(D3DXGatherFragmentsFromFileA(FileName.c_str(), NULL, NULL, 0, &pCompiledBuffer, &pMsgBuffer)))
		{
			char* pMsg = (char*)pMsgBuffer->GetBufferPointer();
			std::cerr << pMsg << std::endl;
			pMsgBuffer->Release();
			return r;
		}
		m_pFragmentLinker->AddFragments((DWORD*)pCompiledBuffer->GetBufferPointer());
	}

	for (unsigned int i=0; i<FragmentNames.size(); ++i)
	{
		VertexShaderFragmentPtr pFragment(new VertexShaderFragment(m_pFragmentLinker, FragmentNames[i]));
		r.push_back(pFragment);
	}

	return r;
}
//-----------------------------------------------------------------------------
D3D9Driver::PixelShaderFragmentArray D3D9Driver::CreatePixelShaderFragmentsFromFile(const string& FileName, const NameArray& FragmentNames)
{
	PixelShaderFragmentArray r;

	if (m_LoadedShaderSources.insert(FileName).second)
	{
		ID3DXBuffer* pCompiledBuffer, *pMsgBuffer;
		if (FAILED(D3DXGatherFragmentsFromFileA(FileName.c_str(), NULL, NULL, 0, &pCompiledBuffer, &pMsgBuffer)))
		{
			char* pMsg = (char*)pMsgBuffer->GetBufferPointer();
			std::cerr<<pMsg<<std::endl;
			pMsgBuffer->Release();
			return r;
		}
		m_pFragmentLinker->AddFragments((DWORD*)pCompiledBuffer->GetBufferPointer());
	}

	for (unsigned int i=0; i<FragmentNames.size(); ++i)
	{
		PixelShaderFragmentPtr pFragment(new PixelShaderFragment(m_pFragmentLinker, FragmentNames[i]));
		r.push_back(pFragment);
	}

	return r;
}
//-----------------------------------------------------------------------------
VertexShaderPtr D3D9Driver::CreateVertexShader(const VertexShaderFragmentArray& Fragments)
{
	return VertexShaderPtr(new VertexShader(m_pFragmentLinker.get(), Fragments));
}
//-----------------------------------------------------------------------------
PixelShaderPtr D3D9Driver::CreatePixelShader(const PixelShaderFragmentArray& Fragments)
{
	return PixelShaderPtr(new PixelShader(m_pFragmentLinker.get(), Fragments));
}
//-----------------------------------------------------------------------------
ShaderProgramPtr D3D9Driver::CreateShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader)
{
	return ShaderProgramPtr(new ShaderProgram(pVertexShader, pPixelShader));
}
//-----------------------------------------------------------------------------
void D3D9Driver::SetVertexDeclaration(DriverVertexDeclarationPtr pVertexDeclaration)
{
	m_pID3DDevice->SetVertexDeclaration(pVertexDeclaration->m_pID3DVertexDeclaration.get());
}
//-----------------------------------------------------------------------------
void D3D9Driver::SetVertexStream(unsigned int StreamNumber, DriverVertexBufferPtr pVertexBuffer, unsigned int Stride)
{
	m_pID3DDevice->SetStreamSource(StreamNumber, pVertexBuffer->m_pID3DVertexBuffer.get(), 0, Stride);
}
//-----------------------------------------------------------------------------
void D3D9Driver::DrawIndexedTriangleList(DriverIndexBufferPtr pIndexBuffer, unsigned int NumVertices, unsigned int TriangleCount)
{
	m_pID3DDevice->SetIndices(pIndexBuffer->m_pID3DIndexBuffer.get());
	m_pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, NumVertices, 0, TriangleCount);
}
//-----------------------------------------------------------------------------
void D3D9Driver::SetShaderProgram(ShaderProgramPtr pShaderProgram)
{
	m_pID3DDevice->SetVertexShader(pShaderProgram->m_pVertexShader->m_pID3DVertexShader.get());
	m_pID3DDevice->SetPixelShader(pShaderProgram->m_pPixelShader->m_pID3DPixelShader.get());
}
//-----------------------------------------------------------------------------
void D3D9Driver::Begin()
{
	m_pID3DDevice->BeginScene();
}
//-----------------------------------------------------------------------------
void D3D9Driver::End()
{
	m_pID3DDevice->EndScene();
}
//-----------------------------------------------------------------------------

#endif
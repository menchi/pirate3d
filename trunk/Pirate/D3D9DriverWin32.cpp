#include "D3D9HLSLShader.h"
#include "SMeshBuffer.h"
#include "D3D9DriverWin32.h"
#include "OS.h"
#include "ImageLoader.h"
#include "FileSystem.h"
#include <stdio.h>

namespace Pirate
{

D3D9Driver::D3D9Driver(s32 width, s32 height, HWND window, BOOL fullscreen, BOOL stencibuffer, FileSystem* io)
: m_pID3D(NULL), m_D3DLibrary(0), m_pID3DDevice(NULL), m_bFullScreen(fullscreen), m_bStencilBuffer(stencibuffer),
  m_bDeviceLost(FALSE), m_uiPrimitivesDrawn(0), m_uiTextureCreationFlags(0), m_pPrevRenderTarget(0), m_uiMaxTextureUnits(0),
  m_ScreenSize(width, height), m_pFileSystem(io), m_bResetRenderStates(TRUE), m_pLastVertexType(NULL)

{
#ifdef _DEBUG
	SetDebugName("D3D9Driver");
#endif

	if (m_pFileSystem)
		m_pFileSystem->Grab();

	SetTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, TRUE);
	SetTextureCreationFlag(ETCF_CREATE_MIP_MAPS, TRUE);
	SetTextureCreationFlag(ETCF_CREATE_USING_D3DX, TRUE);

	// set ExposedData to 0
	memset(&m_ExposedData, 0, sizeof(m_ExposedData));

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		m_pCurrentTexture[i] = 0;
	}
}

D3D9Driver::~D3D9Driver()
{
	DeleteMaterialRenders();
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		if (m_pCurrentTexture[i])
			m_pCurrentTexture[i]->Drop();

	if (m_pLastVertexType)
		m_pLastVertexType->Release();

	// delete textures
	DeleteAllTextures();

	if (m_pID3DDevice)
	{
		if (m_pID3DDevice->Release() > 0)
			Printer::Log("The D3D device has a non-zero reference count, meaning some objects were not released.", ELL_ERROR);
	}

	if (m_pID3D)
		m_pID3D->Release();

	if (m_pFileSystem)
		m_pFileSystem->Drop();

}

BOOL D3D9Driver::InitDriver(s32 width, s32 height, HWND hwnd, u32 bits, BOOL fullScreen, BOOL vsync, BOOL antiAlias)
{
	HRESULT hr;
	m_bFullScreen = fullScreen;

	if (!m_pID3D)
	{
		m_D3DLibrary = LoadLibrary( L"d3d9.dll" );

		if (!m_D3DLibrary)
		{
			Printer::Log("Error, could not load d3d9.dll.", ELL_ERROR);
			return FALSE;
		}

		typedef IDirect3D9 * (__stdcall *D3DCREATETYPE)(UINT);
		D3DCREATETYPE d3dCreate = (D3DCREATETYPE) GetProcAddress(m_D3DLibrary, "Direct3DCreate9");

		if (!d3dCreate)
		{
			Printer::Log("Error, could not get process address of Direct3DCreate9.", ELL_ERROR);
			return FALSE;
		}

		//just like pID3D = Direct3DCreate9(D3D_SDK_VERSION);
		m_pID3D = (*d3dCreate)(D3D_SDK_VERSION);

		if (!m_pID3D)
		{
			Printer::Log("Error initializing D3D.", ELL_ERROR);
			return FALSE;
		}
	}

	// print device information
	D3DADAPTER_IDENTIFIER9 dai;
	if (!FAILED(m_pID3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
	{
		char tmp[512];

		s32 Product = HIWORD(dai.DriverVersion.HighPart);
		s32 Version = LOWORD(dai.DriverVersion.HighPart);
		s32 SubVersion = HIWORD(dai.DriverVersion.LowPart);
		s32 Build = LOWORD(dai.DriverVersion.LowPart);

		sprintf_s(tmp, "%s %s %d.%d.%d.%d", dai.Description, dai.Driver, Product, Version, SubVersion, Build);
		Printer::Log(tmp, ELL_INFORMATION);
	}

	D3DDISPLAYMODE d3ddm;
	hr = m_pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hr))
	{
		Printer::Log("Error: Could not get Adapter Display mode.", ELL_ERROR);
		return false;
	}

	ZeroMemory(&m_Present, sizeof(m_Present));

	m_Present.BackBufferCount = 1;
	m_Present.EnableAutoDepthStencil = TRUE;
	if (vsync)
		m_Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else
		m_Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (fullScreen)
	{
		m_Present.BackBufferWidth = width;
		m_Present.BackBufferHeight = height;
		// request 32bit mode if user specified 32 bit, added by Thomas Stüfe
		if (bits == 32)
			m_Present.BackBufferFormat = D3DFMT_X8R8G8B8;
		else
			m_Present.BackBufferFormat = D3DFMT_R5G6B5;
		m_Present.SwapEffect = D3DSWAPEFFECT_FLIP;
		m_Present.Windowed = FALSE;
		m_Present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		m_Present.BackBufferFormat	= d3ddm.Format;
		m_Present.SwapEffect		= D3DSWAPEFFECT_COPY;
		m_Present.Windowed			= TRUE;
	}

	D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

	// enable anti alias if possible and desired
	if (antiAlias)
	{
		DWORD qualityLevels = 0;

		if (SUCCEEDED(m_pID3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
			devtype, m_Present.BackBufferFormat, !fullScreen,
			D3DMULTISAMPLE_4_SAMPLES, &qualityLevels)))
		{
			// enable multi sampling
			m_Present.MultiSampleType    = D3DMULTISAMPLE_4_SAMPLES;
			m_Present.MultiSampleQuality = qualityLevels-1;
			m_Present.SwapEffect         = D3DSWAPEFFECT_DISCARD;
		}
		else
			if (SUCCEEDED(m_pID3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				devtype, m_Present.BackBufferFormat, !fullScreen,
				D3DMULTISAMPLE_2_SAMPLES, &qualityLevels)))
			{
				// enable multi sampling
				m_Present.MultiSampleType    = D3DMULTISAMPLE_2_SAMPLES;
				m_Present.MultiSampleQuality = qualityLevels-1;
				m_Present.SwapEffect         = D3DSWAPEFFECT_DISCARD;
			}
			else
				if (SUCCEEDED(m_pID3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
					devtype, m_Present.BackBufferFormat, !fullScreen,
					D3DMULTISAMPLE_NONMASKABLE, &qualityLevels)))
				{
					// enable non maskable multi sampling
					m_Present.SwapEffect         = D3DSWAPEFFECT_DISCARD;
					m_Present.MultiSampleType    = D3DMULTISAMPLE_NONMASKABLE;
					m_Present.MultiSampleQuality = qualityLevels-1;
				}
				else
				{
					Printer::Log("Anti aliasing disabled because hardware/driver lacks necessary caps.", ELL_WARNING);
					antiAlias = FALSE;
				}
	}

	// check stencil buffer compatibility
	if (m_bStencilBuffer)
	{
		m_Present.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(m_pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
			m_Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, m_Present.AutoDepthStencilFormat)))
		{
			m_Present.AutoDepthStencilFormat = D3DFMT_D24X4S4;
			if(FAILED(m_pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
				m_Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, m_Present.AutoDepthStencilFormat)))
			{
				m_Present.AutoDepthStencilFormat = D3DFMT_D15S1;
				if(FAILED(m_pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
					m_Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE, m_Present.AutoDepthStencilFormat)))
				{
					Printer::Log("Device does not support stencilbuffer, disabling stencil buffer.", ELL_WARNING);
					m_bStencilBuffer = FALSE;
				}
			}
		}
		else
			if(FAILED(m_pID3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, devtype,
				m_Present.BackBufferFormat, m_Present.BackBufferFormat, m_Present.AutoDepthStencilFormat)))
			{
				Printer::Log("Depth-stencil format is not compatible with display format, disabling stencil buffer.", ELL_WARNING);
				m_bStencilBuffer = FALSE;
			}
	}
	// do not use else here to cope with flag change in previous block
	if (!m_bStencilBuffer)
	{
		m_Present.AutoDepthStencilFormat = D3DFMT_D32;
		if(FAILED(m_pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
			m_Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, m_Present.AutoDepthStencilFormat)))
		{
			m_Present.AutoDepthStencilFormat = D3DFMT_D24X8;
			if(FAILED(m_pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
				m_Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, m_Present.AutoDepthStencilFormat)))
			{
				m_Present.AutoDepthStencilFormat = D3DFMT_D16;
				if(FAILED(m_pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
					m_Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
					D3DRTYPE_SURFACE, m_Present.AutoDepthStencilFormat)))
				{
					Printer::Log("Device does not support required depth buffer.", ELL_WARNING);
					return FALSE;
				}
			}
		}
	}

	// create device
	hr = m_pID3D->CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_Present, &m_pID3DDevice);

	if(FAILED(hr))
		hr = m_pID3D->CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd,
		D3DCREATE_MIXED_VERTEXPROCESSING , &m_Present, &m_pID3DDevice);

	if(FAILED(hr))
		hr = m_pID3D->CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_Present, &m_pID3DDevice);

	if (FAILED(hr))
		Printer::Log("Was not able to create Direct3D9 device.", ELL_ERROR);

	if (!m_pID3DDevice)
	{
		Printer::Log("Was not able to create Direct3D9 device.", ELL_ERROR);
		return FALSE;
	}

	// get caps
	m_pID3DDevice->GetDeviceCaps(&m_Caps);

	// disable stencilbuffer if necessary
	if (m_bStencilBuffer &&
		(!(m_Caps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
		!(m_Caps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
		!(m_Caps.StencilCaps & D3DSTENCILCAPS_KEEP)))
	{
		Printer::Log("Device not able to use stencil buffer, disabling stencil buffer.", ELL_WARNING);
		m_bStencilBuffer = FALSE;
	}

	// set default vertex shader
	m_pID3DDevice->SetVertexDeclaration(NULL);

	// enable antialiasing
	if (antiAlias)
		m_pID3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	m_bResetRenderStates = TRUE;
/*
	// create materials
	createMaterialRenderers();

	MaxTextureUnits = core::min_((u32)Caps.MaxSimultaneousTextures, MATERIAL_MAX_TEXTURES);

	// set the renderstates
	setRenderStates3DMode();
*/
	// set maximal anisotropy
	m_pID3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, min(16, m_Caps.MaxAnisotropy));
	m_pID3DDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, min(16, m_Caps.MaxAnisotropy));
	m_pID3DDevice->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, min(16, m_Caps.MaxAnisotropy));
	m_pID3DDevice->SetSamplerState(3, D3DSAMP_MAXANISOTROPY, min(16, m_Caps.MaxAnisotropy));

	// set exposed data
	m_ExposedData.D3D9 = m_pID3D;
	m_ExposedData.D3DDev9 = m_pID3DDevice;
	m_ExposedData.HWnd =  reinterpret_cast<s64>(hwnd);

	// so far so good.
	return TRUE;
}

//! applications must call this method before performing any rendering. returns false if failed.
BOOL D3D9Driver::BeginScene(BOOL backBuffer, BOOL zBuffer, SColor color)
{
	m_uiPrimitivesDrawn = 0;

	HRESULT hr;

	if (!m_pID3DDevice)
		return FALSE;

	if (m_bDeviceLost)
	{
		if(FAILED(hr = m_pID3DDevice->TestCooperativeLevel()))
		{
			if (hr == D3DERR_DEVICELOST)
				return FALSE;

			if (hr == D3DERR_DEVICENOTRESET)
				Reset();
			return FALSE;
		}
	}

	DWORD flags = 0;

	if (backBuffer)
		flags |= D3DCLEAR_TARGET;

	if (zBuffer)
		flags |= D3DCLEAR_ZBUFFER;

	if (m_bStencilBuffer)
		flags |= D3DCLEAR_STENCIL;

	hr = m_pID3DDevice->Clear( 0, NULL, flags, color.color, 1.0, 0);
	if (FAILED(hr))
		Printer::Log("DIRECT3D9 clear failed.", ELL_WARNING);

	hr = m_pID3DDevice->BeginScene();
	if (FAILED(hr))
	{
		Printer::Log("DIRECT3D9 begin scene failed.", ELL_WARNING);
		return FALSE;
	}

	return TRUE;
}



//! applications must call this method after performing any rendering. returns false if failed.
BOOL D3D9Driver::EndScene(u32 windowId, rect<s32>* sourceRect)
{
	if (m_bDeviceLost)
		return FALSE;

//	m_FPSCounter.RegisterFrame(Timer::GetRealTime(), m_uiPrimitivesDrawn);

	HRESULT hr = m_pID3DDevice->EndScene();
	if (FAILED(hr))
	{
		Printer::Log("DIRECT3D9 end scene failed.", ELL_WARNING);
		return FALSE;
	}

	RECT* srcRct = 0;
	RECT sourceRectData;
	if ( sourceRect )
	{
		srcRct = &sourceRectData;
		sourceRectData.left = sourceRect->UpperLeftCorner.X;
		sourceRectData.top = sourceRect->UpperLeftCorner.Y;
		sourceRectData.right = sourceRect->LowerRightCorner.X;
		sourceRectData.bottom = sourceRect->LowerRightCorner.Y;
	}

	hr = m_pID3DDevice->Present(srcRct, NULL, (HWND)((UINT_PTR)windowId), NULL);

	if (hr == D3DERR_DEVICELOST)
	{
		m_bDeviceLost = TRUE;
		Printer::Log("DIRECT3D9 device lost.", ELL_WARNING);
	}
	else
		if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
		{
			Printer::Log("DIRECT3D9 present failed.", ELL_WARNING);
			return FALSE;
		}

	return TRUE;
}

//! resets the device
BOOL D3D9Driver::Reset()
{
	// reset
	HRESULT hr;
	Printer::Log("Resetting D3D9 device.", ELL_INFORMATION);
	if (FAILED(hr = m_pID3DDevice->Reset(&m_Present)))
	{
		if (hr == D3DERR_DEVICELOST)
		{
			m_bDeviceLost = TRUE;
			Printer::Log("Resetting failed due to device lost.", ELL_WARNING);
		}
		else
			Printer::Log("Resetting failed.", ELL_WARNING);
		return FALSE;
	}

	m_bDeviceLost = FALSE;
	m_bResetRenderStates = TRUE;
//	LastVertexType = (E_VERTEX_TYPE)-1;

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		if (m_pCurrentTexture[i])
			m_pCurrentTexture[i]->Drop();

		m_pCurrentTexture[i] = 0;
	}

	m_pID3DDevice->SetVertexDeclaration(NULL);
	SetRenderStates3DMode();
/*	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);
	setAmbientLight(AmbientLight);
*/
	return TRUE;
}

//! sets the needed renderstates
BOOL D3D9Driver::SetRenderStates3DMode()
{
	if (!m_pID3DDevice)
		return FALSE;

//	if (CurrentRenderMode != ERM_3D)
//	{
		// switch back the matrices
		/*		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&Matrices[ETS_VIEW]));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&Matrices[ETS_WORLD]));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&Matrices[ETS_PROJECTION]));
		*/
//		pID3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

//		ResetRenderStates = true;
//	}

	if (m_bResetRenderStates || m_LastMaterial != m_Material)
	{
		// unset old material

		if (/*CurrentRenderMode == ERM_3D &&*/
			m_LastMaterial.ShaderType != m_Material.ShaderType &&
			m_LastMaterial.ShaderType >= 0 && m_LastMaterial.ShaderType < (s32)m_MaterialRenderers.size())
			m_MaterialRenderers[m_LastMaterial.ShaderType].Renderer->OnUnsetMaterial();

		// set new material.

		if (m_Material.ShaderType >= 0 && m_Material.ShaderType < (s32)m_MaterialRenderers.size())
			m_MaterialRenderers[m_Material.ShaderType].Renderer->OnSetMaterial(
			m_Material, m_LastMaterial, m_bResetRenderStates, this);
	}

	BOOL shaderOK = TRUE;
	if (m_Material.ShaderType >= 0 && m_Material.ShaderType < (s32)m_MaterialRenderers.size())
		shaderOK = m_MaterialRenderers[m_Material.ShaderType].Renderer->OnRender(this);

	m_LastMaterial = m_Material;

	m_bResetRenderStates = FALSE;

//	CurrentRenderMode = ERM_3D;

	return shaderOK;
}

SExposedVideoData D3D9Driver::GetExposedVideoData()
{
	return m_ExposedData;
}

//! queries the features of the driver, returns true if feature is available
BOOL D3D9Driver::QueryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
	switch (feature)
	{
	case EVDF_MULTITEXTURE:
	case EVDF_BILINEAR_FILTER:
		return TRUE;
	case EVDF_RENDER_TO_TARGET:
		return m_Caps.NumSimultaneousRTs > 0;
	case EVDF_HARDWARE_TL:
		return (m_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
	case EVDF_MIP_MAP:
		return (m_Caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) != 0;
	case EVDF_STENCIL_BUFFER:
		return m_bStencilBuffer &&  m_Caps.StencilCaps;
	case EVDF_VERTEX_SHADER_1_1:
		return m_Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_VERTEX_SHADER_2_0:
		return m_Caps.VertexShaderVersion >= D3DVS_VERSION(2,0);
	case EVDF_VERTEX_SHADER_3_0:
		return m_Caps.VertexShaderVersion >= D3DVS_VERSION(3,0);
	case EVDF_PIXEL_SHADER_1_1:
		return m_Caps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case EVDF_PIXEL_SHADER_1_2:
		return m_Caps.PixelShaderVersion >= D3DPS_VERSION(1,2);
	case EVDF_PIXEL_SHADER_1_3:
		return m_Caps.PixelShaderVersion >= D3DPS_VERSION(1,3);
	case EVDF_PIXEL_SHADER_1_4:
		return m_Caps.PixelShaderVersion >= D3DPS_VERSION(1,4);
	case EVDF_PIXEL_SHADER_2_0:
		return m_Caps.PixelShaderVersion >= D3DPS_VERSION(2,0);
	case EVDF_PIXEL_SHADER_3_0:
		return m_Caps.PixelShaderVersion >= D3DPS_VERSION(3,0);
	case EVDF_HLSL:
		return m_Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_TEXTURE_NPOT:
		return (m_Caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;
	default:
		return FALSE;
	};
}

//! Removes a texture from the texture cache and deletes it, freeing lot of
//! memory.
void D3D9Driver::RemoveTexture(D3D9Texture* texture)
{
	for (u32 i=0; i<m_Textures.size(); ++i)
		if (m_Textures[i].Surface == texture)
		{
			texture->Drop();
			m_Textures.erase(i);
		}
}


//! Removes all texture from the texture cache and deletes them, freeing lot of
//! memory.
void D3D9Driver::RemoveAllTextures()
{
	DeleteAllTextures();
}


//! Returns a texture by index
D3D9Texture* D3D9Driver::GetTextureByIndex(u32 i)
{
	if ( i < m_Textures.size() )
		return m_Textures[i].Surface;

	return 0;
}


//! Returns amount of textures currently loaded
s32 D3D9Driver::GetTextureCount()
{
	return m_Textures.size();
}


//! loads a Texture
D3D9Texture* D3D9Driver::GetTexture(const c8* filename)
{
	D3D9Texture* texture = FindTexture(filename);

	if (texture)
		return texture;

	FileReader* file = m_pFileSystem->CreateAndOpenFile(filename);
	BOOL errorReported = FALSE;

	if (file)
	{
		texture = LoadTextureFromFile(file, filename);
		file->Drop();

		if (texture)
		{
			AddTexture(texture);
			texture->Drop(); // drop it becaus we created it, one grab too much
		}
	}
	else
	{
		errorReported = TRUE;
		Printer::Log("Could not open file of texture", filename, ELL_ERROR);
	}

	if (!texture && !errorReported)
		Printer::Log("Could not load texture", filename, ELL_ERROR);

	return texture;
}



//! loads a Texture
D3D9Texture* D3D9Driver::GetTexture(FileReader* file)
{
	D3D9Texture* texture = 0;

	if (file)
	{
		texture = FindTexture(file->GetFileName());

		if (texture)
			return texture;

		texture = LoadTextureFromFile(file );

		if (texture)
		{
			AddTexture(texture);
			texture->Drop(); // drop it because we created it, one grab too much
		}
	}

	if (!texture)
		Printer::Log("Could not load texture", file->GetFileName(), ELL_ERROR);

	return texture;
}

//! Creates a render target texture.
D3D9Texture* D3D9Driver::CreateRenderTargetTexture(const dimension2d<s32>& size)
{
	return new D3D9Texture(this, size, 0);
}

//! sets a render target
BOOL D3D9Driver::SetRenderTarget(D3D9Texture* texture,
								 BOOL clearBackBuffer, BOOL clearZBuffer,
								 SColor color)
{
	// check for valid render target

	D3D9Texture* tex = texture;

	if (texture && !tex->IsRenderTarget())
	{
		Printer::Log("Fatal Error: Tried to set a non render target texture as render target.", ELL_ERROR);
		return FALSE;
	}

	if (texture && (tex->GetSize().Width > m_ScreenSize.Width ||
		tex->GetSize().Height > m_ScreenSize.Height ))
	{
		Printer::Log("Error: Tried to set a render target texture which is bigger than the screen.", ELL_ERROR);
		return FALSE;
	}

	// check if we should set the previous RT back

	BOOL ret = TRUE;

	if (tex == 0)
	{
		if (m_pPrevRenderTarget)
		{
			if (FAILED(m_pID3DDevice->SetRenderTarget(0, m_pPrevRenderTarget)))
			{
				Printer::Log("Error: Could not set back to previous render target.", ELL_ERROR);
				ret = FALSE;
			}

			m_CurrentRendertargetSize = dimension2d<s32>(0,0);
			m_pPrevRenderTarget->Release();
			m_pPrevRenderTarget = 0;
		}
	}
	else
	{
		// we want to set a new target. so do this.

		// store previous target

		if (!m_pPrevRenderTarget)
			if (FAILED(m_pID3DDevice->GetRenderTarget(0, &m_pPrevRenderTarget)))
			{
				Printer::Log("Could not get previous render target.", ELL_ERROR);
				return FALSE;
			}

			// set new render target

			if (FAILED(m_pID3DDevice->SetRenderTarget(0, tex->GetRenderTargetSurface())))
			{
				Printer::Log("Error: Could not set render target.", ELL_ERROR);
				return FALSE;
			}

			m_CurrentRendertargetSize = tex->GetSize();
	}

	if (clearBackBuffer || clearZBuffer)
	{
		DWORD flags = 0;

		if (clearBackBuffer)
			flags |= D3DCLEAR_TARGET;

		if (clearZBuffer)
			flags |= D3DCLEAR_ZBUFFER;

		m_pID3DDevice->Clear(0, NULL, flags, color.color, 1.0f, 0);
	}

	return ret;
}

//! sets the current Texture
BOOL D3D9Driver::SetTexture(s32 stage, D3D9Texture* texture)
{
	if (m_pCurrentTexture[stage] == texture)
		return TRUE;

	if (m_pCurrentTexture[stage])
		m_pCurrentTexture[stage]->Drop();

	m_pCurrentTexture[stage] = texture;

	if (!texture)
	{
		m_pID3DDevice->SetTexture(stage, 0);
	}
	else
	{
		m_pID3DDevice->SetTexture(stage, texture->GetDX9Texture());
		texture->Grab();
	}
	return TRUE;
}

//! returns a device dependent texture from a software surface (IImage)
D3D9Texture* D3D9Driver::CreateDeviceDependentTexture(Image* surface, const char* name)
{
	return new D3D9Texture(surface, this, m_uiTextureCreationFlags, name);
}

//! Enables or disables a texture creation flag.
void D3D9Driver::SetTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, BOOL enabled)
{
	if (flag == ETCF_CREATE_MIP_MAPS && !QueryFeature(EVDF_MIP_MAP))
		enabled = FALSE;

	if (enabled && ((flag == ETCF_ALWAYS_16_BIT) || (flag == ETCF_ALWAYS_32_BIT)
		|| (flag == ETCF_OPTIMIZED_FOR_QUALITY) || (flag == ETCF_OPTIMIZED_FOR_SPEED)))
	{
		// disable other formats
		SetTextureCreationFlag(ETCF_ALWAYS_16_BIT, FALSE);
		SetTextureCreationFlag(ETCF_ALWAYS_32_BIT, FALSE);
		SetTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, FALSE);
		SetTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, FALSE);
	}

	// set flag
	m_uiTextureCreationFlags = (m_uiTextureCreationFlags & (~flag)) |
		((((u32)!enabled)-1) & flag);
}

//! Returns if a texture creation flag is enabled or disabled.
BOOL D3D9Driver::GetTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag)
{
	return (m_uiTextureCreationFlags & flag)!=0;
}

// returns the current size of the screen or rendertarget
dimension2d<s32> D3D9Driver::GetCurrentRenderTargetSize()
{
	if ( m_CurrentRendertargetSize.Width == 0 )
		return m_ScreenSize;
	else
		return m_CurrentRendertargetSize;
}

//! deletes all textures
void D3D9Driver::DeleteAllTextures()
{
	for (u32 i=0; i<m_Textures.size(); ++i)
		m_Textures[i].Surface->Drop();

	m_Textures.clear();
}

//! looks if the image is already loaded
D3D9Texture* D3D9Driver::FindTexture(const c8* filename)
{
	if (!filename)
		filename = "";

	SSurface s;
	SDummyTexture dummy(filename);
	s.Surface = &dummy;

	s32 index = m_Textures.binary_search(s);
	if (index != -1)
		return m_Textures[index].Surface;

	return 0;
}

//! opens the file and loads it into the surface
D3D9Texture* D3D9Driver::LoadTextureFromFile(FileReader* file, const c8 *hashName )
{
	D3D9Texture* texture = 0;

	Image* image = CreateImageFromFile(file);

	if (image)
	{
		// create texture from surface
		texture = CreateDeviceDependentTexture(image, hashName ? hashName : file->GetFileName() );
		Printer::Log("Loaded texture", file->GetFileName());
		image->Drop();
	}
	else
	{
		if (GetTextureCreationFlag(ETCF_CREATE_USING_D3DX))
		{
			// lazy. It should hand over to anther CreateDeviceDependentTexture
			texture = new D3D9Texture(this, m_uiTextureCreationFlags, hashName ? hashName : file->GetFileName());
			if (texture)
				return texture;
		}
	}

	return texture;
}

//! creates a Texture
D3D9Texture* D3D9Driver::AddTexture(const dimension2d<s32>& size, const c8* name, ECOLOR_FORMAT format)
{
	if (!name)
		return 0;

	Image* image = new Image(format, size);
	D3D9Texture* t = CreateDeviceDependentTexture(image, name);
	image->Drop();
	AddTexture(t);

	if (t)
		t->Drop();

	return t;
}

//! Creates a texture from a loaded IImage.
D3D9Texture* D3D9Driver::AddTexture(const c8* name, Image* image)
{
	if (!name || !image)
		return 0;

	D3D9Texture* t = CreateDeviceDependentTexture(image, name);
	AddTexture(t);
	t->Drop();
	return t;
}

//! adds a surface, not loaded or created by the Engine
void D3D9Driver::AddTexture(D3D9Texture* texture)
{
	if (texture)
	{
		SSurface s;
		s.Surface = texture;
		texture->Grab();

		m_Textures.push_back(s);
	}
}

//! Can be called by an IMaterialRenderer to make its work easier.
void D3D9Driver::SetBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, BOOL resetAllRenderstates)
{
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; i++)
	{
		m_pID3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, material.TextureWrap[i]);
		m_pID3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, material.TextureWrap[i]);
	}

	return;
}

//! deletes all material renderers
void D3D9Driver::DeleteMaterialRenders()
{
	// delete material renderers
	for (int i=0; i<(int)m_MaterialRenderers.size(); ++i)
		if (m_MaterialRenderers[i].Renderer)
			m_MaterialRenderers[i].Renderer->Drop();

	m_MaterialRenderers.clear();
}

//! Returns pointer to material renderer or null
D3D9HLSLShader* D3D9Driver::GetMaterialRenderer(u32 idx)
{
	if ( idx < m_MaterialRenderers.size() )
		return m_MaterialRenderers[idx].Renderer;

	return 0;
}

//! Adds a new material renderer to the video device.
s32 D3D9Driver::AddMaterialRenderer(D3D9HLSLShader* renderer, const char* name)
{
	if (!renderer)
		return -1;

	SMaterialRenderer r;
	r.Renderer = renderer;
	r.Name = name;
/*
	if (name == 0 && (m_MaterialRenderers.size() < sizeof(sBuiltInMaterialTypeNames) / sizeof(char*)))
	{
		// set name of built in renderer so that we don't have to implement name
		// setting in all 5 available renderers.
		r.Name = sBuiltInMaterialTypeNames[MaterialRenderers.size()];
	}
*/
	m_MaterialRenderers.push_back(r);
	renderer->Grab();

	return m_MaterialRenderers.size()-1;
}

//! sets a material
void D3D9Driver::SetMaterial(const SMaterial& material)
{
	m_Material = material;

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		SetTexture(i, m_Material.Textures[i]);
	}
}

//! Sets a vertex shader constant.
void D3D9Driver::SetVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	if (data)
		m_pID3DDevice->SetVertexShaderConstantF(startRegister, data, constantAmount);
}

//! Sets a pixel shader constant.
void D3D9Driver::SetPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	if (data)
		m_pID3DDevice->SetPixelShaderConstantF(startRegister, data, constantAmount);
}

//! Sets a constant for the vertex shader based on a name.
BOOL D3D9Driver::SetVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	if (m_Material.ShaderType >= 0 && m_Material.ShaderType < (s32)m_MaterialRenderers.size())
	{
		D3D9HLSLShader* r = (D3D9HLSLShader*)m_MaterialRenderers[m_Material.ShaderType].Renderer;
		return r->SetVariable(true, name, floats, count);
	}

	return FALSE;
}

//! Sets a constant for the pixel shader based on a name.
BOOL D3D9Driver::SetPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	if (m_Material.ShaderType >= 0 && m_Material.ShaderType < (s32)m_MaterialRenderers.size())
	{
		D3D9HLSLShader* r = (D3D9HLSLShader*)m_MaterialRenderers[m_Material.ShaderType].Renderer;
		return r->SetVariable(false, name, floats, count);
	}

	return FALSE;
}

//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
s32 D3D9Driver::AddHighLevelShaderMaterialFromFiles(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback)
{
	FileReader* vsfile = 0;
	FileReader* psfile = 0;

	if (vertexShaderProgram)
	{
		vsfile = m_pFileSystem->CreateAndOpenFile(vertexShaderProgram);
		if (!vsfile)
		{
			Printer::Log("Could not open vertex shader program file",
				vertexShaderProgram, ELL_WARNING);
			return -1;
		}
	}

	if (pixelShaderProgram)
	{
		psfile = m_pFileSystem->CreateAndOpenFile(pixelShaderProgram);
		if (!psfile)
		{
			Printer::Log("Could not open pixel shader program file",
				pixelShaderProgram, ELL_WARNING);
			if (vsfile)
				vsfile->Drop();
			return -1;
		}
	}

	s32 result = AddHighLevelShaderMaterialFromFiles(
		vsfile, vertexShaderEntryPointName, vsCompileTarget,
		psfile, pixelShaderEntryPointName, psCompileTarget,
		callback);

	if (psfile)
		psfile->Drop();

	if (vsfile)
		vsfile->Drop();

	return result;
}

//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
s32 D3D9Driver::AddHighLevelShaderMaterialFromFiles(
	FileReader* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	FileReader* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback)
{
	c8* vs = 0;
	c8* ps = 0;

	if (vertexShaderProgram)
	{
		s32 size = vertexShaderProgram->GetSize();
		if (size)
		{
			vs = new c8[size+1];
			vertexShaderProgram->Read(vs, size);
			vs[size] = 0;
		}
	}

	if (pixelShaderProgram)
	{
		s32 size = pixelShaderProgram->GetSize();
		if (size)
		{
			ps = new c8[size+1];
			pixelShaderProgram->Read(ps, size);
			ps[size] = 0;
		}
	}

	s32 result = this->AddHighLevelShaderMaterial(
		vs, vertexShaderEntryPointName, vsCompileTarget,
		ps, pixelShaderEntryPointName, psCompileTarget,
		callback);

	delete [] vs;
	delete [] ps;

	return result;
}

//! Adds a new material renderer to the VideoDriver, based on a high level shading
//! language.
s32 D3D9Driver::AddHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback)
{
	s32 nr = -1;

	D3D9HLSLShader* hlsl = new D3D9HLSLShader(
		m_pID3DDevice, this, nr,
		vertexShaderProgram,
		vertexShaderEntryPointName,
		vsCompileTarget,
		pixelShaderProgram,
		pixelShaderEntryPointName,
		psCompileTarget,
		callback);

	hlsl->Drop();
	return nr;
}

//! Creates a software image from a file.
Image* D3D9Driver::CreateImageFromFile(const char* filename)
{
	if (!filename)
		return 0;

	Image* image = 0;
	FileReader* file = m_pFileSystem->CreateAndOpenFile(filename);

	if (file)
	{
		image = CreateImageFromFile(file);
		file->Drop();
	}
	else
		Printer::Log("Could not open file of image", filename, ELL_ERROR);

	return image;
}


//! Creates a software image from a file.
Image* D3D9Driver::CreateImageFromFile(FileReader* file)
{
	if (!file)
		return 0;

	Image* image = 0;

	ImageLoaderPSD LoaderPSD;
	image = LoaderPSD.LoadImageFromFile(file);

	if (image)
		return image;
/*
	u32 i;

	// try to load file based on file extension
	for (i=0; i<SurfaceLoader.size(); ++i)
	{
		if (SurfaceLoader[i]->isALoadableFileExtension(file->getFileName()))
		{
			// reset file position which might have changed due to previous loadImage calls
			file->seek(0);
			image = SurfaceLoader[i]->loadImage(file);
			if (image)
				return image;
		}
	}

	// try to load file based on what is in it
	for (i=0; i<SurfaceLoader.size(); ++i)
	{
		// dito
		file->seek(0);
		if (SurfaceLoader[i]->isALoadableFileFormat(file))
		{
			file->seek(0);
			image = SurfaceLoader[i]->loadImage(file);
			if (image)
				return image;
		}
	}
*/
	return 0; // failed to load
}

//! Draws a mesh buffer
void D3D9Driver::DrawMeshBuffer( const SD3D9MeshBuffer* mb )
{
	if (!mb)
		return;

/*	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType);
*/
	IDirect3DVertexDeclaration9* pNewVT = mb->GetVertexType();
	if (pNewVT != m_pLastVertexType)
	{
		if (m_pLastVertexType)
			m_pLastVertexType->Release();
		m_pLastVertexType = pNewVT;
		m_pLastVertexType->AddRef();
		m_pID3DDevice->SetVertexDeclaration(pNewVT);
	}

	if (SetRenderStates3DMode())
	{
		for (u32 i=0; i<mb->m_SrcToVBMap.size(); i++)
		{
			u32 srcIndex = mb->m_SrcToVBMap[i].SourceIndex;
			m_pID3DDevice->SetStreamSource(srcIndex, mb->GetVertexBuffer(srcIndex), 0, mb->GetVertexPitch(srcIndex));
		}
		m_pID3DDevice->SetIndices(mb->GetIndexBuffer());
		m_pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mb->GetVertexCount(), 0, mb->GetPrimitiveCount());		
	}
}

//! creates a video driver
D3D9Driver* CreateDirectX9Driver(s32 width, s32 height, HWND window,
								 u32 bits, BOOL fullscreen, BOOL stencilbuffer,
								 BOOL vsync, BOOL antiAlias, FileSystem* io)
{
	D3D9Driver* dx9 =  new D3D9Driver(width, height, window, fullscreen, stencilbuffer, io);
	if (!dx9->InitDriver(width, height, window, bits, fullscreen, vsync, antiAlias))
	{
		dx9->Drop();
		dx9 = 0;
	}

	return dx9;
}

}
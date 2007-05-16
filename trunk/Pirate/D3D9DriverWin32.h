#ifndef _PIRATE_D3D9DRIVER_H_
#define _PIRATE_D3D9DRIVER_H_
#include "D3D9HLSLShader.h"
#include "RefObject.h"
#include "SColor.h"
#include "rect.h"
#include "pirateArray.h"
#include "D3D9Texture.h"
#include "SMaterial.h"
#include <windows.h>

namespace Pirate
{

//! enumeration for querying features of the video driver.
enum E_VIDEO_DRIVER_FEATURE
{
	//! Is driver able to render to a surface?
	EVDF_RENDER_TO_TARGET = 0,

	//! Is hardeware transform and lighting supported?
	EVDF_HARDWARE_TL,

	//! Are multiple textures per material possible?
	EVDF_MULTITEXTURE,

	//! Is driver able to render with a bilinear filter applied?
	EVDF_BILINEAR_FILTER,

	//! Can the driver handle mip maps?
	EVDF_MIP_MAP,

	//! Can the driver update mip maps automatically?
	EVDF_MIP_MAP_AUTO_UPDATE,

	//! Are stencilbuffers switched on and does the device support stencil buffers?
	EVDF_STENCIL_BUFFER,

	//! Is Vertex Shader 1.1 supported?
	EVDF_VERTEX_SHADER_1_1,

	//! Is Vertex Shader 2.0 supported?
	EVDF_VERTEX_SHADER_2_0,

	//! Is Vertex Shader 3.0 supported?
	EVDF_VERTEX_SHADER_3_0,

	//! Is Pixel Shader 1.1 supported?
	EVDF_PIXEL_SHADER_1_1,

	//! Is Pixel Shader 1.2 supported?
	EVDF_PIXEL_SHADER_1_2,

	//! Is Pixel Shader 1.3 supported?
	EVDF_PIXEL_SHADER_1_3,

	//! Is Pixel Shader 1.4 supported?
	EVDF_PIXEL_SHADER_1_4,

	//! Is Pixel Shader 2.0 supported?
	EVDF_PIXEL_SHADER_2_0,

	//! Is Pixel Shader 3.0 supported?
	EVDF_PIXEL_SHADER_3_0,

	//! Are ARB vertex programs v1.0 supported?
	EVDF_ARB_VERTEX_PROGRAM_1,

	//! Are ARB fragment programs v1.0 supported?
	EVDF_ARB_FRAGMENT_PROGRAM_1,

	//! Is GLSL supported?
	EVDF_ARB_GLSL,

	//! Is HLSL supported?
	EVDF_HLSL,

	//! Are non-power-of-two textures supported?
	EVDF_TEXTURE_NPOT,

	//! Are framebuffer objects supported?
	EVDF_FRAMEBUFFER_OBJECT
};

struct SExposedVideoData
{
	//! Pointer to the IDirect3D9 interface
	IDirect3D9* D3D9;

	//! Pointer to the IDirect3D9 interface
	IDirect3DDevice9* D3DDev9;

	//! Window handle. Get with for example 
	//! HWND h = reinterpret_cast<HWND>(exposedData.D3D9.HWnd)
	s64 HWnd;
};

class FileSystem;
class FileReader;
struct SD3D9MeshBuffer;

class D3D9Driver : public virtual RefObject
{
public:
	//! constructor
	D3D9Driver(s32 width, s32 height, HWND window, BOOL fullscreen, BOOL stencibuffer, FileSystem* io);
	//! destructor
	~D3D9Driver();

	//! initialises the Direct3D API
	BOOL InitDriver(s32 width, s32 height, HWND hwnd, u32 bits, BOOL fullScreen, BOOL vsync, BOOL antiAlias);

	//! applications must call this method before performing any rendering. returns false if failed.
	BOOL BeginScene(BOOL backBuffer, BOOL zBuffer, SColor color);

	//! applications must call this method after performing any rendering. returns false if failed.
	BOOL EndScene(u32 windowId = 0, rect<s32>* sourceRect=0);

	//! Returns driver and operating system specific data about the VideoDriver.
	SExposedVideoData GetExposedVideoData();

	//! queries the features of the driver, returns true if feature is available
	BOOL QueryFeature(E_VIDEO_DRIVER_FEATURE feature);

	//! loads a Texture
	D3D9Texture* GetTexture(const c8* filename);

	//! loads a Texture
	D3D9Texture* GetTexture(FileReader* file);

	//! Returns a texture by index
	D3D9Texture* GetTextureByIndex(u32 index);

	//! Returns amount of textures currently loaded
	s32 GetTextureCount();

	//! creates a Texture
	D3D9Texture* AddTexture(const dimension2d<s32>& size, const c8* name, ECOLOR_FORMAT format = ECF_A8R8G8B8);

	//! looks if the image is already loaded
	D3D9Texture* FindTexture(const c8* filename);

	//! Removes a texture from the texture cache and deletes it, freeing lot of
	//! memory. 
	void RemoveTexture(D3D9Texture* texture);

	//! Removes all texture from the texture cache and deletes them, freeing lot of
	//! memory. 
	void RemoveAllTextures();

	//! Creates a render target texture.
	D3D9Texture* CreateRenderTargetTexture(const dimension2d<s32>& size);

	//! sets a render target
	BOOL SetRenderTarget(D3D9Texture* texture, BOOL clearBackBuffer, BOOL clearZBuffer, SColor color);

	// returns the current size of the screen or rendertarget
	dimension2d<s32> GetCurrentRenderTargetSize();

	//! Enables or disables a texture creation flag.
	void SetTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, BOOL enabled);

	//! Returns if a texture creation flag is enabled or disabled.
	BOOL GetTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag);

	//! Creates a software image from a file.
	Image* CreateImageFromFile(const char* filename);

	//! Creates a software image from a file.
	Image* CreateImageFromFile(FileReader* file);

	//! sets a material
	void SetMaterial(const SMaterial& material);

	//! Adds a new material renderer to the video device.
	s32 AddMaterialRenderer(D3D9HLSLShader* renderer, const char* name = 0);

	//! Can be called by an IMaterialRenderer to make its work easier.
	void SetBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, BOOL resetAllRenderstates);

	//! deletes all material renderers
	void DeleteMaterialRenders();

	//! Returns pointer to material renderer or null
	D3D9HLSLShader* GetMaterialRenderer(u32 idx);

	void DrawMeshBuffer( const SD3D9MeshBuffer* mb );

	//! Sets a vertex shader constant.
	void SetVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

	//! Sets a pixel shader constant.
	void SetPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

	//! Sets a constant for the vertex shader based on a name.
	BOOL SetVertexShaderConstant(const c8* name, const f32* floats, int count);

	//! Sets a constant for the pixel shader based on a name.
	BOOL SetPixelShaderConstant(const c8* name, const f32* floats, int count);

	//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
	//! but tries to load the programs from files. 
	s32 AddHighLevelShaderMaterialFromFiles(
		const c8* vertexShaderProgram,
		const c8* vertexShaderEntryPointName = "main",
		E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_2_0,
		const c8* pixelShaderProgram = 0, 
		const c8* pixelShaderEntryPointName = "main",
		E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_2_0,
		IShaderConstantSetCallBack* callback = 0); 

	//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
	//! but tries to load the programs from files. 
	s32 AddHighLevelShaderMaterialFromFiles(
		FileReader* vertexShaderProgram,
		const c8* vertexShaderEntryPointName = "main",
		E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_2_0,
		FileReader* pixelShaderProgram = 0, 
		const c8* pixelShaderEntryPointName = "main",
		E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_2_0,
		IShaderConstantSetCallBack* callback = 0); 

	//! Adds a new material renderer to the VideoDriver, based on a high level shading
	//! language.
	s32 AddHighLevelShaderMaterial(
		const c8* vertexShaderProgram,
		const c8* vertexShaderEntryPointName = "main",
		E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_2_0,
		const c8* pixelShaderProgram = 0,
		const c8* pixelShaderEntryPointName = "main",
		E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_2_0,
		IShaderConstantSetCallBack* callback = 0);

private:
	//! resets the device
	BOOL Reset();

	//! sets the needed renderstates
	BOOL SetRenderStates3DMode();

	//! deletes all textures
	void DeleteAllTextures();

	//! sets the current Texture
	BOOL SetTexture(s32 stage, D3D9Texture* texture);

	//! returns a device dependent texture from a software surface (IImage)
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	D3D9Texture* CreateDeviceDependentTexture(Image* surface, const char* name);

	//! opens the file and loads it into the surface
	D3D9Texture* LoadTextureFromFile(FileReader* file, const c8* hashName = 0);

	//! adds a surface, not loaded or created by Engine
	void AddTexture(D3D9Texture* surface);

	//! Creates a texture from a loaded IImage.
	D3D9Texture* AddTexture(const c8* name, Image* image);

	struct SSurface
	{
		D3D9Texture* Surface;

		BOOL operator < (const SSurface& other) const
		{
			return Surface->GetName() < other.Surface->GetName();
		}
	};

	struct SDummyTexture : public D3D9Texture
	{
		SDummyTexture(const char* name) : D3D9Texture(name) {};
	};

	struct SMaterialRenderer
	{
		stringc Name;
		D3D9HLSLShader* Renderer;
	};

	HINSTANCE m_D3DLibrary;
	IDirect3D9* m_pID3D;
	IDirect3DDevice9* m_pID3DDevice;

	FileSystem* m_pFileSystem;	

	array<SSurface> m_Textures;
	array<SMaterialRenderer> m_MaterialRenderers;

	u32 m_uiTextureCreationFlags;
	D3D9Texture* m_pCurrentTexture[MATERIAL_MAX_TEXTURES];
	IDirect3DSurface9* m_pPrevRenderTarget;
	dimension2d<s32> m_CurrentRendertargetSize;
	u32 m_uiMaxTextureUnits;

	IDirect3DVertexDeclaration9* m_pLastVertexType;
	SMaterial m_Material, m_LastMaterial;

	D3DPRESENT_PARAMETERS m_Present;
	D3DCAPS9 m_Caps;

	dimension2d<s32> m_ScreenSize;

	BOOL m_bFullScreen;
	BOOL m_bStencilBuffer;
	BOOL m_bDeviceLost;
	BOOL m_bResetRenderStates;

	u32 m_uiPrimitivesDrawn;
	SExposedVideoData m_ExposedData;
};

}

#endif
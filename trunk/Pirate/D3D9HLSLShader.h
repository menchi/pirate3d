#ifndef _PIRATE_D3D9_HLSL_SHADER_H_
#define _PIRATE_D3D9_HLSL_SHADER_H_

#include <d3d9.h>
#include <d3dx9shader.h>
#include "SMaterial.h"
#include "RefObject.h"

namespace Pirate
{

class IShaderConstantSetCallBack;
class D3D9Driver;

//! Compile target enumeration for the addHighLevelShaderMaterial() method. 
enum E_VERTEX_SHADER_TYPE
{
	EVST_VS_1_1 = 0,
	EVST_VS_2_0,
	EVST_VS_2_a,
	EVST_VS_3_0,

	//! This is not a type, but a value indicating how much types there are.
	EVST_COUNT
};

//! Names for all vertex shader types, each entry corresponds to a E_VERTEX_SHADER_TYPE entry.
const c8* const VERTEX_SHADER_TYPE_NAMES[] = {
	"vs_1_1",
	"vs_2_0",
	"vs_2_a",
	"vs_3_0",
	0 };

//! Compile target enumeration for the addHighLevelShaderMaterial() method. 
enum E_PIXEL_SHADER_TYPE
{
	EPST_PS_1_1 = 0,
	EPST_PS_1_2,
	EPST_PS_1_3,	
	EPST_PS_1_4,
	EPST_PS_2_0,
	EPST_PS_2_a,
	EPST_PS_2_b,
	EPST_PS_3_0,

	//! This is not a type, but a value indicating how much types there are.
	EPST_COUNT
};

//! Names for all pixel shader types, each entry corresponds to a E_PIXEL_SHADER_TYPE entry.
const c8* const PIXEL_SHADER_TYPE_NAMES[] = {
	"ps_1_1", 
	"ps_1_2",
	"ps_1_3",
	"ps_1_4",
	"ps_2_0",
	"ps_2_a",
	"ps_2_b",
	"ps_3_0",
	0 };

//! Class for using vertex and pixel shaders with D3D9
class D3D9HLSLShader : public virtual RefObject
{
public:

	//! Public constructor
	D3D9HLSLShader( IDirect3DDevice9* d3ddev, D3D9Driver* driver, 
					s32& outMaterialTypeNr, 
					const c8* vertexShaderProgram,
					const c8* vertexShaderEntryPointName,
					E_VERTEX_SHADER_TYPE vsCompileTarget,
					const c8* pixelShaderProgram, 
					const c8* pixelShaderEntryPointName,
					E_PIXEL_SHADER_TYPE psCompileTarget,
					IShaderConstantSetCallBack* callback);

	//! Destructor
	~D3D9HLSLShader();

	virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
		BOOL resetAllRenderstates, D3D9Driver* services);

	virtual void OnUnsetMaterial();

	virtual BOOL OnRender(D3D9Driver* service);

	//! sets a variable in the shader. 
	virtual BOOL SetVariable(BOOL vertexShader, const c8* name, const f32* floats, int count);

protected:

	HRESULT StubD3DXCompileShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
		LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
		LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader,
		LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable);

	HRESULT StubD3DXCompileShaderFromFile(LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, 
		LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
		LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs,
		LPD3DXCONSTANTTABLE* ppConstantTable);

	BOOL CreateHLSLVertexShader(const char* vertexShaderProgram, 
		const char* shaderEntryPointName,
		const char* shaderTargetName);

	BOOL CreateHLSLPixelShader(const char* pixelShaderProgram, 
		const char* shaderEntryPointName,
		const char* shaderTargetName);

	void PrintHLSLVariables(LPD3DXCONSTANTTABLE table);

	IDirect3DDevice9* m_pID3DDevice;
	D3D9Driver* m_pDriver;
	IShaderConstantSetCallBack* m_pCallBack;

	IDirect3DVertexShader9* m_pVertexShader;
	IDirect3DVertexShader9* m_pOldVertexShader;
	IDirect3DPixelShader9* m_pPixelShader;

	LPD3DXCONSTANTTABLE m_pVSConstantsTable;
	LPD3DXCONSTANTTABLE m_pPSConstantsTable;
};

}

#endif
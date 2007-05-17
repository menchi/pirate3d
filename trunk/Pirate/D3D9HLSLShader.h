#ifndef _PIRATE_D3D9_HLSL_SHADER_H_
#define _PIRATE_D3D9_HLSL_SHADER_H_

#include "SMaterial.h"
#include "RefObject.h"

namespace Pirate
{

class IShaderConstantSetCallBack;
class D3D9Driver;

//! Class for using vertex and pixel shaders with D3D9
class D3D9HLSLShader : public virtual RefObject
{
public:

	//! Public constructor
	D3D9HLSLShader( IDirect3DDevice9* d3ddev, D3D9Driver* driver, 
					s32& outMaterialTypeNr, 
					const c8* vertexShaderProgram,
					const c8* vertexShaderEntryPointName,
					const c8* pixelShaderProgram, 
					const c8* pixelShaderEntryPointName,
					IShaderConstantSetCallBack* callback);

	//! Destructor
	~D3D9HLSLShader();

	virtual void OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
		BOOL resetAllRenderstates, D3D9Driver* services);

	virtual void OnUnsetMaterial();

	virtual BOOL OnRender(D3D9Driver* service);

	//! sets a variable in the shader. 
	virtual BOOL SetVariable(BOOL vertexShader, const c8* name, const f32* floats, int count);

	virtual BOOL IsTransparent() { return FALSE; };

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
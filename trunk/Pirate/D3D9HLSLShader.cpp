#include "D3D9HLSLShader.h"
#include "D3D9Driver.h"
#include "IShaderConstantSetCallback.h"
#include "OS.h"

namespace Pirate
{

//! Public constructor
D3D9HLSLShader::D3D9HLSLShader(IDirect3DDevice9* d3ddev,
	D3D9Driver* driver, s32& outMaterialTypeNr, 
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	const c8* pixelShaderProgram, 
	const c8* pixelShaderEntryPointName,
	IShaderConstantSetCallBack* callback 
)
: m_pID3DDevice(d3ddev), m_pDriver(driver), m_pCallBack(callback), 
m_pVertexShader(0), m_pOldVertexShader(0), m_pPixelShader(0),
m_pVSConstantsTable(0), m_pPSConstantsTable(0)
{
	if (m_pCallBack)
		m_pCallBack->Grab();

	outMaterialTypeNr = -1;

	// now create shaders
	if (!CreateHLSLVertexShader(vertexShaderProgram, vertexShaderEntryPointName, "vs_3_0"))
		return;

	if (!CreateHLSLPixelShader(pixelShaderProgram, pixelShaderEntryPointName, "ps_3_0"))
		return;

	// register myself as new material
	outMaterialTypeNr = m_pDriver->AddMaterialRenderer(this);
}


//! Destructor
D3D9HLSLShader::~D3D9HLSLShader()
{
	if (m_pVSConstantsTable)
		m_pVSConstantsTable->Release();

	if (m_pPSConstantsTable)
		m_pPSConstantsTable->Release();

	if (m_pCallBack)
		m_pCallBack->Drop();

	if (m_pVertexShader)
		m_pVertexShader->Release();

	if (m_pPixelShader)
		m_pPixelShader->Release();
}

BOOL D3D9HLSLShader::OnRender(D3D9Driver* service)
{
	if (m_pVSConstantsTable)
		m_pVSConstantsTable->SetDefaults(m_pID3DDevice);

	// call callback to set shader constants
	if (m_pCallBack && (m_pVertexShader || m_pPixelShader))
		m_pCallBack->OnSetConstants(service, 0);

	return TRUE;
}

void D3D9HLSLShader::OnSetMaterial(SMaterial& material, const SMaterial& lastMaterial,
	BOOL resetAllRenderstates, D3D9Driver* services) 
{
	if (material.ShaderType != lastMaterial.ShaderType || resetAllRenderstates)
	{		
		if (m_pVertexShader)
		{
			// save old vertex shader
			m_pID3DDevice->GetVertexShader(&m_pOldVertexShader);

			// set new vertex shader
			if (FAILED(m_pID3DDevice->SetVertexShader(m_pVertexShader)))
				Printer::Log("Could not set vertex shader.");
		}

		// set new pixel shader
		if (m_pPixelShader)
		{
			if (FAILED(m_pID3DDevice->SetPixelShader(m_pPixelShader)))
				Printer::Log("Could not set pixel shader.");
		}
	}

	services->SetBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

void D3D9HLSLShader::OnUnsetMaterial() 
{
	if (m_pVertexShader)
		m_pID3DDevice->SetVertexShader(m_pOldVertexShader);

	if (m_pPixelShader)
		m_pID3DDevice->SetPixelShader(0);
}



HRESULT D3D9HLSLShader::StubD3DXCompileShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
	LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
	LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader,
	LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable)
{
	return D3DXCompileShader(pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
}

HRESULT D3D9HLSLShader::StubD3DXCompileShaderFromFile(LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, 
	LPD3DXINCLUDE pInclude, LPCSTR pFunctionName,
	LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs,
	LPD3DXCONSTANTTABLE* ppConstantTable)
{
	return D3DXCompileShaderFromFileA(pSrcFile, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
}

BOOL D3D9HLSLShader::CreateHLSLVertexShader(const char* vertexShaderProgram,
	const char* shaderEntryPointName,
	const char* shaderTargetName)
{
	if (!vertexShaderProgram)
		return true;

	LPD3DXBUFFER buffer = 0;
	LPD3DXBUFFER errors = 0;

	// compile without debug info

	HRESULT h = StubD3DXCompileShader(
		vertexShaderProgram,
		(UINT)strlen(vertexShaderProgram), 
		0, // macros
		0, // no includes
		shaderEntryPointName,
		shaderTargetName,
		0, // no flags 
		&buffer,
		&errors,
		&m_pVSConstantsTable);


	if (FAILED(h))
	{
		Printer::Log("HLSL vertex shader compilation failed:");
		if (errors)
		{
			Printer::Log((c8*)errors->GetBufferPointer());
			errors->Release();
			if (buffer)
				buffer->Release();
		}
		return FALSE;
	}

	if (errors)
		errors->Release();

	if (buffer)
	{
		if (FAILED(m_pID3DDevice->CreateVertexShader((DWORD*)buffer->GetBufferPointer(),
			&m_pVertexShader)))
		{
			Printer::Log("Could not create hlsl vertex shader.");
			buffer->Release();
			return FALSE;
		}

		buffer->Release();
		return TRUE;
	}

	return FALSE;
}


BOOL D3D9HLSLShader::CreateHLSLPixelShader(const char* pixelShaderProgram, 
	const char* shaderEntryPointName,
	const char* shaderTargetName)
{
	if (!pixelShaderProgram)
		return true;

	LPD3DXBUFFER buffer = 0;
	LPD3DXBUFFER errors = 0;

	HRESULT h = StubD3DXCompileShader(
		pixelShaderProgram,
		(UINT)strlen(pixelShaderProgram), 
		0, // macros
		0, // no includes
		shaderEntryPointName,
		shaderTargetName,
		0, // no flags (D3DXSHADER_DEBUG)
		&buffer,
		&errors,
		&m_pPSConstantsTable);

	if (FAILED(h))
	{
		Printer::Log("HLSL pixel shader compilation failed:");
		if (errors)
		{
			Printer::Log((c8*)errors->GetBufferPointer());
			errors->Release();
			if (buffer)
				buffer->Release();
		}
		return FALSE;
	}

	if (errors)
		errors->Release();

	if (buffer)
	{
		if (FAILED(m_pID3DDevice->CreatePixelShader((DWORD*)buffer->GetBufferPointer(),
			&m_pPixelShader)))
		{
			Printer::Log("Could not create hlsl pixel shader.");
			buffer->Release();
			return FALSE;
		}

		buffer->Release();
		return TRUE;
	}

	return FALSE;
}


BOOL D3D9HLSLShader::SetVariable(BOOL vertexShader, const c8* name, 
	const f32* floats, int count)
{
	LPD3DXCONSTANTTABLE tbl = vertexShader ? m_pVSConstantsTable : m_pPSConstantsTable;
	if (!tbl)
		return FALSE;

	// currently we only support top level parameters. 
	// Should be enough for the beginning. (TODO)

	D3DXHANDLE hndl = tbl->GetConstantByName(NULL, name);
	if (!hndl)
	{
		stringc s = "HLSL Variable to set not found: '";
		s += name;
		s += "'. Available variables are:";
		Printer::Log(s.c_str(), ELL_WARNING);
		PrintHLSLVariables(tbl);
		return false;
	}

	HRESULT hr = tbl->SetFloatArray(m_pID3DDevice, hndl, floats, count);
	if (FAILED(hr))
	{
		Printer::Log("Error setting float array for HLSL variable", ELL_WARNING);
		return FALSE;
	}

	return TRUE;
}


void D3D9HLSLShader::PrintHLSLVariables(LPD3DXCONSTANTTABLE table)
{
	// currently we only support top level parameters. 
	// Should be enough for the beginning. (TODO)

	// print out constant names
	D3DXCONSTANTTABLE_DESC tblDesc;
	HRESULT hr = table->GetDesc(&tblDesc);
	if (!FAILED(hr))
	{
		for (int i=0; i<(int)tblDesc.Constants; ++i)
		{
			D3DXCONSTANT_DESC d;
			UINT n = 1;
			D3DXHANDLE cHndl = table->GetConstant(NULL, i);
			if (!FAILED(table->GetConstantDesc(cHndl, &d, &n)))
			{
				stringc s = "  '";
				s += d.Name;
				s += "' Registers:[begin:";
				s += (int)d.RegisterIndex;
				s += ", count:";
				s += (int)d.RegisterCount;
				s += "]";
				Printer::Log(s.c_str());
			}
		}
	}
}


} // end namespace
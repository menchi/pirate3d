#include "DeviceWin32.h"
#include "RenderableObject.h"
#include <crtdbg.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	typedef POSITION<float, 2> XY;
	typedef COLOR<> RGBA;
	typedef GenVertexStruct<TYPE_LIST<XY, RGBA> > XYRGBA;
/*
	XYRGBA v[3];
	Field<XY>(v[0])[0] = 0.0f; Field<XY>(v[0])[1] = 0.5f; Field<RGBA>(v[0])[0] = 1.0f; Field<RGBA>(v[0])[1] = 0.0f; Field<RGBA>(v[0])[2] = 0.0f; Field<RGBA>(v[0])[3] = 1.0f;
	Field<XY>(v[1])[0] = 0.5f; Field<XY>(v[1])[1] =-0.5f; Field<RGBA>(v[1])[0] = 0.0f; Field<RGBA>(v[1])[1] = 1.0f; Field<RGBA>(v[1])[2] = 0.0f; Field<RGBA>(v[1])[3] = 1.0f;
	Field<XY>(v[2])[0] =-0.5f; Field<XY>(v[2])[1] =-0.5f; Field<RGBA>(v[2])[0] = 0.0f; Field<RGBA>(v[2])[1] = 0.0f; Field<RGBA>(v[2])[2] = 1.0f; Field<RGBA>(v[2])[3] = 1.0f;

	VertexBufferPtr pVB = VertexBuffer::Create<XYRGBA>(3);
	XYRGBA* pBuffer = pVB->GetBufferPtr<XYRGBA>();
	unsigned short index = 0;
	memcpy(pBuffer, v, sizeof(v));

	IndexBufferPtr pIB = IndexBuffer::Create(3);
	unsigned int* pIndices = pIB->GetBufferPtr();
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
*/
	DeviceWin32 Device(640, 480);
	VideoDriverPtr pDriver = Device.GetVideoDriver();
/*
	MeshBufferPtr pMB = MeshBuffer::Create(&pVB, &index, 1, pIB);
	pMB->CreateDriverResources(pDriver);

	const char* TwoToThree = "TwoToThree";
	VertexShaderFragmentPtr pVSFragment;
	pDriver->CreateVertexShaderFragmentsFromFile("VertexColor.hlsl", &TwoToThree, &pVSFragment, 1);
	VertexShaderPtr pVertexShader = pDriver->CreateVertexShader(&pVSFragment, 1);

	const char* PassColor = "PassColor";
	PixelShaderFragmentPtr pPSFragment;
	pDriver->CreatePixelShaderFragmentsFromFile("VertexColor.hlsl", &PassColor, &pPSFragment, 1);
	PixelShaderPtr pPixelShader = pDriver->CreatePixelShader(&pPSFragment, 1);

	ShaderProgramPtr pShaderProgram = pDriver->CreateShaderProgram(pVertexShader, pPixelShader);

	MaterialPtr pMaterial = Material::Create(pShaderProgram);
	RenderableObjectPtr pObj = RenderableObject::Create(&pMB, &pMaterial, 1);
*/
	Canvas canvas = Device.GetCanvas();
	while(Device.Run())
	{
		canvas << BackGroundColor(Colorf(0.0f, 0.3f, 0.2f, 1.0f)) << Eraser() << EndFrame;
	}
}
#include "DeviceWin32.h"
#include "RenderableObject.h"
#include "MeshBuffer.h"
#include "Material.h"
#include "VertexFormat.h"
#include "Canvas.h"
#include "PaintTools.h"
#include "VideoDriver.h"
#include <crtdbg.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	using namespace std;

	typedef POSITION<float, 2> XY;
	typedef COLOR<> RGBA;
	typedef GenVertexStruct<TYPE_LIST<XY, RGBA> > XYRGBA;

	XYRGBA v[3];
	Field<XY>(v[0])[0] = 0.0f; Field<XY>(v[0])[1] = 0.5f; 
	Field<RGBA>(v[0])[0] = 1.0f; Field<RGBA>(v[0])[1] = 0.0f; Field<RGBA>(v[0])[2] = 0.0f; Field<RGBA>(v[0])[3] = 1.0f;
	Field<XY>(v[1])[0] = 0.5f; Field<XY>(v[1])[1] =-0.5f; 
	Field<RGBA>(v[1])[0] = 0.0f; Field<RGBA>(v[1])[1] = 1.0f; Field<RGBA>(v[1])[2] = 0.0f; Field<RGBA>(v[1])[3] = 1.0f;
	Field<XY>(v[2])[0] =-0.5f; Field<XY>(v[2])[1] =-0.5f; 
	Field<RGBA>(v[2])[0] = 0.0f; Field<RGBA>(v[2])[1] = 0.0f; Field<RGBA>(v[2])[2] = 1.0f; Field<RGBA>(v[2])[3] = 1.0f;

	vector<VertexBufferPtr> VB(1);
	VB[0] = VertexBuffer::Create<XYRGBA>(3);
	XYRGBA* pBuffer = VB[0]->GetBufferPtr<XYRGBA>();
	memcpy(pBuffer, v, sizeof(v));

	vector<unsigned short> SI(1);
	SI[0] = 0;

	IndexBufferPtr pIB = IndexBuffer::Create(3);
	unsigned int* pIndices = pIB->GetBufferPtr();
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;

	DeviceWin32 Device(640, 480);
	VideoDriverPtr pDriver = Device.GetVideoDriver();

	MeshBufferPtr pMB = MeshBuffer::Create(VB, SI, pIB);
	pMB->CreateDriverResources(pDriver);

	VideoDriver::NameArray VSNames(1);
	VSNames[0] = "VSMain";
	VideoDriver::VertexShaderFragmentArray VSFragments = pDriver->CreateVertexShaderFragmentsFromFile("VertexColor.vert", VSNames);
	VertexShaderPtr pVertexShader = pDriver->CreateVertexShader(VSFragments);

	VideoDriver::NameArray PSName(2);
	PSName[0] = "PSMain";
	PSName[1] = "InvertColor";
	VideoDriver::PixelShaderFragmentArray PSFragments = pDriver->CreatePixelShaderFragmentsFromFile("VertexColor.frag", PSName);
	PixelShaderPtr pPixelShader = pDriver->CreatePixelShader(PSFragments);

	ShaderProgramPtr pShaderProgram = pDriver->CreateShaderProgram(pVertexShader, pPixelShader);

	MaterialPtr pMaterial = Material::Create(pShaderProgram);
	RenderableObjectPtr pObj = RenderableObject::Create(&pMB, &pMaterial, 1);

	Canvas& canvas = Device.GetCanvas();
	while(Device.Run())
	{
		canvas << BackGroundColor(Colorf(0.0f, 0.2f, 0.2f, 1.0f)) << erase << pObj << endf;
	}
}
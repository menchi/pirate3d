#include "DeviceWin32.h"
#include "RenderableObject.h"
#include "MeshBuffer.h"
#include "Material.h"
#include "Canvas.h"
#include "PaintTools.h"
#include "VideoDriver.h"
#include "OpenGLVertexFormat.h"
#include "HierarchyGenerator.h"
#include <crtdbg.h>

using namespace std;
using namespace boost::mpl;

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	typedef GenVertexStruct<TYPE_LIST<POSITION2f, COLOR4f> > VertexStruct;

	vector<VertexBufferPtr> VB_(1);
	VB_[0] = VertexBuffer::Create<VertexStruct>(3);
	VertexStruct* v_ = VB_[0]->GetBufferPtr<VertexStruct>();

	field<POSITION2f>(v_[0])[0] = 0.0f; field<POSITION2f>(v_[0])[1] = 0.5f; 
	field<COLOR4f>(v_[0])[0] = 1.0f; field<COLOR4f>(v_[0])[1] = 0.0f; field<COLOR4f>(v_[0])[2] = 0.0f; field<COLOR4f>(v_[0])[3] = 1.0f;
	field<POSITION2f>(v_[1])[0] = 0.5f; field<POSITION2f>(v_[1])[1] =-0.5f; 
	field<COLOR4f>(v_[1])[0] = 0.0f; field<COLOR4f>(v_[1])[1] = 1.0f; field<COLOR4f>(v_[1])[2] = 0.0f; field<COLOR4f>(v_[1])[3] = 1.0f;
	field<POSITION2f>(v_[2])[0] =-0.5f; field<POSITION2f>(v_[2])[1] =-0.5f; 
	field<COLOR4f>(v_[2])[0] = 0.0f; field<COLOR4f>(v_[2])[1] = 0.0f; field<COLOR4f>(v_[2])[2] = 1.0f; field<COLOR4f>(v_[2])[3] = 1.0f;

	vector<unsigned short> SI(1);
	SI[0] = 0;

	IndexBufferPtr pIB = IndexBuffer::Create(3);
	unsigned int* pIndices = pIB->GetBufferPtr();
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;

	DeviceWin32 Device(640, 480);
	VideoDriverPtr pDriver = Device.GetVideoDriver();

	MeshBufferPtr pMB = MeshBuffer::Create(VB_, SI, pIB);
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
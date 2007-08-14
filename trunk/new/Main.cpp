#include "DeviceWin32.h"
#include "MeshBuffer.h"
#include <crtdbg.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	typedef GenVertexStruct<TYPE_LIST<POSITION<>, NORMAL<>, TEXCOORD<> > > MyVertexFormat;
	MyVertexFormat v;
	Field<POSITION<> >(v).v[2] = 1.23f;
	int s = sizeof(MyVertexFormat);

	typedef GenVertexStruct<TYPE_LIST<POSITION<float,2>, COLOR<> > > MyVertexFormat2;
	MyVertexFormat2 v2;
	Field<COLOR<> >(v2).v[1] = 0.7f;
	s = sizeof(MyVertexFormat2);

	VertexBufferPtr pVB = VertexBuffer::Create<MyVertexFormat>(3);
	MyVertexFormat* pBuffer = pVB->GetBufferPtr<MyVertexFormat>();
	unsigned short index = 0;

	IndexBufferPtr pIB = IndexBuffer::Create(3);

	MeshBufferPtr pMB = MeshBuffer::Create(&pVB, &index, 1, pIB);

	DeviceWin32 Device(640, 480);

	Canvas canvas = Device.GetCanvas();
	while(Device.Run())
	{
		canvas << BackGroundColor(Colorf(0.0f, 0.3f, 0.2f, 1.0f)) << Eraser() << WorkArea(160, 120, 320, 240) 
			   << BackGroundColor(Colorf(0.0f, 0.2f, 0.2f, 1.0f)) << Eraser() << EndFrame;
	}
}
#ifndef _PIRATE_D3D9_DRIVER_RESOURCES_H_
#define _PIRATE_D3D9_DRIVER_RESOURCES_H_

#include "D3D9Wrapper.h"
#include "SmartPointer.h"

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)
FWD_DECLARE(MeshBuffer)

class DriverVertexBuffer {
private:
	DriverVertexBuffer(IDirect3DDevice9Ptr pD3DDevice, int size);

	IDirect3DVertexBuffer9Ptr m_pID3DVertexBuffer;

	friend class D3D9Driver;
};

class DriverIndexBuffer {
private:
	DriverIndexBuffer(IDirect3DDevice9Ptr pD3DDevice, int size);

	IDirect3DIndexBuffer9Ptr m_pID3DIndexBuffer;

	friend class D3D9Driver;
};

class DriverVertexDeclaration {
private:
	DriverVertexDeclaration(IDirect3DDevice9Ptr pD3DDevice, MeshBufferPtr pMeshBuffer);

	IDirect3DVertexDeclaration9Ptr m_pID3DVertexDeclaration;

	friend class D3D9Driver;
};

#endif
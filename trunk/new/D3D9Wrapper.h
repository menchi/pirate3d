#ifndef _PIRATE_D3D9_WRAPPER_H_
#define _PIRATE_D3D9_WRAPPER_H_

#include "boost/intrusive_ptr.hpp"
#include <d3d9.h>

#pragma comment(lib, "d3d9")

typedef boost::intrusive_ptr<IDirect3D9>					IDirect3D9Ptr;
typedef boost::intrusive_ptr<IDirect3DDevice9>				IDirect3DDevice9Ptr;
typedef boost::intrusive_ptr<IDirect3DSurface9>				IDirect3DSurface9Ptr;
typedef boost::intrusive_ptr<IDirect3DVertexBuffer9>		IDirect3DVertexBuffer9Ptr;
typedef boost::intrusive_ptr<IDirect3DIndexBuffer9>			IDirect3DIndexBuffer9Ptr;
typedef boost::intrusive_ptr<IDirect3DVertexDeclaration9>	IDirect3DVertexDeclaration9Ptr;

template<class T> void intrusive_ptr_add_ref(T* p)
{
	p->AddRef();
}

template<class T> void intrusive_ptr_release(T* p)
{
	p->Release();
}

#endif
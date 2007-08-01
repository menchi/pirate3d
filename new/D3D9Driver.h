#ifndef _PIRATE_D3D9_DRIVER_H_
#define _PIRATE_D3D9_DRIVER_H_

#include "CompileConfig.h"
#include "boost/intrusive_ptr.hpp"

typedef boost::intrusive_ptr<IDirect3D9> IDirect3D9Ptr;
typedef boost::intrusive_ptr<IDirect3DDevice9> IDirect3DDevice9Ptr;

template<class T> void intrusive_ptr_add_ref(T* p)
{
	p->AddRef();
}

template<class T> void intrusive_ptr_release(T* p)
{
	p->Release();
}

class D3D9Driver {
public:
	D3D9Driver(HWND window, int width, int height, bool fullScreen);
	~D3D9Driver();

private:
	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;

	IDirect3D9Ptr m_pID3D9;
	IDirect3DDevice9Ptr m_pID3DDevice;
};

#endif
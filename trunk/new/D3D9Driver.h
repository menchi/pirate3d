#ifndef _PIRATE_D3D9_DRIVER_H_
#define _PIRATE_D3D9_DRIVER_H_

#include "CompileConfig.h"
#include "boost/intrusive_ptr.hpp"
#include "boost/tr1/memory.hpp"
#include "boost/tr1/type_traits.hpp"

#define DEFINE_SHARED_PTR(T) typedef SmartPtrDefiner<T>::shared_ptr_type T##SharedPtr;
#define DEFINE_WEAK_PTR(T) typedef SmartPtrDefiner<T>::weak_ptr_type T##WeakPtr;
#define DEFINE_SMART_PTR(T) DEFINE_SHARED_PTR(T) DEFINE_WEAK_PTR(T)
#define TYPE_DEFINE_WITH_SMART_PTR(T, U) typedef T U; DEFINE_SMART_PTR

template<class T, bool IsCOM>
struct SmartPtrTraits{};

template<class T>
struct SmartPtrTraits<T, true> {
	typedef boost::intrusive_ptr<T> shared_ptr_type;
	typedef T* weak_ptr_type;
};

template<class T>
struct SmartPtrTraits<T, false> {
	typedef std::tr1::shared_ptr<T> shared_ptr_type;
	typedef std::tr1::weak_ptr<T> weak_ptr_type;
};

template<class T>
struct SmartPtrDefiner
{
	typedef typename SmartPtrTraits<T, std::tr1::is_base_of<IUnknown, T>::value>::shared_ptr_type shared_ptr_type;
	typedef typename SmartPtrTraits<T, std::tr1::is_base_of<IUnknown, T>::value>::weak_ptr_type weak_ptr_type;
};


DEFINE_SMART_PTR(IDirect3D9)
DEFINE_SMART_PTR(IDirect3DDevice9)
DEFINE_SMART_PTR(IDirect3DSurface9)

struct null_deleter
{
	void operator()(void const *) const
	{
	}
};


template<class T> void intrusive_ptr_add_ref(T* p)
{
	p->AddRef();
}

template<class T> void intrusive_ptr_release(T* p)
{
	p->Release();
}

class Canvas;
DEFINE_SMART_PTR(Canvas);

class D3D9Driver {
public:
	typedef IDirect3DSurface9 RenderTargetType;
	DEFINE_SMART_PTR(RenderTargetType)

	D3D9Driver(HWND window, int width, int height, bool fullScreen);
	~D3D9Driver();

	CanvasSharedPtr GetCanvas();

	void Clear(bool color, bool z, bool stencil)
	{
		DWORD flag = 0;
		flag = (color)? flag & D3DCLEAR_TARGET : flag;
		flag = (z)? flag & D3DCLEAR_ZBUFFER : flag;
		flag = (stencil)? flag & D3DCLEAR_STENCIL : flag;
		m_pID3DDevice->Clear(0, 0, flag, 0, 1, 0);
	}

	void SwapBuffer()
	{
		m_pID3DDevice->Present(NULL, NULL, NULL, NULL);
	}

private:
	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;

	IDirect3D9SharedPtr m_pID3D9;
	IDirect3DDevice9SharedPtr m_pID3DDevice;

	CanvasSharedPtr m_pCanvas;
};

typedef D3D9Driver VideoDriver;
DEFINE_SMART_PTR(VideoDriver)

class Canvas {
public:
	typedef VideoDriver::RenderTargetType RenderTargetType;
	DEFINE_SMART_PTR(RenderTargetType);

	Canvas(VideoDriverSharedPtr pVideoDriver, RenderTargetTypeSharedPtr pRenderTarget) 
		: m_pVideoDriver(pVideoDriver), m_pRenderTarget(pRenderTarget) 
	{}

	~Canvas()
	{

	}

	Canvas& operator << (Canvas& (*op)(Canvas&))
	{
		return (*op)(*this);
	}

	static Canvas& Eraser(Canvas& canvas)
	{
		VideoDriverSharedPtr pVideoDriver(canvas.m_pVideoDriver);
		pVideoDriver->Clear(true, true, true);
		return canvas;
	}
	static Canvas& EndFrame(Canvas& canvas)
	{
		VideoDriverSharedPtr(canvas.m_pVideoDriver)->SwapBuffer();
		return canvas;
	}

private:
	VideoDriverSharedPtr m_pVideoDriver;
	RenderTargetTypeSharedPtr m_pRenderTarget;
};

#endif
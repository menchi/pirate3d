#ifndef _PIRATE_D3D9_DRIVER_H_
#define _PIRATE_D3D9_DRIVER_H_

#include "CompileConfig.h"
#include <vector>

#ifdef _PIRATE_COMPILE_WITH_D3D9_

DEFINE_SMART_PTR(IDirect3D9)
DEFINE_SMART_PTR(IDirect3DDevice9)
DEFINE_SMART_PTR(IDirect3DSurface9)
DEFINE_SMART_PTR(IDirect3DVertexBuffer9)
DEFINE_SMART_PTR(IDirect3DIndexBuffer9)

FWD_DECLARE(Canvas)
FWD_DECLARE(VertexBuffer)
FWD_DECLARE(IndexBuffer)
FWD_DECLARE(D3D9Driver)
TYPE_DEFINE_WITH_SMART_PTR(D3D9Driver, VideoDriver)

class D3D9Driver {
public:
	~D3D9Driver();

	CanvasPtr GetCanvas() { return m_pCanvas; }
	VertexBufferPtr CreateVertexBuffer(int size);
	IndexBufferPtr CreateIndexBuffer(int size);

	void SetBackgroundColor(Colorf color) { m_BackgroundColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a); }
	void Clear(bool color, bool z, bool stencil);
	void SetViewport(int x, int y, int w, int h);
	void SwapBuffer() {	m_pID3DDevice->Present(NULL, NULL, NULL, NULL);	}

	static D3D9DriverPtr CreateVideoDriver(HWND window, int width, int height, bool fullScreen);

private:
	D3D9Driver(HWND window, int width, int height, bool fullScreen);

	int m_iWidth, m_iHeight;
	bool m_bIsFullScreen;
	D3DCOLOR m_BackgroundColor;

	IDirect3D9* m_pID3D9;
	IDirect3DDevice9Ptr m_pID3DDevice;

	CanvasPtr m_pCanvas;
	std::vector<VertexBufferPtr> m_VertexBuffers;
};

class VertexBuffer {
private:
	VertexBuffer(IDirect3DDevice9Ptr pD3DDevice, int size)
	{
		IDirect3DVertexBuffer9* pVB;
		pD3DDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
		m_pID3DVertexBuffer = IDirect3DVertexBuffer9Ptr(pVB, false);
	}

	IDirect3DVertexBuffer9Ptr m_pID3DVertexBuffer;

	friend class D3D9Driver;
};

class IndexBuffer {
private:
	IndexBuffer(IDirect3DDevice9Ptr pD3DDevice, int size)
	{
		IDirect3DIndexBuffer9* pIB;
		pD3DDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &pIB, NULL);
		m_pID3DIndexBuffer = IDirect3DIndexBuffer9Ptr(pIB, false);
	}

	IDirect3DIndexBuffer9Ptr m_pID3DIndexBuffer;

	friend class D3D9Driver;
};

#endif

#endif
#ifndef _PIRATE_CANVAS_H_
#define _PIRATE_CANVAS_H_

#include "PaintTools.h"

class Canvas {
public:
	typedef VideoDriver::RenderTargetType RenderTargetType;
	DEFINE_SMART_PTR(RenderTargetType);

	Canvas(VideoDriverPtr pVideoDriver, RenderTargetTypePtr pRenderTarget) : 
	m_pVideoDriver(pVideoDriver), m_pRenderTarget(pRenderTarget)
	{}

	VideoDriverPtr GetVideoDriver() { return m_pVideoDriver.lock(); }

	Canvas& operator << (Canvas& (*op)(Canvas&))
	{
		return (*op)(*this);
	}

	Canvas& operator << (const PaintTool& tool)
	{
		tool.Use(m_pVideoDriver.lock());
		return *this;
	}

	Canvas& operator << (const Eraser& eraser)
	{
		VideoDriverPtr pDriver(m_pVideoDriver);
		pDriver->Clear(true, true, true);
		return *this;
	}

private:
	VideoDriverWeakPtr m_pVideoDriver;
	RenderTargetTypePtr m_pRenderTarget;
};

Canvas& EndFrame(Canvas& canvas);

#endif
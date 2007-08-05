#ifndef _PIRATE_CANVAS_H_
#define _PIRATE_CANVAS_H_

#include "PaintTools.h"

class Canvas {
public:
	Canvas(VideoDriverPtr pVideoDriver) : m_pVideoDriver(pVideoDriver) {}

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

private:
	VideoDriverWeakPtr m_pVideoDriver;
};

Canvas& EndFrame(Canvas& canvas);

#endif
#ifndef _PIRATE_PAINT_TOOLS_H_
#define _PIRATE_PAINT_TOOLS_H_

#include "VideoDriver.h"

class WorkArea : public PaintTool {
public:
	WorkArea(int x, int y, int w, int h) : m_iX(x), m_iY(y), m_iWidth(w), m_iHeight(h) {}

	virtual void Use(VideoDriverPtr pDriver) const
	{
		pDriver->SetViewport(m_iX, m_iY, m_iWidth, m_iHeight);
	}

private:
	int m_iX, m_iY;
	int m_iWidth, m_iHeight;
};

class BackGroundColor : public PaintTool {
public:
	BackGroundColor(Colorf color) : m_Color(color) {}
	virtual void Use(VideoDriverPtr pDriver) const
	{
		pDriver->SetBackgroundColor(m_Color);
	}

private:
	Colorf m_Color;
};

class Eraser {
public:
	Eraser() : m_bColor(true), m_bZ(true), m_bStencil(true) {}
	Eraser(bool color, bool z, bool stencil) : m_bColor(color), m_bZ(true), m_bStencil(stencil) {}
	virtual void Use(VideoDriverPtr pDriver) const
	{
		pDriver->Clear(m_bColor, m_bZ, m_bStencil);
	}

private:
	bool m_bColor, m_bZ, m_bStencil;
};

#endif
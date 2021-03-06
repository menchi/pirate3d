#ifndef _PIRATE_PAINT_TOOLS_H_
#define _PIRATE_PAINT_TOOLS_H_

#include "VideoDriverFwd.h"

class WorkArea {
public:
	WorkArea(int x, int y, int w, int h) : m_iX(x), m_iY(y), m_iWidth(w), m_iHeight(h) {}
	void Use(VideoDriverPtr pDriver) const;

private:
	int m_iX, m_iY;
	int m_iWidth, m_iHeight;
};

class BackGroundColor {
public:
	BackGroundColor(const Colorf& color) : m_Color(color) {}
	void Use(VideoDriverPtr pDriver) const;

private:
	Colorf m_Color;
};

class Eraser {
public:
	Eraser(bool color, bool z, bool stencil) : m_bColor(color), m_bZ(true), m_bStencil(stencil) {}
	void Use(VideoDriverPtr pDriver) const;

private:
	bool m_bColor, m_bZ, m_bStencil;
};

#endif
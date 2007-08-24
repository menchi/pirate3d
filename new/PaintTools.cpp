#include "PaintTools.h"
#include "VideoDriver.h"

//-----------------------------------------------------------------------------
void WorkArea::Use(VideoDriverPtr pDriver) const
{
	pDriver->SetViewport(m_iX, m_iY, m_iWidth, m_iHeight);
}
//-----------------------------------------------------------------------------
void BackGroundColor::Use(VideoDriverPtr pDriver) const
{
	pDriver->SetBackgroundColor(m_Color);
}
//-----------------------------------------------------------------------------
void Eraser::Use(VideoDriverPtr pDriver) const
{
	pDriver->Clear(m_bColor, m_bZ, m_bStencil);
}
//-----------------------------------------------------------------------------
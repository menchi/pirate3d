#ifndef _PIRATE_VIDEO_DRIVER_H_
#define _PIRATE_VIDEO_DRIVER_H_

#include "D3D9Driver.h"
#include "OpenGLDriver.h"

class PaintTool
{
public:
	virtual void Use(VideoDriverPtr pDriver) const = 0;
};

#endif
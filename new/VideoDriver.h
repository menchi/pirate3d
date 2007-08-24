#ifndef _PIRATE_VIDEO_DRIVER_H_
#define _PIRATE_VIDEO_DRIVER_H_

#include "CompileConfig.h"

#if defined(_PIRATE_COMPILE_WITH_D3D9_)
#include "D3D9DriverResources.h"
#include "D3D9Driver.h"
#elif defined(_PIRATE_COMPILE_WITH_OPENGL_)
#include "OpenGLDriverResources.h"
#include "OpenGLDriver.h"
#endif

#endif
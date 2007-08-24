#ifndef _PIRATE_VIDEO_DRIVER_FWD_H_
#define _PIRATE_VIDEO_DRIVER_FWD_H_

#include "CompileConfig.h"
#include "SmartPointer.h"

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)

FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

#if defined(_PIRATE_COMPILE_WITH_D3D9_)
class D3D9Driver;
TYPE_DEFINE_WITH_SMART_PTR(D3D9Driver, VideoDriver)
#elif defined(_PIRATE_COMPILE_WITH_OPENGL_)
class OpenGLDriver;
TYPE_DEFINE_WITH_SMART_PTR(OpenGLDriver, VideoDriver)
#endif

#endif
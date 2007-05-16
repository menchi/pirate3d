#ifndef _PIRATE_H_
#define _PIRATE_H_

#include "D3D9HLSLShader.h"
#include "IShaderConstantSetCallback.h"
#include "DeviceWin32.h"
#include "pirateMath.h"
#include "pirateArray.h"
#include "pirateString.h"
#include "ImageLoader.h"
#include "Image.h"
#include "SMeshBuffer.h"
#include "CameraSceneNode.h"
#include "MeshSceneNode.h"
#include "SceneManager.h"

namespace Pirate
{
	DeviceWin32* InvokePirate(s32 Width, s32 Height, BOOL fullscreen=FALSE, BOOL stencilbuffer=FALSE, BOOL vsync=FALSE);
}

#endif
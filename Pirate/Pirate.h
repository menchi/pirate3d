#ifndef _PIRATE_H_
#define _PIRATE_H_

#include "DeviceWin32.h"
#include "D3D9Driver.h"
#include "D3D9HLSLShader.h"
#include "SceneManager.h"
#include "MeshSceneNode.h"
#include "CameraFPSSceneNode.h"
#include "IShaderConstantSetCallback.h"
#include "SMesh.h"
#include "FileSystem.h"
#include "os.h"

namespace Pirate
{
	DeviceWin32* InvokePirate(s32 Width, s32 Height, BOOL fullscreen=FALSE, BOOL stencilbuffer=FALSE, BOOL vsync=FALSE);
}

#endif
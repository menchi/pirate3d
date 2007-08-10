#ifndef _PIRATE_COMPILE_CONFIG_H_
#define _PIRATE_COMPILE_CONFIG_H_

#include <Windows.h>

#define _PIRATE_COMPILE_WITH_D3D9_
//#define _PIRATE_COMPILE_WITH_OPENGL_

#ifdef _PIRATE_COMPILE_WITH_D3D9_
#include <d3d9.h>

#pragma comment(lib, "d3d9")
#elif defined(_PIRATE_COMPILE_WITH_OPENGL_)
#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#endif

#define _PIRATE_USE_D3DX_MATH_

#ifdef _PIRATE_USE_D3DX_MATH_
#include <d3dx9.h>
#endif

//! define a break macro for debugging.
#if defined(_DEBUG)
#include <cassert>
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#else 
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#include "SmartPointer.h"
#include "BaseTypes.h"

#endif
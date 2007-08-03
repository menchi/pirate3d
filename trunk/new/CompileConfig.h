#ifndef _PIRATE_COMPILE_CONFIG_H_
#define _PIRATE_COMPILE_CONFIG_H_

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9")

#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

#define _PIRATE_USE_D3DX_MATH_

//! define a break macro for debugging.
#if defined(_DEBUG)
#include "assert.h"
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#else 
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#include "SmartPointer.h"
#include "BaseTypes.h"

#endif
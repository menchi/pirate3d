#ifndef _PIRATE_COMPILE_CONFIG_H_
#define _PIRATE_COMPILE_CONFIG_H_

#include <Windows.h>

//#define _PIRATE_COMPILE_WITH_D3D9_
#define _PIRATE_COMPILE_WITH_OPENGL_

#define _PIRATE_USE_D3DX_MATH_

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
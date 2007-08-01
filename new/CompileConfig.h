#ifndef _PIRATE_COMPILE_CONFIG_H_
#define _PIRATE_COMPILE_CONFIG_H_

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9")

#define _PIRATE_USE_D3DX_MATH_

//! define a break macro for debugging.
#if defined(_DEBUG)
#include "assert.h"
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#else 
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#endif
#ifndef _PIRATE_TYPES_H_
#define _PIRATE_TYPES_H_

namespace Pirate
{
	typedef unsigned char	u8; 

	typedef signed char		s8; 

	typedef char			c8; 

	typedef unsigned short	u16;

	typedef signed short	s16; 

	typedef unsigned int	u32;

	typedef signed int		s32; 

	typedef __int64			s64; 

	typedef float			f32; 

	typedef double			f64;

	typedef u32				BOOL;

	#define TRUE			1

	#define FALSE			0
} // end namespace Pirate


#include <wchar.h>
//! Defines for s{w,n}printf because these methods do not match the ISO C
//! standard on Windows platforms, but it does on all others.
#define swprintf _snwprintf
#define snprintf _snprintf


//! define a break macro for debugging.
#if defined(_DEBUG)
#include "assert.h"
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#else 
#define _PIRATE_DEBUG_BREAK_IF( _CONDITION_ )
#endif


#if defined(_DEBUG) && defined(_MSC_VER) && (_MSC_VER > 1299) 

#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#include <stdlib.h>
#include <crtdbg.h>
#define new DEBUG_CLIENTBLOCK
#endif 

#endif
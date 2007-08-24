#include "D3D9Wrapper.h"
#include "CompileConfig.h"
#include <iostream>

#ifdef _PIRATE_COMPILE_WITH_D3D9_

//-----------------------------------------------------------------------------
void intrusive_ptr_release(IDirect3D9* p)
{
	if (p->Release() > 0)
		std::clog<<"Some Direc3D Objects not released\n";
}
//-----------------------------------------------------------------------------

#endif
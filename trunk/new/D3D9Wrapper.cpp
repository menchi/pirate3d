#include "D3D9Wrapper.h"
#include <iostream>

void intrusive_ptr_release(IDirect3D9* p)
{
	if (p->Release() > 0)
		std::clog<<"Some Direc3D Objects not released\n";
}
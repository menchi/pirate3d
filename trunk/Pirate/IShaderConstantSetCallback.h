#ifndef _PIRATE_I_SHADER_CONSTANT_SET_CALLBACK_H_
#define _PIRATE_I_SHADER_CONSTANT_SET_CALLBACK_H_

#include "RefObject.h"

namespace Pirate
{

class D3D9Driver;

//! Interface making it possible to set constants for gpu programs every frame. 
class IShaderConstantSetCallBack : public virtual RefObject
{
public:

	//! Destructor.
	virtual ~IShaderConstantSetCallBack() {}

	//! Called by the engine when the vertex and/or pixel shader constants for an material renderer should be set.
	virtual void OnSetConstants(D3D9Driver* services, s32 userData) = 0;
};

} // end namespace

#endif
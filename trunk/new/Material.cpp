#include "Material.h"

//-----------------------------------------------------------------------------
MaterialPtr Material::Create(ShaderProgramPtr pShaderProgram)
{
	return MaterialPtr(new Material(pShaderProgram));
}
//-----------------------------------------------------------------------------
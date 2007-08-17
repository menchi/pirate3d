#ifndef _PIRATE_MATERIAL_H_
#define _PIRATE_MATERIAL_H_

#include "SmartPointer.h"

FWD_DECLARE(ShaderProgram)
FWD_DECLARE(Material)

class Material {
public:
	ShaderProgramPtr GetShaderProgram() { return m_pShaderProgram; }
	static MaterialPtr Create(ShaderProgramPtr pShaderProgram);

private:
	Material(ShaderProgramPtr pShaderProgram) : m_pShaderProgram(pShaderProgram) {}

	ShaderProgramPtr m_pShaderProgram;
};

#endif
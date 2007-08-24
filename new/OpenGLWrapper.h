#ifndef _PIRATE_OPENGL_WRAPPER_H_
#define _PIRATE_OPENGL_WRAPPER_H_

#include "glew.h"
#include <gl/gl.h>
#include <gl/glu.h>

static const unsigned int MAX_TEXTURE_UNIT = 8;

void PrintShaderInfoLog(GLuint obj);
void PrintProgramInfoLog(GLuint obj);

inline GLvoid* BufferObjectPtr(unsigned int idx)
{
	return (GLvoid*)( ((char*)0) + idx );
}

#endif
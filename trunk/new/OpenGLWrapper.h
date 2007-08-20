#ifndef _PIRATE_OPENGL_WRAPPER_H_
#define _PIRATE_OPENGL_WRAPPER_H_

#include "glew.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <cstdio>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

static const unsigned int MAX_TEXTURE_UNIT = 8;

void PrintShaderInfoLog(GLuint obj);
void PrintProgramInfoLog(GLuint obj);

inline GLvoid* BufferObjectPtr(unsigned int idx)
{
	return (GLvoid*)( ((char*)NULL) + idx );
}

#endif
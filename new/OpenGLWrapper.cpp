#include "OpenGLWrapper.h"
#include "CompileConfig.h"
#include <iostream>

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

//-----------------------------------------------------------------------------
void EnableClientStateTexCoord(int i)
{
	glClientActiveTexture(GL_TEXTURE0 + i);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}
//-----------------------------------------------------------------------------
void DisableClientStateTexCoord(int i)
{
	glClientActiveTexture(GL_TEXTURE0 + i);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
//-----------------------------------------------------------------------------
void TexCoordPointer_i(int index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	glClientActiveTexture(GL_TEXTURE0 + index);
	glTexCoordPointer(size, type, stride, pointer);
}
//-----------------------------------------------------------------------------
void PrintShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::clog << infoLog << std::endl;
		free(infoLog);
	}
}
//-----------------------------------------------------------------------------
void PrintProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::clog << infoLog << std::endl;
		free(infoLog);
	}
}
//-----------------------------------------------------------------------------

#endif
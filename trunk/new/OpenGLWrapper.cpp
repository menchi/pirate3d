#include "OpenGLWrapper.h"
#include "CompileConfig.h"
#include <iostream>

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

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
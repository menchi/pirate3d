#ifndef _PIRATE_OPENGL_DRIVER_RESOURCES_H_
#define _PIRATE_OPENGL_DRIVER_RESOURCES_H_

#include "SmartPointer.h"
#include "OpenGLWrapper.h"
#include <vector>
#include <string>

FWD_DECLARE(VertexBuffer)

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)

FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

typedef std::pair<unsigned short, VertexBufferPtr> StreamIndexVertexBufferPair;

class DriverVertexBuffer {
public:
	~DriverVertexBuffer() { glDeleteBuffers(1, &m_uiVertexBufferID); }
	void Fill(void* pData, unsigned int Size);

private:
	DriverVertexBuffer(unsigned int NumVertices, unsigned int VertexSize);

	GLuint m_uiVertexBufferID;
	unsigned int m_uiVertexSize;

	friend class OpenGLDriver;
};

class DriverIndexBuffer {
public:
	~DriverIndexBuffer() { glDeleteBuffers(1, &m_uiIndexBufferID); }
	void Fill(void* pData, unsigned int Size);

private:
	DriverIndexBuffer(unsigned int NumIndices);

	GLuint m_uiIndexBufferID;

	friend class OpenGLDriver;
};

class DriverVertexDeclaration {
public:
	~DriverVertexDeclaration();

private:
	DriverVertexDeclaration(StreamIndexVertexBufferPair* ppVertexBuffers, unsigned int NumVertexBuffers);

	struct VertexParam {
		GLuint Size;
		GLenum Type;
		GLsizei Stride;
		GLvoid* Pointer;
	};

	VertexParam* m_pVertex;
	VertexParam* m_pNormal;
	VertexParam* m_pColor;
	VertexParam* m_ppTexCoords[MAX_TEXTURE_UNIT];

	friend class OpenGLDriver;
};

class VertexShaderFragment {
public:
	~VertexShaderFragment();

private:
	VertexShaderFragment(const std::string& Source);

	GLuint m_uiGLVertexShader;

	friend class OpenGLDriver;
	friend class VertexShader;
};

class VertexShader {
private:
	VertexShader(VertexShaderFragmentPtr* ppFragments, unsigned int NumFragments);

	std::vector<GLuint> m_Fragments;

	friend class OpenGLDriver;
	friend class ShaderProgram;
};

class PixelShaderFragment {
public:
	~PixelShaderFragment();

private:
	PixelShaderFragment(const std::string& Source);

	GLuint m_uiGLFragmentShader;

	friend class OpenGLDriver;
	friend class PixelShader;
};

class PixelShader {
private:
	PixelShader(PixelShaderFragmentPtr* ppFragments, unsigned int NumFragments);

	std::vector<GLuint> m_Fragments;

	friend class OpenGLDriver;
	friend class ShaderProgram;
};

class ShaderProgram {
private:
	ShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader);

	GLuint m_uiGLShaderProgram;

	friend class OpenGLDriver;
};

#endif
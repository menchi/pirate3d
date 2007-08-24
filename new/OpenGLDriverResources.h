#ifndef _PIRATE_OPENGL_DRIVER_RESOURCES_H_
#define _PIRATE_OPENGL_DRIVER_RESOURCES_H_

#include "OpenGLWrapper.h"
#include "SmartPointer.h"
#include <vector>
#include <string>

struct VertexElement;

FWD_DECLARE(DriverVertexBuffer)
FWD_DECLARE(DriverIndexBuffer)
FWD_DECLARE(DriverVertexDeclaration)

FWD_DECLARE(VertexShaderFragment)
FWD_DECLARE(VertexShader)
FWD_DECLARE(PixelShaderFragment)
FWD_DECLARE(PixelShader)
FWD_DECLARE(ShaderProgram)

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
	typedef std::vector<VertexElement> VertexElementArray;

	typedef std::vector<unsigned short> StreamIndexArray;
	typedef std::vector<const VertexElementArray*> VertexFormatArray;
	typedef std::vector<unsigned short> NumVertexElementsArray;

	DriverVertexDeclaration(const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats);

	struct VertexParam {
		unsigned short Index;
		GLuint Size;
		GLenum Type;
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
	friend class ShaderProgram;
};

class VertexShader {
private:
	typedef std::vector<VertexShaderFragmentPtr> VertexShaderFragmentArray;

	VertexShader(const VertexShaderFragmentArray& Fragments);

	VertexShaderFragmentArray m_Fragments;

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
	friend class ShaderProgram;
};

class PixelShader {
private:
	typedef std::vector<PixelShaderFragmentPtr> PixelShaderFragmentArray;

	PixelShader(const PixelShaderFragmentArray& Fragments);

	PixelShaderFragmentArray m_Fragments;

	friend class OpenGLDriver;
	friend class ShaderProgram;
};

class ShaderProgram {
public:
	~ShaderProgram();

private:
	ShaderProgram(const VertexShaderPtr pVertexShader, const PixelShaderPtr pPixelShader);

	GLuint m_uiGLShaderProgram;

	friend class OpenGLDriver;
};

#endif
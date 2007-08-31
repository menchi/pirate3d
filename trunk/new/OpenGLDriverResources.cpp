#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include "OpenGLDriverResources.h"

//-----------------------------------------------------------------------------
DriverVertexBuffer::DriverVertexBuffer(unsigned int NumVertices, unsigned int VertexSize) : m_uiVertexSize(VertexSize)
{
	glGenBuffers(1, &m_uiVertexBufferID);
}
//-----------------------------------------------------------------------------
void DriverVertexBuffer::Fill(void* pData, unsigned int Size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, Size, pData, GL_STATIC_DRAW);
}
//-----------------------------------------------------------------------------
DriverIndexBuffer::DriverIndexBuffer(unsigned int NumIndices)
{
	glGenBuffers(1, &m_uiIndexBufferID);
}
//-----------------------------------------------------------------------------
void DriverIndexBuffer::Fill(void* pData, unsigned int Size)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, pData, GL_STATIC_DRAW);
}
//-----------------------------------------------------------------------------
DriverVertexDeclaration::DriverVertexDeclaration(const StreamIndexArray& StreamIndices, const VertexFormatArray& VertexFormats)
: VertexClientState(glDisableClientState), NormalClientState(glDisableClientState), ColorClientState(glDisableClientState)
{
	using namespace std;
	using namespace std::tr1;

	fill(TexCoordClientState, TexCoordClientState + MAX_TEXTURE_UNIT, glDisableClientState);

	function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunction;
	function<void (GLenum, GLsizei, const GLvoid*)> NormalPointerFunction(glNormalPointer);

	for (unsigned int i=0; i<StreamIndices.size(); ++i)
	{
		const VertexFormat& format = *VertexFormats[i];
		unsigned int offset = 0;
		
		for (unsigned int j=0; j<format.size(); ++j)
		{
			const VertexElement* pElement = &format[j];
			offset = (j == 0)? 0: offset + format[j-1].TypeSize;

			switch (pElement->Usage)
			{
			case GL_VERTEX_ARRAY:
				VertexClientState = glEnableClientState;
				PointerFunction = glVertexPointer;
				m_GLPointerFunctions.push_back(bind(PointerFunction, pElement->Size, pElement->Type, _1, BufferObjectPtr(offset)));
				break;
			case GL_NORMAL_ARRAY:
				NormalClientState = glEnableClientState;
				m_GLPointerFunctions.push_back(bind(NormalPointerFunction, pElement->Type, _1, BufferObjectPtr(offset)));
				break;
			case GL_COLOR_ARRAY:
				ColorClientState = glEnableClientState;
				PointerFunction = glColorPointer;
				m_GLPointerFunctions.push_back(bind(PointerFunction, pElement->Size, pElement->Type, _1, BufferObjectPtr(offset)));
				break;
			case GL_TEXTURE_COORD_ARRAY:
				TexCoordClientState[pElement->UsageIndex] = glEnableClientState;
				PointerFunction = glTexCoordPointer;
				m_TexCoordIndices.push_back(pElement->UsageIndex);
				m_GLTexCoordPointers.push_back(bind(PointerFunction, pElement->Size, pElement->Type, _1, BufferObjectPtr(offset)));
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
VertexShaderFragment::VertexShaderFragment(const std::string& Source) : m_uiGLVertexShader(glCreateShader(GL_VERTEX_SHADER))
{
	const char* pSource[1] = { {Source.c_str()} };
	glShaderSource(m_uiGLVertexShader, 1, pSource, NULL);
	glCompileShader(m_uiGLVertexShader);
	PrintShaderInfoLog(m_uiGLVertexShader);
}
//-----------------------------------------------------------------------------
VertexShaderFragment::~VertexShaderFragment()
{
	glDeleteShader(m_uiGLVertexShader);
}
//-----------------------------------------------------------------------------
VertexShader::VertexShader(const VertexShaderFragmentArray& Fragments) : m_Fragments(Fragments)
{
}
//-----------------------------------------------------------------------------
PixelShaderFragment::PixelShaderFragment(const std::string& Source) : m_uiGLFragmentShader(glCreateShader(GL_FRAGMENT_SHADER))
{
	const char* pSource[1] = { {Source.c_str()} };
	glShaderSource(m_uiGLFragmentShader, 1, pSource, NULL);
	glCompileShader(m_uiGLFragmentShader);
	PrintShaderInfoLog(m_uiGLFragmentShader);
}
//-----------------------------------------------------------------------------
PixelShaderFragment::~PixelShaderFragment()
{
	glDeleteShader(m_uiGLFragmentShader);
}
//-----------------------------------------------------------------------------
PixelShader::PixelShader(const PixelShaderFragmentArray& Fragments) : m_Fragments(Fragments)
{
}
//-----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(VertexShaderPtr pVertexShader, PixelShaderPtr pPixelShader) : m_uiGLShaderProgram(glCreateProgram())
{
	unsigned int n = (unsigned int)pVertexShader->m_Fragments.size();
	for (unsigned int i=0; i<n; ++i)
		glAttachShader(m_uiGLShaderProgram, pVertexShader->m_Fragments[i]->m_uiGLVertexShader);

	n = (unsigned int)pPixelShader->m_Fragments.size();
	for (unsigned int i=0; i<n; ++i)
		glAttachShader(m_uiGLShaderProgram, pPixelShader->m_Fragments[i]->m_uiGLFragmentShader);

	glLinkProgram(m_uiGLShaderProgram);
	PrintProgramInfoLog(m_uiGLShaderProgram);
}
//-----------------------------------------------------------------------------
ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_uiGLShaderProgram);
}
//-----------------------------------------------------------------------------

#endif
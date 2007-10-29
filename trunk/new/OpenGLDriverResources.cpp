#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include "OpenGLDriverResources.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include <numeric>

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
{
	using namespace boost;

	const unsigned int n = accumulate(VertexFormats.begin(), VertexFormats.end(), 0u, lambda::_1 + lambda::bind(&VertexFormat::size, lambda::_2));
	m_ClientStateFunctions.resize(n);
	m_PointerFunctions.resize(n);


	ClientStateFunctionArray::iterator c_itr = m_ClientStateFunctions.begin();
	PointerFunctionArray::iterator p_itr = m_PointerFunctions.begin();
	for (unsigned int i=0; i<StreamIndices.size(); ++i)
	{
		const VertexFormat& format = *VertexFormats[i];
/*		unsigned int offset = 0;
		
		for (unsigned int j=0; j<format.size(); ++j)
		{
			const VertexElement* pElement = &format[j];

			m_ClientStateFunctions.push_back(pElement->ClientState);
			m_PointerFunctions.push_back(pElement->Pointer);
		}
		*/		
		c_itr = transform(format.begin(), format.end(), c_itr, std::mem_fun_ref(&VertexElement::GetClientStateFunction));
		p_itr = transform(format.begin(), format.end(), p_itr, std::mem_fun_ref(&VertexElement::GetPointerFunction));
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
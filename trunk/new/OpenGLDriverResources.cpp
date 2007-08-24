#include "CompileConfig.h"

#ifdef _PIRATE_COMPILE_WITH_OPENGL_

#include "OpenGLDriverResources.h"
#include "VertexFormat.h"

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
: m_pVertex(0), m_pNormal(0), m_pColor(0)
{
	for (unsigned int i=0; i<MAX_TEXTURE_UNIT; ++i)
		m_ppTexCoords[i] = NULL;

	const unsigned int n = (unsigned int)StreamIndices.size();
	for (unsigned int i=0; i<n; ++i)
	{
		const VertexElementArray& VertexFormat = *VertexFormats[i];

		unsigned int NumVertexElements = (unsigned int)VertexFormat.size();
		for (unsigned int j=0; j<NumVertexElements; ++j)
		{
			VertexParam p;
			p.Index = StreamIndices[i];

			const VertexElement* pElement = &VertexFormat[j];
			switch (pElement->Type)
			{
			case DECLTYPE_FLOAT1:
				p.Size = 1;
				p.Type = GL_FLOAT;
				break;
			case DECLTYPE_FLOAT2:
				p.Size = 2;
				p.Type = GL_FLOAT;
				break;
			case DECLTYPE_FLOAT3:
				p.Size = 2;
				p.Type = GL_FLOAT;
				break;
			case DECLTYPE_FLOAT4:
				p.Size = 4;
				p.Type = GL_FLOAT;
				break;
			}

			p.Pointer = BufferObjectPtr(pElement->Offset);

			switch (pElement->Usage)
			{
			case DECLUSAGE_POSITION:
				m_pVertex = new VertexParam(p);
				break;
			case DECLUSAGE_NORMAL:
				m_pNormal = new VertexParam(p);
				break;
			case DECLUSAGE_COLOR:
				m_pColor = new VertexParam(p);
				break;
			case DECLUSAGE_TEXCOORD:
				m_ppTexCoords[pElement->UsageIndex] = new VertexParam(p);
				break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
DriverVertexDeclaration::~DriverVertexDeclaration()
{
	if (m_pVertex) delete m_pVertex;
	if (m_pNormal) delete m_pNormal;
	if (m_pColor) delete m_pColor;
	for (unsigned int i=0; i<MAX_TEXTURE_UNIT; ++i)
		if (m_ppTexCoords[i]) delete m_ppTexCoords[i];
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
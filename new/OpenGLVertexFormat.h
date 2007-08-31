#ifndef _PIRATE_OPENGL_VERTEX_FORMAT_H_
#define _PIRATE_OPENGL_VERTEX_FORMAT_H_

#include "VertexAttribute.h"
#include "VertexFormat.h"
#include "OpenGLWrapper.h"

template<unsigned int NumValues>
struct DeclType<float, NumValues> {
	typedef float type;
	enum { type_id = GL_FLOAT };
	enum { size = NumValues };
};

template<>
struct DeclUsage<VertexUsage::Position, 0> {
	enum { usage = GL_VERTEX_ARRAY };
	enum { index = 0 };
};

template<>
struct DeclUsage<VertexUsage::Normal, 0> {
	enum { usage = GL_VERTEX_ARRAY };
	enum { index = 0 };
};

template<>
struct DeclUsage<VertexUsage::Color, 0> {
	enum { usage = GL_COLOR_ARRAY };
	enum { index = 0 };
};

template<unsigned char UsageIndex>
struct DeclUsage<VertexUsage::TexCoord, UsageIndex> {
	enum { usage = GL_TEXTURE_COORD_ARRAY };
	enum { index = UsageIndex };
};

typedef struct GLVertexElement {
	GLVertexElement() {}
	template<class T>
	GLVertexElement(const T& Attribute)
	:TypeSize(sizeof(T)), Size(T::decl_type::size), Type(T::decl_type::type_id), Usage(T::decl_usage::usage), UsageIndex(T::decl_usage::index)
	{
	}

	unsigned short TypeSize;
	GLint Size;
	GLenum Type;
	GLenum Usage;
	GLint UsageIndex;
} VertexElement;

typedef std::vector<VertexElement> VertexElementArray;

typedef VertexFormat_<GLVertexElement> VertexFormat;

template<class VertexStruct>
VertexFormat& GetVertexFormat()
{
	static VertexFormat_<VertexElement> format;
	format.Init<VertexStruct::type_list>();
	return format;
}

#endif
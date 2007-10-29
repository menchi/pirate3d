#ifndef _PIRATE_OPENGL_VERTEX_FORMAT_H_
#define _PIRATE_OPENGL_VERTEX_FORMAT_H_

#include "VertexAttribute.h"
#include "VertexFormat.h"
#include "OpenGLWrapper.h"
#include "boost/mpl/list.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/pop_front.hpp"
#include "boost/mpl/size.hpp"
#include "boost/tr1/functional.hpp"

template<unsigned int NumValues>
struct DeclType<float, NumValues> {
	typedef float type;
	enum { type_id = GL_FLOAT };
	enum { size = NumValues };
};

template<>
struct DeclUsage<VertexUsage::Position, 0> {
	typedef std::tr1::function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunctionType;
	enum { usage = GL_VERTEX_ARRAY };
	enum { index = 0 };
	static std::tr1::function<void ()> ClientStateFunction()
	{
		using namespace std::tr1;

		function<void (GLenum)> f(glEnableClientState);
		return function<void ()>(bind(f, GL_VERTEX_ARRAY));
	}
	static std::tr1::function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunction()
	{
		using namespace std::tr1;
		return function<void (GLint, GLenum, GLsizei, const GLvoid*)>(glVertexPointer);
	}
};

template<>
struct DeclUsage<VertexUsage::Normal, 0> {
	typedef std::tr1::function<void (GLenum, GLsizei, const GLvoid*)> PointerFunctionType;
	enum { usage = GL_NORMAL_ARRAY };
	enum { index = 0 };
	static std::tr1::function<void ()> ClientStateFunction()
	{
		using namespace std::tr1;

		function<void (GLenum)> f(glEnableClientState);
		return function<void ()>(bind(f, GL_NORMAL_ARRAY));
	}
	static std::tr1::function<void (GLenum, GLsizei, const GLvoid*)> PointerFunction()
	{
		using namespace std::tr1;
		return function<void (GLenum, GLsizei, const GLvoid*)>(glNormalPointer);
	}
};

template<>
struct DeclUsage<VertexUsage::Color, 0> {
	typedef std::tr1::function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunctionType;
	enum { usage = GL_COLOR_ARRAY };
	enum { index = 0 };
	static std::tr1::function<void ()> ClientStateFunction()
	{
		using namespace std::tr1;

		function<void (GLenum)> f(glEnableClientState);
		return function<void ()>(bind(f, GL_COLOR_ARRAY));
	}
	static std::tr1::function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunction()
	{
		using namespace std::tr1;
		return function<void (GLint, GLenum, GLsizei, const GLvoid*)>(glColorPointer);
	}
};

template<unsigned char UsageIndex>
struct DeclUsage<VertexUsage::TexCoord, UsageIndex> {
	typedef std::tr1::function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunctionType;
	enum { usage = GL_TEXTURE_COORD_ARRAY };
	enum { index = UsageIndex };
	static std::tr1::function<void ()> ClientStateFunction()
	{
		using namespace std::tr1;

		function<void (int)> f(EnableClientStateTexCoord);
		return function<void ()>(bind(f, (int)UsageIndex));
	}
	static std::tr1::function<void (GLint, GLenum, GLsizei, const GLvoid*)> PointerFunction()
	{
		using namespace std::tr1;

		function<void (int, GLint, GLenum, GLsizei, const GLvoid*)>(TexCoordPointer_i)
		return bind(TexCoordPointer_i, (int)UsageIndex, _1, _2, _3, _4);
	}
};

typedef struct GLVertexElement {
	typedef std::tr1::function<void ()> ClientStateFunction;
	typedef std::tr1::function<void (GLsizei)> PointerFunction;

	GLVertexElement() {}

	template<class T>
	GLVertexElement(const T& Attribute, unsigned int Offset): ClientState(T::decl_usage::ClientStateFunction()),
	Pointer(std::tr1::bind(T::decl_usage::PointerFunction(), (GLint)T::decl_type::size, (GLenum)T::decl_type::type_id,
			std::tr1::placeholders::_1, BufferObjectPtr(Offset)))
	{
	}

	ClientStateFunction GetClientStateFunction() const { return ClientState; }
	PointerFunction GetPointerFunction() const { return Pointer; }

	ClientStateFunction ClientState;
	PointerFunction Pointer;
} VertexElement;

typedef VertexFormat_<GLVertexElement> VertexFormat;

class VertexFormatConverter {
public:
	VertexFormatConverter() {}

	template<class TList>
	void Convert(VertexElement* pVertexElement)
	{
		Offset = TypeSize = 0;
		ConvertNextVertexElement(TList(), pVertexElement);
	}

private:
	template<class TList>
	void ConvertNextVertexElement(TList TypeList, VertexElement* pVertexElement)
	{
		*pVertexElement = VertexElement(typename front<TList>::type(), Offset);
		Offset += sizeof(front<TList>::type);
		ConvertNextVertexElement(typename boost::mpl::pop_front<TList>::type(), ++pVertexElement);
	}

	void ConvertNextVertexElement(boost::mpl::l_end TypeList, VertexElement* pVertexElement) {}

	unsigned short Offset;
	unsigned short TypeSize;
};

template<class VertexStruct>
VertexFormat& GetVertexFormat()
{
	static VertexFormat format((VertexStruct::type_list()), VertexFormatConverter());
	return format;
}

#endif
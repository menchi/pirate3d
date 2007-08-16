#ifndef _PIRATE_VERTEX_FORMAT_H_
#define _PIRATE_VERTEX_FORMAT_H_

#include "boost/tr1/type_traits.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/pop_front.hpp"
#include "boost/mpl/size.hpp"
#include <stack>

#define TYPE_LIST boost::mpl::list

enum E_VERTEX_USAGE {
	DECLUSAGE_POSITION = 0,
	DECLUSAGE_BLENDWEIGHT,   // 1
	DECLUSAGE_BLENDINDICES,  // 2
	DECLUSAGE_NORMAL,        // 3
	DECLUSAGE_PSIZE,         // 4
	DECLUSAGE_TEXCOORD,      // 5
	DECLUSAGE_TANGENT,       // 6
	DECLUSAGE_BINORMAL,      // 7
	DECLUSAGE_TESSFACTOR,    // 8
	DECLUSAGE_POSITIONT,     // 9
	DECLUSAGE_COLOR,         // 10
};

struct VertexElement {
	unsigned short Offset;
	unsigned char Type;
	unsigned char Usage;
	unsigned char UsageIndex;
};

class VertexFormatHelper {
public:
	static VertexFormatHelper& Instance()
	{
		static VertexFormatHelper helper;
		return helper;
	}

	void push(unsigned int size, unsigned char type, unsigned char usage, unsigned char usageIndex)
	{
		Sizes.push(size);
		Types.push(type);
		Usages.push(usage);
		UsageIndices.push(usageIndex);
	}

	unsigned short pop_size() 
	{
		unsigned short v = Sizes.top();
		Sizes.pop();
		return v; 
	}

	unsigned char pop_type() 
	{
		unsigned char v = Types.top();
		Types.pop();
		return v; 
	}

	unsigned char pop_usage() 
	{
		unsigned char v = Usages.top();
		Usages.pop();
		return v; 
	}

	unsigned char pop_usage_index() 
	{
		unsigned char v = UsageIndices.top();
		UsageIndices.pop();
		return v; 
	}

private:
	VertexFormatHelper() {};
	VertexFormatHelper(const VertexFormatHelper& rhs) {};

	std::stack<unsigned short> Sizes;
	std::stack<unsigned char> Types;
	std::stack<unsigned char> Usages;
	std::stack<unsigned char> UsageIndices;
};

template <class TList>
struct GenScatterHierarchy
	: public boost::mpl::front<TList>::type
	, public GenScatterHierarchy<typename boost::mpl::pop_front<TList>::type>
{
	typedef typename boost::mpl::front<TList>::type ListType;

	enum { Size = sizeof(ListType) };

	GenScatterHierarchy()
	{
		VertexFormatHelper::Instance().push(Size, ListType::Type, ListType::Usage, ListType::UsageIndex);
	}
};

template <>
struct GenScatterHierarchy<boost::mpl::l_end> {};

template <class TList>
struct GenVertexStruct : public GenScatterHierarchy<TList>
{
	GenVertexStruct()
	{
		VertexFormatHelper& helper = VertexFormatHelper::Instance();

		const unsigned int n = boost::mpl::size<TList>::value;
		for (int i=0; i<n; ++i)
		{
			VertexFormat[i].Offset = (i == 0)? 0: (helper.pop_size() + VertexFormat[i-1].Offset);
			VertexFormat[i].Type = helper.pop_type();
			VertexFormat[i].Usage = helper.pop_usage();
			VertexFormat[i].UsageIndex = helper.pop_usage_index();
		}
		helper.pop_size();
	}

	static VertexElement VertexFormat[boost::mpl::size<TList>::value];
};

template <class TList>
VertexElement GenVertexStruct<TList>::VertexFormat[boost::mpl::size<TList>::value];

template <class T, class TList>
T& Field(GenVertexStruct<TList>& obj)
{
	return obj;
}

template <class T, class TList>
const T& Field(const GenVertexStruct<TList>& obj)
{
	return obj;
}

#define CHECK_VERTEX_ATTRIBUTE(T, COUNT, INDEX) BOOST_STATIC_ASSERT \
	(std::tr1::is_floating_point<T>::value \
	&& sizeof(T) == sizeof(float) \
	&& Count > 0 && Count < 5 )

template<typename T = float, unsigned int Count = 3, unsigned int Index = 0>
struct POSITION {
	CHECK_VERTEX_ATTRIBUTE(T, Count, Index);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = DECLUSAGE_POSITION };
	T v[Count];

	T& operator[] (unsigned int i) { return v[i]; }
};

template<typename T = float, unsigned int Count = 3, unsigned int Index = 0>
struct NORMAL {
	CHECK_VERTEX_ATTRIBUTE(T, Count, Index);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = DECLUSAGE_NORMAL };
	T v[Count];

	T& operator[] (unsigned int i) { return v[i]; }
};

template<typename T = float, unsigned int Count = 2, unsigned int Index = 0>
struct TEXCOORD {
	CHECK_VERTEX_ATTRIBUTE(T, Count, Index);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = DECLUSAGE_TEXCOORD };
	T v[Count];

	T& operator[] (unsigned int i) { return v[i]; }
};

template<typename T = float, unsigned int Count = 4, unsigned int Index = 0>
struct COLOR {
	CHECK_VERTEX_ATTRIBUTE(T, Count, Index);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = DECLUSAGE_COLOR };
	T v[Count];

	T& operator[] (unsigned int i) { return v[i]; }
};

#endif
#ifndef _PIRATE_VIDEO_DRIVER_H_
#define _PIRATE_VIDEO_DRIVER_H_

#include "D3D9Driver.h"
#include "OpenGLDriver.h"
#include "boost/tr1/type_traits.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/vector_c.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/pop_front.hpp"
#include "boost/mpl/push_back.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/at.hpp"
#include "boost/foreach.hpp"
#include <vector>
#include <list>

template<typename T, unsigned int Count, unsigned int Index = 0>
struct VTX_ATB_POSITION {
	BOOST_STATIC_ASSERT(std::tr1::is_floating_point<T>::value && sizeof(T) == sizeof(float) && Count > 0 && Count < 5);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
				  (NumElement == 2)? D3DDECLTYPE_FLOAT2:
				  (NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = D3DDECLUSAGE_POSITION };
	typedef T AutoType;
};

template<typename T, unsigned int Count, unsigned int Index = 0>
struct VTX_ATB_NORMAL {
	BOOST_STATIC_ASSERT(std::tr1::is_floating_point<T>::value && sizeof(T) == sizeof(float) && Count > 0 && Count < 5);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
				  (NumElement == 2)? D3DDECLTYPE_FLOAT2:
				  (NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = D3DDECLUSAGE_NORMAL };
	typedef T AutoType;
};

template<typename T, unsigned int Count, unsigned int Index = 0>
struct VTX_ATB_TEXCOORD {
	BOOST_STATIC_ASSERT(std::tr1::is_floating_point<T>::value && sizeof(T) == sizeof(float) && Count > 0 && Count < 5);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
				  (NumElement == 2)? D3DDECLTYPE_FLOAT2:
				  (NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = D3DDECLUSAGE_TEXCOORD };
	typedef T AutoType;
};

struct VertexElement {
	unsigned short Offset;
	unsigned char Type;
	unsigned char Usage;
	unsigned char UsageIndex;
};

template<typename P0, typename P1=void, typename P2=void>
struct VertexFormat {
	typename P0::AutoType _0[P0::NumElement];
	typename P1::AutoType _1[P1::NumElement];
	typename P2::AutoType _2[P2::NumElement];
	static const VertexElement Elements[3];
};

template<typename P0, typename P1, typename P2> const VertexElement VertexFormat<P0, P1, P2>::Elements[3] = 
{ 
	{ 0, P0::Type, P0::Usage, P0::UsageIndex },
	{ sizeof(P0::AutoType)*P0::NumElement, P1::Type, P1::Usage, P1::UsageIndex },
	{ sizeof(P0::AutoType)*P0::NumElement + sizeof(P1::AutoType)*P1::NumElement, P2::Type, P2::Usage, P2::UsageIndex }
};

template<typename P0, typename P1>
struct VertexFormat<P0, P1, void> {
	typename P0::AutoType _0[P0::NumElement];
	typename P1::AutoType _1[P1::NumElement];
	static const VertexElement Elements[2];
};

template<typename P0, typename P1> const VertexElement VertexFormat<P0, P1, void>::Elements[2] = 
{ 
	{ 0, P0::Type, P0::Usage, P0::UsageIndex },
	{ sizeof(P0::AutoType)*P0::NumElement, P1::Type, P1::Usage, P1::UsageIndex }
};

template<typename P0>
struct VertexFormat<P0, void, void> {
	typename P0::AutoType _0[P0::NumElement];
	static const VertexElement Elements[1];
};

template<typename P0> const VertexElement VertexFormat<P0, void, void>::Elements[1] = 
{ 
	{ 0, P0::Type, P0::Usage, P0::UsageIndex }
};

typedef VertexFormat<VTX_ATB_POSITION<float,3>> MyVertex;

extern std::list<unsigned short> TempSizes;
extern std::vector<VertexElement> TempElements;

template <class TList, template <class> class Unit>
struct GenScatterHierarchy
	: public Unit<typename boost::mpl::front<TList>::type>
	, public GenScatterHierarchy<typename boost::mpl::pop_front<TList>::type, Unit>
{
	typedef typename boost::mpl::front<TList>::type ListType;
	typedef Unit<typename boost::mpl::front<TList>::type> LeftType;
	typedef GenScatterHierarchy<typename boost::mpl::pop_front<TList>::type, Unit> RightType;

	enum { Size = sizeof(Unit<typename boost::mpl::front<TList>::type>) };

	typedef typename boost::mpl::push_back<typename RightType::SizeVector, boost::mpl::int_<Size> >::type SizeVector;

	GenScatterHierarchy()
	{
		TempSizes.push_front(Size);
		const VertexElement e = { 0, ListType::Type, ListType::Usage, ListType::UsageIndex};
		TempElements.push_back(e);
/*		Elements[n].Type = LeftType::Type;
		Elements[n].Usage = LeftType::Usage;
		Elements[n].UsageIndex = LeftType::UsageIndex;
*/	}
};

template <template <class> class Unit>
struct GenScatterHierarchy<boost::mpl::l_end, Unit>
{
	typedef boost::mpl::vector_c<int> SizeVector;
	enum { Size = 0 };
};

template <class TList, template <class> class Unit>
struct GenVertexFormat : public GenScatterHierarchy<TList, Unit>
{
	enum { Size = GenScatterHierarchy<TList, Unit>::Size };

	GenVertexFormat()
	{
		const unsigned int n = boost::mpl::size<TList>::value;
		for (int i=0; i<n; ++i)
		{
			Elements[i].Offset = (i == 0)? 0: (TempSizes.front() + Elements[i-1].Offset);
			Elements[i].Type = TempElements[n-i-1].Type;
			Elements[i].Usage = TempElements[n-i-1].Usage;
			Elements[i].UsageIndex = TempElements[n-i-1].UsageIndex;
		}
		TempSizes.clear();
	}

	static VertexElement Elements[boost::mpl::size<TList>::value];
};

template <class T, class TList, template <class> class Unit>
T& Field(GenScatterHierarchy<TList, Unit>& obj)
{
	return static_cast<Unit<T>&>(obj).value_;
}

template <class T> 
struct Holder {
	T value_;
	Holder() {}
	Holder(const T& rhs) { value_ = rhs; }
	operator T() { return value_; }
};

template<typename T, unsigned int Count, unsigned int Index = 0>
struct POSITION {
	BOOST_STATIC_ASSERT(std::tr1::is_floating_point<T>::value && sizeof(T) == sizeof(float) && Count > 0 && Count < 5);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = D3DDECLUSAGE_POSITION };
	T v[Count];
};

template<typename T, unsigned int Count, unsigned int Index = 0>
struct NORMAL {
	BOOST_STATIC_ASSERT(std::tr1::is_floating_point<T>::value && sizeof(T) == sizeof(float) && Count > 0 && Count < 5);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = D3DDECLUSAGE_NORMAL };
	T v[Count];
};

template<typename T, unsigned int Count, unsigned int Index = 0>
struct TEXCOORD {
	BOOST_STATIC_ASSERT(std::tr1::is_floating_point<T>::value && sizeof(T) == sizeof(float) && Count > 0 && Count < 5);
	enum { UsageIndex = Index };
	enum { NumElement = Count };
	enum { Type = (NumElement == 1)? D3DDECLTYPE_FLOAT1: 
		(NumElement == 2)? D3DDECLTYPE_FLOAT2:
		(NumElement == 3)? D3DDECLTYPE_FLOAT3: D3DDECLTYPE_FLOAT4};
	enum { Usage = D3DDECLUSAGE_TEXCOORD };
	T v[Count];
};

template <class TList, template <class> class Unit>
VertexElement GenVertexFormat<TList, Unit>::Elements[boost::mpl::size<TList>::value];

template <unsigned int Index, class TList, template <class> class Unit>
void GetVertexFormatDescription(GenScatterHierarchy<TList, Unit> format, VertexElement& e)
{
	e.Offset = 0;
	e.Type = boost::mpl::front<TList>::type::Type;
	e.Usage = boost::mpl::front<TList>::type::Usage;
	e.UsageIndex = boost::mpl::front<TList>::type::UsageIndex;
}

class PaintTool
{
public:
	virtual void Use(VideoDriverPtr pDriver) const = 0;
};

#endif
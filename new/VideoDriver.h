#ifndef _PIRATE_VIDEO_DRIVER_H_
#define _PIRATE_VIDEO_DRIVER_H_

#include "D3D9Driver.h"
#include "OpenGLDriver.h"
#include "boost/tr1/type_traits.hpp"

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

class PaintTool
{
public:
	virtual void Use(VideoDriverPtr pDriver) const = 0;
};

#endif
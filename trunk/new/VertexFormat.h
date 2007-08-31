#ifndef _PIRATE_VERTEX_FORMAT_H_
#define _PIRATE_VERTEX_FORMAT_H_

#include "boost/mpl/list.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/pop_front.hpp"
#include "boost/mpl/size.hpp"
#include <vector>

template<class VertexElementType>
struct VertexFormat_ {
	typedef typename std::vector<VertexElementType> VertexElementArray;

	VertexFormat_() {}

	template<class TList>
	void Init()
	{
		m_VertexElements.resize(boost::mpl::size<TList>::value);
		GetVertexFormatFromTypeList(TList(), &m_VertexElements[0]);
	}

	typename VertexElementArray::size_type size() const 
	{ 
		return m_VertexElements.size(); 
	}

	const VertexElementType& operator[] (unsigned int i) const
	{
		return m_VertexElements[i];
	}

	VertexElementArray m_VertexElements;
};

template<class TList, class VertexElementType> 
void GetVertexFormatFromTypeList(TList TypeList, VertexElementType* pVertexElement)
{
	*pVertexElement = VertexElementType(typename front<TList>::type());
	GetVertexFormatFromTypeList(typename boost::mpl::pop_front<TList>::type(), ++pVertexElement);
}

template<class VertexElementType> 
void GetVertexFormatFromTypeList(boost::mpl::l_end TypeList, VertexElementType* pVertexElement) {}

#endif
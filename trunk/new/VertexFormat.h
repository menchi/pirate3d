#ifndef _PIRATE_VERTEX_FORMAT_H_
#define _PIRATE_VERTEX_FORMAT_H_

#include <vector>

template<class VertexElementType>
struct VertexFormat_ {
	typedef typename std::vector<VertexElementType> VertexElementArray;

	template<class TList, class ConverterType>
	VertexFormat_(TList TypeList, ConverterType& Converter) 
	{
		m_VertexElements.resize(boost::mpl::size<TList>::value);
		Converter.Convert<TList>(&m_VertexElements[0]);
	}

	unsigned int size() const 
	{ 
		return (unsigned int)m_VertexElements.size(); 
	}

	typename VertexElementArray::const_iterator begin() const
	{ 
		return m_VertexElements.begin();
	}

	typename VertexElementArray::const_iterator end() const
	{ 
		return m_VertexElements.end();
	}

	const VertexElementType& operator[] (unsigned int i) const
	{
		return m_VertexElements[i];
	}

	VertexElementArray m_VertexElements;
};

#endif
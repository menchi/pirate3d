#ifndef _PIRATE_HIERARCHY_GENERATOR_H_
#define _PIRATE_HIERARCHY_GENERATOR_H_

#define TYPE_LIST boost::mpl::list

#include "boost/mpl/list.hpp"
#include "boost/mpl/front.hpp"
#include "boost/mpl/pop_front.hpp"

template <class TList>
struct GenScatterHierarchy
	: public boost::mpl::front<TList>::type
	, public GenScatterHierarchy<typename boost::mpl::pop_front<TList>::type>
{
};

template <>
struct GenScatterHierarchy<boost::mpl::l_end> {};

template <class TList>
struct GenVertexStruct : public GenScatterHierarchy<TList>
{
	typedef TList type_list;
	GenVertexStruct() {};
};

template <class T, class TList>
T& field(GenVertexStruct<TList>& obj)
{
	return obj;
}

template <class T, class TList>
const T& field(const GenVertexStruct<TList>& obj)
{
	return obj;
}

#endif
#ifndef _PIRATE_SMART_POINTER_H_
#define _PIRATE_SMART_POINTER_H_

#include "boost/intrusive_ptr.hpp"
#include "boost/tr1/memory.hpp"
#include "boost/tr1/type_traits.hpp"

#define DEFINE_SHARED_PTR(T) typedef SmartPtrDefiner<T>::shared_ptr_type T##Ptr;
#define DEFINE_WEAK_PTR(T) typedef SmartPtrDefiner<T>::weak_ptr_type T##WeakPtr;
#define DEFINE_SMART_PTR(T) DEFINE_SHARED_PTR(T) DEFINE_WEAK_PTR(T)
#define TYPE_DEFINE_WITH_SMART_PTR(T, U) typedef T U; DEFINE_SMART_PTR(U)

#define FWD_DECLARE(T) class T; DEFINE_SMART_PTR(T)

template<class T, bool IsCOM>
struct SmartPtrTraits{};

template<class T>
struct SmartPtrTraits<T, true> {
	typedef boost::intrusive_ptr<T> shared_ptr_type;
	typedef T* weak_ptr_type;
};

template<class T>
struct SmartPtrTraits<T, false> {
	typedef std::tr1::shared_ptr<T> shared_ptr_type;
	typedef std::tr1::weak_ptr<T> weak_ptr_type;
};

template<class T>
struct SmartPtrDefiner
{
	typedef typename SmartPtrTraits<T, std::tr1::is_base_of<IUnknown, T>::value>::shared_ptr_type shared_ptr_type;
	typedef typename SmartPtrTraits<T, std::tr1::is_base_of<IUnknown, T>::value>::weak_ptr_type weak_ptr_type;
};

template<class T> void intrusive_ptr_add_ref(T* p)
{
	p->AddRef();
}

template<class T> void intrusive_ptr_release(T* p)
{
	p->Release();
}

#endif
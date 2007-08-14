#ifndef _PIRATE_SMART_POINTER_H_
#define _PIRATE_SMART_POINTER_H_

#include "boost/tr1/memory.hpp"

#define DEFINE_SHARED_PTR(T) typedef std::tr1::shared_ptr<T> T##Ptr;
#define DEFINE_WEAK_PTR(T) typedef std::tr1::weak_ptr<T> T##WeakPtr;
#define DEFINE_SMART_PTR(T) DEFINE_SHARED_PTR(T) DEFINE_WEAK_PTR(T)
#define TYPE_DEFINE_WITH_SMART_PTR(T, U) typedef T U; DEFINE_SMART_PTR(U)

#define FWD_DECLARE(T) class T; DEFINE_SMART_PTR(T)

#endif
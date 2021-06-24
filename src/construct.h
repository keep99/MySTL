#ifndef TOYSTL_SRC_CONSTRUCT_H_
#define TOYSTL_SRC_CONSTRUCT_H_

#include <new>  // placement new

#include "iterator_base.h"
#include "type_traits.h"
#include "utility.h"

namespace toystl {
template <class T1, class T2>
void construct(T1* ptr, const T2& value) {
  // placement new
  ::new ((void*)ptr) T1(value);
}

// 可变参数模板
template <class T, class... Args>
void construct(T* ptr, Args&&... args) {
  ::new ((void*)ptr) T(toystl::forward<Args>(args)...);
}

template <class T>
void construct(T* ptr) {
  ::new ((void*)ptr) T();
}

template <class T>
void destroy(T* ptr) {
  ptr->~T();
}

//如果元素的数值型别有 无用的 析构函数，什么也不做，
template <class ForwardIterator>
void _destroy_aux(ForwardIterator first, ForwardIterator last, true_type) {}

//如果元素的数值型别有 有用的 析构函数
template <class ForwardIterator>
void _destroy_aux(ForwardIterator first, ForwardIterator last, false_type) {
  for (; first != last; first++) {
    destroy(&*first);
  }
}

// 针对迭代器的 destroy 版本
template <class ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last) {
  using valueType = typename iterator_traits<ForwardIterator>::value_type;
  using trivialDestructor =
      typename __type_traits<valueType>::has_trivial_destructor;
  _destroy_aux(first, last, trivialDestructor());
}

// 针对char* wchar_t*的特化版本，不做任何操作
void destroy(char*, char*) {}
void destroy(wchar_t*, wchar_t*) {}
}  // namespace toystl

#endif  // TOYSTL_SRC_CONSTRUCT_H_
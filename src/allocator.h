#ifndef TOYSTL_SRC_ALLOCATOR_H_
#define TOYSTL_SRC_ALLOCATOR_H_

#include <cstddef>

#include "alloc.h"
#include "construct.h"
#include "utility.h"

namespace toystl {
template <class T>
class allocator {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <class U>
  class rebind {
   public:
    using other = allocator<U>;
  };

  static pointer allocate() {
    return static_cast<pointer>(alloc::allocate(sizeof(T)));
    // return static_cast<T*>(::operator new(sizeof(T)));
  }

  static pointer allocate(size_type n) {
    if (n == 0) {
      return nullptr;
    }

    /* SGI 默认使用第二级空间配置器 */
    pointer tmp = static_cast<pointer>(alloc::allocate(sizeof(T) * n));

    return tmp;
    // if (n == 0)
    //     return nullptr;
    // return static_cast<T*>(::operator new(n * sizeof(T)));
  }

  static void deallocate(pointer p) {
    if (p == nullptr) {
      return;
    }
    alloc::deallocate(static_cast<void*>(p), sizeof(T));
    // if (p == nullptr)
    //     return;
    // ::operator delete(p);
  }

  static void deallocate(pointer p, size_type n) {
    if (p == nullptr) {
      return;
    }

    if (n != 0) {
      alloc::deallocate(static_cast<void*>(p), sizeof(T) * n);
    }
    // if (p == nullptr)
    //     return;
    // ::operator delete(p);
  }

  static void construct(pointer ptr, const_reference value) {
    toystl::construct(ptr, value);
  }

  static void construct(pointer ptr, T&& value) {
    toystl::construct(ptr, toystl::move(value));
  }

  template <class... Args>
  static void construct(pointer ptr, Args&&... args) {
    toystl::construct(ptr, toystl::forward<Args>(args)...);
  }

  static void construct(pointer ptr) { toystl::construct(ptr); }

  static void destroy(T* ptr) { toystl::destroy(ptr); }

  static void destroy(T* first, T* last) {
    while (first != last) {
      toystl::destroy(first, last);
      first++;
    }
  }
};
}  // namespace toystl

#endif  // TOYSTL_SRC_ALLOCATOR_H_
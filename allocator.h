#ifndef _ALLOCATOR_H
#define _ALLOCATIR_H

// 为 alloc 类封装接口
#include "alloc.h"
#include "construct.h"

namespace MySTL {
    template<class T, class Alloc>
    class simple_alloc {
    public:
        static T* allocate(size_t n) {
            return 0 == n ? nullptr : (T*)Alloc::allocate(n * sizeof(T));
        }
        static T* allocate(void) {
            return (T*)Alloc::allocate(n * sizeof(T));
        }
        static void deallocate(T* p, size_t n) {
            if(n != 0) {
                Alloc::deallocate(p, n * sizeof(T));
            }
        }
        static void deallocate(T*) {
            Alloc::deallocate(p, sizeof(T));
        }
    }
}

#endif
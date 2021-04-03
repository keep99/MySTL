/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-02 23:56:23
 * @LastEditTime: 2021-04-03 15:40:16
 * @LastEditors: Chen.Yu
 */
#ifndef _ALLOCATOR_H
#define _ALLOCATIR_H

// 为 alloc 类封装接口
#include "alloc.h"
#include "construct.h"

#include <cstddef>

namespace MySTL {
    template<class T>
    class allocator {
    public:
        using value_type        = T;
        using pointer           = T*;
        using const_pointer     = const T*;
        using reference         = T&;
        using const_reference   = const T&;
        using size_type         = std::size_t;
        using difference_type   = std::ptrdiff_t;


        template <class U>
        class rebind {
            using other = allocator<U>;
        };

        static pointer allocate() {
            return allocator::allocate(1);
        }

        static pointer allocate(size_type n) {
            if(n == 0) {
                return 0;
            }

            //直接默认调用第二级空间配置器了？
            return static_cast<pointer>(alloc::allocate(sizeof(T) * n));
        }

        static void deallocate(pointer ptr) {
            allocator::deallocate(ptr, 1);
        }

        static void deallocate(pointer p, size_type n) {
            if(n != 0) {
                alloc::deallocate(static_cast<void*>(ptr), n * sizeof(T));
            }
        }

        static void construct(pointer ptr, const_reference value) {
            MySTL::construct(ptr, value);
        }
        
        template <class ...Args>
        inline void construct(pointer* ptr, Args&&... args) {
            MySTL::construct(ptr, MySTL::forward<Args>(args)...);
        }

        static void construct(pointer ptr) {
            MySTL::construct(ptr);
        }

        static void destroy(pointer ptr) {
            ptr->~T();
        }

        static void destory(pointer first, pointer last) {
            while(first != last) {
                destroy(&*first);
                first++:
            }
        }
    };
}

#endif
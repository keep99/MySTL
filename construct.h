/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-02 21:56:56
 * @LastEditTime: 2021-05-11 01:25:58
 * @LastEditors: Chen.Yu
 */
#ifndef _CONSTRUCT_H
#define _CONSTRUCT_H

#include <new> //placement new

#include "type_traits.h"

namespace MySTL {
    template <class T1, class T2>
    inline void construct(T1* ptr1, const T2& value) {
        //placement new
        new(ptr1) T1(value);
    }

    // 可变参数模板
    template <class T, class ... Args>
    inline void construct(T* ptr, Args&&... args) {
        ::new(ptr) T(MySTL::forward<Args>(args)...);
    }

    template <class T>
    inline void construct(T* ptr) {
        ::new(ptr) T();
    }

    template <class T>
    inline void destroy(T* ptr) {
        ptr->~T();
    }

    //如果元素的数值型别有 无用的 析构函数，什么也不做，
    template <class ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, true_type) {

    }

    //如果元素的数值型别有 有用的 析构函数
    template<class ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, false_type) {
        for(; first != last; first++) {
            destroy(&*first);
        }
    }    

    // 针对迭代器的 destroy 版本
    template<class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        using valueType = typename iterator_traits<ForwardIterator>::value_type;
        using trivialDestructor = typename __type_traits<valueType>::has_trivial_destructor;
        _destroy_aux(first, last, trivialDestructor());
    }

    // 针对char* wchar_t*的特化版本，不做任何操作
   inline void destroy(char*, char*) {}
   inline void destroy(wchar_t*, wchar_t*) {}

}
#endif
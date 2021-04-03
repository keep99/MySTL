/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-02 15:29:19
 * @LastEditTime: 2021-04-03 15:07:00
 * @LastEditors: Chen.Yu
 */
#ifndef _UTILITY_MOVE_FORWARD_H
#define _UTILITY_MOVE_FORWARD_H

#include "type_traits.h"

namespace MySTL {
    // forward 和 move 的实现参考 C++ Primer P610 - 614
    // 关键技术：引用折叠 & + & = &; && + & = &; & + && = &; && + && = &&。

    // forward ———— 完美转发。完美转发实现了参数在传递过程中保持其值属性的功能，即若是左值，则传递之后仍然是左值，若是右值，则传递之后仍然是右值。
    // forward执行到右值的有条件转换(传递右值，返回右值引用；传递左值，返回左值引用) 
    
    // 参数如果是左值，传入此模板函数 
    template <class T>
    T&& forward(typename MySTL::remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }

    // 参数如果是右值，传入此模板函数
    template <class T>
    T&& forward(typename MySTL::remove_reference<T>::type&& t) noexcept {
        return static_cast<T&&>(t);
    }

    // move
    // move执行到右值的无条件转换(不管传递左值还是右值，最后都返回右值引用)
    template <class T>
    typename MySTL::remove_reference<T>::type&& move(T&& t) noexcept {
        using return_type = typename MySTL::remove_reference<T>::type&&;
        return static_cast<return_type>(t);
    }

    //swap
    template <class T>
    void swap(T& lhs, T& rhs) {
        auto tmp(MySTL::move(lhs));
        lhs = MySTL::move(rhs);
        rhs = MySTL::move(tmp);
    }

    template <class ForwardIterator1, class ForwardIterator2> 
    ForwardIterator2 swap_range(ForwardItertor1 first1, ForwardIterator1 last1, ForwardIterator2 first2) {
        for(; first1 != last1; first1++, first2++) {
            MySTL::swap(*first1, *first2);
        }

        return first2;
    }

    template <class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N]) {
        MySTL::swap_range(a, a + N, b);
    }
} 

#endif




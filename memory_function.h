/*
 * @Description: 内存的基本处理工具 《STL源码剖析》P70。这些内存处理函数具有 "commit or rollback" 语意。
 * @Author: Chen.Yu
 * @Date: 2021-04-02 23:29:46
 * @LastEditTime: 2021-04-12 20:58:50
 * @LastEditors: Chen.Yu
 */
#ifndef _MEMORY_FUNCTION_H
#define _MEMORY_FUNCTION_H

#include "iterator_base.h" 
#include "construct.h"
#include "type_traits.h"

#include <string.h>
#include <new>

namespace MySTL {
    /* 主要包括 uninitialized_copy uninitialized_copy_n
     * uninitialized_fill uninitialized_fill_n 
     * uninitialized_move uninitialized_move_n */

    // 获取对象或函数 arg 的地址
    template <class T>
    T* addressof(T& arg) noexcept {
        return reinterpret_cast<T*> (
            &const_cast<char&>(
                reinterpret_cast<const volatile char&>(arg)
            )
        );
    }

    // TO DO : fixed
    // 下面这四个函数能直接这样写？copy 的话，需要考虑 元素是否是 POD
    // 是POD，直接 copy就好了
    // 不是POD，需要调用 construct

    /***********************************************************************************/
    // uninitialized_copy
    // 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
    /***********************************************************************************/
    // POD版本
    // 如果是 POD 型别，执行流程会转入以下函数，这是通过 函数模板的参数推导机制而得的。
    template <class InputIterator, class ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, true_type) {
        return MySTL::copy(first, last, result);
    }

    // 非POD版本
    // commit or rollback
    template <class InputIterator, class ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, false_type) {
        auto cur = result;
        try 
        {
            for (; first != last; ++first, ++cur) 
            {
                MySTL::construct(&*cur, *first);
            }
        }
        catch (...) 
        {
            for(; result != cur; ++result) 
            {
                MySTL::destroy(&*result);
            }
        }

        return cur;
    }

    template <class InputIterator, class ForwardIterator, class T, class T1>
    ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T1*) {
        using isPOD = typename __type_traits<T1>::is_POD_type;
        return __uninitialized_copy_aux(first, last, result, isPOD());
    }


    template <class InputIterator, class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        return __uninitialized_copy(first, last, result, value_type(result));
    }

    // 针对char* 和 wchar_t* 两种型别，可以采用最具效率的做法，memmove（直接移动内存内容）来执行复制行为。
    inline char* uninitialized_copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }


    /***********************************************************************************/
    // uninitialized_copy_n 
    // 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
    /***********************************************************************************/
    template <class RandomAccessIter, class Size, class ForwardIter>
    ForwardIter __uninitialized_copy_n(RandomAccessIter first, Size n, ForwardIter result, random_access_iterator_tag) {
        return uninitialized_copy(first, first + n, result);
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter __uninitialized_copy_n(InputIter first, Size n, ForwardIter result, input_iterator_tag) {
        ForwardIter cur = result;
        try
        {
            for(; n > 0; --n, ++first, ++cur) {
                MySTL::construct(&*cur, &*first);
                return cur;
            }
        }
        catch(...)
        {
            for(; result != cur; ++result) 
            {
                MySTL::destroy(&*result);
            }
        } 
    }

    template <class ForwardIterator, class Size, class T>
    void uninitialized_fill_n(ForwardIterator first, Size count, const T& value) {
        using Value = typename tinySTL::iterator_traits<ForwardIterator>::value_type;
        ForwardIterator current = first;
        try {
            while (count-- > 0) {
                ::new (static_cast<void*>(*current)) Value(value);

                ++current;
            }
        } catch (...) {
            while (first != current) {
                first->~Value();
            }
            throw;
        }
    }


    /***********************************************************************************/
    // uninitialized_fill
    // 在 [first, last) 区间内填充元素值
    /***********************************************************************************/
    template <class ForwardIter, class T>
    void __uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T& value, true_type) {
        MySTL::fill(first, last, value);
    }

    template <class ForwardIter, class T>
    void __uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T& value, false_type) {
        ForwardIter cur = first;
        try
        {
            for(; cur != last; ++cur) {
                MySTL::construct(&*cur, &*first);
            }
        }
        catch(...)
        {
            for(; first != cur; ++first) 
            {
                MySTL::destroy(&*first);
            }
        }
    }

    template <class ForwardIter, class T, class T1>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value, T1*) {
        using is_POD = typename __type_traits<T1>::is_POD_type;
        __uninitialized_fill_aux(first, last, value, is_POD());
    }

    template <class ForwardIter, class T>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value) {
        return __uninitialized_fill(first, last, value, value_type(first));
    }

    
    /***********************************************************************************/
    // uninitialized_fill_n
    // 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
    /***********************************************************************************/
    template <class ForwardIter, class Size, class T>
    ForwardIter __uninitialized_fill_n_aux(ForwardIter first, Size n, const T& value, true_type) {
        return MySTL::fill_n(first, n, value);
    }

    template <class ForwardIter, class Size, class T>
    ForwardIter __uninitialized_fill_n_aux(ForwardIter first, Size n, const T& value, false_type) {
        ForwardIter cur = first;
        try
        {
            for(; n > 0; n--, ++cur) {
                MySTL::construct(&*cur, value);
            }
        }
        catch(...)
        {
            for(; first != cur; ++first) 
            {
                MySTL::destroy(&*first);
            }
        }

        return cur;
    }


    template <class ForwardIter, class Size, class T, class T1>
    ForwardIter __uninitialized_fill_n(ForwardIter first, Size n, const T& value, T1*) {
        using is_POD = typename __type_traits<T1>::is_POD_type;
        __uninitialized_fill_n_aux(first, n, value, is_POD());
    }
    

    // 迭代器 first 指向欲初始化空间的起始处
    // n 表示欲初始化空间的大小
    // value 表示初值
    template <class ForwardIter, class Size, class T>
    ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value) {
        return __uninitialized_fill_n(first, n, value, value_type(first));
    }
} 
#endif
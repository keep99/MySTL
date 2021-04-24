/*
 * @Description: 基本的算法
 * @Author: Chen.Yu
 * @Date: 2021-04-12 17:09:22
 * @LastEditTime: 2021-04-24 18:27:09
 * @LastEditors: Chen.Yu
 */
#ifndef _ALGOBASE_H_
#define _ALGOBASE_H_
#include "iterator_base.h"
// #include "iterator.h"
#include "utility.h"
#include "type_traits.h"

#include <string.h>


namespace MySTL {
    /***********************************************************************************/
    // max
    // 取二者中的较大值，语义相等时保证返回第一个参数
    /***********************************************************************************/  
    template <class T>
    inline const T& max(const T& lhs, const T& rhs) {
        return lhs < rhs ? rhs : lhs;
    }

    /* 重载版本使用函数对象 com 进行比较 */
    template <class T, class Compare>
    inline const T& max(const T& lhs, const T& rhs, Compare com) {
        return com(lhs, rhs) ? rhs : lhs;
    }


    /***********************************************************************************/
    // min
    // 取二者中的较大值，语义相等时保证返回第一个参数
    /***********************************************************************************/  
    template <class T>
    inline const T& max(const T& lhs, const T& rhs) {
        return rhs < lhs ? rhs : lhs;
    }

    /* 重载版本使用函数对象 com 进行比较 */
    template <class T, class Compare>
    inline const T& max(const T& lhs, const T& rhs, Compare com) {
        return com(rhs, lhs) ? rhs : lhs;
    }


    /*****************************************************************************************/
    // iter_swap
    // 将两个迭代器所指对象对调
    /*****************************************************************************************/
    template <class FIter1, class FIter2, class T>
    inline void __iter_swap(FIter1 a, FIter1 b, T*) {
        T tmp = *a;
        *a = *b;
        *b = tmp;
    }

    template <class FIter1, class FIter2>
    inline void iter_swap(FIter1 lhs, FIter2 rhs)
    {
        __iter_swap(lhs, rhs, value_type(lhs));
    }


    /***********************************************************************************/
    // copy
    // 把 [first, last) 区间内的元素拷贝到 [result, result + (last - first)) 内
    // 具体实现看 《STL 源码剖析》 P314
    /***********************************************************************************/
    // 1、1完全泛化版本
    template <class InputIter, class OutputIter>
    struct __copy_dispatch {
        OutputIter operator()(InputIter first, InputIter last, OutputIter result) {
            return __copy(first, last, result, iterator_category(first));
        }
    };
    /***********************************************************************************/
    template <class InputIter, class OutputIter>
    inline OutputIter __copy(InputIter first, InputIter last, OutputIter result, input_iterator_tag) {
        // 以迭代器等同与否，决定循环是否继续。速度慢
        for(; first != last; ++first) {
            *result = *first;
        }

        return result;
    }

    template <class RandomIter, class OutputIter>
    inline OutputIter __copy(RandomIter first, RandomIter last, OutputIter result, random_access_iterator_tag) {
        return __copy_d(first, last, result, distance_type(first));
    }

    template <class RandomIter, class OutputIter, class Distance> 
    inline OutputIter __copy_d(RandomIter first, RandomIter last, OutputIter result, Distance*) {
        for(Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }

        return result;
    }
    /***********************************************************************************/


    // 1、2偏特化版本（1），两个参数都是 T* 指针形式
    template <class T>
    struct __copy_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator; 
            return __copy_t(first, last, result, t());
        }
    };
    // 1、3偏特化版本（2），第一个参数是 const T*，第二个参数是 T*
    template <class T>
    struct __copy_dispatch<T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator; 
            return __copy_t(first, last, result, t());
        }
    };
    /***********************************************************************************/
    // 以下版本适合于 "指针所指对象具备 trival assignment operator"
    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }
    // 以下版本适合于 "指针所指对象具备 non-trival assignment operator"
    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, false_type) {
        // 指针毕竟是一种 random_access_iterator_tag，所以交给 __copy_d 来处理
        return __copy_d(first, last, result, (ptrdiff_t*)(0));
    }
    /***********************************************************************************/


    // 3、特殊版本（1），重载形式
    /* memmove : 由src所指内存区域复制count个字节到dest所指内存区域 */
    inline char* copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    // 2、特殊版本（2），重载形式
    inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    // 1、完全泛化版本
    template <class InputIter, class OutputIter>
    inline OutputIter copy(InputIter first, InputIter last, OutputIter result) {
        return __copy_dispatch<InputIter, OutputIter>()(first, last, result);
    }


    /*****************************************************************************************/
    // copy_backward
    // 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
    // copy_backward 具体的实现过程参考 https://blog.csdn.net/dongyu_1989/article/details/80061944
    /*****************************************************************************************/
    // 底层的实现1
    template <class BidirectionalIter1, class BidirectionalIter2>
    inline BidirectionalIter2 __copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        while (first != last) {
            *--result = *--last;
            return result;
        }
    }

    // 底层的实现2
    // 有“无关紧要的赋值操作符” 会执行下面这个函数
    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, true_type) {
        const ptrdiff_t N = last - first;
        memmove(result - N, first, sizeof(T) * N);
        return result - N;
    }

    // 没有“无关紧要的赋值操作符” 会执行下面这个函数
    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, true_type) {
        return __copy_backward(first, last, result);
    }

    // __copy_backward_dispatch的泛化版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    struct __copy_backward_dispatch {
        BidirectionalIter2 operator()(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
            return __copy_backward(first, last, result);
        }
    };

    // __copy_backward_dispatch的特化版本1
    template <class T>
    struct __copy_backward_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator;
            return __copy_backward_t(first, last, result, t());
        }
    };

    // __copy_backward_dispatch的特化版本2
    template <class T>
    struct __copy_backward_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator;
            return __copy_backward_t(first, last, result, t());
        }
    };

    // 1、完全泛化版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    inline BidirectionalIter2 copy(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return __copy_backward_dispatch<BidirectionalIter1, BidirectionalIter2>()(first, last, result);
    }

    


} // namespace MySTL



#endif



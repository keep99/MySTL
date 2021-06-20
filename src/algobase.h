#ifndef _ALGOBASE_H_
#define _ALGOBASE_H_

#include "iterator_base.h"
#include "utility.h"
#include "type_traits.h"

#include <string.h>


namespace MySTL {
    /***********************************************************************************/
    // max
    // 取二者中的较大值，语义相等时保证返回第一个参数
    /***********************************************************************************/  
    template <class T>
    const T& max(const T& lhs, const T& rhs) {
        return lhs < rhs ? rhs : lhs;
    }

    /* 重载版本使用函数对象 com 进行比较 */
    template <class T, class Compare>
    const T& max(const T& lhs, const T& rhs, Compare com) {
        return com(lhs, rhs) ? rhs : lhs;
    }


    /***********************************************************************************/
    // min
    // 取二者中的较小值，语义相等时保证返回第一个参数
    /***********************************************************************************/  
    template <class T>
    const T& min(const T& lhs, const T& rhs) {
        return rhs < lhs ? rhs : lhs;
    }

    /* 重载版本使用函数对象 com 进行比较 */
    template <class T, class Compare>
    const T& min(const T& lhs, const T& rhs, Compare com) {
        return com(rhs, lhs) ? rhs : lhs;
    }


    /*****************************************************************************************/
    // iter_swap
    // 将两个迭代器所指对象对调
    /*****************************************************************************************/
    template <class FIter1, class FIter2, class T>
    void __iter_swap(FIter1 a, FIter2 b, T*) {
        T tmp = *a;
        *a = *b;
        *b = tmp;
    }

    template <class FIter1, class FIter2>
    void iter_swap(FIter1 lhs, FIter2 rhs)
    {
        __iter_swap(lhs, rhs, value_type(lhs));
    }


    /***********************************************************************************/
    // copy
    // 把 [first, last) 区间内的元素拷贝到 [result, result + (last - first)) 内
    // 具体实现看 《STL 源码剖析》 P314
    /***********************************************************************************/
    template <class RandomIter, class OutputIter, class Distance> 
    OutputIter __copy_d(RandomIter first, RandomIter last, OutputIter result, Distance*) {
        for(Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }

        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter __copy(InputIter first, InputIter last, OutputIter result, input_iterator_tag) {
        // 以迭代器等同与否，决定循环是否继续。速度慢
        for(; first != last; ++first) {
            *result = *first;
        }

        return result;
    }

    template <class RandomIter, class OutputIter>
    OutputIter __copy(RandomIter first, RandomIter last, OutputIter result, random_access_iterator_tag) {
        return __copy_d(first, last, result, distance_type(first));
    }

    // 1、1完全泛化版本
    template <class InputIter, class OutputIter>
    struct __copy_dispatch {
        OutputIter operator()(InputIter first, InputIter last, OutputIter result) {
            return __copy(first, last, result, iterator_category(first));
        }
    };

    /***********************************************************************************/
    // 以下版本适合于 "指针所指对象具备 trival assignment operator"
    template <class T>
    T* __copy_t(const T* first, const T* last, T* result, true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }
    // 以下版本适合于 "指针所指对象具备 non-trival assignment operator"
    template <class T>
    T* __copy_t(const T* first, const T* last, T* result, false_type) {
        // 指针毕竟是一种 random_access_iterator_tag，所以交给 __copy_d 来处理
        return __copy_d(first, last, result, (ptrdiff_t*)(0));
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
    struct __copy_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator; 
            return __copy_t(first, last, result, t());
        }
    };
    /***********************************************************************************/
    // 3、特殊版本（1），重载形式
    /* memmove : 由src所指内存区域复制count个字节到dest所指内存区域 */
    char* copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    // 2、特殊版本（2），重载形式
    wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    // 1、完全泛化版本
    template <class InputIter, class OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result) {
        return __copy_dispatch<InputIter, OutputIter>()(first, last, result);
    }


    /*****************************************************************************************/
    // copy_backward
    // 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
    // copy_backward 具体的实现过程参考 https://blog.csdn.net/dongyu_1989/article/details/80061944
    /*****************************************************************************************/
    // 底层的实现1
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 __copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        while (first != last) {
            *--result = *--last;
        }

        return result;
    }

    // 底层的实现2
    // 有“无关紧要的赋值操作符” 会执行下面这个函数
    template <class T>
    T* __copy_backward_t(const T* first, const T* last, T* result, true_type) {
        const ptrdiff_t N = last - first;
        memmove(result - N, first, sizeof(T) * N);
        return result - N;
    }

    // 没有“无关紧要的赋值操作符” 会执行下面这个函数
    template <class T>
    T* __copy_backward_t(const T* first, const T* last, T* result, false_type) {
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
    BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return __copy_backward_dispatch<BidirectionalIter1, BidirectionalIter2>()(first, last, result);
    }


    /*****************************************************************************************/
    // copy_if
    // 把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
    /*****************************************************************************************/
    template <class InputIterator, class OutputIterator, class UnaryPredicate>
    OutputIterator copy_if(InputIterator first, InputIterator last, OutputIterator result, UnaryPredicate unary_pred) {
        for(; first != last; ++first) {
            if(unary_pred(*first)) {
                *result++ = *first;
            }
        }

        return result;
    }

    /*****************************************************************************************/
    // copy_n
    // 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
    // 返回一个 pair 分别指向拷贝结束的尾部
    /*****************************************************************************************/
    template <class InputIterator, class Size, class OutputIterator>
    MySTL::pair<InputIterator, OutputIterator> __copy_n(InputIterator first, Size n, OutputIterator result, input_iterator_tag) {
        for(; n > 0; --n, ++first, ++result) {
            *result = *first;
        }

        return MySTL::pair<InputIterator, OutputIterator>(first, result);
    }

    template <class InputIterator, class Size, class OutputIterator>
    MySTL::pair<InputIterator, OutputIterator> __copy_n(InputIterator first, Size n, OutputIterator result, random_access_iterator_tag) {
        auto last = first + n;
        return pair<InputIterator, OutputIterator>(last, MySTL::copy(first, last, result));
    }
    
    template <class InuputIterator, class Size, class OutputIterator>
    MySTL::pair<InuputIterator, OutputIterator> copy_n(InuputIterator first, Size n, OutputIterator result) {
        return __copy_n(first, n, result, iterator_category(first));
    }


    /*****************************************************************************************/
    // move
    // 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
    // 具体的优化策略：参考的 copy
    /*****************************************************************************************/
    template <class InputIterator, class OutputIterator>
    struct __move_dispatch {
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
            return __move(first, last, result, iterator_category(first));
        }
    };

    template <class InputIterator, class OutputIterator>
    OutputIterator __move(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
        for(; first != last; ++first, ++result) {
            *result = MySTL::move(*first);
        }

        return result;
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator __move(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
        return __move_d(first, last, result, distance_type(first));
    }

    template <class RandomIter, class OutputIter, class Distance>
    OutputIter __move_d(RandomIter first, RandomIter last, OutputIter result, Distance*) {
        for(Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }

        return result;
    }

    template <class T>
    struct __move_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator;
            return __move_t(first, last, result, t());
        }
    };

    template <class T>
    struct __move_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator;
            return __move_t(first, last, result, t());
        }
    };

    template <class T>
    T* __move_t(const T* first, const T* last, T* result, true_type) {
        const size_t n = static_cast<size_t>(last - first);
        if(n != 0) {
            memmove(result, first, n * sizeof(T));
        }

        return result + n;
    }

    template <class T>
    T* __move_t(const T* first, const T* last, T* result, false_type) {
        return __move_d(first, last, result, (ptrdiff_t*)(0));
    }

    template <class InputIterator, class OutputIterator>
    OutputIterator move(InputIterator first, InputIterator last, OutputIterator result) {
        return __move_dispatch<InputIterator, OutputIterator>()(first, last, result);
    }


    /*****************************************************************************************/
    // move_backward
    // 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
    /*****************************************************************************************/
    template <class BidirectionalIter1, class BidirectionalIter2>
    struct __move_backward_dispatch {
        BidirectionalIter2 operator()(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
            return __move_backward(first, last, result, iterator_category(first));
        }
    };

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 __move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result, bidirectional_iterator_tag) {
        while(first != last) {
            // * 和 -- 的优先级相同，结合方向从右到左
            *--result = MySTL::move(*--last);
        }

        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 __move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result, random_access_iterator_tag) {
        return __move_backward_d(first, last, result, distance_type(first));
    }

    template <class RandomIter, class BidirectionalIter2, class Distance>
    BidirectionalIter2 __move_backward_d(RandomIter first, RandomIter last, BidirectionalIter2 result, Distance*) {
        for(Distance n = last - first; n > 0; --n) {
            *--result = MySTL::move(*--last);
        }

        return result;
    }

    template <class T>
    struct __move_backward_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator;
            return __move_backward_t(first, last, result, t());
        }
    };

    template <class T>
    struct __move_backward_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            using t = typename __type_traits<T>::has_trivial_assignment_operator;
            return __move_backward_t(first, last, result, t());
        }
    };

    template <class T>
    T* __move_backward_t(const T* first, const T* last, T* result, MySTL::true_type) {
        const size_t n = static_cast<size_t>(last - first);
        if(n != 0) {
            result -= n;
            memmove(result, first, n * sizeof(T));
        }

        return result;
    }

    template <class T>
    T* __move_backward_t(const T* first, const T* last, T* result, MySTL::false_type) {
        return __move_backward_d(first, last, result, (ptrdiff_t*)(0));
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return __move_backward_dispatch<BidirectionalIter1, BidirectionalIter2>()(first, last, result);
    }
    

    /*****************************************************************************************/
    // equal
    // 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
    /*****************************************************************************************/
    template <class InputIterator1, class InputIterator2>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
        for(; first1 != last1; ++first1, ++first2) {
            if(*first1 != *first2) {
                return false;
            }
        }

        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIterator1, class InputIterator2, class Compare>
    bool euqal(InputIterator1 frist1, InputIterator1 last1, InputIterator2 first2, Compare comp) {
        for(; frist1 != last1; ++frist1, ++first2) {
            if(!comp(*frist1, *first2)) {
                return false;
            }
        }

        return true;
    }


    /*****************************************************************************************/
    // mismatch
    // 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
    /*****************************************************************************************/
    template <class InputIter1, class InputIter2>
    MySTL::pair<InputIter1, InputIter2>
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
        while(first1 != last1 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return MySTL::pair<InputIter1, InputIter2>(first1, first2);
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIter1, class InputIter2, class Compare>
    MySTL::pair<InputIter1, InputIter2>
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compare comp) {
        while(first1 != last1 && comp(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return MySTL::pair<InputIter1, InputIter2>(first1, first2);
    }


    /*****************************************************************************************/
    // fill
    // 为 [first, last)区间内的所有元素填充新值
    /*****************************************************************************************/   
    /* 特例化 */
    void fill(unsigned char* first, unsigned char* last, const unsigned char& c) {
        unsigned char tmp = c;
        memset(first, tmp, last - first);
    }

    void fill(signed char* first, signed char* last, const signed char& c) {
        signed char tmp = c;
        memset(first, static_cast<unsigned char>(tmp), last - first);
    }

    void fill(char* first, char* last, const char& c) {
        char tmp = c;
        memset(first, static_cast<unsigned char>(tmp), last - first);
    }


    template <class ForwardIter, class T>
    void fill(ForwardIter first, ForwardIter last, const T& value) {
        for(; first != last; ++first) {
            *first = value;
        }
    }


    /*****************************************************************************************/
    // fill_n
    // 从 first 位置开始填充 n 个值
    /*****************************************************************************************/
    /* 特例化 */
    template <class Size>
    unsigned char* fill_n(unsigned char* first, Size n, const unsigned char& c) {
        fill(first, first + n, c);
        return first + n;
    }

    template <class Size>
    signed char* fill_n(signed char* first, Size n, const signed char& c) {
        fill(first, first + n, c);
        return first + n;
    }

    template <class Size>
    char* fill_n(char* first, Size n, const char& c) {
        fill(first, first + n, c);
        return first + n;
    }

    template <class OutputIter, class Size, class T>
    OutputIter fill_n(OutputIter first, Size n, const T& value) {
        for(; n > 0; --n, ++first) {
            *first = value;
        }

        return first;
    }


    /*****************************************************************************************/
    // lexicographical_compare
    // 检查第一范围 [first1, last1) 是否按照字典序小于第二范围 [first2, last2)
    // 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
    // (1)如果第一序列的元素较小，返回 true ，否则返回 false
    // (2)如果到达 last1 而尚未到达 last2 返回 true
    // (3)如果到达 last2 而尚未到达 last1 返回 false
    // (4)如果同时到达 last1 和 last2 返回 false
    /*****************************************************************************************/
    template <class InputIter1, class InputIter2>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
        for(; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if(*first1 < *first2) {
                return true;
            }
            if(*first2 < *first1) {
                return false;
            }
            // 如果不满足上述条件，表示两值相等，那么进行下一族元素值得比较
        }

        // 如果第一序列到达尾端，而第二序列尚有余额，那么第一序列小于第二序列
        return first1 == last1 && first2 != last2;
    }

    // 重载对象使用 函数对象 comp 代替比较操作
    template <class InputIter1, class InputIter2, class Compare> 
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, Compare comp) {
        for(; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if(comp(*first1, *first2)) {
                return true;
            }
            if(comp(*first2, *first1)) {
                return false;
            }
            // 如果不满足上述条件，表示两值相等，那么进行下一族元素值得比较
        }

        // 如果第一序列到达尾端，而第二序列尚有余额，那么第一序列小于第二序列
        return first1 == last1 && first2 != last2;
    }

    // 针对原始指针得 const unsigned char* 得特化版本
    bool lexicographical_compare(const unsigned char* first1,
                                 const unsigned char* last1,
                                 const unsigned char* first2, 
                                 const unsigned char* last2) 
    {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        // 先比较相同长度的一段，memcmp 速度极快
        // memcmp :
        // 如果返回值 < 0，则表示 str1 小于 str2。
        // 如果返回值 > 0，则表示 str2 小于 str1。
        // 如果返回值 = 0，则表示 str1 等于 str2。
        const auto result = memcmp(first1, first2, MySTL::min(len1, len2));
        // 若相等，长度较长得比较大
        return result != 0 ? result < 0 : (len1 < len2);
    }
} // namespace MySTL
#endif
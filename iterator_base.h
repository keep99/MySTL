/*
 * @Description: 五种迭代器型别、基础迭代器iterator_base、iterator_traits、advance、distance。
 * @Author: Chen.Yu
 * @Date: 2021-04-02 20:54:47
 * @LastEditTime: 2021-04-10 22:44:15
 * @LastEditors: Chen.Yu
 */
#ifndef _ITERATOR_H
#define _ITERATOR_H

#include <cstddef> // ptrdiff_t

namespace MySTL {
    // 五种迭代器型别
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // template<class T, class Distance>
    // struct input_iterator {
    //     typedef input_iterator_tag iterator_category;
    //     typedef T                  value_type;
    //     typedef Distance           difference_type;
    //     typedef T*                 pointer;
    //     typedef T&                 reference;
    // };

    // template <class T, class Distance>
    // struct output_iterator {
	// 	typedef output_iterator_tag iterator_category;
	// 	typedef void                value_type;
	// 	typedef void                difference_type;
	// 	typedef void                pointer;
	// 	typedef void                reference;
	// };

	// template <class T, class Distance> 
	// struct forward_iterator {
	// 	typedef forward_iterator_tag	iterator_category;
	// 	typedef T						value_type;
	// 	typedef Distance				difference_type;
	// 	typedef T*						pointer;
	// 	typedef T&						reference;
	// };

	// template <class T, class Distance> 
	// struct bidirectional_iterator {
	// 	typedef bidirectional_iterator_tag	iterator_category;
	// 	typedef T							value_type;
	// 	typedef Distance					difference_type;
	// 	typedef T*							pointer;
	// 	typedef T&							reference;
	// };

	// template <class T, class Distance> 
	// struct random_access_iterator {
	// 	typedef random_access_iterator_tag	iterator_category;
	// 	typedef T							value_type;
	// 	typedef Distance					difference_type;
	// 	typedef T*							pointer;
	// 	typedef T&							reference;
	// };

    // 自行开发的迭代器最好继承自以下这个 std::iterator
    // STL源码剖析 P100
    template<class Category, 
             class T,
             class Distance = std::ptrdiff_t,
             class Pointer = T*,
             class Reference = T&>
    struct iterator_base {
        using iterator_category  = Category;
        using value_type         = T;
        using difference_type    = Distance;
        using pointer            = Pointer; 
        using reference          = Reference;
    };


    // 榨汁机 traits，榨取迭代器的相应型别
    template<class Iterator>
    struct iterator_traits {
        using iterator_category     = typename Iterator::iterator_category;
        using value_type            = typename Iterator::value_type;
        using difference_type       = typename Iterator::differece_type;
        using pointer               = typename Iterator::pointer;
        using reference             = typename Iterator::reference;
    };

    // 为原生指针而设计的 traits 偏特化
    // 原生指针不是类，不能为他们定义内嵌类型（如 Iterator::value_type）。
    template<class T>
    struct iterator_traits<T*> {
        using iterator_category = random_access_iterator_tag;  // 原生指针是一种 Random Access Iterator
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    };

    // pointer to const 的特化版本
    template<class T>
    struct iterator_traits<const T*> {
        using iterator_category = random_access_iterator_tag;  // 原生指针是一种 Random Access Iterator
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;
    };

    // 获取迭代器的类型
    // 等价于 iterator_traits<Iterator>::iterator_category()
    template<class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
        using category = typename iterator_traits<Iterator>::iterator_category;
        return category();
    }

    // 获取迭代器的 difference type
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type* difference_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 获取迭代器的 value type
    template<class Iterator>
    inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }


    // 以下函数用于让迭代器前进 n 个距离

    // advance 函数及其各种偏特化版本
    namespace detail {
        template <class InputIterator, class Distance>
        inline void __advance(InputIterator& it, Distance n, input_iterator_tag) {
            while (n-- > 0) {
                ++it;
            }
        }

        template <class BidirectionalIterator, class Distance>
        inline void __advance(BidirectionalIterator& it, Distance, bidirectional_iterator_tag) {
            if (n > 0) {
                while(n-- > 0) {
                    ++it;
                }
            } else {
                while(n++ < 0) {
                    --it;
                }
            }
        }

        template <class RandomAccessIterator, class Distance>
        inline void __advance(RandomAccessIterator& it, Distance n, random_access_iterator_tag) {
            it += n;
        }
    } // namespace detail

    /* 对外开放的上层控制接口，调用上述重载的__advance() */
    template <class InputIterator, class Distance>
    inline void advance(InputIterator& it, Distance n) {
        detail::__advance(it, n, iterator_category(it));
    }


    // 以下函数用于计算迭代器之间的距离

    // distance 函数及其各种偏特化版本的函数
    namespace detail {
        template <class InputIterator>
        typename iterator_traits<InputIterator>::difference_type
        __distance(InputIterator first, InputIterator last, input_iterator_tag) {
            typename iterator_trait<InputIterator>::difference_type n = 0;
            // 逐一累计距离
            while (first != last) {
                ++n; 
                ++first;
            }
            
            return n;
        }

        template <class RandomAccessIterator>
        typename iterator_traits<RandomAccessIterator>::difference_type
        __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
            // 直接计算距离
            return last - first;
        }
    } //namespace detail

    /* 对外开放的上层控制接口，调用上述重载的__distance() */
    template <class InputItertor>
    inline typename iterator_traits<InputItertor>::difference_type
    distance(InputItertor first, InputItertor last) {
        return detail::__distance(first, last, iterator_category(first));
    }
    
    /* 冗余的两个函数 */
    // /**
    //  * 当前迭代器向前移动 n 步后的迭代器
    //  * @tparam InputIterator 迭代器类型。Input Iterator 和它的"强化"迭代器都是合法的参数。
    //  * @param it 迭代器
    //  * @param n 距离
    //  * @return 移动后的迭代器
    //  */
    // template <class InputIterator>
    // inline InputIterator next(InputIterator it, typename iterator_traits<InputIterator>::difference_type n = 1) {
    //     advance(it, n);

    //     return it;
    // }
    // /**
    //  * 当前迭代器向后移动 n 步后的迭代器
    //  * @tparam BidirectionalIterator 迭代器类型。Bidirectional Iterator 和它的"强化"迭代器都是合法的参数。
    //  * @param it 迭代器
    //  * @param n 距离（非负数）
    //  * @return 移动后的迭代器
    //  */ 
    // template <class BidirectionalIterator>
    // inline BidirectionalIterator prev(BidirectionalIterator it, typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
    //     advance(it, -n);

    //     return it;
    // }
}

#endif
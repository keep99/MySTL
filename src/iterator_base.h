#ifndef _ITERATOR_BASE_H_
#define _ITERATOR_BASE_H_

#include <cstddef> // ptrdiff_t

#include <type_traits>

#include "type_traits.h"  // std::is_convertible

namespace MySTL {
    // 五种迭代器型别
    struct input_iterator_tag {};
    struct output_iterator_tag {};
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


    template <class T>
    struct has_iterator_cat
    {
    private:
        struct two { char a; char b; };
        template <class U> static two test(...);
        template <class U> static char test(typename U::iterator_category* = 0);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template <class Iterator, bool>
    struct iterator_traits_impl {};

    template <class Iterator>
    struct iterator_traits_impl<Iterator, true>
    {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type        value_type;
        typedef typename Iterator::pointer           pointer;
        typedef typename Iterator::reference         reference;
        typedef typename Iterator::difference_type   difference_type;
    };

    template <class Iterator, bool>
    struct iterator_traits_helper {};

    template <class Iterator>
    struct iterator_traits_helper<Iterator, true>
        : public iterator_traits_impl<Iterator,
          std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
          std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
    {
    };


    // 萃取迭代器的特性
    template <class Iterator>
    struct iterator_traits 
        : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

    // 为原生指针而设计的 traits 偏特化
    // 原生指针不是类，不能为他们定义内嵌类型（如 Iterator::value_type）。
    template<class T>
    struct iterator_traits<T*> {
        using iterator_category = random_access_iterator_tag;  // 原生指针是一种 Random Access Iterator
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    };

    // pointer to const 的特化版本
    template<class T>
    struct iterator_traits<const T*> {
        using iterator_category = random_access_iterator_tag;  // 原生指针是一种 Random Access Iterator
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;
    };

    // 获取迭代器的类型
    template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of
    : public m_bool_constant<std::is_convertible<
    typename iterator_traits<T>::iterator_category, U>::value>
    {
    };

    // 萃取某种迭代器
    template <class T, class U>
    struct has_iterator_cat_of<T, U, false> : public false_type {};

    template <class Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

    template <class Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

    template <class Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

    template <class Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

    template <class Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

    template <class Iterator>
    struct is_iterator 
        : public m_bool_constant<is_input_iterator<Iterator>::value ||
                                 is_output_iterator<Iterator>::value>
    {
    };


    // 萃取某个迭代器的 category
    // 等价于 iterator_traits<Iterator>::iterator_category()
    template<class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
        using category = typename iterator_traits<Iterator>::iterator_category;
        return category();
    }

    // 获取迭代器的 distance type
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
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
        inline void __advance(BidirectionalIterator& it, Distance n, bidirectional_iterator_tag) {
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
            typename iterator_traits<InputIterator>::difference_type n = 0;
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
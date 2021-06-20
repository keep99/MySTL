#ifndef _ITERATOR_H_
#define _ITERATOR_H_

#include "iterator_base.h"
#include "iterator_reverse_iterator.h"  // 反向迭代器适配器
#include "iterator_insert_iterator.h"   // 插入迭代器适配器
#include "iterator_move_iterator.h"     // 移动迭代器适配器

namespace MySTL {
    // 四个辅助函数，帮助我们方便使用 插入迭代器，移动迭代器
    template <class Container>
    inline back_insert_iterator<Container> back_inserter(Container& c) {
        return back_insert_iterator<Container>(c);
    }

    template <class Container>
    inline front_insert_iterator<Container> front_inserter(Container& c) {
        return front_insert_iterator<Container>(c);
    }

    template <class Container>
    inline insert_iterator<Container> inserter(Container& c, typename Container::iterator i) {
        return insert_iterator<Container>(c, i);
    }

    template <class Iterator>
    move_iterator<Iterator> make_move_iterator(const Iterator& i) {
        return move_iterator<Iterator>(i);
    }

    template <class Container>
    auto begin(Container& c) -> decltype(c.begin()) {
        return c.begin();
    }

    template <class Container>
    auto begin(const Container& c) -> decltype(c.begin()) {
        return c.begin();
    }

    template< class T, std::size_t N >
    T* begin(T (&array)[N]) {
        return &array;
    }

    template< class Container>
    constexpr auto cbegin(const Container& c) noexcept -> decltype(MySTL::begin(c)) {
        return MySTL::begin(c);
    }

    template <class Container>
    auto end(Container& c) -> decltype(c.end()) {
        return c.end();
    }

    template <class Container>
    auto end(const Container& c) -> decltype(c.end()) {
        return c.end();
    }

    template <class T, std::size_t N>
    T* end(T (&array)[N]) {
        return array + N;
    }

    template <class Container>
    constexpr auto cend(const Container& c) noexcept -> decltype(MySTL::end(c)) {
        return MySTL::end(c);
    }

    template <class Container>
    auto rbegin(Container& c) -> decltype(c.rbegin()) {
        return c.rbegin();
    }

    template <class Container>
    auto rbegin(const Container& c) -> decltype(c.rbegin()) {
        return c.rbegin();
    }

    template <class T, std::size_t N>
    reverse_iterator<T*> rbegin(T (&array)[N]) {
        return reverse_iterator<T*>(&array);
    }

    template <class Container>
    auto crbegin(const Container& c) -> decltype(rbegin(c)) {
        return rbegin(c);
    }

    template <class Container>
    auto rend(Container& c) -> decltype(c.rend()) {
        return c.rend();
    }

    template <class Container>
    auto rend(const Container& c) -> decltype(c.rend()) {
        return c.rend();
    }

    template<class T, std::size_t N>
    reverse_iterator<T*> rend(T (&array)[N]) {
        return reverse_iterator<T*>(array + N);
    }

    template<class Container>
    auto crend(const Container& c) -> decltype(rend(c)) {
        return rend(c);
    }
}

#endif

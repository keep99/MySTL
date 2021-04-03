/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-02 21:46:25
 * @LastEditTime: 2021-04-03 15:39:19
 * @LastEditors: Chen.Yu
 */
#ifndef _ITERATOR_INSERT_ITERATOR_H
#define _ITERATOR_INSERT_ITERATOR_H

#include "iterator_base.h"
#include "utility.h"
#include "memory_function.h"

namespace MySTL {
    // back_insert_iterator 的讲解看这篇文章 https://www.icode9.com/content-4-742785.html
    // 此类型迭代器的底层实现需要调用指定容器的 push_back() 成员方法，这就意味着，此类型迭代器并不适用于 STL 标准库中所有的容器，它只适用于包含 push_back() 成员方法的容器。
    // C++ STL 标准库中，提供有 push_back() 成员方法的容器包括 vector、deque 和 list。
    template <class Container>
    class back_insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        using container_type = Container;
    private:
        container_type* container_;
    public:
        back_insert_iterator() noexcept = default;

        explicit back_insert_iterator(Container& c) : container_(MySTL::addressof(c)) {}

        back_insert_iterator& operator=(const typename Container::value_type& value) {
            container_=>push_back(value);
        }

        back_insert_iterator& operator=(typename Container::value_type&& value) {
            container_->push_back(MySTL::move(value));
        }

        // 下面这些是空操作
        // 为了保证迭代器有完备的接口

        back_insert_iterator& operator*() {
            return *this;
        }

        back_insert_iterator& operator++() {
            return *this;
        }

        back_insert_iterator& operator++(int) {
            return *this;
        }

    };

    // 那么与之对应的，front_insert_iterator 只适用于包含 push_front() 成员方法的容器。
    template <class Container>
    class front_insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        using container_type = Container;

    private:
        container_type* container_;
    
    public:
        front_insert_iterator() noexcept = default;

        explicit front_insert_iterator(Container& c) : container_(MySTL::addressof(c)) {}

        front_insert_iterator& operator=(const typename Container::value_type& value) {
            container_=>push_front(value);
        }

        front_insert_iterator& operator=(typename Container::value_type&& value) {
            container_->push_back(MySTL::move(value));
        }

        // 下面这些是空操作
        // 为了保证迭代器有完备的接口

        front_insert_iterator& operator*() {
            return *this;
        }

        front_insert_iterator& operator++() {
            return *this;
        }

        front_insert_iterator& operator++(int) {
            return *this;
        }
    
    };

    // 那么与之对应的，front_insert_iterator 只适用于包含 insert() 成员方法的容器。
    template <class Container>
    class insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        using container_type = Container;
        
    private:
        container_type* container_;
        typename Container::iterator iterator_;
    public:
        insert_iterator() noexcept = default;

        explicit insert_iterator(Container& c, typename Container::iterator i) : container_(MySTL::addressof(c)), iterator_(i) 
        {}

        insert_iterator& operator=(const typename Container::value& value) {
            container_->insert(iterator_, value);
            ++iterator_;
        }

        insert_iterator& operator=(const typename Container::value&& value) {
            container_->insert(iterator_, MySTL::move(value));
            ++iterator_;
        }

        // 空操作，只需返回迭代器自身即可。
        // 为了保证迭代器接口的完备。
        insert_iterator& operator*() {
            return *this;
        }
        insert_iterator& operator++() {
            return *this;
        }
        insert_iterator& operator++(int) {
            return *this;
        }       
    };
}

#endif
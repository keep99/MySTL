#ifndef _ITERATOR_INSERT_ITERATOR_H
#define _ITERATOR_INSERT_ITERATOR_H

#include "iterator_base.h"
#include "utility.h"
#include "memory_function.h"

namespace MySTL {
    // 参考 https://www.icode9.com/content-4-742785.html
    // back_insert_iterator 迭代器可用于在指定容器的末尾处添加新元素
    // 此类型迭代器的底层实现需要调用指定容器的 push_back() 成员方法，这就意味着，此类型迭代器并不适用于 STL 标准库中所有的容器，它只适用于包含 push_back() 成员方法的容器。
    // C++ STL 标准库中，提供有 push_back() 成员方法的容器包括 vector、deque 和 list。
    template <class Container>
    class back_insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        // 继承获得类型 iterator_category 、 value_type 、 difference_type 、 pointer 及 reference

        using container_type = Container;
    private:
        container_type* container_;
    public:
        back_insert_iterator() noexcept = default;

        explicit back_insert_iterator(Container& c) : container_(&c) {}

        // C++ Primer P610 重载版本一般是两个，一个版本接受一个指向const的左值引用，另外一个版本接受一个指向非const的右值引用
        // // 对赋值运算符（=）进行了重载，借助此运算符，我们可以直接将新元素插入到目标容器的尾部
        back_insert_iterator& operator=(const typename Container::value_type& value) {
            container_->push_back(value);
            
            return *this;
        }

        back_insert_iterator& operator=(typename Container::value_type&& value) {
            container_->push_back(MySTL::move(value));
            
            return *this;
        }

        back_insert_iterator& operator*() const {
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

        explicit front_insert_iterator(Container& c) : container_(&c) {}

        // 对赋值运算符（=）进行了重载，借助此运算符，我们可以直接将新元素插入到目标容器的头部
        front_insert_iterator& operator=(const typename Container::value_type& value) {
            container_->push_front(value);

            return *this;
        }

        front_insert_iterator& operator=(typename Container::value_type&& value) {
            container_->push_back(MySTL::move(value));

            return *this;
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

    // 那么与之对应的，insert_iterator 只适用于包含 insert() 成员方法的容器。
    template <class Container>
    class insert_iterator : public iterator_base<output_iterator_tag, void, void, void, void> {
    public:
        using container_type = Container;
        
    private:
        container_type* container_;
        typename Container::iterator iterator_;
    public:
        insert_iterator() noexcept = default;

        explicit insert_iterator(Container& c, typename Container::iterator i) : container_(&c), iterator_(i) 
        {}

        insert_iterator& operator=(const typename Container::value& value) {
            container_->insert(iterator_, value);
            ++iterator_;

            return *this;
        }

        insert_iterator& operator=(const typename Container::value&& value) {
            container_->insert(iterator_, MySTL::move(value));
            ++iterator_;

            return *this;
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
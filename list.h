/*
 * @Description: list 容器。从这个容器开始，为了清楚，声明和定义分开。
 * @Author: Chen.Yu
 * @Date: 2021-04-04 14:21:48
 * @LastEditTime: 2021-04-15 02:26:59
 * @LastEditors: Chen.Yu
 */
#ifndef _LIST_H_
#define _LIST_H_

#include "allocator.h"
#include "type_traits"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "functional_base.h"  // TO DO
#include "algorithm_base.h" // TO DO
#include "utility.h"

namespace MySTL
{
    namespace detail
    {
        // 双向链表节点
        template <class T>
        struct list_node
        {
            list_node *previous; // 前一个节点
            list_node *next;     // 后一个节点
            T data;              // 值
        };

        // list 迭代器
        // TO DO : list_const_iterator
        template <class T, class Reference, class Pointer>
        struct list_iterator
        {
            using iterator_category             = bidirectional_iterator_tag;
            using value_type                    = T;
            using difference_type               = std::ptrdiff_t;
            using pointer                       = Pointer;
            using reference                     = Reference;

            using iterator                      = list_iterator<T, T&, T*>;
            using const_iterator                = list_iterator<T, const T&, const T*>;
            using self                          = list_iterator<T, Reference, Pointer>;

            using link_type                     = detail::list_node<T>*;

            link_type node_;

            // 构造函数
            list_iterator() = default;

            explicit list_iterator(link_type x) : node_(x){};

            explicit list_iterator(const iterator &x) : node_(x.node_) {}

            // 重载操作符

            reference operator*() const
            {
                return node_->data;
            }

            pointer operator->() const
            {
                return &(operator*());
            }

            // 前置++
            self& operator++()
            {
                node_ = node_->next;

                return *this;
            }

            // 后置++
            self operator++(int)
            {
                self tmp = *this;
                node_ = node_->next;

                return tmp;
            }

            // 前置--
            self operator--()
            {
                node_ = node_->previous;

                return *this;
            }
            
            // 后置--
            self operator--(int)
            {
                self tmp = *this;
                node_ = node_->previous;

                return tmp;
            }

            bool operator==(const self &other) const
            {
                return node_ == other.node_;
            }

            bool operator!=(const slef &other) const
            {
                return node_ != other.node_;
            }
        };
    } // detail

    // 双向链表
    template <class T, class Allocator = allocator<T>> 
    class list
    {
    public:
        using value_type                = T;
        using pointer                   = T*;
        using const_pointer             = const T*;
        using reference                 = T&;
        using const_reference           = const T&;
        using size_type                 = std::size_t;
        using difference_type           = std::ptrdiff_t;

        using iterator                  = detail::list_iterator<T, T&, T *>;
        using const_iterator            = detail::list_iterator<T, const T&, const T*>;
        using reverse_iterator          = MySTL::reverse_iterator<iterator>;
        using const_reverse_iterator    = MySTL::reverse_iterator<const_iterator>;

        using link_type                 = detail::list_node<T>*;

        using allocator_type            = Allocator;
        using data_allocator            = Allocator;

    private:
        // allocator 的 rebind。list 希望能按照 T 一样的策略 来分配 list_node<T> 类型的对象。
        // rebind实现了对不同类型使用同一种内存分配策略的要求。具体参考https://bbs.csdn.net/topics/200079053
        using list_node_allocator = typename Allocator::template rebind<detail::list_node<T>>::other; // 这里的 template 是为了说明后面跟的是个模板，用法同 typename
        
        // 上面也可以写成,参考https://bbs.csdn.net/topics/110079160
        // using list_node_allocator = typename Allocator::rebind<detail::list_node<T>>::other;

        // SGI STL 中 整个 list class 中的 data 只有一个指针。
        // 只要这一个 node_ ，便可表示整个环状双向链表
        // 指向末尾的空白节点
        link_type node_ = nullptr;
        // GCC 无，VC 有
        // size_type size_ = 0;


    public:
        /* 构造、赋值、移动、析构函数 */

        list()
        {
            node_ = allocate_node();
            node_->previous = node->next = node_;
        }

        list(size_type count, const_reference value)
        { insert(begin(), count, value); }

        explicit list(size_type count)
        { insert(begin(), count, value_type()); }

        template <class InputIterator>
        list(InputIterator first, InputIterator last)
        { insert(begin(), first, last); }

        list(const list &other) : list(other.begin(), other.end()) 
        { insert(begin(), other.cbegin(), other.cend()); }

        list(std::initializer_list<T> ilist)
        { insert(begin(), ilist.cbegin(), ilist.cend()); }

        list(list &&other) noexcept : {
            move_from(other);
        }

        list &operator=(const list &other)
        {
            if (this != &other)
            {
                // 超过这个区域，tmp 自动销毁
                list tmp(other);
                swap(tmp);
            }

            return *this;
        }

        list &operator=(list &&other) noexcept
        {
            if (this != &other)
            {
                move_from(other);
            }

            return *this;
        }

        list &operator=(std::initializer_list<T> ilist)
        {
            *this = list(ilist);

            return *this;
        }

        ~list()
        {
            delete_list();
        }

        // assin 的相关代码需要修改
        template <calss InputIterator>
        void assign(InputIterator first, InputIterator last)
        {
            using is_Integral = typename is_integral<InputIterator>;
            assign_dispatch(first, last, is_Integral());
        }

        void assign(size_type count, const_reference value)
        {
            fill_assign(count, value);
        }

        void assign(std::initializer_list<T> ilist)
        {
            assign_dispatch(ilist.begin(), ilist.end(), false_type);
        }

        allocator_type get_allocator() const
        {
            return list_node_allocator();
        }


        /* 元素访问相关操作 */
        
        reference front()
        {
            return *begin();
        }

        const_reference front() const
        {
            return *begin();
        }

        reference back()
        {
            return *(--end());
        }

        const_reference back() const
        {
            return *(--end());
        }


        /* 迭代器相关操作 */

        iterator begin()
        {
            // list_iterator<T, T&, T*> 和 list_node<T> 都是只有一个成员：list_type node_;
            // 所以可以互相转换
            return static_cast<iterator>(node_->next);
        }

        const_iterator begin() const
        {
            return static_cast<iterator>(node_->next);
        }

        const_iterator cbegin() const
        {
            return static_cast<const_iterator>(node_->next);
        }

        iterator end() {
            return static_cast<iterator>(node_);
        }

        const_iterator end() const {
            return static_cast<const_iterator>(node_);
        }

        const_iterator cend() const {
            return static_cast<const_iterator>(node_);
        }

        reverse_iterator rbegin() {
            return static_cast<reverse_iterator>(end());
        }

        const_reverse_iterator rbegin() const {
            return static_cast<const_reverse_iterator>(end());
        }

        const_reverse_iterator crbegin() const {
            return static_cast<const_reverse_iterator>(end());
        }

        reverse_iterator rend() {
            return static_cast<reverse_iterator>(begin());
        }

        const_reverse_iterator rend() const {
            return static_cast<const_reverse_iterator>(begin());
        }

        const_reverse_iterator crend() const {
            return static_cast<const_reverse_iterator>(begin());
        }


        /* 容量相关操作 */

        bool empty() const {
            return node_->next = node_;
        }

        // fixed。list 无大小
        size_type size() const {
            size_type result = 0;
            result = MySTL::distance(begin(), end());

            return result;
        }

        size_type max_size() const {
            return size_type(-1);
            // return std::numeric_limits<size_type>::max();
        }


        /* 修改器 */

        void clear() {
            erase(begin(), end());
        }

        iterator insert(const_iterator position, const_reference value) {
            return insert_aux(position, value);
        }

        iterator insert(const_iterator position, T&& value)  {
            return insert_aux(position, MySTL::move(value));
        }

        iterator insert(const_iterator position, size_type count, const_reference) {
            while (count--) {
                position = insert(position, value);
            }

            return static_cast<iterator>(position.node_);
        }

        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last) {
            return insert_range_aux(position, first, last, is_integral<InputIterator>());
        }

        iterator insert(const_iterator position, std::initializer_list<T> ilist) {
            return insert(position, ilist.begin(), ilist.end());
        }

        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            return insert(position, T(MySTL::forward<Args>(args)...));
        }

        iterator erase(const_iterator position) {
            link_type currentNode = position.node_;
            link_type previousNode = currentNode->previous;
            link_type nextNode = currentNode->next;

            // 在链表中断开节点
            previousNode->next = nextNode;
            nextNode->previous = previousNode;

            //销毁节点
            destroy_node(currentNode);

            return static_cast<iterator>(nextNode);
        }

        iterator erase(const_iterator first, const_iterator last) {
            while (first != last) {
                first = erase(first);
            }

            return static_cast<iterator>(last.node_);
        }

        void push_back(const_reference value) {
            insert(end(), value);
        }

        void push_back(T&& value) {
            insert(end(), MySTL::move(value));
        }

        template <class... Args>
        void emplace_back(Args&&... args) {
            emplace(end(), MySTL::forward(args)...);
        }

        void pop_back() {
            erase(--end());
        }

        void push_front(const_reference value) {
            insert(begin(), value);
        }

        void push_front(T&& value) {
            insert(begin(), MySTL::move(value));
        }

        template <class... Args>
        void emplace_front(Args&&.. args) {
            emplace(begin(), MySTL::forward<Args>(args)...);
        }

        void pop_front() {
            erase(begin());
        }

        void resize(size_type count, const_reference value) {
            if (count == size()) {
                return;
            } else if (count < size()) {
                erase(MySTL::next(begin(), count), end());
            } else {
                insert(end(), count - size(), value);
            }
        }

        void resize(size_type count) {
            resize(count, T());
        }

        void swap(list& other) {
            using MySTL::swap;
            swap(node_, other.node_);
        }


        /* list 相关操作 */

        // 归并两个已经排过序的链表，将 other 归并到 *this(first1) 中
        template <class Compare>
        void merge(list& other, Compare compare) {
            if (this == other) {
                return;
            }

            auto first1 = begin();
            auto last1 = end();
            auto first2 = other.begin();
            auto last2 = end();

            while (first1 != last1 && first2 != last2) {
                if (compare(*first2, *first1)) {
                    auto next = first2 + 1;
                    transfer(first1, first2, next);
                    first2 = next;
                } else {
                    ++first1;
                }

                if (first2 != last2) {
                    transfer(last1, first2, last2);
                }
            }
        }

        template <class Compare>
        void merge(list&& other, Compare compare) {
            merge(other, compare);
        }

        /* C++ Primer P 587 在类模板自己的作用域中，我们可以直接使用模板名而不提供实参。比如 这里的形参类型，不是 list<T>, 而是直接写 list */
        void merge(list& other) {
            merge(other, MySTL::less<T>());
        }

        void merge(list&& other) {
            merge(other, MySTL::less<T>());
        }

        /**
         * @description: 将链表 other 接合到 position 所指的位置之前， other 必须不同于 *this
         * @param  {*}
         * @return {*}
         * @param {const_iterator} position
         */
        void splice(const_iterator position, list& other) {
            if (!other.empty()) {
                transfer(position, other.begin(), other.end())p;
            }   
        }

        void splice(const_iterator postion, list&& other) {
            splice(position, other);
        }

        void splice(iterator position, list&, iterator it) {
            // it 如果 是下面这两种情况，没有意义，什么也不做
            if (position == it || position == it + 1) {
                return;
            }

            transfer(position, it, it + 1);
        }

        void splice(iterator position, list&& other, iterator it) {
            splice(position, other, it);
        }

        void splice(iterator position, list&, iterator first, iterator last) {
            if (first != last) {
                transfer(position, first, last);
            }
        }

        void splice(iterator position, list&& other, iterator first, iterator last) {
            splice(position, other, first, last);
        }

        /**
         * @description: 如果链表中有元素符合一元判断式，那么就把这个元素给删除
         * @param  {*}
         * @return {*}
         */
        template <class UnaryPredicate>
        void remove_if(UnaryPredicate p) {
            auto first = begin();
            auto last = end();
            while (first != last) {
                if (UnaryPredicate(*first)) {
                    first = erase(first);
                } else {
                    ++first;
                }
            }
        }

        // Lambda 的运用
        void remove(const_reference value) {
            remove_if([value](const_reference other) {
                return value == other;
            });
        }

        // 反转链表 TO DO
        void reverse() {
            // 表示链表没有节点或者只有一个节点，不用反转
            if (node_->next == node || node_->next->next == node_) {
                return;
            }

            using MySTL::swap;
            swap(node->previous, node_->next);
            auto it = node->previous;
            while (it != node_) {
                swap(it->previous, it->next);
                it = it->previous;
            }
        }

        template <class BinaryPredicate>
        void unique(BinaryPredicate p) {
            if (node_->next == node || node_->next->next == node_) {
                return;
            }

            auto first = begin();
            auto last = end();

            auto next = first + 1;
            while (next != last) {
                if (p(*first, *next)) {
                    next = erase(next);
                } else {
                    ++first;
                    ++next;
                }
            }
        }

        /**
         * @description: 移除数值相同的连续元素
         * @param  {*}
         * @return {*}
         */
        void unique() {
            unique(MySTL::equal_to<T>());
        }

        // 参考这篇博文：https://www.kancloud.cn/digest/stl-sources/177268
        // 参考 SGI STL 的实现
        // 由于 STL 本身的排序算法 sort 接受的是随机访问迭代器，但是双向链表 list 的迭代器是双向迭代器，因此，不能使用 STL 本身的排序算法 sort ，必须得自己定义
        /**
         * @description: 对链表进行归并排序
         * @param  {*}
         * @return {*}
         */
        template <class Compare>
        void sort(iterator f1, iterator f2, Compare compare) {
            // 以下判断，如果是空链表或者仅仅只有一个链表节点，不需要排序，直接返回
            if (node_->next == node_ || node_->next->next == node_) {
                return;
            }

            list carry;             // carry 链表起到搬运的作用
            // counter 链表起到中间存储的作用
            // 对于 counter[i] 里面最多能存储 2 ^ (i + 1) 个节点
            list counter[64];       
            size_type fill = 0;
            while (!empty()) {
                // 第一步：将当前链表的第一个节点放在 carry 链表的表头
                carry.splice(carry.begin(), *this, begin());
                size_type i = 0;
                while(i < fill && !counter[i].empty()) {
                    // 第二步：将链表 carry 合并到 counter[i]
                    counter[i].merge(carry);
                    // 第三步：交换链表 carry 和 counter[i] 的内容
                    carry.swap(counter[i++]);
                }
                // 第四步：交换链表 carry 和 counter[i] 的内容
                carry.swap(counter[i]);
                // 第五步：
                if(i == fill) {
                    ++fill;
                }
            }

            // 第六步：把低位不满足进位的剩余数据全部有序的合并到上一位
            for (size_type j = 0; j < fill; ++j) {
                counter[j].merge(counter[j - 1]);
            }

            // 第七步：最后将已经排好序的链表内容交换到当前链表
            swap(counter[fill - 1]);
        }


        void sort() {
            sort(begin(), end(), MySTL::less<T>());
        }



    private:
        /* 以下是 helper functions，不暴露这些接口 */
        void fill_assign(size_type n, const_reference value);

        template <class Integer>
        void assign_dispatch(Integer n, Integer value, true_type) {
            fill_assign(static_cast<size_type>(n), static_cast<T>(value));
        }

        template <class InputIterator>
        void assign_dispatch(Integer n, Integer value, true_type);







    /*                          以下为第一版                                                                                                          */
        link_type allocate_node() {
            return list_node_allocator::allocate(1);
        }

        void deallocate_node(link_type node) {
            list_node_allocator::deallocate(node);
        }

        link_type create_node(const_reference value) {
            auto newNode = allocate_node();
            return list_node_allocator::construct(newNode, value);
        }

        link_type destroy_node(link_type node) {
            list_node_allocator::destroy(node);
            deallocate_node(node);
        }
        
        /**
         * @description: 在 position 前插入一个值为 value 的节点
         * @param  {*}
         * @return {*}
         */
        template <class Y>
        iterator insert_aux(const_iterator position, Y&& value) {
            // C++ Primer P614 : 当用于一个指向模板参数类型的右值引用函数参数（T&&）时，forward 会保持实参类型的所有细节
            link_type newNode = create_node(MySTL::forward<Y>(value));
            newNode->previous = position.node_->previous;
            newNode->next     = position.node_;
            newNode->previous->next = newNode->next->previous = newNode;

            return static_cast<iterator>(newNode);
        }

        template <class InputIterator>
        iterator insert_range_aux(const_iterator position, InputIterator first, InputIterator last, true_type) {
            insert(position, first, last);
        }

        template <class InputIterator>
        iterator insert_range_aux(const_iterator position, InputIterator first, InputIterator last, false_type) {
            if (first == last) {
                return position;
            }

            auto result = insert(position, *first);
            while (++first != last) {
                insert(position, *first);
            }

            return result;
        }

        /**
         * @description: 将 [first, last) 范围的节点移动到 position 之前
         * @param  {*}
         * @return {*}
         * @param {iterator} position
         * @param {iterator} first
         * @param {iterator} last
         */
        void transfer(iterator position, iterator first, iterator last) {
            auto positionNode = position.node_;
            auto firstNode = first.node_;
            auto lastNode = last.node_->previous;

            // 这一段需要画图理解
            //第一步将 first 的前面的节点 和 last 后面的节点 接起来
            firstNode->previous->next = last->next;
            last->next->previous = firstNode->previous;

            //第二步将 first 到 last 这一段插入到 position->previous 到 position 之间
            firstNode->previous = positionNode->previous;
            lastNode->next = positionNode;
            firstNode->previous->next = firstNode;
            lastNode->next->previous = lastNode;
        }

        // 清空链表
        void delete_list() {
            if (node_ == nullptr) {
                return;
            }

            clear();
            // node_->data 为 nullptr，所以只需要回收内存空间即可
            deallocate_node(node_);
            node_ = nullptr;
        }

        // 清空链表，然后将 other 所指的链表移动到当前链表
        void move_from(list& other) {
            delete_list();
            node_ = other.node_;
            other.node_ = nullptr;
        }
    }; // class list

    template <class T, class Allocator>
    void fill_assign(size_type n, const_reference value) {

    }

    template <class T, class Allocator>
    template <class InputIterator>
    void assign_dispatch(Integer n, Integer value, true_type) {

    }







    template <class T, class Allocator>
    bool operator==(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        using const_iterator = list<T, Allocator>::const_iterator;
        const_iterator end1 = left.end();
        const_iterator end2 = right.end();

        const_iterator i1 = left.begin();
        const_iterator i2 = right.begin();

        while (i1 != end1 && i2 != end2 && *i1 == *i2) {
            ++i1;
            ++i2;
        }

        return i1 == end1 && i2 == end2;
    }

    /* 以下为 函数模板 */
    template <class T, class Allocator>
    bool operator!=(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return !(left == right);
    }

    template <class T, class Allocator>
    bool operator<(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return MySTL::lexicographical_compare(left.cbegin(), left.end(), right.cbegin(), right.cend());
    }

    bool operator<=(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return !(left > right);
    }

    template <class T, class Allocator>
    bool operator>(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return right < left;
    }

    template <class T, class Allocator>
    bool operator>=(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return !(left < right);
    }

    template <class T, class Allocator>
    void swap(list<T, Allocator>& left, list<T, Allocator>& right) {
        left.swap(right);
    }
} // namespace MySTL

#endif
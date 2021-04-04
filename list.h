/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-04 14:21:48
 * @LastEditTime: 2021-04-04 21:53:48
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
        //双向链表节点
        template <class T>
        struct list_node
        {
            list_node *previous; // 前一个节点
            list_node *next;     // 后一个节点
            T data;              // 值
        };

        // list 迭代器
        template <class T, class Reference, class Pointer>
        struct list_iterator
        {
            using iterator_category = bidirectional_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = Pointer;
            using reference = Reference;

            using iterator = list_iterator<T, T &, T *>;
            using const_iterator = list_iterator<T, const T &, const T *>;
            using self = list_iterator<T, T &, T *>;

            using link_type = detail::list_node<T> *;

            link_type node_;

            list_iterator() = default;

            explicit list_iterator(link_type x) : node_(x){};

            explicit list_iterator(const iterator &x) : node_(x.node_) {}

            reference operator*() const
            {
                return node_->data;
            }

            reference operator->() const
            {
                return &(operator*());
            }

            self &operator++()
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

            self operator--()
            {
                node_ = node_->previous();

                return *this;
            }

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

            bool operator==(const const_iterator &other) const
            {
                return node_ == other.node_;
            }

            bool operator!=(const self &other) const
            {
                return node_ != other.node_;
            }

            bool operator!=(const const_iterator &other) conts
            {
                return node_ != other.node_;
            }
        };
    }

    //双向链表
    template <class T, class Allocator = allocator<T>> 
    class list
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using iterator = detail::list_iterator<T, T &, T *>;
        using const_iterator = detail::list_iterator<T, const T &, const T *>;
        using reverse_iterator = MySTL::reverse_iterator<iterator>;
        using const_reverse_iterator = MySTL::reverse_iterator<const_iterator>;

        using link_type = detail::list_node<T> *;

        using allocator_type = Allocator;

    private:
        // ???
        using list_node_allocator = typename Allocator::template rebind<detail::list_node<T>>::other;

        // node_ 指向末尾的空白节点，即 end()
        // 他的前一个节点是 begin() ， 后一个节点是 back()
        link_type node_ = nullptr;

        // size_type size_ = 0;

        list_node_allocator listNodeAllocator;

    public:
        // 构造函数
        list()
        {
            node_ = allocate_node();
            node_->previous = node->next = node_;
        }

        list(size_type count, const_reference value)
        {
            insert(begin(), count, value);
        }

        explicit list(size_type count) : list(count, T()) {}

        template <class InputIterator>
        list(InputIterator first, InputIterator last)
        {
            insert(begin(), first, last);
        }

        list(const list &other) : list(other.begin(), other.end()) {}

        list(std::initializer_list<T> ilist) : list(ilist.begin(), ilist.end()) {}

        list(list &&other)
        {
            move_from(other);
        }

        ~list()
        {
            delete_list();
        }

        list &operator=(list &&other) noexcept
        {
            if (this != other)
            {
                move_from(other);
            }

            return *this;
        }

        list &operator=(const list &other)
        {
            if (this != &other)
            {
                list tmp(other);
                swap(tmp);
            }

            return *this;
        }

        list &operator=(std::initializer_list<T> ilist)
        {
            *this = list(ilist);

            return *this;
        }

        template <calss InputIterator>
        void assign(InputIterator first, InputIterator last)
        {
            *this = list(first, last);
        }

        void assign(size_type count, const_reference value)
        {
            *this = list(count, value);
        }

        void assign(std::initializer_list<T> list)
        {
            *this = list(ilist);
        }

        allocator_type get_allocator() const
        {
            return listNodeAllocator;
        }

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

        bool empty() const {
            return node_->next = node_;
        }

        // fixed。链表不能有大小的。
        size_type size() const {
            size_type result = 0;
            result = distance(begin(), end());
        }

        size_type max_size() const {
            return size_type(-1);
            // return std::numeric_limits<size_type>::max();
        }

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
        void emplace_bakc(Args&&... args) {
            emplace(end(), MySTL::forward(args)...);
        }

        void pop_bakc() {
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

        //参考 SGI STL 的实现
        /**
         * @description: 对链表进行归并排序
         * @param  {*}
         * @return {*}
         */
        template <class Compare>
        void sort(iterator f1, iterator f2, Compare compare) {
            if (node_->next == node_ || node_->next->next == node_) {
                return;
            }

            list carry;
            list counter[64];
            size_type fill = 0;
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());
                size_type i = 0;
                while(i < fill && !counter[i].empty()) {
                    counter[i].merge(carry);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if(i == fill) {
                    ++fill;
                }
            }

            // 合并 counter 中所有的链表
            for (size_type j = 0; j < fill; ++j) {
                counter[j].merge(counter[j - 1]);
            }

            swap(counter[fill - 1]);
        }


        void sort() {
            sort(begin(), end(), MySTL::less<T>());
        }

    private:
        link_type allocate_node() {
            return listNodeAllocator.allocate(1);
        }

        void deallocate_node(link_type node) {
            listNodeAllocator.deallocate(node);
        }

        link_type create_node(const_reference value) {
            auto newNode = allocate_node();
            return listNodeAllocator.construct(newNode, value);
        }

        link_type destroy_node(link_type node) {
            listNodeAllocator.destroy(node);
            deallocate_node(node);
        }
        
        /**
         * @description: 在 position 前插入一个值为 value 的节点
         * @param  {*}
         * @return {*}
         */
        template <class Y>
        iterator insert_aux(const_iterator position, Y&& value) {
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

    template <class T, class Allocator>
    bool operator!=(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return !(left == right);
    }

    template <class T, class Allocator>
    bool operator<(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return MySTL::lexicographical_compare(left.cbegin(), left.end(), right.cbegin(), right.cend());
    }

    template <class T, class Allocator>
    bool operator>(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return right < left;
    }

    bool operator<=(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        return !(left > right);
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
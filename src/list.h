#ifndef _LIST_H_
#define _LIST_H_

#include <initializer_list>

#include "allocator.h"
#include "type_traits"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "functional.h"
#include "algobase.h"
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

            list_iterator(link_type x) : node_(x){};

            list_iterator(const iterator &x) : node_(x.node_) {}

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
            // fixed : 前置 ++ -- 返回左值
            self& operator--()
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

            bool operator!=(const self &other) const
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
        using pointer                   = T*;
        using value_type                = T;
        using const_pointer             = const T*;
        using reference                 = T&;
        using const_reference           = const T&;
        using size_type                 = std::size_t;
        using difference_type           = std::ptrdiff_t;

        using iterator                  = detail::list_iterator<T, T&, T*>;
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
            node_->previous = node_->next = node_;
        }

        list(size_type count, const_reference value)
        { 
            node_ = allocate_node();
            node_->previous = node_->next = node_;
            insert(begin(), count, value); 
        }

        explicit list(size_type count)
        { 
            node_ = allocate_node();
            node_->previous = node_->next = node_;
            insert(begin(), count, value_type()); 
        }

        // 这边只接受传入迭代器
        template <class InputIterator, typename std::enable_if<
            MySTL::is_input_iterator<InputIterator>::value, int>::type = 0>
        list(InputIterator first, InputIterator last)
        { 
            node_ = allocate_node();
            node_->previous = node_->next = node_;
            insert(begin(), first, last);
        }

        list(const list &other) : list(other.begin(), other.end()) 
        { 
            node_ = allocate_node();
            node_->previous = node_->next = node_;
            insert(begin(), other.cbegin(), other.cend());
        }

        list(std::initializer_list<T> ilist)
        { 
            node_ = allocate_node();
            node_->previous = node_->next = node_;
            insert(begin(), ilist.begin(), ilist.end());
        }

        list(list &&other) noexcept : node_(other.node_) {
            // 其实不用，一开始就是空链表
            // move_from(other);
            other.node_ = nullptr;
        }

        list &operator=(const list &other)
        {
            if (this != &other)
            {
                iterator first1 = begin();
                iterator last1 = end();
                const_iterator first2 = other.begin();
                const_iterator last2 = other.end();
                while (first1 != last1 && first2 != last2) {
                    *first1++ = *first2++;
                }
                if (first2 == last2) {
                    erase(first1, last1);
                }
                else {
                    insert(last1, first2, last2);
                }
            }

            return *this;
        }

        list &operator=(list &&other) noexcept
        {
            if (this != &other)
            {
                delete_list();
                node_ = other.node_;
                other.node_ = nullptr;
            }

            return *this;
        }

        list &operator=(std::initializer_list<T> ilist)
        {
            list tmp(ilist.begin(), ilist.end());
            swap(tmp);

            return *this;
        }

        ~list()
        {
            delete_list();
        }

        // assin 的相关代码需要修改
        template <class InputIterator>
        void assign(InputIterator first, InputIterator last)
        {
            using is_Integral = typename MySTL::is_integral<InputIterator>;
            assign_dispatch(first, last, is_Integral());
        }
            
        void assign(size_type count, const_reference value)
        {
            fill_assign(count, value);
        }

        void assign(std::initializer_list<T> ilist)
        {
            assign_dispatch(ilist.begin(), ilist.end(), false_type());
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
            return static_cast<const_iterator>(node_->next);
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
            return node_->next == node_;
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


        // insert 
        iterator insert(const_iterator position, const_reference value) {
            return insert_aux(position, value);
        }

        iterator insert(const_iterator position, T&& value)  {
            return insert_aux(position, MySTL::move(value));
        }

        iterator insert(const_iterator position, size_type count, const_reference value) {
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

        // emplace
        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            return insert(position, T(MySTL::forward<Args>(args)...));
        }

        // erase clear
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);
        void clear() 
        { erase(begin(), end()); }

        // push_back pop_back 
        void push_back(const_reference value) {
            insert(end(), value);
        }

        void push_back(T&& value) {
            insert(end(), MySTL::move(value));
        }

        template <class... Args>
        void emplace_back(Args&&... args) {
            emplace(end(), MySTL::forward<Args>(args)...);
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
        void emplace_front(Args&&... args) {
            emplace(begin(), MySTL::forward<Args>(args)...);
        }

        void pop_front() {
            erase(begin());
        }

        // resize
        void resize(size_type count, const_reference value);

        void resize(size_type count) {
            resize(count, value_type());
        }

        void swap(list& other) {
            MySTL::swap(node_, other.node_);
        }


        /* list 相关操作 */

        // 合并两个已经排过序的 list，将 other 归并到 *this(first1) 中
        template <class Compare>
        void merge(list& other, Compare compare);

        /* C++ Primer P 587 在类模板自己的作用域中，我们可以直接使用模板名而不提供实参。比如 这里的形参类型，不是 list<T>, 而是直接写 list */
        void merge(list& other) {
            merge(other, MySTL::less<T>());
        }

        // TO DO
        // template <class Compare>
        // void merge(list&& other, Compare compare) {
        //     merge(other, compare);
        // }

        // void merge(list&& other) {
        //     merge(other, MySTL::less<T>());
        // }

        /**
         * @description: 将链表 other 接合到 position 所指的位置之前， other 必须不同于 *this
         * @param  {*}
         * @return {*}
         * @param {const_iterator} position
         */
        void splice(iterator position, list& other) {
            if (!other.empty()) {
                transfer(position, other.begin(), other.end());
            }
        }


        // 将 i 所指的元素接合于 position 所指位置之前。
        void splice(iterator position, list&, iterator it) {
            // it 如果 是下面这两种情况，没有意义，什么也不做
            if (position == it || position == it + 1) {
                return;
            }

            transfer(position, it, it + 1);
        }


        // 将 [first, last) 内的所有元素接合于 position 所指位置之前
        // position 和 [first, last) 可指向同一个 list
        // 但 position 不能位于 [first, last) 内
        void splice(iterator position, list&, iterator first, iterator last) {
            if (first != last) {
                transfer(position, first, last);
            }
        }

        /**
         * @description: 如果链表中有元素符合一元判断式，那么就把这个元素给删除
         * @param  {*}
         * @return {*}
         */
        template <class UnaryPredicate>
        void remove_if(UnaryPredicate p);

        // Lambda 的运用
        void remove(const_reference value) {
            remove_if([value](const_reference other) {
                return value == other;
            });
        }

        // 反转链表 TO DO
        void reverse();

        // 删除连续的重复元素
        template <class BinaryPredicate>
        void unique(BinaryPredicate pred);

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
        void sort(iterator f1, iterator f2, Compare compare);


        void sort() {
            sort(begin(), end(), MySTL::less<T>());
        }

    private:
        /* helper functions */

        /* 用 n 个元素为容器赋值 */
        void fill_assign(size_type n, const_reference value);

        

        template <class Integer>
        void assign_dispatch(Integer n, Integer value, true_type) {
            fill_assign(static_cast<size_type>(n), static_cast<T>(value));
        }

        template <class InputIterator>
        void assign_dispatch(InputIterator first2, InputIterator last2, false_type);

        link_type allocate_node() {
            return list_node_allocator::allocate(1);
        }

        void deallocate_node(link_type node) {
            list_node_allocator::deallocate(node);
        }

        template <class ...Args>
        link_type create_node(Args&&... args) {
            auto newNode = allocate_node();
            data_allocator::construct(&(newNode->data), MySTL::forward<Args>(args)...);

            return newNode;
        }

        void destroy_node(link_type node) {
            list_node_allocator::destroy(node);
            deallocate_node(node);
        }
        
        /**
         * @description: 在 position 前插入一个值为 value 的节点
         * @param  {*}
         * @return {*}
         */
        template <class Y>
        iterator insert_aux(const_iterator position, Y&& value);

        template <class InputIterator>
        iterator insert_range_aux(const_iterator position, InputIterator first, InputIterator last, true_type) {
            insert(position, first, last);
        }

        template <class InputIterator>
        iterator insert_range_aux(const_iterator position, InputIterator first, InputIterator last, false_type);

        /**
         * @description: 将 [first, last) 范围的节点移动到 position 之前。《STL源码剖析》 P139
         * @param  {*}
         * @return {*}
         * @param {iterator} position
         * @param {iterator} first
         * @param {iterator} last
         */
        void transfer(iterator position, iterator first, iterator last);

        // 清空链表
        void delete_list();

    }; // class list

    template <class T, class Allocator>
    typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator position) {
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

    template <class T, class Allocator>
    typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator first, iterator last) {
        while (first != last) {
            first = erase(first);
        }

        return static_cast<iterator>(last.node_);
    }

    template <class T, class Allocator>
    typename list<T, Allocator>::iterator list<T, Allocator>::erase(const_iterator position) {
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

    template <class T, class Allocator>
    typename list<T, Allocator>::iterator list<T, Allocator>::erase(const_iterator first, const_iterator last) {
        while (first != last) {
            first = erase(first);
        }

        return static_cast<iterator>(last.node_);
    }

    template <class T, class Allocator>
    void list<T, Allocator>::fill_assign(size_type n, const_reference value) {
        iterator i = begin();
        iterator e = end();
        for (; i != e && n > 0; --n, ++i) {
            *i = value;
        }
        if (n > 0) {
            insert(end(), n, value);
        }
        else {
            erase(i, e);
        }
     }

    template <class T, class Allocator>
    template <class InputIterator>
    void list<T, Allocator>::assign_dispatch(InputIterator first2, InputIterator last2, false_type) {
        iterator first1 = begin();
        iterator last1 = end();
        for(; first1 != last1 && first2 != last2; ++first1, ++first2) {
            *first1 = *first2;
        }
        if(first2 == last2) {
            erase(first1, last1);
        }
        else {
            insert(last1, first2, last2);
        }
    }

    template <class T, class Allocator>
    void list<T, Allocator>::resize(size_type count, const_reference value) {
        iterator i = begin();
        size_type len = 0;
        for(; i != end() && len < count; ++i, ++len) 
                ;
        if(len == count) {
            erase(i, end());
        }
        else {
            insert(end(), count - len, value);
        }
    }

    template <class T, class Allocator>
    template <class Compare>
    void list<T, Allocator>::merge(list& other, Compare compare) {
        if (this == &other) {
            return;
        }

        auto first1 = begin();
        auto last1 = end();
        auto first2 = other.begin();
        auto last2 = other.end();

        while (first1 != last1 && first2 != last2) {
            if (compare(*first2, *first1)) {
                auto next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            } else {
                ++first1;
            }
        }

        if (first2 != last2) {
            transfer(last1, first2, last2);
        }
    }

    template <class T, class Allocator>
    template <class UnaryPredicate>
    void list<T, Allocator>::remove_if(UnaryPredicate p) {
        auto first = begin();
        auto last = end();
        while (first != last) {
            if (UnaryPredicate(*first)) {
                // 返回下一个位置
                first = erase(first);
            } else {
                ++first;
            }
        }
    }

    // 反转链表 TO DO
    template <class T, class Allocator>
    void list<T, Allocator>::reverse() {
        // 表示链表没有节点或者只有一个节点，不用反转
        if (node_->next == node_ || node_->next->next == node_) {
            return;
        }

        // TO DO：这个暂时不能理解
        // using MySTL::swap;
        // swap(node_->previous, node_->next);
        // auto it = node->previous;
        // while (it != node_) {
        //     swap(it->previous, it->next);
        //     it = it->previous;
        // }

        // 《STL源码剖析》 P142
        iterator first = begin();
        ++first;
        while(first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }

    template <class T, class Allocator>
    template <class BinaryPredicate>
    void list<T, Allocator>::unique(BinaryPredicate pred) {
        // 如果是空链表或者是链表只有一个元素
        if (node_->next == node_ || node_->next->next == node_) {
            return;
        }

        auto first = begin();
        auto last = end();

        auto next = ++first;
        while (next != last) {
            if (pred(*first, *next)) {
                next = erase(next);
            } else {
                ++first;
                ++next;
            }
        }
    }

    // 参考这篇博文：https://www.kancloud.cn/digest/stl-sources/177268
    // 参考 SGI STL 的实现
    // 由于 STL 本身的排序算法 sort 接受的是随机访问迭代器，但是双向链表 list 的迭代器是双向迭代器，因此，不能使用 STL 本身的排序算法 sort ，必须得自己定义
    /**
     * @description: 对链表进行归并排序
     * @param  {*}
     * @return {*}
     */
    template <class T, class Allocator>
    template <class Compare>
    void list<T, Allocator>::sort(iterator f1, iterator f2, Compare compare) {
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


    /* private helper function */

    /**
     * @description: 在 position 前插入一个值为 value 的节点
     * @param  {*}
     * @return {*}
     */
    template <class T, class Allocator>
    template <class Y>
    typename list<T, Allocator>::iterator list<T, Allocator>::insert_aux(const_iterator position, Y&& value) {
        // C++ Primer P614 : 当用于一个指向模板参数类型的右值引用函数参数（T&&）时，forward 会保持实参类型的所有细节
        link_type newNode = create_node(MySTL::forward<Y>(value));
        newNode->previous = position.node_->previous;
        newNode->next     = position.node_;
        newNode->previous->next = newNode->next->previous = newNode;

        return static_cast<iterator>(newNode);
    }

    template <class T, class Allocator>
    template <class InputIterator>
    typename list<T, Allocator>::iterator list<T, Allocator>::insert_range_aux(const_iterator position, InputIterator first, InputIterator last, false_type) {
        iterator r(position.node_);

        if (first == last) {
            return r;
        }

        auto result = insert(position, *first);
        while (++first != last) {
            insert(position, *first);
        }

        return result;
    }

    /**
     * @description: 将 [first, last) 范围的节点移动到 position 之前。《STL源码剖析》 P139
     * @param  {*}
     * @return {*}
     * @param {iterator} position
     * @param {iterator} first
     * @param {iterator} last
     */
    template <class T, class Allocator>
    void list<T, Allocator>::transfer(iterator position, iterator first, iterator last) {
        auto positionNode = position.node_;
        auto firstNode = first.node_;
        auto lastNode = last.node_;

        // 这一段参看 《STL源码剖析》 P139
        lastNode->previous->next = positionNode;
        firstNode->previous->next = lastNode;
        positionNode->previous->next = firstNode;

        auto tmp = positionNode->previous;
        positionNode->previous = lastNode->previous;
        lastNode->previous = firstNode->previous;
        firstNode->previous = tmp;
    }

    // 清空链表
    template <class T, class Allocator>
    void list<T, Allocator>::delete_list() {
        if (node_ == nullptr) {
            return;
        }

        clear();
        // node_->data 为 nullptr，所以只需要回收内存空间即可
        deallocate_node(node_);
        node_ = nullptr;
    }

    template <class T, class Allocator>
    bool operator==(const list<T, Allocator>& left, const list<T, Allocator>& right) {
        using const_iterator = typename list<T, Allocator>::const_iterator;
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

    template <class T, class Allocator>
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
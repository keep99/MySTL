/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-05-07 16:50:59
 * @LastEditTime: 2021-05-11 15:08:06
 * @LastEditors: Chen.Yu
 */
#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include "allocator.h"
#include "iterator_base.h"
#include "algo.h"
#include "vector.h"
#include "allocator.h" 
#include "construct.h"
#include "utility.h"
#include "hash_fun.h"


namespace MySTL {
    template <class T>
    struct hashtable_node 
    {
        hashtable_node*     next;
        T                   value;
    };

    template <class Key, class Value, class HashFcn, class ExtractKey,
              class EqualKey, class Allocator = MySTL::allocator<Value>>
    class hashtable;

    template <class Key, class Value, class HashFcn, class ExtractKey, 
              class EqualKey, class Allocator>
    class hashtable_iterator;

    template <class Key, class Value, class HashFcn, class ExtractKey, 
              class EqualKey, class Allocator>
    struct hashtable_const_iterator;

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    struct hashtable_iterator
    {
        using value_type            = Value;
        using reference             = Value&;
        using pointer               = Value*;
        using difference_type       = std::ptrdiff_t;
        using iterator_category     = forward_iterator_tag;
        using size_type             = size_t;

        using iterator              = hashtable_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;
        using const_iterator        = hashtable_const_iterator<Key, Value, HashFcn, 
                                                               ExtractKey, EqualKey, Allocator>;
        
        using hashtable_type        = hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;

        using Node                  = hashtable_node<Value>;

        Node* cur_;             // 迭代器目前所指之节点
        hashtable_type* ht_;    // 保持对容器的连结关系（因为可能需要从 bucket 跳到 bucket）

        hashtable_iterator(Node* n, hashtable_type* tab) : cur_(n), ht_(tab) {}

        hashtable_iterator() {}

        reference operator*() const
        { return cur_ -> value; }

        pointer operator->() const 
        { return &(operator*()); }

        // 前置 ++
        // TODO
        iterator& operator++()
        {
            const Node* old = cur_;
            cur_ = cur_->next;
            if(!cur_) {
                size_type bucket = ht_->bkt_num(old->value);
                while(!cur && ++bucket < ht_->buckets_.size()) {
                    cur_ = ht_->buckets_[bucket];
                }
            }

            return *this;
        }

        // 后置 ++
        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this; // 调用 operator++

            return tmp;
        }

        bool operator==(const iterator& right) const
        { return cur_ == right.cur_; }

        bool operator!=(const iterator& right) const
        { return cur_ != right.cur_; }
    };

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    struct hashtable_const_iterator
    {
        using value_type            = Value;
        using reference             = const Value&;
        using pointer               = const Value*;
        using difference_type       = std::ptrdiff_t;
        using iterator_category     = forward_iterator_tag;
        using size_type             = size_t;

        using iterator              = hashtable_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;
        using const_iterator        = hashtable_const_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;
        
        using hashtable_type        = hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;

        using Node                  = hashtable_node<Value>;

        const Node* cur_;      // 迭代器目前所指之节点
        const hashtable_type* ht_;  // 保持对容器的连结关系（因为可能需要从 bucket 跳到 bucket）

        hashtable_const_iterator(const Node* n, const hashtable_type* tab) : cur_(n), ht_(tab) {}

        hashtable_const_iterator() {}

        reference operator*() const
        { return cur_ -> value; }

        pointer operator->() const 
        { return &(operator*()); }

        // 前置 ++
        // TO DO
        const_iterator& operator++()
        {
            const Node* old = cur_;
            cur_ = cur_->next;
            if(!cur_) {
                size_type bucket = ht_->bkt_num(old->value);
                while(!cur_ && ++bucket < ht_->buckets_.size()) {
                    cur_ = ht_->buckets_[bucket];
                }
            }

            return *this;
        }

        // 后置 ++
        const_iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this; // 调用 operator++

            return tmp;
        }

        bool operator==(const const_iterator& right) const
        { return cur_ == right.cur_; }

        bool operator!=(const const_iterator& right) const
        { return cur_ != right.cur_; }
    };

    // 虽然开链法并不要求表格大小必须为质数，但是仍然以质数来设计表格的大小
    // 并且先将28个质数（逐渐呈现大约两倍的关系）计算好，以被随时访问
    // 同时提供一个函数，用来查询这28个质数之中，“最接近某数并大于某数”的质数
    enum { num_primes = 29 };
    
    static const unsigned long prime_list[num_primes] = {
        5ul,          53ul,         97ul,         193ul,       389ul,
        769ul,        1543ul,       3079ul,       6151ul,      12289ul,
        24593ul,      49157ul,      98317ul,      196613ul,    393241ul,
        786433ul,     1572869ul,    3145739ul,    6291469ul,   12582917ul,
        25165843ul,   50331653ul,   100663319ul,  201326611ul, 402653189ul,
        805306457ul,  1610612741ul, 3221225473ul, 4294967291ul
    };

    inline unsigned long next_prime(unsigned long n)
    {
        const unsigned long* first = prime_list;
        const unsigned long* last = prime_list + num_primes;
        const unsigned long* pos = MySTL::lower_bound(first, last, n);

        return pos == last ? *(last - 1) : *pos;
    }

    // 前置声明
    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    class hashtable;
    
    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    bool operator==(const hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht1,
                    const hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht2);


    // 模板参数的含义
    // Key          节点的实值型别
    // Value        节点的键值型别
    // HashFcn      hash function 的函数型别
    // ExtractKey   从节点中取出键值的方法（函数或者仿函数）
    // EqualKey     判断键值是否相同的方法（函数或者仿函数）
    // Allocator    空间配置器
    template <class Key, class Value, class HashFcn, class ExtractKey, 
              class EqualKey, class Allocator> // 前置声明时，已经给了 Allocator 默认值
    class hashtable {
    public:
        // 声明为 友元类，因为 hashtable_iterator 和 hashtable_const_iterator 需要用到 hashtable 的私有成员
        friend struct hashtable_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;
        friend struct hashtable_const_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;

    public:
        using key_type              = Key;
        using value_type            = Value; 
        using hasher                = HashFcn;              // 哈希函数
        using key_equal             = EqualKey;             // 判断 key 是否相等的函数

        using size_type             = std::size_t;
        using difference_type       = std::ptrdiff_t;
        using reference             = value_type&;
        using const_reference       = const value_type&;
        using pointer               = value_type*;
        using const_pointer         = const value_type*;

        using iterator              = hashtable_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;
        using const_iterator        = hashtable_const_iterator<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>;


        using allocator_type        = Allocator;
        using data_allocator        = Allocator;

        allocator_type get_allocator() const { return allocator_type(); }

    private:
        using hashtable_node_allocator = typename Allocator::template rebind<node_type>::other;
        // using hashtable_node_pointer_allocator = typename Allocator::template rebind<hashtable_node<value_type>*>::other;

        using node_type             = hashtable_node<Value>;
        using node_ptr              = node_type*;

    private:
        // hashtable 的成员变量
        // 占大小
        hasher hash_;
        key_type equals_;
        ExtractKey getkey_;
        MySTL::vector<node_type*, Allocator> buckets_; // 以 vector 来完成，动态扩充能力
        size_t numElements_;

    public:
        // 构造、赋值、移动、析构函数
        hashtable(size_type n,
                  const HashFcn& hash,
                  const EqualKey& equals,
                  const ExtractKey& getkey)
            : hash_(hash),
              equals_(equals),
              getkey_(getkey),
              numElements_(0)
        {
            initialize_buckets(n);
        }

        hashtable(size_type n,
                  const HashFcn& hash,
                  const EqualKey& equals)
            : hash_(hash),
              equals_(equal),
              getkey_(ExtractKey()),
              numElements_(0)
        {
            initialize_buckets(n);
        }

        hashtable(const hashtable& other)
            : hash_(other.hash_),
              equals_(other.equals_),
              getkey_(other.getkey_),
              numElements_(0)
        {
            copy_init(other);
        }

        hashtable& operator=(const hashtable& other)
        {
            if(&other != this) {
                clear();
                hash_ = other.hash_;
                equals_ = other.equals_;
                getkey_ = other.getkey_;
                copy_init(other);
            }

            return *this;
        }

        hashtable(hashtable&& other) noexcept
            : hash_(other.hash_),
              equals_(other.equals_),
              getkey_(other.getkey_),
              numElements_(other.numElements_)
        {
            buckets_ = MySTL::move(other.buckets_);
        }

        hashtable& operator=(hashtable&& other) noexcept
        {
            hashtable tmp(MySTL::move(other));
            swap(tmp);

            return *this;
        }

        ~hashtable()
        { clear(); }

        
        // 迭代器相关操作
        iterator begin()
        {
            for(size_type n = 0; n < buckets_.size(); ++n) {
                if(buckets_[n] != nullptr) {
                    return iterator(buckets_[n], this);
                }
            }

            return end();
        }

        iterator begin() const
        {
            return cbegin();
        }

        iterator end() 
        {
            return iterator(nullptr, this);
        }

        iterator end() const
        {
            return cend();
        }

        const_iterator cbegin() const 
        {
            for(size_type n = 0; n < buckets_.size(); ++n) {
                if(buckets_[n] != nullptr) {
                    return const_iterator(buckets_[n], this);
                }
            }

            return cend();
        }

        const_iterator cend() const 
        {
            return const_iterator(nullptr, this);
        }

        template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
        friend bool operator==(const hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht1,
                               const hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht2);

        // 容器相关操作
        bool empty() const { return size() == 0; }
        size_type size() const { return numElements_; }
        size_type max_size() const { return size_type(-1); }

        // 修改容器相关操作

        // emplace / emplace_hint

        // insert 相关
        
        // 在不需要重建表格的情况下插入新节点。键值不允许重复
        MySTL::pair<iterator, bool> insert_unique_noresize(const value_type& value);

        // 在不需要重建表格的情况下插入新节点。键值允许重复
        iterator insert_equal_noresize(const value_type& value);

        
        MySTL::pair<iterator, bool> insert_unique(const value_type& value)
        {
            resize(numElements_ + 1);
            return insert_unique_noresize(value);
        }

        iterator insert_equal(const value_type& value)
        {
            resize(numElements_ + 1);
            return insert_equal_noresize(value);
        }

        template <class InputIterator>
        void insert_unique(InputIterator first, InputIterator last)
        {
            insert_unique(first, last, iterator_category(first));
        }

        template <class InputIterator>
        void insert_equal(InputIterator first, InputIterator last)
        {
            insert_equal(first, last, iterator_category(first));
        }

        template <class InputIterator>
        void insert_unique(InputIterator first, InputIterator last, 
                           input_iterator_tag)
        {
            for (; first != last; ++first) {
                insert_unique(*first);
            }
        }

        template <class InputIterator>
        void insert_equal(InputIterator first, InputIterator last, 
                          input_iterator_tag)
        {
            for (; first != last; ++first) {
                insert_equal(*first);
            }
        }

        template <class ForwardIterator>
        void insert_unique(ForwardIterator first, ForwardIterator last,
                           forward_iterator_tag)
        {
            size_type n = MySTL::distance(first, last);
            resize(numElements_ + n);
            for(; n > 0; --n, ++first) {
                insert_unique_noresize(*first);
            }
        }

        template <class ForwardIterator>
        void insert_equal(ForwardIterator first, ForwardIterator last,
                          forward_iterator_tag)
        {
            size_type n = MySTL::distance(first, last);
            resize(numElements_ + n);
            for(; n > 0; --n, ++first) {
                insert_equal_noresize(*first);
            }
        }

        // erase / clear / swap / resize
        // 删除键值为 key 的所有节点
        size_type erase(const key_type& key);
        // 删除迭代器 it 处的节点
        void erase(const iterator& it);
        // 删除迭代器 first 到 last 的节点
        void erase(iterator first, iterator last);
        void erase(const const_iterator& it);
        void erase(const_iterator first, const_iterator last);
        
        void clear();

        void swap(hashtable& ht);

        void resize(size_type num_elements_hint);


        // 查找相关操作

        reference find_or_insert(const value_type& obj);

        // 搜索键值为 key 的元素
        iterator find(const key_type& key);
        const_iterator find(const key_type& key) const;
        // 计算键值为 key 的元素个数
        size_type count(const key_type& key) const;

        pair<iterator, iterator>
        equal_range(const key_type& key);

        pair<const_iterator, const_iterator>
        euqal_range(const key_type& key) const;

        // bucket interface
        
        size_type bucket_count() const
        { return buckets_.size(); }

        size_type max_bucket_count() const
        { return prime_list[static_cast<int>(num_primes - 1)]; }

        // 在某个 bucket 中节点的个数
        size_type elems_in_bucket(size_type n) const
        {
            size_type result = 0;
            for(node_ptr cur = buckets_[n]; cur; cur = cur -> next) {
                ++result;
            }

            return result;
        }

        // // hash policy

        // float load_factor() const
        // {}

        // float max_load_factor() const
        // {}

        // void rehash(size_type count);

        // void reserve(size_type count)
        // {}

        hasher hash_fcn() const { return hash_; }
        key_equal key_eq() const { return equals_; }
        
    private:
        // helper function

        // initialize_buckets
        void initialize_buckets(size_type n);
        void copy_init(const hashtable& ht);

        // node
        node_ptr new_node(const value_type& obj);
        void destroy_node(node_ptr n);


        // hash
        size_type next_size(size_type n) const;
        // size_type hash(const key_type& key, size_type n) const;
        // size_type hash(const key_type& key);
        // void rehash_if_need(size_type n);

        // insert node
        // pair<iterator, bool> insert_node_unique(node_ptr np);
        // iterator insert_node_multi(node_ptr np);

        // bucket operator
        // 在第 n 个 bucket 内，删除 [first, last) 的节点
        void erase_bucket(size_type n, node_ptr first, node_ptr last);
        // 在第 n 个 bucket 内，删除 [buckets_[n], last) 的节点
        void erase_bucket(size_type n, node_ptr last);

        // 判断元素的落脚处 bkt_num
        // 版本一：接受实值 和 buckets 个数
        size_type bkt_num(const value_type& obj, size_t n) const 
        { return bkt_num_key(get_key(obj), n); }

        // 版本二：只接受实值
        size_type bkt_num(const value_type& obj) const
        { return bkt_num_key(get_key(obj)); }

        // 版本三：只接受键值
        size_type bkt_num_key(const key_type& key) const
        { return bkt_num_key(key, buckets_.size()); }

        // 版本四：接受键值 和 buckets 个数
        size_type bkt_num_key(const key_type& key, size_t n) const
        { return hash_(key) % n; }
    };

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    bool operator==(const hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht1,
                               const hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht2)
    {
        using Node = typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::Node;
        if(ht1.size() != ht2.size()) {
            return false;
        }

        for(int n = 0; n < ht1.buckets_.size(); ++n) {
            Node* cur1 = ht1.buckets_[n];
            Node* cur2 = ht2.buckets_[n];
            for(; cur1 && cur2 && cur1 -> value == cur2 -> value; cur1 = cur1 -> next, cur2 = cur2 -> next) {}
            if(cur1 || cur2) {
                return false;
            }
        }

        return true;
    }

    // 重载 MySTL 的 swap
    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void swap(hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht1,
              hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>& ht2)
    {
        ht1.swap(ht2);
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    MySTL::pair<typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::iterator, bool> 
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        insert_unique_noresize(const value_type& value)
    {
        const size_type n = bkt_num(value);
        node_type* first = buckets_[n];

        // 如果 buckets_[n] 已经被占用，此时 first 不为 0，于是进入以下循环
        // 走过 bucket 所对应的整个链表
        for (node_type* cur = first; cur; cur = cur -> next) {
            if (equals_(getkey_(cur -> value), getkey_(value))) {
                // 如果发现与链表中的某键值相同，就不插入，立刻返回
                return pair<iterator, bool>(iterator(cur, this), false);
            }
        }

        node_type* tmp = new_node(value);
        tmp->next = first;
        buckets_[n] = tmp;
        ++numElements_;
        return MySTL::pair<iterator, bool>(iterator(tmp, this), true);
    }

    // 在不需要重建表格的情况下插入新节点。键值允许重复
    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::iterator
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        insert_equal_noresize(const value_type& value)
    {
        const size_type n = bkt_num(value);
        node_type* first = buckets_[n];

        for (node_type* cur = first; cur; cur = cur -> next) {
            // 如果发现与链表中的某键值相同，就马上插入，然后返回
            if (equals_(getkey_(cur -> value), getkey_(value))) {
                node_type* tmp = new_node(value);
                tmp->next = cur->next;
                cur->next = tmp;
                ++numElements_;
                return iterator(tmp, this);
            }
        }

        // 进行至此，没有发现重复的键值
        node_type* tmp = new_node(value);
        tmp->next = first;
        buckets_[n] = tmp;
        ++numElements_;
        return iterator(tmp, this);
    }



    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::size_type 
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        erase(const key_type& key)
    {
        const size_type n = bkt_num_key(key);
        node_type* first = buckets_[n];
        size_type erased = 0;

        if (first) {
            // 相当于删除链表中的节点的操作
            node_type* cur = first;
            node_type* cur_next = cur->next;
            while (cur_next) {
                if (equals_(getkey_(cur_next->value), key)) {
                    cur->next = cur_next->next;
                    destroy_node(cur_next);
                    cur_next = cur->next;
                    ++erased;
                    --numElements_;
                }
                else {
                    cur = cur_next;
                    cur_next = cur->next;
                }
            }

            if(equals_(getkey_(first->value), key)) {
                buckets_[n] = first->next;
                destroy_node(first);
                ++erased;
                --numElements_;
            }
        }

        return erased;
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        erase(const iterator& it)
    {
        node_type* p = it.cur_;
        if (p) {
            const size_type n = bkt_num(p->value);
            node_type* cur = buckets_[n];

            if (cur == p) { // 如果要删除的节点就在 buckets_[n] 的头部，直接删除
                buckets_[n] = cur->next;
                destroy_node(cur);
                --numElements_;
            }
            else { // 否则，需要找到这个节点，然后删除
                node_type* next = cur->next;
                while(next) {
                    if(next == p) {
                        cur->next = next->next;
                        destroy_node(next);
                        --numElements_;
                        break;
                    }
                    else {
                        cur = next;
                        next = cur->next;
                    }
                }
            }
        }
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        erase(iterator first, iterator last)
    {
        size_type f_bucket = first.cur_ ? bkt_num(first.cur_->value) : buckets_.size();
        size_type l_bucket = last.cur_ ? bkt_num(last.cur_->value) : buckets_.size();

        if (first.cur_ == last.cur_) {
            return;
        }
        else if (f_bucket == l_bucket) {
            // 如果 bucket 在同一个位置
            erase_bucket(f_bucket, first.cur_, last.cur_);
        }
        else {
            erase_bucket(f_bucket, first.cur_, 0);
            for (size_type n = f_bucket + 1; n < l_bucket; ++n) {
                erase_bucket(n, 0);
            }
            if (l_bucket != buckets_.size()) {
                erase_bucket(l_bucket, last.cur_);
            }
        }
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        erase(const const_iterator& it)
    {
        erase(iterator(const_cast<node_type*>(it.cur_), const_cast<node_type*>(it.ht_)));
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::erase(const_iterator first, const_iterator last)
    {
        erase(iterator(const_cast<node_type*>(first.cur_), const_cast<hashtable*>(first.ht_)),
              iterator(const_cast<node_type*>(last.cur_), const_cast<hashtable*>(last.ht_)));
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        clear()
    {
        for(size_type i = 0; i < buckets_.size(); ++i) {
            node_ptr cur = buckets_[i];
            while(cur != nullptr) {
                // 将 bucket list 中的每一个节点删除掉
                node_ptr next = cur -> next;
                destroy_node(cur);
                cur = next;
            }
            buckets_[i] = nullptr;  // 令 bucket 内容为 空
        }

        numElements_ = 0;  // 令总节点个数为0

        // 注意，buckets vector 并未释放掉空间，仍保持原来大小
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        swap(hashtable& ht)
    {
        MySTL::swap(hash_, ht.hash_);
        MySTL::swap(equals_, ht.equals_);
        MySTL::swap(getkey_, ht.getkey_);
        buckets_.swap(ht.buckets_);
        MySTL::swap(numElements_, ht.numElements_);
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        resize(size_type num_elements_hint)
    {
        // 表格重建与否的判断原则，拿元素个数（将新增元素计入后）和 bucket vector 的大小来比，如果前者大于后者，就重建表格
        // 由此可以判知，每个 bucket 最多放 buckets_.size() 个节点
        const size_type old_n = buckets_.size();
        if (numElements_ > old_n) {
            const size_type n = next_size(num_elements_hint);
            if (n > old_n) {
                vector<node_type*, Allocator> tmp(n, static_cast<node_type*>(0)); // 设立新的 buckets
                try
                {
                    for (size_type bucket = 0; bucket < old_n; ++bucket) {
                        node_type* first = buckets_[bucket];  // 指向节点所对应的串行的起始节点
                        // 以下处理每一个旧的 bucket 所含的每一个节点
                        while (first) {
                            // 以下找出节点落在哪一个新的 bucket 内
                            size_type new_bucket = bkt_num(first -> val, n);
                            // (1) 令旧的 bucket 指向其所对应之串行的下一个节点（以便迭代处理）
                            buckets_[bucket] = first -> next;
                            // (2)(3) 将当前节点插入到新 bucket 内，成为其对应串行的第一个节点
                            first -> next = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            // (4) 回到旧的 bucket 所指的待处理串行，准备处理下一个节点
                            first = buckets[bucket];
                        }
                    }
                    buckets_.swap(tmp);
                }
                catch(...)
                {
                    for(size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                        while(tmp[bucket]) {
                            node_type* next = tmp[bucket] -> next;
                            destroy_node(tmp[bucket]);
                            tmp[bucket] = next;
                        }
                    }

                    throw;
                }
            }
        }
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>    
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::reference 
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        find_or_insert(const value_type& obj)
    {
        resize(numElements_ + 1);

        size_type n = bkt_num(obj);
        node_type* first = buckets_[n];

        for(node_type* cur = first; cur; cur = cur->next) {
            if(equals_(getkey_(cur->value), getkey_(obj))) {
                return cur->value;
            }
        }

        node_type* tmp = new_node(obj);
        tmp->next = first;
        buckets_[n] = tmp;
        ++numElements_;
        return tmp->value;
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::iterator 
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        find(const key_type& key)
    {
        size_type n = bkt_num_key(key);
        node_type* first;
        for(first = buckets_[n];
            first && equals_(getkey_(first->value), key);
            first = first->next)
        { 
        }
        return iterator(first, this);
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::const_iterator 
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        find(const key_type& key) const
    {
        size_type n = bkt_num_key(key);
        node_type* first;
        for(first = buckets_[n];
            first && equals_(getkey_(first->value), key);
            first = first->next)
        { 
        }
        return const_iterator(first, this);
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::size_type 
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        count(const key_type& key) const
    {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        // 以下，从 bucket list 的头开始，一一对比每个元素的键值，对比成功就累加1
        for(const node_type* cur = buckets_[n]; cur; cur = cur->next) {
            if(equals_(getkey_(cur->value), key)) {
                ++result;
            }
        }

        return result;
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    pair<typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::iterator,
         typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::iterator>
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        equal_range(const key_type& key)
    {
        using Pii = pair<iterator, iterator>;
        const size_type n = bkt_num_key(key); // 决定 key 位于 #n bucket
        
        // 如果 buckets_[n] 已被占用，此时 first 不为 0，进入下面的循环
        for (node_type* first = buckets_[n]; first; first = first->next) { 
            // 在 list 中找到 一个节点，其键值等于 key，进入以下循环
            // 然后再找最后一个
            if (equals_(getkey_(first->value), key)) {
                for(node_type* cur = first->next; cur; cur = cur->next) {
                    // 如果找到一个节点的键值不等于 key，则返回
                    if(!equals_(getkey_(first->value), key)) {
                        return Pii(iterator(first, this), iterator(cur, this));
                    }
                }
                // 如果找到最后都没有找到一个节点的键值不等于 key，进入下一个 非空 bucket
                // 返回的迭代器，一个是指向 第一个键值等于 key 的节点，另外一个是下一个非空 bucket 的头节点
                for(size_type m = n + 1; m < buckets_.size(); ++m) {
                    if(buckets_[m]) {
                        return Pii(iterator(first, this), iterator(buckets_[m], this));
                    }
                }

                return Pii(iterator(first, this), end());
            }
        }

        return Pii(end(), end());
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    pair<typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::const_iterator,
         typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::const_iterator>
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        euqal_range(const key_type& key) const
    {
        using Pii = pair<const_iterator, const_iterator>;
        const size_type n = bkt_num_key(key); // 决定 key 位于 #n bucket
        
        // 如果 buckets_[n] 已被占用，此时 first 不为 0，进入下面的循环
        for (node_type* first = buckets_[n]; first; first = first->next) { 
            // 在 list 中找到 一个节点，其键值等于 key，进入以下循环
            // 然后再找最后一个
            if (equals_(getkey_(first->value), key)) {
                for(node_type* cur = first->next; cur; cur = cur->next) {
                    // 如果找到一个节点的键值不等于 key，则返回
                    if(!equals_(getkey_(first->value), key)) {
                        return Pii(const_iterator(first, this), const_iterator(cur, this));
                    }
                }
                // 如果找到最后都没有找到一个节点的键值不等于 key，进入下一个 非空 bucket
                // 返回的迭代器，一个是指向 第一个键值等于 key 的节点，另外一个是下一个非空 bucket 的头节点
                for(size_type m = n + 1; m < buckets_.size(); ++m) {
                    if(buckets_[m]) {
                        return Pii(const_iterator(first, this), const_iterator(buckets_[m], this));
                    }
                }

                return Pii(const_iterator(first, this), end());
            }
        }

        return Pii(end(), end());
    }


    /************************************************************************************************************/
    // helper function
    
    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        initialize_buckets(size_type n)
    {
        const size_type bucket_nums = next_size(n);  // 返回最接近 n 并大于或等于 n 的质数
        buckets_.reserve(bucket_nums);
        buckets_.insert(buckets_.end(), bucket_nums, static_cast<node_type*>(0));
        numElements_ = 0;
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        copy_init(const hashtable& ht)
    {
        buckets_.clear();
        buckets_.reserve(ht.buckets_.size());
        buckets_.insert(buckets_.end(), ht.buckets_.size(), static_cast<node_type*>(0));
        try
        {
            for(size_type i = 0; i < ht.buckets_.size(); ++i) {
                node_ptr cur = ht.buckets_[i];
                if(cur) {
                    node_ptr copy = new_node(cur -> value);
                    buckets_[i] = copy;

                    for(node_ptr next = cur -> next; next; cur = next, next = cur -> next) { // 复制链表
                        copy -> next = new_node(next -> value);
                        copy = copy -> next;
                    }
                }
            }
            numElements_ = ht.numElements_;
        }
        catch(...)
        {
            clear();
        }
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::node_ptr
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        new_node(const value_type& obj)
    {
        node_ptr tmp = hashtable_node_allocator::allocate(1);
        tmp -> next = nullptr;
        try
        {
            data_allocator::construct(&tmp ->value, obj);
        }
        catch(...)
        {
            data_allocator::deallocate(tmp);
            throw;
        }

        return tmp;
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        destroy_node(node_ptr n)
    {
        data_allocator::destroy(&node -> val);
        hashtable_node_allocator::deallocate(n);
        n = nullptr;
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    typename hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::size_type
    hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        next_size(size_type n) const
    {
        return next_prime(n);
    }

    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        erase_bucket(size_type n, node_ptr first, node_ptr last)
    {
        node_type* cur = buckets_[n];
        if (cur == first) {
            erase_bucket(n, last);
        }
        else {
            node_type* cur_next;
            for(cur_next = cur->next; cur_next != first; cur = next, next = cur->next) {}
            while(cur_next != last) {
                cur->next = cur_next->next;
                destroy_node(cur_next);
                cur_next = cur->next;
                --numElements_;
            }
        }
    }
    
    template <class Key, class Value, class HashFcn, class ExtractKey, class EqualKey, class Allocator>
    void hashtable<Key, Value, HashFcn, ExtractKey, EqualKey, Allocator>::
        erase_bucket(size_type n, node_ptr last)
    {
        node_type* cur = buckets_[n];
        while (cur != last) {
            node_type* next = cur->next;
            destroy_node(cur);
            cur = next;
            buckets_[n] = cur;
            --numElements_;
        }
    }
} // namespace MySTL

#endif
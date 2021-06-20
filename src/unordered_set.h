#ifndef _UNORDERED_SET_H
#define _UNORDERED_SET_H

#include "hashtable.h"
#include "functional.h"
#include "hash_fun.h"
#include "allocator.h"

#include <initializer_list>

namespace MySTL {
    /*****************************************************************************************/

    // 模板类 unordered_set，键值不允许重复
    // 参数一代表键值类型，参数二代表哈希函数，缺省使用 mystl::hash，
    // 参数三代表键值比较方式，缺省使用 mystl::equal_to
    template <class Value,
              class HashFcn = MySTL::hash<Value>,
              class EqualKey = MySTL::equal_to<Value>,
              class Allocator = MySTL::allocator<Value>>
    class unordered_set;

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    inline bool 
    operator==(const unordered_set<Value, HashFcn, EqualKey, Allocator>& us1,
               const unordered_set<Value, HashFcn, EqualKey, Allocator>& us2);
    
    template <class Value, class HashFcn, class EqualKey, class Allocator>
    class unordered_set
    {
    private:
        using hashtable_type = hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Allocator>;
        hashtable_type ht_; // 底层以 hashtable 完成

    public:
        using key_type          = typename hashtable_type::key_type;
        using value_type        = typename hashtable_type::value_type;
        using hasher            = typename hashtable_type::hasher;
        using key_equal         = typename hashtable_type::key_equal;
        
        using size_type         = typename hashtable_type::size_type;
        using difference_type   = typename hashtable_type::difference_type;
        using pointer           = typename hashtable_type::const_pointer;
        using const_pointer     = typename hashtable_type::const_pointer;
        using reference         = typename hashtable_type::const_reference;
        using const_reference   = typename hashtable_type::const_reference;
        
        using iterator          = typename hashtable_type::const_iterator;
        using const_iterator    = typename hashtable_type::const_iterator;

        using allocator_type    = typename hashtable_type::allocator_type;

        hasher hash_fcn() const { return ht_.hash_fcn(); }
        key_equal key_eq() const { return ht_.key_eq(); }
        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:
        // 构造、赋值、移动、析构函数
        unordered_set()
            : ht_(100, hasher(), key_equal())
        {}

        explicit unordered_set(size_type bucket_size)
            : ht_(bucket_size, hasher(), key_equal())
        {}

        unordered_set(size_type bucket_size, const hasher& hf)
            : ht_(bucket_size, hf, key_equal())
        {}

        template <class InputIterator>
        unordered_set(InputIterator first, InputIterator last,
                      const size_type bucket_size = 100,
                      const hasher& hf = hasher(),
                      const EqualKey& equal = key_equal())
            : ht_(bucket_size, hf, equal)
        {
            ht_.insert_unique(first, last);
        }

        // 考虑到简单实现，删除
        // unordered_set(std::initializer_list<value_type> ilist,
        //               const size_type bucket_size = 100,
        //               const hasher& hf = hasher(),
        //               const EqualKey& equal = key_equal())
        //     : ht_(bucket_size, hf, equal)
        // {
        //     for(auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
        //         ht_.insert_unique(*first);
        // }

        unordered_set(const unordered_set& rhs)
            : ht_(rhs.ht_)
        {}

        unordered_set(const unordered_set&& rhs) noexcept
            : ht_(MySTL::move(rhs.ht_))
        {}

        unordered_set& operator=(const unordered_set& rhs)
        {
            // 其实这里的自赋值处理可以不用，hashtable 保证
            if(this != &rhs) {
                ht_ = rhs.ht_;
            }

            return *this;
        }
        
        unordered_set& operator=(unordered_set&& rhs) noexcept
        {
            // 其实这里的自赋值处理可以不用，hashtable 保证
            if(this != &rhs) {
                ht_ = MySTL::move(rhs.ht_);
            }

            return *this;
        }

        // 考虑到简单实现，删除
        // unordered_set& operator=(std::initializer_list<value_type> ilist)
        // {
            
        // }

        ~unordered_set() = default;

        // 迭代器相关

        iterator begin()
        { return ht_.begin(); }
        const_iterator begin() const
        { return ht_.begin(); }
        iterator       end()
        { return ht_.end(); }
        const_iterator end() const
        { return ht_.end(); }

        const_iterator cbegin() const
        { return ht_.cbegin(); }
        const_iterator cend() const
        { return ht_.cend(); }

        // 容量相关
        
        size_type size() const { return ht_.size(); }
        size_type max_size() const { return ht_.max_size(); }
        bool empty() const { return ht_.empty(); }
        void swap(unordered_set& us) { ht_.swap(us.ht_); }
        
        // 修改容器操作

        // insert
        pair<iterator, bool> insert(const value_type& value)
        { return ht_.insert_unique(value); }
        
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            ht_.insert_unique(first, last);
        }

        // insert_noresize
        pair<iterator, bool> insert_noresize(const value_type& obj) {
            pair<typename hashtable_type::iterator, bool> p = ht_.insert_unique_noresize(obj);
            return pair<iterator, bool>(p.first, p.second);
        }

        iterator find(const key_type& key) const { return ht_.find(key); }

        size_type count(const key_type& key) const { return ht_.count(key); }

        pair<iterator, iterator> equal_range(const key_type& key) const 
        { return ht_.equal_range(key); }

        size_type erase(const key_type& key) { return ht_.erase(key); }
        
        void erase(iterator it) { return ht_.erase(it); }

        void erase(iterator f, iterator l) { return ht_.erase(f, l); }

        void clear() { ht_.clear(); }

    public:
        void resize(size_type hint) { return ht_.resize(hint); }

        size_type bucket_count() const { return ht_.bucket_count(); }

        size_type max_bucket_count() const { return ht_.max_bucket_count(); }

        size_type elems_in_bucket(size_type n) const { return ht_.elems_in_bucket(n); }
    };

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    inline bool 
    operator==(const unordered_set<Value, HashFcn, EqualKey, Allocator>& us1,
               const unordered_set<Value, HashFcn, EqualKey, Allocator>& us2)
    {
        return us1.ht_ == us2.ht_;
    }

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    inline bool 
    operator!=(const unordered_set<Value, HashFcn, EqualKey, Allocator>& us1,
               const unordered_set<Value, HashFcn, EqualKey, Allocator>& us2)
    {
        return !(us1 == us2);
    }

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    void swap(unordered_set<Value, HashFcn, EqualKey, Allocator>& us1,
              unordered_set<Value, HashFcn, EqualKey, Allocator>& us2)
    {
        us1.swap(us2);
    }


    /*****************************************************************************************/

    // 模板类 unordered_multiset，键值允许重复
    // 参数一代表键值类型，参数二代表哈希函数，缺省使用 mystl::hash，
    // 参数三代表键值比较方式，缺省使用 mystl::equal_to
    template <class Value,
              class HashFcn = MySTL::hash<Value>,
              class EqualKey = MySTL::equal_to<Value>,
              class Allocator = MySTL::allocator<Value>>
    class unordered_multiset;

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    inline bool 
    operator==(const unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us1,
               const unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us2);
    
    template <class Value, class HashFcn, class EqualKey, class Allocator>
    class unordered_multiset
    {
    private:
        using hashtable_type = hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Allocator>;
        hashtable_type ht_; // 底层以 hashtable 完成

    public:
        using key_type          = typename hashtable_type::key_type;
        using value_type        = typename hashtable_type::value_type;
        using hasher            = typename hashtable_type::hasher;
        using key_equal         = typename hashtable_type::key_equal;
        
        using size_type         = typename hashtable_type::size_type;
        using difference_type   = typename hashtable_type::difference_type;
        using pointer           = typename hashtable_type::const_pointer;
        using const_pointer     = typename hashtable_type::const_pointer;
        using reference         = typename hashtable_type::const_reference;
        using const_reference   = typename hashtable_type::const_reference;
        
        using iterator          = typename hashtable_type::const_iterator;
        using const_iterator    = typename hashtable_type::const_iterator;

        using allocator_type    = typename hashtable_type::allocator_type;

        hasher hash_fcn() const { return ht_.hash_fcn(); }
        key_equal key_eq() const { return ht_.key_eq(); }
        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:
        // 构造、赋值、移动、析构函数
        unordered_multiset()
            : ht_(100, hasher(), key_equal())
        {}

        explicit unordered_multiset(size_type bucket_size)
            : ht_(bucket_size, hasher(), key_equal())
        {}

        unordered_multiset(size_type bucket_size, const hasher& hf)
            : ht_(bucket_size, hf, key_equal())
        {}

        template <class InputIterator>
        unordered_multiset(InputIterator first, InputIterator last,
                           const size_type bucket_size = 100,
                           const hasher& hf = hasher(),
                           const EqualKey& equal = key_equal())
            : ht_(bucket_size, hf, equal)
        {
            ht_.insert_equal(first, last);
        }

        // 考虑到简单实现，删除
        // unordered_set(std::initializer_list<value_type> ilist,
        //               const size_type bucket_size = 100,
        //               const hasher& hf = hasher(),
        //               const EqualKey& equal = key_equal())
        //     : ht_(bucket_size, hf, equal)
        // {
        //     for(auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
        //         ht_.insert_equal(*first);
        // }

        unordered_multiset(const unordered_multiset& rhs)
            : ht_(rhs.ht_)
        {}

        unordered_multiset(const unordered_multiset&& rhs) noexcept
            : ht_(MySTL::move(rhs.ht_))
        {}

        unordered_multiset& operator=(const unordered_multiset& rhs)
        {
            // 其实这里的自赋值处理可以不用，hashtable 保证
            if(this != &rhs) {
                ht_ = rhs.ht_;
            }

            return *this;
        }
        
        unordered_multiset& operator=(unordered_multiset&& rhs) noexcept
        {
            // 其实这里的自赋值处理可以不用，hashtable 保证
            if(this != &rhs) {
                ht_ = MySTL::move(rhs.ht_);
            }

            return *this;
        }

        // 考虑到简单实现，删除
        // unordered_multiset& operator=(std::initializer_list<value_type> ilist)
        // {
            
        // }

        ~unordered_multiset() = default;

        // 迭代器相关

        iterator begin()
        { return ht_.begin(); }
        const_iterator begin() const
        { return ht_.begin(); }
        iterator       end()
        { return ht_.end(); }
        const_iterator end() const
        { return ht_.end(); }

        const_iterator cbegin() const
        { return ht_.cbegin(); }
        const_iterator cend() const
        { return ht_.cend(); }

        // 容量相关
        
        size_type size() const { return ht_.size(); }
        size_type max_size() const { return ht_.max_size(); }
        bool empty() const { return ht_.empty(); }
        void swap(unordered_multiset& us) { ht_.swap(us.ht_); }
        
        // 修改容器操作

        // insert
        pair<iterator, bool> insert(const value_type& value)
        { return ht_.insert_equal(value); }
        
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            ht_.insert_equal(first, last);
        }

        // insert_noresize
        pair<iterator, bool> insert_noresize(const value_type& obj) {
            pair<typename hashtable_type::iterator, bool> p = ht_.insert_equal_noresize(obj);
            return pair<iterator, bool>(p.first, p.second);
        }

        iterator find(const key_type& key) const { return ht_.find(key); }

        size_type count(const key_type& key) const { return ht_.count(key); }

        pair<iterator, iterator> equal_range(const key_type& key) const 
        { return ht_.equal_range(key); }

        size_type erase(const key_type& key) { return ht_.erase(key); }
        
        void erase(iterator it) { return ht_.erase(it); }

        void erase(iterator f, iterator l) { return ht_.erase(f, l); }

        void clear() { ht_.clear(); }

    public:
        void resize(size_type hint) { return ht_.resize(hint); }

        size_type bucket_count() const { return ht_.bucket_count(); }

        size_type max_bucket_count() const { return ht_.max_bucket_count(); }

        size_type elems_in_bucket(size_type n) const { return ht_.elems_in_bucket(n); }
    };

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    inline bool 
    operator==(const unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us1,
               const unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us2)
    {
        return us1.ht_ == us2.ht_;
    }

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    inline bool 
    operator!=(const unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us1,
               const unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us2)
    {
        return !(us1 == us2);
    }

    template <class Value, class HashFcn, class EqualKey, class Allocator>
    void swap(unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us1,
              unordered_multiset<Value, HashFcn, EqualKey, Allocator>& us2)
    {
        us1.swap(us2);
    }
} // namespace MySTL




#endif
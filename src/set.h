/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-05-14 19:21:28
 * @LastEditTime: 2021-05-15 03:45:25
 * @LastEditors: Chen.Yu
 */
#ifndef _SET_H_
#define _SET_H_
#include "rb_tree.h"
#include "functional.h"
#include "allocator.h"
#include "utility.h"
#include "algobase.h"

namespace MySTL {
    // set
    template <class Key, class Compare = MySTL::less<Key>, class Allocator = MySTL::allocator<Key>>
    class set
    {
    public:
        using key_type              = Key;
        using value_type            = Key;
        using key_compare           = Compare;
        using value_compare         = Compare;

    private:
        using rb_tree_type          = MySTL::rb_tree<value_type, key_type, MySTL::identity<value_type>, Compare, Allocator>;
        rb_tree_type tree_;    // 采用红黑树来实现

    public:
        using size_type                 = typename rb_tree_type::size_type;
        using difference_type           = typename rb_tree_type::difference_type;
        using reference                 = typename rb_tree_type::const_reference;
        using const_reference           = typename rb_tree_type::const_reference;
        using pointer                   = typename rb_tree_type::const_pointer;
        using const_pointer             = typename rb_tree_type::const_pointer;
        // iterator 定义为 RB-tree 的 const_iterator ，这表示 set 的迭代器无法执行写入操作。
        // 这是因为 set 的元素有一定次序安排，不允许用户在任意处进行写入操作
        using iterator                  = typename rb_tree_type::const_iterator;
        using const_iterator            = typename rb_tree_type::const_iterator;
        using reverse_iterator          = typename rb_tree_type::const_reverse_iterator;
        using const_reverse_iterator    = typename rb_tree_type::const_reverse_iterator;

    public:
        set() {}
        set(const set& x) : tree_(x.tree_) {}
        set& operator=(const set& s) { tree_ = s.tree_;}

        template <class InputIterator>
        set(InputIterator first, InputIterator last) {
            tree_.insert_unique(first, last);
        }

        iterator               begin()         noexcept
        { return tree_.begin(); }
        const_iterator         begin()   const noexcept
        { return tree_.begin(); }
        iterator               end()           noexcept
        { return tree_.end(); }
        const_iterator         end()     const noexcept
        { return tree_.end(); }

        reverse_iterator       rbegin()        noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator rbegin()  const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator       rend()          noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator rend()    const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator         cbegin()  const noexcept
        { return begin(); }
        const_iterator         cend()    const noexcept
        { return end(); }
        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }
        const_reverse_iterator crend()   const noexcept
        { return rend(); }




	  	bool empty()const {return tree_.empty();}
	  	size_type size()const {return tree_.size();}
	  	void swap(set& x) {tree_.swap(x.tree_);}

	  	pair<iterator,bool> insert(const value_type& x)
	  	{
	  		MySTL::pair<typename rb_tree_type::iterator, bool> p = tree_.insert_unique(x);
	  		return MySTL::pair<iterator, bool>(p.first, p.second);
	  	}
        
        iterator insert(iterator position, const value_type& x) {
            using tree_iterator = typename rb_tree_type::iterator;
            return tree_.insert_unique((tree_iterator&)position, x);
        }

	  	template<class InputIterator>
	  	void insert(InputIterator first,InputIterator last)
	  	{
	  		tree_.insert_unique(first, last);
	  	}

	  	void erase(iterator position)
	  	{
            using tree_iterator = typename rb_tree_type::iterator;
	  		tree_.erase((tree_iterator&)position);
	  	}

	  	size_type erase(const key_type& x)
	  	{
	  	    return tree_.erase(x);
	  	}

	  	void erase(iterator first,iterator last)
	  	{
            using tree_iterator = typename rb_tree_type::iterator;
	  		tree_.erase((tree_iterator&)first,(tree_iterator&)last);
        }

        void clear() {tree_.clear();}

        iterator find(const key_type& x) const { return tree_.find(x);}
        size_type count(const key_type& x) const { return tree_.count(x);}
        iterator lower_bound(const key_type& x) const {return tree_.lower_bound(x);}
        iterator upper_bound(const key_type& x) const {return tree_.upper_bound(x);}
        pair<iterator,iterator> equal_range(const key_type& x) const {return tree_.equal_range(x);}

        friend bool operator==(const set&, const set&);
        friend bool operator<(const set&, const set&);
    };

    template <class Key, class Compare, class Allocator>
    bool operator== (const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs)
    {
        return lhs.size() == rhs.size() && MySTL::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class Key, class Compare, class Allocator>
    bool operator< (const set<Key, Compare, Allocator>& lhs, const set<Key, Compare, Allocator>& rhs)
    {
        return MySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    // multiset
    template <class Key, class Compare = MySTL::less<Key>, class Allocator = MySTL::allocator<Key>>
    class multiset
    {
    public:
        using key_type              = Key;
        using value_type            = Key;
        using key_compare           = Compare;
        using value_compare         = Compare;

    private:
        using rb_tree_type          = MySTL::rb_tree<value_type, key_type, MySTL::identity<value_type>, Compare, Allocator>;
        rb_tree_type tree_;    // 采用红黑树来实现

    public:
        using size_type                 = typename rb_tree_type::size_type;
        using difference_type           = typename rb_tree_type::difference_type;
        using reference                 = typename rb_tree_type::const_reference;
        using const_reference           = typename rb_tree_type::const_reference;
        using pointer                   = typename rb_tree_type::const_pointer;
        using const_pointer             = typename rb_tree_type::const_pointer;
        // iterator 定义为 RB-tree 的 const_iterator ，这表示 multiset 的迭代器无法执行写入操作。
        // 这是因为 multiset 的元素有一定次序安排，不允许用户在任意处进行写入操作
        using iterator                  = typename rb_tree_type::const_iterator;
        using const_iterator            = typename rb_tree_type::const_iterator;
        using reverse_iterator          = typename rb_tree_type::const_reverse_iterator;
        using const_reverse_iterator    = typename rb_tree_type::const_reverse_iterator;

    public:
        multiset() {}
        multiset(const multiset& x) : tree_(x.tree_) {}
        multiset& operator=(const multiset& s) { tree_ = s.tree_;}

        template <class InputIterator>
        multiset(InputIterator first, InputIterator last) {
            tree_.insert_equal(first, last);
        }

        iterator               begin()         noexcept
        { return tree_.begin(); }
        const_iterator         begin()   const noexcept
        { return tree_.begin(); }
        iterator               end()           noexcept
        { return tree_.end(); }
        const_iterator         end()     const noexcept
        { return tree_.end(); }

        reverse_iterator       rbegin()        noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator rbegin()  const noexcept
        { return const_reverse_iterator(end()); }
        reverse_iterator       rend()          noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator rend()    const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator         cbegin()  const noexcept
        { return begin(); }
        const_iterator         cend()    const noexcept
        { return end(); }
        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }
        const_reverse_iterator crend()   const noexcept
        { return rend(); }




	  	bool empty()const {return tree_.empty();}
	  	size_type size()const {return tree_.size();}
	  	void swap(multiset& x) {tree_.swap(x.tree_);}

	  	pair<iterator,bool> insert(const value_type& x)
	  	{
	  		MySTL::pair<typename rb_tree_type::iterator, bool> p = tree_.insert_equal(x);
	  		return MySTL::pair<iterator, bool>(p.first, p.second);
	  	}
        
        iterator insert(iterator position, const value_type& x) {
            using tree_iterator = typename rb_tree_type::iterator;
            return tree_.insert_equal((tree_iterator&)position, x);
        }

	  	template<class InputIterator>
	  	void insert(InputIterator first,InputIterator last)
	  	{
	  		tree_.insert_equal(first, last);
	  	}

	  	void erase(iterator position)
	  	{
            using tree_iterator = typename rb_tree_type::iterator;
	  		tree_.erase((tree_iterator&)position);
	  	}

	  	size_type erase(const key_type& x)
	  	{
	  	    return tree_.erase(x);
	  	}

	  	void erase(iterator first,iterator last)
	  	{
            using tree_iterator = typename rb_tree_type::iterator;
	  		tree_.erase((tree_iterator&)first,(tree_iterator&)last);
        }

        void clear() {tree_.clear();}

        iterator find(const key_type& x) const { return tree_.find(x);}
        size_type count(const key_type& x) const { return tree_.count(x);}
        iterator lower_bound(const key_type& x) const {return tree_.lower_bound(x);}
        iterator upper_bound(const key_type& x) const {return tree_.upper_bound(x);}
        pair<iterator,iterator> equal_range(const key_type& x) const {return tree_.equal_range(x);}

        friend bool operator==(const multiset&, const multiset&);
        friend bool operator<(const multiset&, const multiset&);
    };

    template <class Key, class Compare, class Allocator>
    bool operator== (const multiset<Key, Compare, Allocator>& lhs, const multiset<Key, Compare, Allocator>& rhs)
    {
        return lhs.size() == rhs.size() && MySTL::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class Key, class Compare, class Allocator>
    bool operator< (const multiset<Key, Compare, Allocator>& lhs, const multiset<Key, Compare, Allocator>& rhs)
    {
        return MySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

} // namespace MySTL

#endif
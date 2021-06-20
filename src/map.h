#ifndef _MAP_H_
#define _MAP_H_
#include "rb_tree.h"
#include "functional.h"
#include "allocator.h"
#include "utility.h"
#include "algobase.h"

namespace MySTL {
    // map
    template <class Key, class T, class Compare = MySTL::less<Key>, class Allocator = MySTL::allocator<Key>>
    class map
    {
    public:
        using key_type              = Key;
        using data_type             = T;
        using mapped_type           = T;
        using value_type            = pair<const Key, T>;
        using key_compare           = Compare;
        using value_compare         = Compare;

    private:
        using rb_tree_type          = MySTL::rb_tree<key_type, value_type, MySTL::selectfirst<value_type>, Compare, Allocator>;
        rb_tree_type tree_;    // 采用红黑树来实现

    public:
        using size_type                 = typename rb_tree_type::size_type;
        using difference_type           = typename rb_tree_type::difference_type;
        using reference                 = typename rb_tree_type::const_reference;
        using const_reference           = typename rb_tree_type::const_reference;
        using pointer                   = typename rb_tree_type::const_pointer;
        using const_pointer             = typename rb_tree_type::const_pointer;
        // map 并没有像 set 一样将 iterator 定义为 RB-tree 的
        // const_iterator。因为它允许用户通过其迭代器在修改元素的实值（value）。
        using iterator                  = typename rb_tree_type::iterator;
        using const_iterator            = typename rb_tree_type::const_iterator;
        using reverse_iterator          = typename rb_tree_type::const_reverse_iterator;
        using const_reverse_iterator    = typename rb_tree_type::const_reverse_iterator;

    public:
        map() {}
        map(const map& x) : tree_(x.tree_) {}
        map& operator=(const map& s) { tree_ = s.tree_;}

        template <class InputIterator>
        map(InputIterator first, InputIterator last) {
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
	  	void swap(map& x) {tree_.swap(x.tree_);}

	  	pair<iterator,bool> insert(const value_type& x)
	  	{
	  		return tree_.insert_unique(x);
	  	}

        pair<iterator,bool> insert(value_type&& x)
	  	{
	  		return tree_.insert_unique(MySTL::move(x));
	  	}
        
        iterator insert(iterator position, const value_type& x) {
            return tree_.insert_unique(position, x);
        }

	  	template<class InputIterator>
	  	void insert(InputIterator first,InputIterator last)
	  	{
	  		tree_.insert_unique(first, last);
	  	}

	  	void erase(iterator position)
	  	{
	  		tree_.erase(position);
	  	}

	  	size_type erase(const key_type& x)
	  	{
	  	    return tree_.erase(x);
	  	}

	  	void erase(iterator first,iterator last)
	  	{
	  		tree_.erase(first, last);
        }

        void clear() {tree_.clear();}

        iterator find(const key_type& x) { return tree_.find(x); }
        const_iterator find(const key_type& x) const { return tree_find(x); }
        size_type count(const key_type& x) { return tree_.count(x); }
        iterator lower_bound(const key_type& x) { return tree_.lower_bound(x); }
        const_iterator lower_bound(const key_type& x) const { return tree_.lower_bound(x); } 
        iterator upper_bound(const key_type& x) { return tree_.upper_bound(x); }
        const_iterator upper_bound(const key_type& x) const { return tree_.upper_bound(x); } 
        pair<iterator,iterator> equal_range(const key_type& x) { return tree_.equal_range(x); }
        pair<const_iterator,const_iterator> equal_range(const key_type& x) const { return tree_.equal_range(x); }

        template <class K1, class T1, class C1, class A1>
        friend bool operator==(const map&, const map&);
        template <class K1, class T1, class C1, class A1>
        friend bool operator<(const map&, const map&);
    };

    template <class K1, class T1, class C1, class A1>
    bool operator== (const map<K1, T1, C1, A1>& lhs, const map<K1, T1, C1, A1>& rhs)
    {
        return lhs.size() == rhs.size() && MySTL::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class K1, class T1, class C1, class A1>
    bool operator< (const map<K1, T1, C1, A1>& lhs, const map<K1, T1, C1, A1>& rhs)
    {
        return MySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    // multimap
    template <class Key, class T, class Compare = MySTL::less<Key>, class Allocator = MySTL::allocator<Key>>
    class multimap
    {
    public:
        using key_type              = Key;
        using data_type             = T;
        using mapped_type           = T;
        using value_type            = pair<const Key, T>;
        using key_compare           = Compare;
        using value_compare         = Compare;

    private:
        using rb_tree_type          = MySTL::rb_tree<key_type, value_type, MySTL::identity<value_type>, Compare, Allocator>;
        rb_tree_type tree_;    // 采用红黑树来实现

    public:
        using size_type                 = typename rb_tree_type::size_type;
        using difference_type           = typename rb_tree_type::difference_type;
        using reference                 = typename rb_tree_type::const_reference;
        using const_reference           = typename rb_tree_type::const_reference;
        using pointer                   = typename rb_tree_type::const_pointer;
        using const_pointer             = typename rb_tree_type::const_pointer;
        // multimap 并没有像 set 一样将 iterator 定义为 RB-tree 的
        // const_iterator。因为它允许用户通过其迭代器在修改元素的实值（value）。
        using iterator                  = typename rb_tree_type::iterator;
        using const_iterator            = typename rb_tree_type::const_iterator;
        using reverse_iterator          = typename rb_tree_type::const_reverse_iterator;
        using const_reverse_iterator    = typename rb_tree_type::const_reverse_iterator;

    public:
        multimap() {}
        multimap(const multimap& x) : tree_(x.tree_) {}
        multimap& operator=(const multimap& s) { tree_ = s.tree_;}

        template <class InputIterator>
        multimap(InputIterator first, InputIterator last) {
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
	  	void swap(multimap& x) {tree_.swap(x.tree_);}

	  	pair<iterator,bool> insert(const value_type& x)
	  	{
	  		return tree_.insert_equal(x);
	  	}
        
        iterator insert(iterator position, const value_type& x) {
            return tree_.insert_equal(position, x);
        }

	  	template<class InputIterator>
	  	void insert(InputIterator first,InputIterator last)
	  	{
	  		tree_.insert_equal(first, last);
	  	}

	  	void erase(iterator position)
	  	{
	  		tree_.erase(position);
	  	}

	  	size_type erase(const key_type& x)
	  	{
	  	    return tree_.erase(x);
	  	}

	  	void erase(iterator first,iterator last)
	  	{
	  		tree_.erase(first, last);
        }

        void clear() {tree_.clear();}

        iterator find(const key_type& x) { return tree_.find(x); }
        const_iterator find(const key_type& x) const { return tree_find(x); }
        size_type count(const key_type& x) { return tree_.count(x); }
        iterator lower_bound(const key_type& x) { return tree_.lower_bound(x); }
        const_iterator lower_bound(const key_type& x) const { return tree_.lower_bound(x); } 
        iterator upper_bound(const key_type& x) { return tree_.upper_bound(x); }
        const_iterator upper_bound(const key_type& x) const { return tree_.upper_bound(x); } 
        pair<iterator,iterator> equal_range(const key_type& x) { return tree_.equal_range(x); }
        pair<const_iterator,const_iterator> equal_range(const key_type& x) const { return tree_.equal_range(x); }

        template <class K1, class T1, class C1, class A1>
        friend bool operator==(const multimap&, const multimap&);
        template <class K1, class T1, class C1, class A1>
        friend bool operator<(const multimap&, const multimap&);
    };

    template <class K1, class T1, class C1, class A1>
    bool operator== (const multimap<K1, T1, C1, A1>& lhs, const multimap<K1, T1, C1, A1>& rhs)
    {
        return lhs.size() == rhs.size() && MySTL::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template <class K1, class T1, class C1, class A1>
    bool operator< (const multimap<K1, T1, C1, A1>& lhs, const multimap<K1, T1, C1, A1>& rhs)
    {
        return MySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

} // namespace MySTL

#endif
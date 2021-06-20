#ifndef _STACK_H_
#define _STACK_H_
#include "deque.h"

namespace MySTL {
    template <class T, class Container = MySTL::deque<T>>
    class stack
    {
    public:
        using container_type    = Container;
        using value_type        = typename Container::value_type;
        using size_type         = typename Container::size_type;
        using reference         = typename Container::reference;
        using const_reference   = typename Container::const_reference; 

    private:
        container_type c;  // 底层容器
    
    public:
        stack() = default;

        explicit stack(size_type n) : c(n)
        {
        }

        stack(size_type n, const value_type& value) : c(n, value)
        {
        }

        template <class InputIter>
        stack(InputIter first, InputIter last) : c(first, last)
        {
        }

        stack(std::initializer_list<T> ilist)  : c(ilist.begin(), ilist.end())
        {
        }

        stack(const Container& c) : c(c)
        {
        }

        stack& operator=(const stack& rhs)
        {
            c = rhs.c;
            return *this;
        }

        stack& operator=(std::initializer_list<T> ilist) 
        {
            c = ilist; 
            return *this;
        }

        ~stack() = default;

        // 访问元素相关操作
        reference       top()       { return c.back(); }
        const_reference top() const { return c.back(); }

        // 容量相关操作
        bool      empty() const noexcept { return c.empty(); }
        size_type size()  const noexcept { return c.size(); }

        // 修改容器相关操作

        template <class... Args>
        void emplace(Args&& ...args)
        { c.emplace_back(mystl::forward<Args>(args)...); }

        void push(const value_type& value)
        { c.push_back(value); }
        void push(value_type&& value)      
        { c.push_back(mystl::move(value)); }

        void pop() 
        { c.pop_back(); }

        void clear() 
        {
            while (!empty())
            pop();
        }

        void swap(stack& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))
        { mystl::swap(c, rhs.c); }

    public:
    friend bool operator==(const stack& lhs, const stack& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator<(const stack& lhs, const stack& rhs) { return lhs.c_ <  rhs.c_; }
    };

    // 重载比较操作符
    template <class T, class Container>
    bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return lhs == rhs;
    }

    template <class T, class Container>
    bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return lhs < rhs;
    }

    template <class T, class Container>
    bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T, class Container>
    bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return rhs < lhs;
    }

    template <class T, class Container>
    bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T, class Container>
    bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(lhs < rhs);
    }

    // 重载 mystl 的 swap
    template <class T, class Container>
    void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }    

} // namespace MySTL
#endif

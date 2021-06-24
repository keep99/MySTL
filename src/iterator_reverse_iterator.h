#ifndef _ITERATOR_REVERSE_ITERATOR_H
#define _ITERATOR_REVERSE_ITERATOR_H

#include "iterator_base.h"

namespace toystl {
    // 反向迭代器
    template <class Iterator>
    class reverse_iterator {
    private:
        Iterator current_;
    public:
        using iterator_category = typename iterator_traits<Iterator>::iterator_category;
        using value_type        = typename iterator_traits<Iterator>::value_type;
        using difference_type   = typename iterator_traits<Iterator>::difference_type;
        using pointer           = typename iterator_traits<Iterator>::pointer;
        using reference         = typename iterator_traits<Iterator>::reference;

        using iterator_type     = Iterator;

        reverse_iterator() {}
        explicit reverse_iterator(iterator_type& x) : current_(x) {}
        
        template <class U>
        explicit reverse_iterator(const reverse_iterator<U>& other) : current_(other.base()) {}
        
        template <class U>
        reverse_iterator& operator=(const reverse_iterator<U>& other) {
            current_ = other.base();
            return *this;
        }

        iterator_type base() const {
            return current_;
        }

        // TO DO
        reference operator*() const {
            return *(current_ - 1);
            // 以上比较关键，堆逆向迭代器取值，就是将“对应正向迭代器”后退一格而后取值
        }

        pointer operator->() const {
            return &(operator*());
        }

        // operator++ 是后退
        reverse_iterator& operator++() {
            --current_;
            return *this;
        }

        // operator++ 是后退
        reverse_iterator& operator++(int) {
            reverse_iterator tmp = *this;
            --current_;
            return tmp;
        }

        // operator-- 是前进
        reverse_iterator& operator--() {
            ++current_;
            return *this;
        }

        // operator-- 是前进
        reverse_iterator& operator--(int) {
            reverse_iterator tmp = *this;
            ++current_;
            return tmp;
        }

        reverse_iterator operator+(difference_type n) const {
            return reverse_iterator(current_ - n);
        }

        reverse_iterator operator+=(difference_type n) {
            current_ -= n;
            return *this;
        }

        reverse_iterator operator-(difference_type n) const {
            return reverse_iterator(current_ + n);
        }

        reverse_iterator& operator-=(difference_type n) {
            current_ += n;
            return *this;
        }

        reference operator[](difference_type n) const {
            return *((*this + n));
        }
    };

    //全局函数，重载 == != > < >= <= + -
    template <class Iterator>
    bool operator==(reverse_iterator<Iterator> left, reverse_iterator<Iterator> right) {
        return left.base() == right.base();
    }
    template <class Iterator>
    bool operator!=(reverse_iterator<Iterator> left, reverse_iterator<Iterator> right) {
        return left.base() != right.base();
    }

    // >
    // >= < <= 都调用 >
    template <class Iterator>
    inline bool operator>(reverse_iterator<Iterator> left, reverse_iterator<Iterator> right) {
        return right.base() > left.base();
    }

    // >=
    template <class Iterator>
    inline bool operator>=(reverse_iterator<Iterator> left, reverse_iterator<Iterator> right) {
        return !(right > left);
    }
    
    // <
    template <class Iterator>
    inline bool operator<(reverse_iterator<Iterator> left, reverse_iterator<Iterator> right) {
        return right > left;
    }

    // <=
    template <class Iterator>
    inline bool operator<=(reverse_iterator<Iterator> left, reverse_iterator<Iterator> right) {
        return !(left > right);
    }

    template <class Iterator>
    inline reverse_iterator<Iterator> operator+(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) 
    {
        return reverse_iterator<Iterator>(it.base() - n);
    }

    template <class Iterator>
    inline typename reverse_iterator<Iterator>::difference_type operator-(
           const reverse_iterator<Iterator>& x,
           const reverse_iterator<Iterator>& y) {
        return y.base() - x.base();
    }
    
    /**
     * @description: 将普通迭代去转换成反向迭代器
     * @param  {*}
     * @return {*}
     */

    
    template <class Iterator>
    reverse_iterator<Iterator> make_reverse_iterator(Iterator it) {
        return reverse_iterator<Iterator>(it);
    }
    
}

#endif
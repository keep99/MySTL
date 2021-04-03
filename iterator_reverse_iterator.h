/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-02 14:50:09
 * @LastEditTime: 2021-04-02 21:52:09
 * @LastEditors: Chen.Yu
 */
#ifndef _ITERATOR_REVERSE_ITERATOR_H
#define _ITERATOR_REVERSE_ITERATOR_H

#include "iterator_base.h"

namespace MySTL {
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

        reverse_iterator() : current_() {}
        explicit reverse_iterator(current) : current_(current) {}
        
        template <class U>
        explicit reverse_iterator(const reverse_iterator<U>& other) : current_(other.base()) {}

        
        template <class U>
        reverse_iterator& operator=(const reverse_iterator<U>& other) {
            current_ = other.base();
            return *this;
        }

        /**
         * @description: 获取普通迭代器
         * @param  {*}
         * @return {*}
         */
        iterator_type base() const {
            return current_;
        }

        reference operator*() const {
            return *(current_ - 1);
        }

        pointer operator->() const {
            return &(operator*());
        }

        reverse_iterator& operator++() {
            --current_;
            return *this;
        }

        reverse_iterator& operator++(int) {
            reverse_iterator tmp = *this;
            --current_;
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
            return *operator+(n);
        }
    };

    //全局函数，重载 == != > < >= <= + -
    template <class LeftIterator, class RightIterator>
    bool operator==(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() == right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator!=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() != right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator>(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() > right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator>=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() >= right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator<(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() < right.base();
    }
    template <class LeftIterator, class RightIterator>
    bool operator<=(reverse_iterator<LeftIterator> left, reverse_iterator<RightIterator> right) {
        return left.base() <= right.base();
    }
    template <class Iterator>
    reverse_iterator<Iterator> operator+(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) {
        return it + n;
    }
    template <class Iterator>
    reverse_iterator<Iterator> operator-(
            typename reverse_iterator<Iterator>::difference_type n,
            const reverse_iterator<Iterator> &it) {
        return it - n;
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
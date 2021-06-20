#ifndef _ITERATOR_MOVE_ITERATOR_H
#define _ITERATOR_MOVE_ITERATOR_H

#include "iterator_base.h"
#include "utility.h"             

namespace MySTL {
    template <class Iterator>
    class move_iterator {
    public:
        using iterator_category         = typename iterator_traits<Iterator>::iterator_category;
        using value_type                = typename iterator_traits<Iterator>::value_type;
        using difference_type           = typename iterator_traits<Iterator>::difference_type;
        using pointer                   = typename iterator_traits<Iterator>::pointer;
        using reference                 = typename iterator_traits<Iterator>::reference;
        using iterator_type             = Iterator;

    private:
        Iterator current_;
        
    public:
        move_iterator() = default;
        explicit move_iterator(iterator_type i) : current_(i) {}
        
        template <class U>
        explicit move_iterator(const move_iterator<U>& other) : current_(other.current_) {}

        template <class U>
        move_iterator& operator=(const move_iterator<U>& other) {
            current_ = other.current_;
        }

        iterator_type base() const {
            return current_;
        }
        
        // 在解引用的时候强制转换成右值引用，实现从一个容器中 "移走" 所有的元素
        // 返回当前元素的右值引用
        reference operator*() {
            return MySTL::move(*current_);
        }

        pointer operator->() const {
            return base();
        }

        // C++ Primer P605 尾置返回类型
        auto operator[](difference_type n) const -> decltype(MySTL::move(current_[n])) {
            return MySTL::move(current_[n]);
        }

        //前置++
        move_iterator& operator++() {
            ++current_;

            return *this;
        }

        //后置++
        move_iterator& operator++(int) {
            move_iterator tmp = *this;
            ++current_;

            return tmp;
        }

        //前置--
        move_iterator& operator--() {
            --current_;

            return *this;
        }

        //后置--
        move_iterator& operator--(int) {
            move_iterator tmp = *this;
            --current_;

            return tmp;
        }

        move_iterator operator+(difference_type n) const {
            return move_iterator(current_ + n);
        }

        move_iterator operator-(difference_type n) const {
            return move_iterator(current_ - n);
        }

        move_iterator operator+=(difference_type n) {
            current_ += n;

            return *this;
        }

        move_iterator operator-=(difference_type n) {
            current_ -= n;
            
            return *this;
        }
    };

    template <class Iterator1, class Iterator2>
    bool operator==(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) {
        return lhs.base() == rhs.base();
    } 

    // != 通过 == 来实现
    template <class Iterator1, class Iterator2>
    bool operator!=(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) {
        return !(lhs == rhs);
    }

    template <class Iterator1, class Iterator2>
    bool operator<(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) {
        return lhs.base() < rhs.base();
    }

    template <class Iterator1, class Iterator2>
    bool operator>(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) {
        return rhs < lhs;
    }

    template <class Iterator1, class Iterator2>
    bool operator<=(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) {
        return !(rhs < lhs);
    }

    template <class Iterator1, class Iterator2>
    bool operator>=(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) {
        return !(lhs < rhs);
    }

    template <class Iterator>
    move_iterator<Iterator> operator+(typename move_iterator<Iterator>::difference_type n, const move_iterator<Iterator>& it) {
        return it + n;
    }

    template <class Iterator1, class Iterator2> 
    auto operator-(const move_iterator<Iterator1>& lhs, const move_iterator<Iterator2>& rhs) -> decltype(lhs.base() - rhs.base()) {
        return lhs.base() - rhs.base();
    }
}

#endif

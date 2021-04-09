/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-03 14:03:48
 * @LastEditTime: 2021-04-09 16:34:13
 * @LastEditors: Chen.Yu
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"
#include "memory_function.h"
#include "algorithm_base.h"  //TO DO

namespace MySTL {
    template<class T, class Alloc = allocator<T>> 
    class vector {
    public:
        // vetor 的嵌套型别定义
        // typedef T           value_type;
        // typedef T*          pointer;
        // typedef T*          iterator;
        // typedef const T*    const_iterator;
        // typedef T&          reference;
        // typedef size_t      size_type;
        // typedef ptrdiff_t   difference_type;

        using value_type             = T;
        using pointer                = T*;
        using const_pointer          = const T*;
        using iterator               = T*;
        using const_iterator         = const T*;
        using reference              = T&;
        using const_reference        = const T&;
        using size_type              = size_t;
        using difference_type        = ptrdiff_t;

        using reverse_iterator       = MySTL::reverse_iterator<iterator>;
        using const_reverse_iterator = MySTL::reverse_iterator<const_iterator>;

        using allocator_type         = allocator;

    private:    
        allocator_type dataAllocator;       //内存空间配置器

        iterator start_;
        iterator finish_;
        iterator end_of_storage_;

        // 扩容因子 设为 1.5
        constexpr static double grow_factor = 1.5;

    public:
        // 构造函数
        vector() : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}
        vector(size_type n, const T& value) { fill_initialize(n, value); }
        vector(int       n, const T& value) { fill_initialize(n, value); }
        vector(long      n, const T& value) { fill_initialize(n, value); }
        
        explicit vector(size_type n) : vector(n, T()) {}

        template <class InputIterator>
        vector(InputIterator first, InputIterator last) : vector() {
            initialize_aux(first, last, is_integral<InputIterator>());
        } 
    
        // 列表初始化
        vector(const std::initializer_list<T> iList) : vector(iList.begin(), iList.end())
        {}

        // 拷贝构造函数
        vector(const vector& other) : vector(other.begin(), other.end()) {}

        // 移动构造函数
        vector(const vector&& other) noexcept: vector() {
            swap(other);
        }

        // 析构函数
        ~vector() {
            clear();
            if (start_ != nullptr) {
                dataAllocator::deallocate(start_, capacity());
            }
        }

        // 重载 =
        vector& operator=(const vector& rhs) {
            if(this != rhs) {
                vector tmp(rhs);
                swap(tmp);
            }

            return *this;
        }

        vector& operator=(vector &&rhs) noexcept {
            if (this != &rhs) {
                swap(rhs);
            }
            return *this;
        }

        vector& operator=(std::initializer_list<T> iList) {
            *this = vector(iList);
            return *this;
        }

        // 赋值函数
        vector& assign(size_type n, const_reference value) {
            return *this = vector(n, value);
        }

        template <class InputIterator>
        vector& assign(InputIterator first, InputIterator last) {
            return *this = vector(first, last);
        }

        vector& assign(std::initializer_list<T> ilist) {
            return *this = vector(ilist);
        }

        reference at(size_type position) {
            return const_cast<reference>(check_and_at(position)); // 返回位于指定位置 pos 的元素的引用，有边界检查
        }

        const_reference at(size_type position) const {
            return check_and_at(position);
        }
        
        reference operator[](size_type position) {
            return *(begin() + position);
        }

        const_reference operator[](size_type position) const {
            return *(begin() + position);
        }

        reference front() {
            return *begin();
        }

        const_reference front() const {
            return *begin();
        }

        reference back() {
            return *(end() - 1);
        }

        const_reference back() const {
            return *(end() - 1);
        }

        //元素访问相关函数
        pointer data() {
            return start_;
        }

        const_pointer data() const {
            return start_;
        }

        iterator begin() {
            return start_;
        }

        const_iterator begin() const {
            return start_;
        }

        const_iterator cbegin() const {
            return start_;
        }

        iterator end() {
            return finish_;
        }

        const_iterator end() const {
            return finish_;
        }

        const_iterator cend() const {
            return finish_;
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

        reverse_iterator rend() const {
            return static_cast<reverse_iterator>(begin());
        }

        reverse_iterator crend() const {
            return static_cast<reverse_iterator>(begin());
        }

        bool empty() const {
            return cbegin() == cend();
        }

        size_type  size() const {
            return cend() - cbegin():
        }

        // 返回 vector 可容纳元素的最大个数
        size_type max_size() const {
            return size_type(-1) / sizeof(T);
            // return std::numeric_limit<size_t>::max();
        }

        size_type capacity() const {
            return end_of_storage_ - start_;
        }
        
        // 如果 vector 不断地 push_back 元素，会进行内存的重新分配问题，涉及到分配内存空间，拷贝元素，开销大
        // 为了避免重新分配内存带来的问题，vector提供了reserve函数。
        // 如果 newCapacity 大于当前的 capacity_，则分配新存储；否则，不做任何事情。
        void reserve(size_type newCapacity) {
            if (newCapacity > capacity()) {
                reallocate(newCapacity);
            }
        }

        // resize():重新申请并改变当前vector对象的有效空间大小
        // reserve():重新申请并改变当前vector对象的总空间（_capacity）大小
        void resize(size_type newSize, const_reference x) {
            if(newSize < size()) {
                erase(begin() + newSize, end());
            }
            else {
                insert(end(), newSize - size(), x);
            }
        }

        void shrink_to_fit() {
            if (capacity() > size()) {
                reallocate(size());
            }
        }

        void clear() {
            erase(begin(), end());
        }

        iterator insert(const_iterator position, const_reference value) {
            return insert_aux(position, value);    
        }

        iterator insert(const_iterator position, const T&& value) {
            return insert_aux(position, MySTL::move(value));
        }

        iterator insert(const_iterator position, size_type count, const_reference value) {
            vector tmp(count, value);
            return insert(position, tmp.begin(), tmp.end());
        }

        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last) {
            // 如果目前可用空间容纳的下 [first, last)
            auto newPosition = const_cast<iterator>(position);            
            if (last - first <= capacity() - size()) {
                // 插入点之后的现有元素个数 大于 新增元素个数
                // [postion, cend()) 可以容纳得下 [first, last)
                if (cend() - position >= last - end()) {
                    // 1、因为end()后面都是未使用的空间，所以不仅仅要拷贝，还需要构造，所以不能把这一步和下面一步合成一步来做
                    MySTL::uninitialized_copy(cend() - (last - first), cend(), end());
                    // TO DO : copy_backward
                    // 2、把 [postion, cend(0 - (last - first)) 区间内的每一个元素，以逆行得方向复制到以 end() - 1 为起点，方向也是逆行得区间上。
                    MySTL::copy_backward(position, cend() - (last - first), end()); 
                    // 3、最后把 [first, last) 复制到指定位置
                    MySTL::copy(first, last, newPosition);
                    finish_ += last - first;
                }
                else {
                // [position, cend()) 容不下 [first, last)
                // 1、先将容不下得 [first, end) 的部分先构造、复制到 end() 的起始位置
                // 2、将[position, cend()) 部分的元素 拷贝到 新的末尾
                // 3、最后将 [first, last) 剩下的部分 复制到 指定的位置
                auto insertEndIterator = MySTL::uninitialized_copy(first + (cend() - position), last, end());
                MySTL::uninitialized_copy(position, cend(), insertEndIterator);
                MySTL::copy(first, first + (cend() - position), newPosition); // fixed
                }
                return newPosition;
            }
            
            // 目前的可用空间容不下 [first, last)。那就必须配置额外的内存
            size_type oldSize = size();
            size_type newSize = oldSize != 0 ? 2 * oldSize : 1;

            vector tmp;
            tmp.start_ = dataAllocator::allocate(newSize);
            iterator newPosition = tmp.start_ + (position - cbegin());

            // commit or rollback 
            // 要么产生处所有必要元素，要么不产生任何元素
            try {
                tmp.finish_ = MySTL::uninitialized_copy(cbegin(), position, tmp.start_);
                tmp.finish_ = MySTL::uninitialized_copy(first, last, tmp.finish_);
                tmp.finish_ = MySTL::uninitialized_copy(position, cend(), tmp.finish_);
                tmp.end_of_storage_ = tmp.start_ + newSize;
            } catch (...) {
                dataAllocator.destroy(tmp.start_, tmp.finish_);
                dataAllocator.deallocate(tmp.start_, newSize);
                throw;
            }

            swap(tmp);
            // tmp 是局部的对象，离开作用域自动销毁

            return newPosition;
        }

        iterator insert(const_iterator position, std::initializer_list<value_type> ilist) {
            return insert(position, ilist.begin(), ilist.end());
        }

        // TO DO
        // 在容器就地构造函数
        // C++ Primer P622 转发数据包
        // 组合使用可变参数模板和forward机制来编写函数
        // 保持类型信息是一个两阶段的过程;
        // 1、首先为了保持实参中的类型信息，必须将 emplace 中的函数参数定义为模板类型参数的右值引用
        // 2、其次，当 empalce 将这些参数传递给 construct 时，我们必须使用 forward 来保持实参的原始类型
        // 当然这边的有些不同，实现了 empalce ，没有实现 emplace_back?
        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            return insert(position, T(MySTL::forward<Args>(args)...));
        }

        // 擦除指定位置的元素
        iterator erase(iterator position) {
            if (position + 1 != end()) {
                MySTL::copy(position + 1, finish_, position);
            }
            --finish_;
            dataAllocator.destroy(finish_);

            return position;
        }

        // 擦除 [first, end) 中的所有元素
        iterator erase(iterator first, iterator last) {
            iterator newEnd = MySTL::copy(last, cend(), first);
            dataAllocator.destroy(newEnd, end()); // 销毁元素

            return first;
        }

        // TO DO
        // push_back


        void swap(vector& other) noexcept {
            using MySTL::swap;
            swap(start_, other.start_);
            swap(finish_, other.finish_);
            swap(end_of_storage_, other.end_of_stage_);
        }

    private:
        /**
         * @description: 分配内存空间，以及在内存空间中构造元素
         * @param  {*} 
         * @return {*} 
         * @param {size_type} n 元素个数
         * @param {const_reference} x 元素值
         */
        void fill_initialize(size_type n, const_reference x) {
            iterator result = dataAllocator.allocate(n); // 分配可以容纳 n 个 T 类型大小的内存空间
            MySTL::uninitialized_fill_n(result, n, x); // 在内存空间上构造 n 个 T 类型的元素

            start_ = result;
            finish_ = start_ + n;
            end_of_storage_ = finish_;
        }

        /**
         * @description: ForwardIterator 以及 比 ForwardIterator 更强 的特化版本
         * @param  {*}
         * @return {*}
         */
        template <class ForwardIterator>
        void copy_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
            auto copySize = static_cast<size_type>(distance(first, last));
            start_ = dataAllocator.allocate(copySize);
            finish_ = MySTL::uninitialized_copy(first, last, start_);
            end_of_storage_ = finish_;
        }

        /**
         * @description: 分配内存空间，并将 [first, last) 复制到内存空间上。
         * @param  {*}
         * @return {*}
         */
        template <calss  InputIterator>
        void copy_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
            while (first != last) {
                if (capacity() == size()) {
                    reallocate(empty() ? 1 : vector::grow_factor * size(0));
                }
                push_back(*first);
            }
        }

        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, true_type) {
            fill_initialize(static_cast<size_type>(last - first), static_cast<value_type>(last));
        }

        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, false_type) {
            copy_initialize(first, last, iterator_category(first));
        }

        /**
         * @description: 重新分配内存空大小
         * @param  {*}
         * @return {*}
         * @param {size_type} newSize 新内存空间大小
         */
        void reallocate(size_type newSize) {
            vector tmp;
            tmp.start_ = dataAllocator.allocate(newSize);
            tmp.finish_ = MySTL::uninitialized_copy(start_, finish_, tmp.start_);
            tmp.end_of_storage_ = tmp.start_ + newSize;

            swap(tmp);
        }

        reference check_and_at(size_type position) const {
            if(position >= size()) {
                throw std::out_of_range("vector");
            }

            return *(begin() + position);
        }

        /**
         * @description: 在 position 前面插入指定的元素
         * @param  {*}
         * @return {*}
         */
        template <class U>
        iterator insert_aux(iterator position, U&& value) {
            // 还有备用空间
            if(capacity() > size()) {
                dataAllocator.construct(finish_, *(finish_ - 1));
                ++finish_;
                MySTL::copy_backward(position, finish_ - 2, finish_ - 1);
                *position = MySTL::forward<U>(value);

                return position;
            } else { // 已无备用空间
                size_type oldSize = size();
                size_type newSize = (oldSize != 0) ? (oldSize * grow_factor) : 1;
                vector tmp;
                tmp.start_ = dataAllocator.allocate(newSize);
                auto newPosition = tmp.start_ + (position() - begin());
                tmp.finish_ = MySTL::uninitialized_copy(cbegin(), position, tmp.start_);
                dataAllocator.construct(tmp.finish_++, MySTL::forward<U>(value));
                tmp.finish_ = MySTL::uninitialized_copy(position, end(), tmp.finish_);

                swap(tmp);

                return newPosition;
            }
        }  
    }; // class vector

    // 下面是一些全局函数
    // 重载了 == != < <= > >= 

    // =
    template <class T, class Allocator>
    bool operator==(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return left.size() == right.size() && MySTL::equal(left.cbegin(), left.cend(), right.cbegin());
    }

    // != 依靠 = 来实现
    template <class T, class Allocator>
    bool operator!=(const vector<T, Allocator> &left, const vector<T, Allocator> &right)  {
        return !(left == right);
    }

    template <class T, class Allocator>
    bool operator<(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return MySTL::lexicographical_compare(left.cbegin(), left.cend(), right.cbegin(), right.cend());
    }

    template <class T, class Allocator>
    bool operator>(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return right < left;
    }
    
    template <class T, class Allocator>
    bool operator<=(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return !(right < left);
    }

    template <class T, class Allocator>
    bool operator>=(const vector<T, Allocator> &left, const vector<T, Allocator> &right) {
        return !(left < right);
    }

    template <class T, class Allocator>
    void swap(vector<T, Allocator> &left, vector<T, Allocator>& right) {
        left.swap(right);
    }

}

#endif
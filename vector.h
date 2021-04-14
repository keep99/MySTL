/*
 * @Description: vector
 * @Author: Chen.Yu
 * @Date: 2021-04-03 14:03:48
 * @LastEditTime: 2021-04-14 04:01:45
 * @LastEditors: Chen.Yu
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <initializer_list>

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"
#include "memory_function.h"
#include "algorithm_base.h"  // TO DO

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

        using allocator_type         = Alloc;
        using data_allocator         = Alloc;

    private:
        // 删除这个，占用空间！
        // allocator_type dataAllocator;       // 内存空间配置器

        iterator start_;                    // 目前使用空间的头
        iterator finish_;                   // 目前使用空间的尾
        iterator end_of_storage_;           // 目前可用空间的尾

        // 弃用
        // // 扩容因子 设为 1.5
        // constexpr static double grow_factor = 1.5;

    public:
        // 构造函数（只实现了部分）
        vector() : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}
        vector(size_type n, const T& value) { fill_initialize(n, value); }
        vector(int       n, const T& value) { fill_initialize(n, value); }
        vector(long      n, const T& value) { fill_initialize(n, value); }
        
        explicit vector(size_type n) : vector(n, T()) {}

        
        // 这边可能传入迭代器，也可能传入两个数
        template <class InputIterator>
        vector(InputIterator first, InputIterator last) : vector() {
            // 如果 InputIterator 为整数类型，则此构造函数的效果如同
            // vector(static_cast<size_type>(first), static_cast<value_type>(last)),
            // 实际调用的是 fill_initialize，构造
            // 如果 InputIterator 为迭代器类型，才是迭代器版本的构造函数
            // 实际调用的是 copy_initialize，拷贝
            initialize_aux(first, last, is_integral<InputIterator>());
        }
    
        // 列表初始化
        // 实际上调用的是上面那个构造函数
        vector(const std::initializer_list<T> iList) {
            range_initialize(iList.begin(), iList.end());
        }

        // 拷贝构造函数
        // 调用这个函数的时候，两个vector 的 value_type 必须一样
        vector(const vector& other) : vector(other.begin(), other.end()) {}

        // 移动构造函数, vector列表初始化，已经把 start finish end 初始化为 nullptr 了 
        // 所以这里的效果就是 将这个对象的 end finish end 赋值为 other 的，然后将 other 的 三个成员 置为 nullptr。
        vector(const vector&& other) noexcept: vector() {
            swap(other);
        }

        // TO DO : 这里的做法不直观
        // 析构函数
        // 调用元素的析构函数，然后回收所用的空间
        ~vector() {
            /* 以下的实现不好，层层封装，可读性很差 */
            // clear(); // 析构 vector 内的元素
            // if (start_ != nullptr) {
            //     dataAllocator.deallocate(start_, capacity());
            // }
            // 先析构，后回收内存。在 finish_（包含finish_）之后的位置没有元素，只需要回收内存即可
            data_allocator::destroy(begin(), end());
            data_allocator::deallocate(start_, end_of_storage_ - start_);
            // 不需要重置 start_, finish_, end_of_storage_。
        }

        // 拷贝赋值运算符
        // fixed
        // 分为三种情况，考虑到空间和时间，不能直接构造然后swap
        vector& operator=(const vector& rhs) {
            // if (this != &rhs) {
            //     vector tmp(rhs);
            //     swap(tmp);
            // }

            if (this != &rhs) {
                const size_type len = rhs.size();
                // 如果 rhs.size() > this->capacity()
                if (len > capacity()) {
                    vector tmp(rhs.begin(), rhs.end());
                    swap(tmp);
                }
                else if (size() > = len) {
                    auto i = MySTL::copy(rhs.begin(), rhs.end(), begin());
                    data_allocator::destroy(i, finish_);
                    finish_ = start_ + len;
                }
                else {
                    MySTL::copy(rhs.begin(), rhs.begin() + size(), start_);
                    MySTL::uninitialized_copy(rhs.begin() + size(), rhs.end(), finish_);
                    end_of_storage_ = finish_ = start_ + len;
                }
            }

            return *this;
        }

        // 移动赋值运算符
        // fixed : 移动复制运算符需要释放左侧运算对象
        vector& operator=(vector &&rhs) noexcept {
            data_allocator::destroy(start_, finish_);
            data_allocator::deallocate(start_, end_of_storage_ - start_);
            start_ = rhs.start_;
            finish_ = rhs.finish_;
            end_of_storage_ = rhs.end_of_storage_;
            rhs.start_ = nullptr;
            rhs.finish_ = nullptr;
            rhs.end_of_storage_ = nullptr;

            return *this;
        }

        vector& operator=(std::initializer_list<T> iList) {
            vector tmp(iList.begin(), iList.end());
            swap(tmp);
            
            return *this;
        }

        /* 简化版的赋值函数。释放了原来的数据空间，并分配了新的数据空间 */
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

        /* 访问元素相关操作 */

        // vector.at()，比 operator[] 安全，进行了越界检查
        reference at(size_type position) {
            range_check(position); // 返回位于指定位置 pos 的元素的引用，有边界检查
            return (*this)[position];
        }

        const_reference at(size_type position) const {
            range_check(position);
            return (*this)[position];
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

        pointer data() {
            return start_;
        }
        
        const_pointer data() const {
            return start_;
        }
        

        /* 迭代器相关操作 */

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

        const_reverse_iterator rend() const {
            return static_cast<reverse_iterator>(begin());
        }

        const_reverse_iterator crend() const {
            return static_cast<reverse_iterator>(begin());
        }


        /* 容量相关操作 */
        
        bool empty() const {
            return cbegin() == cend();
        }

        size_type size() const {
            return static_cast<size_type>(cend() - cbegin());
        }

        // 平台允许你放入容器的最大元素数目。参考https://blog.csdn.net/cyningsun/article/details/6227503
        size_type max_size() const {
            return static_cast<size_type>(size_type(-1) / sizeof(T));
            // return std::numeric_limit<size_t>::max();
        }

        size_type capacity() const {
            return static_cast<size_type>(end_of_storage_ - cbegin());
        }
        
        // 为了避免重新分配内存带来的问题，vector提供了reserve函数。
        // 如果 newCapacity 大于当前的 capacity_，则分配新存储；否则，不做任何事情。
        // 如果 vector 不断地 push_back 元素，会进行内存的重新分配问题，涉及到分配内存空间，拷贝元素，开销大
        void reserve(size_type newCapacity) {
            if (newCapacity > capacity()) {
                const size_type old_size = size();
                iterator tmp = data_allocator::allocate(newCapacity);
                MySTL::uninitialized_copy(start_, finish_, tmp);
                data_allocator.destroy(start_, finish_);
                data_allocator::deallocate(start_, end_of_storage_ - start_);
                start_ = tmp;
                finish_ = tmp + old_size;
                end_of_storage_ = tmp + newCapacity;
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

        void resize(size_type newSize) {
            resize(newSize, T());
        }

        // Requests the container to reduce its capacity to fit its size.
        // 请求容器降低其容量和 size 匹配。
        void shrink_to_fit() {
            if(finish_ < end_of_storage_) {
                auto shrink_to_size = size();
                iterator tmp = data_allocator::allocate(shrink_to_size);
                MySTL::uninitialized_copy(start_, finish_, tmp);
                data_allocator.destroy(start_, finish_);
                data_allocator::deallocate(start_, end_of_storage_ - start_);
                start_ = tmp;
                finish_ = tmp + shrink_to_size;
                end_of_storage_ = tmp + shrink_to_size;
            }
        }


        /* 修改容器相关操作 */

        // erase 指定位置的元素
        iterator erase(iterator position) {
            // 如果清除的元素不是最后一个元素，那么需要把清除位置的后面元素往前面挪
            if (position + 1 != end()) {
                // copy 函数，输出区间的起点与输入区间不重叠，没有问题。
                MySTL::copy(position + 1, finish_, position);
            }
            --finish_;
            data_allocator::destroy(finish_);

            return position;
        }

        // erase [first, end) 中的所有元素
        iterator erase(iterator first, iterator last) {
            iterator newEnd = MySTL::copy(last, finish_, first);
            data_allocator::destroy(newEnd, end()); // 销毁元素

            return first;
        }

        // clear vector 中的所有元素，直接调用的 erase
        void clear() {
            erase(begin(), end());
        }

        /* insert */
        // 在指定的位置插入一个元素
        iterator insert(const_iterator position, const_reference value) {
            iterator xposition = const_cast<iterator>(position);
            const size_type n = xposition - begin();
            if(finish_ != end_of_storage_ && xposition == finish_) {
                // 有空间并且是插入点是最后一个位置
                data_allocator::construct(&*finish_, value);
                ++finish_;
            }
            else {
                insert_aux(xposition, value);
            }

            return begin() + n;
        }

        iterator insert(const_iterator position, const T&& value) {
            iterator xposition = const_cast<iterator>(position);
            const size_type n = xposition - begin();
            if(finish_ != end_of_storage_ && xposition == finish_) {
                // 有空间并且是插入点是最后一个位置
                data_allocator::construct(&*finish_, std::move(value));
                ++finish_;
            }
            else {
                insert_aux(xposition, std::move(value));
            }

            return begin() + n;
        }

        iterator insert(const_iterator position, size_type count, const_reference value) {
            return fill_insert(const_cast<iterator>(position), count, value);
        }

        // 如果 InputIterator 类型不是迭代器
        template <class InputIterator>
        void insert_dispatch(const_iterator position, InputIterator n, InputIterator value, true_type) {
            fill_insert(const_cast<iterator>(position), (size_type)n, (T)value);
        }

        template <class InputIterator>
        void insert_dispatch(const_iterator position, InputIterator first, InputIterator last, false_type) {
            range_insert(position, first, last, MySTL::iterator_category(first));
        }

        template <class InputIterator>
        void insert(const_iterator position, InputIterator first, InputIterator last) {
            using is_Integral = typename is_integral<InputIterator>;
            insert_dispatch(position, first, last, is_integral());
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
        template <class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            iterator xpos = const_cast<iterator>(position);
            const size_type n = xpos - start_;
            if (finish_ != end_of_storage_ && xpos == finish_) {
                data_allocator::construct(&*finish_, MySTL::forward<Args>(args)...);
                ++finish_;
            }
            else if (finish_ != end_of_storage_) {
                auto newfinish = finish_;
                data_allocator::construct(&*finish_, *(finish_ - 1));
                ++newfinish;
                MySTL::copy_backward(xpos, finish_ - 1, finish_);
                *xpos = value_type(MySTL::forward<Args>(args)...);
            }
            else {
                reallocate_emplace(xpos, mystl::forward<Args>(args)...);
            }

            return begin() + n;
        }

        // 在容器末尾就地构造元素
        template <class... Args>
        void emplace_back(Args&&... args) {
            if (finish_ < end_of_storage_) {
                data_allocator::construct(&*finish_, MySTL::forward<Args>(args)...);
                ++end;
            }
            else {
                reallocate_emplace(end_, MySTL::forward<Args>(args)...);
            } 
        }

        // TO DO : added
        // push_back
        void push_back(const value_type& value) {
            if (finish_ != end_of_storage_) {
                data_allocator::construct(finish_, value);
                ++finish_;
            }
            else {
                insert_aux(end(), value);
            }
        }
    
        // 这里直接代码重用
        void push_back() {
            push_back(T());
        }

        void push_back(value_type&& value) {
            emplace_back(MySTL::move(value));
        }

        // pop_back
        void pop_back() {
            data_allocator::destroy(finish_);
            --finish_;
        }

        // 与另外一个 vector 交换
        void swap(vector& other) noexcept {
            MySTL::swap(start_, other.start_);
            MySTL::swap(finish_, other.finish_);
            MySTL::swap(end_of_storage_, other.end_of_stage_);
        }


    /*********************************************************************************************************************************/
    /* private 不暴露这些接口，主要是 helper函数 */
    private:
        /**
         * @description: 分配内存空间，以及在内存空间中构造元素
         * @param  {*} 
         * @return {*} 
         * @param {size_type} n 元素个数
         * @param {const_reference} x 元素值
         */
        void fill_initialize(size_type n, const_reference x) {
            iterator result = data_allocator::allocate(n); // 分配可以容纳 n 个 T 类型大小的内存空间
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
            start_ = data_allocator::allocate(copySize);
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

        /* 舍弃这个函数，用的不多，没有复用的必要 */
        // /**
        //  * @description: 重新分配内存空大小
        //  * @param  {*}
        //  * @return {*}
        //  * @param {size_type} newSize 新内存空间大小
        //  */
        // void reallocate(size_type newSize) {
        //     vector tmp;
        //     tmp.start_ = data_allocator::allocate(newSize);
        //     tmp.finish_ = MySTL::uninitialized_copy(start_, finish_, tmp.start_);
        //     tmp.end_of_storage_ = tmp.start_ + newSize;

        //     swap(tmp);
        // }

        void range_check(size_type position) const {
            if(position >= size()) {
                throw std::out_of_range("vector");
            }
        }

        /**
         * @description: 在 position 位置插入指定的元素
         * @param  {*}
         * @return {*}
         */
        void insert_aux(iterator position, const value_type& value) {
            if(finish_ != end_of_storage_) {    // 如果还有备用空间
                // 在备用空间起始处构造一个元素，并以 vector 的最后一个元素值为其初值
                data_allocator::construct(finish_, *(finish_ - 1));
                ++finish_;
                T value_copy = value;
                MySTL::copy_backward(position, finish_ - 2, finish_ - 1);
                *position = value_type;
            }
            else {    // 已无备用空间
                const size_type old_size = size();
                const size_type len = (old_size != 0) ? (old_size * 2) : 1;
                iterator newstart = data_allocator::allocate(len);
                iterator newfinish = newstart;
                try
                {
                    newfinish = uninitialized_copy(start_, position, newstart);
                    data_allocator::construct(newfinish, value);
                    ++newfinish;
                    newfinish = uninitialized_copy(position, finish_, newfinish);
                }
                catch (...) 
                {
                    data_allocator::destroy(newstart, newfinish);
                    data_allocator::deallocate(newstart, len);
                    throw;
                }
                
                // 以下清除并释放旧的 vector
                data_allocator::destroy(start_, finish_);
                data_allocator::deallocate(start_, end_of_storage_ - start_);
                start_ = newstart;
                finish_ = newfinish;
                end_of_storage_ = newstart + len;
            }
        }

        iterator fill_insert(iterator pos, size_type n, const value_type& value) {
            if(n == 0) {
                return pos;
            }

            const size_type before_pos = pos - start_;
            const value_type value_copy = value;
            if(static_cast<size_type>(end_of_storage_ - finish_) >= n) {
                // 如果备用空间大于等于增加的空间
                const size_type elems_after = finish_ - pos;
                auto old_finish = finish_;
                if(elems_after > n) {
                    // 如果插入点后的现有元素个数 大于 新增元素个数
                    // 1、将源区间[finish-n, finish)填充到目标区间[finish, finish+n)
                    // 把插入点后的元素往后面搬，搬到 从 finish_ 开始，给新增元素腾地方 
                    // 注意，和下面的 copy 不同，这里不仅仅需要拷贝，还需要在相应的位置构造元素
                    MySTL::uninitialized_copy(finish_ - n, finish_, finish_);
                    finish_ = finish_ + n;
                    // 2、将源区间[position,old_finish-n)从逆向cpoy到目标区间[old_finish-n, old_finish)
                    // 这里的话，直接拷贝就好了
                    // 注意，这里是 copy_backward ，将 [first, last) 区间内的每一个元素，以逆行的方向复制到 以 result - 1 为起点，方向也为逆行的区间上。
                    MySTL::copy_backward(pos, old_finish - n, old_finish);
                    // 3、将目标区间[position, position+n) 填充成 x_copy
                    MySTL::fill(pos, pos + n, value_copy);
                }
                else {
                    // 如果插入点之后的现有元素个数 小于等于 新增元素个数
                    MySTL::uninitialized_fill_n(finish_, n - elems_after, value_copy);
                    finish_ += n - elems_after;
                    MySTL::uninitialized_copy(pos, old_finish, finish_);
                    fill(pos, old_finish, value_copy);
                }
            }
            else {
                // 如果备用空间不足，配置额外的内存
                const size_type old_size = size();
                const size_type len = old_size + MySTL::max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try
                {
                    new_finish = MySTL::uninitialized_copy(start_, pos, new_start);
                    new_finish = MySTL::uninitialized_fill_n(new_finish, n, value_copy);
                    new_finish = MySTL::uninitialized_copy(pos, finish_, new_finish);
                }
                catch (...)
                {
                    data_allocator::destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }

                // 以下清除并释放旧的 vector
                data_allocator::destroy(start_, finish_);
                data_allocator::deallocate(start_, end_of_storage_ - start_);
                // 以下调整 start finish end_of_storage 到新的 vector
                start_ = new_start;
                finish_ = new_finish;
                end_of_storage_ = new_start + len;
            }

            return start_ + before_pos;
        }

        // template <class InputIterator>
        // void range_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag) {
        //     for ( ; first != last; ++first) {
        //         pos = insert(pos, *first);
        //         ++pos;
        //     }
        // }

        template <class ForwardIterator>
        void range_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
            if (first != last) {
                size_type n = MySTL::distance(first, last);
                if (size_type(end_of_storage_ - finish_) >= n) {
                    // 如果剩下的空间满足需要的大小
                    const size_type elems_after = finish_ - pos;
                    auto old_finish = finish_;
                    if(elems_after > n) {
                        // 如果插入点后的现有元素个数 大于 新增元素个数
                        // 1、将源区间[finish-n, finish)填充到目标区间[finish, finish+n)
                        // 把插入点后的元素往后面搬，搬到 从 finish_ 开始，给新增元素腾地方 
                        // 注意，和下面的 copy 不同，这里不仅仅需要拷贝，还需要在相应的位置构造元素
                        MySTL::uninitialized_copy(finish_ - n, finish_, finish_);
                        finish_ = finish_ + n;
                        // 2、将源区间[position,old_finish-n)从逆向cpoy到目标区间[old_finish-n, old_finish)
                        // 这里的话，直接拷贝就好了
                        // 注意，这里是 copy_backward ，将 [first, last) 区间内的每一个元素，以逆行的方向复制到 以 result - 1 为起点，方向也为逆行的区间上。
                        MySTL::copy_backward(pos, old_finish - n, old_finish);
                        // 3、将目标区间[position, position+n) 填充成 x_copy
                        MySTL::copy(first, last, pos);
                    }
                    else {
                        // 如果插入点之后的现有元素个数 小于等于 新增元素个数
                        ForwardIterator mid = first;
                        // [first, mid) 上的元素个数 等于 插入点之后现有元素的个数
                        MySTL::advance(mid, elems_after);
                        // 1、先把[mid last) 拷贝到 [finish_, finish_ + n - elems_after)
                        uninitialized_copy(mid, last, finish_);
                        finish_ += n - elems_after;
                        // 2、再把[pos, oldfinish) 拷贝到 [finish_, finish_ + elems_after)
                        uninitialized_copy(pos, old_finish, finish_);
                        finish_ += elems_after;
                        // 3、最后一步，再把 [first, mid) 拷贝到 [pos, pos + elems_after)
                        MySTL::copy(first, mid, pos);

                    }
                }
                else {
                    // 如果剩下的空间 不 满足需要的大小，需要配置额外的内存
                    const size_type old_size = size();
                    const size_type len = old_size + MySTL::max(old_size, n);
                    iterator new_start = data_allocator::allocate(len);
                    iterator new_finish = new_start;
                    try
                    {
                        new_finish = MySTL::uninitialized_copy(start_, pos, new_start);
                        new_finish = MySTL::uninitialized_copy(first, last, new_finish);
                        new_finish = MySTL::uninitialized_copy(pos, finish_, new_finish);
                    }
                    catch (...)
                    {
                        data_allocator::destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                        throw;
                    }

                    // 以下清除并释放旧的 vector
                    data_allocator::destroy(start_, finish_);
                    data_allocator::deallocate(start_, end_of_storage_ - start_);
                    // 以下调整 start finish end_of_storage 到新的 vector
                    start_ = new_start;
                    finish_ = new_finish;
                    end_of_storage_ = new_start + len;
                }
            }
        }

        void reallocate_emplace(iterator pos, Args&&... args) {
            const size_type oldsize = size();
            const size_type len = (oldsize != 0) ? (2 * oldsize) : 1;
            iterator newfinish = newstart;
                try
                {
                    newfinish = uninitialized_copy(start_, position, newstart);
                    data_allocator::construct(newfinish, value);
                    ++newfinish;
                    newfinish = uninitialized_copy(position, finish_, newfinish);
                }
                catch (...) 
                {
                    data_allocator::destroy(newstart, newfinish);
                    data_allocator::deallocate(newstart, len);
                    throw;
                }
                
                // 以下清除并释放旧的 vector
                data_allocator::destroy(start_, finish_);
                data_allocator::deallocate(start_, end_of_storage_ - start_);
                start_ = newstart;
                finish_ = newfinish;
                end_of_storage_ = newstart + len;
        }

        template <class Iter>
        void range_initialize(Iter first, Iter last) {
            auto size_type len = distance(first, last);
            const size_type initsize = MySTL::max(static_cast<size_type>(last - first), static_cast<size_type>(16));
            try
            {
                start_ = data_allocator::allocate(initsize);
                finish_ = start_ + len;
                end_of_storage_ = start_ + initsize;
            }
            catch (...)
            {
                data_allocator::deallocate(start_, len);
                start_ = nullptr;
                finish_ = nullptr;
                end_of_storage_ = nullptr;
                throw;
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
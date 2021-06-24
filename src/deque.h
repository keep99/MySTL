#ifndef TOYSTL_SRC_DEQUE_H_
#define TOYSTL_SRC_DEQUE_H_

#include <initializer_list>

#include "algo.h"
#include "algobase.h"
#include "allocator.h"
// #include "iterator_base.h"
// #include "iterator_reverse_iterator.h"
#include "iterator.h"
#include "memory_function.h"
#include "type_traits.h"
#include "utility.h"

namespace toystl {
namespace detail {
constexpr std::size_t min_map_size_ = 8;  // deque map 初始化的大小

inline size_t __deque_buf_size(size_t T_size) {
  return (T_size < 512) ? static_cast<size_t>(512 / T_size)
                        : static_cast<size_t>(1);
}

template <class T, class Reference, class Pointer>
struct deque_iterator {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = Pointer;
  using reference = Reference;
  using map_pointer = T**;
  using size_type = size_t;

  using iterator = deque_iterator<T, T&, T*>;
  using const_iterator = deque_iterator<T, const T&, const T*>;
  using self = deque_iterator;

  // 一个缓冲区中的容量
  static size_t deque_buf_size() { return __deque_buf_size(sizeof(T)); }

  // sizeof(deque_iterator) = 4 * 8 = 32;
  // 保持与容器的联结
  pointer current_;   // 此迭代器所指之缓冲区的现行元素
  pointer first_;     // 此迭代器所指之缓冲区的头
  pointer last_;      // 此迭代器所指之缓冲区的头
  map_pointer node_;  // 指向管控中心

  deque_iterator()
      : current_(nullptr), first_(nullptr), last_(nullptr), node_(nullptr) {}

  deque_iterator(pointer v, map_pointer n)
      : current_(v), first_(*n), last_(*n + deque_buf_size()), node_(n) {}

  deque_iterator(const iterator& other)
      : current_(other.current_),
        first_(other.first_),
        last_(other.last_),
        node_(other.node_) {}

  deque_iterator(iterator&& rhs) noexcept : current_(rhs.current_),
                                            first_(rhs.first_),
                                            last_(rhs.last_),
                                            node_(rhs.node_) {
    rhs.current_ = nullptr;
    rhs.first_ = nullptr;
    rhs.last_ = nullptr;
    rhs.node_ = nullptr;
  }

  self& operator=(const iterator& other) {
    if (this != &other) {
      current_ = other.current_;
      first_ = other.first_;
      last_ = other.last_;
      node_ = other.node_;
    }

    return *this;
  }

  /**
   * @description: 跳到新的缓冲区
   * @param  {*}
   * @return {*}
   * @param {map_pointer} newNode
   */
  void set_node(map_pointer newNode) {
    node_ = newNode;
    first_ = *node_;
    last_ = first_ + static_cast<difference_type>(deque_buf_size());
  }

  /* 重载运算符 */

  reference operator*() const { return *current_; }

  pointer operator->() const { return &(operator*()); }

  // 前置
  self& operator++() {
    ++current_;               // 切换到下一个元素
    if (current_ == last_) {  // 如果已达所在缓冲区的尾端
      set_node(node_ + 1);  // 就切换到下一个节点（亦即缓冲区）
      current_ = first_;    // 的第一个元素
    }

    return *this;
  }

  // 后置，后置通过调用前置来实现
  self operator++(int) {
    auto tmp = *this;
    ++*this;

    return tmp;
  }

  self& operator--() {
    if (current_ == first_) {  // 如果已到达所在缓冲区的头端
      set_node(node_ - 1);     // 就切换到前一节点（亦即缓冲区）
      current_ = last_;        // 的最后一个元素（的下一个位置）
    }
    --current_;  // 切换至前一个位置

    return *this;
  }

  self operator--(int) {
    auto tmp = *this;
    --*this;

    return tmp;
  }

  /* 以下实现随机存取 */
  self& operator+=(difference_type n) {
    difference_type offset = n + (current_ - first_);
    if (offset >= 0 && static_cast<size_t>(offset) < deque_buf_size()) {
      // 目标在同一缓冲区内
      current_ += n;
    }

    else {
      // 标的位置不在同一缓冲区中
      difference_type nodeOffset =
          offset > 0 ? (offset / static_cast<difference_type>(deque_buf_size()))
                     : -static_cast<difference_type>((-offset - 1) /
                                                     deque_buf_size()) -
                           1;
      // 切换至正确的节点（亦即缓冲区）
      set_node(node_ + nodeOffset);
      // 切换至正确的元素
      current_ = first_ +
                 (offset -
                  nodeOffset * static_cast<difference_type>(deque_buf_size()));
    }

    return *this;
  }

  self operator+(difference_type n) const {
    auto tmp = *this;
    return tmp += n;  // 调用 operator+=
  }

  // 使用 operator+= 来完成 operator-=
  self& operator-=(difference_type n) {
    *this += (-n);

    return *this;
  }

  self operator-(difference_type n) const {
    auto tmp = *this;
    return tmp -= n;  // 调用 operator-=
  }

  reference operator[](difference_type n) const { return *(*this + n); }

  // -（两个迭代器之间），==，!=, <, <=, >, >=
  // 存疑
  difference_type operator-(const self& x) const {
    return static_cast<difference_type>(deque_buf_size()) *
               (node_ - x.node_ - 1) +
           (current_ - first_) + (x.last_ - x.current_);
  }

  // 重载比较操作
  bool operator==(const self& rhs) const { return current_ == rhs.current_; }
  bool operator<(const self& rhs) const {
    return node_ == rhs.node_ ? (current_ < rhs.current_) : (node_ < rhs.node_);
  }
  bool operator!=(const self& rhs) const { return !(*this == rhs); }
  bool operator>(const self& rhs) const { return rhs < *this; }
  bool operator<=(const self& rhs) const { return !(rhs < *this); }
  bool operator>=(const self& rhs) const { return !(*this < rhs); }
};
}  // namespace detail

template <class T, class Allocator = allocator<T>>
class deque {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = detail::deque_iterator<T, T&, T*>;
  using const_iterator = detail::deque_iterator<T, const T&, const T*>;
  using reverse_iterator = toystl::reverse_iterator<iterator>;
  using const_reverse_iterator = toystl::reverse_iterator<const_iterator>;

 protected:
  using map_pointer = T**;  // add

  // 一个缓冲区中的容量
  static size_t deque_buf_size() { return detail::__deque_buf_size(sizeof(T)); }

 private:
  using allocator_type = Allocator;
  using node_allocator = Allocator;
  using map_allocator = typename Allocator::template rebind<T*>::other;

  // 这样的话，多了两个成员变量。删除
  // node_allocator nodeAllocator;   // 缓冲区元素的空间配置器
  // map_allocator mapAllocator;     // 中控 map 的空间配置器

  // [start_, finish_)
  iterator start_;   // 指向第一个节点
  iterator finish_;  // 指向最后一个节点

  // 中控器
  T** map_;            // 指向 map, map
                       // 是块连续区间，其中每个元素都是一个指针，指向一个节点(缓冲区)
  size_type mapSize_;  // map 内指针的数目

 public:
  /* 构造函数 */
  deque() {
    empty_initialize();  // TO DO
  }

  explicit deque(size_type count) {
    fill_initialize(count, T());  // TO DO
  }

  deque(size_type count, const_reference value) {
    fill_initialize(count, value);
  }

  template <class InputIterator,
            typename std::enable_if<
                toystl::is_input_iterator<InputIterator>::value, int>::type = 0>
  deque(InputIterator first, InputIterator last) {
    copy_initialize(first, last, iterator_category(first));
  }

  deque(const deque& other) {
    copy_initialize(other.begin(), other.end(), toystl::forward_iterator_tag());
  }

  // TO DO : 为什么这边用完 move,不需要再把 other.start_ 置为 nullptr ？
  // fixed : 这边调用的是 iterator 的 移动构造函数，直接移动
  deque(deque&& other) noexcept : start_(toystl::move(other.start_)),
                                  finish_(toystl::move(other.finish_)),
                                  map_(other.map_),
                                  mapSize_(other.mapSize_) {
    other.map_ = nullptr;
    other.mapSize_ = 0;
  }

  deque(std::initializer_list<T> ilist) {
    copy_initialize(ilist.begin(), ilist.end(), toystl::forward_iterator_tag());
  }

  /* 析构函数 */
  ~deque() {
    if (map_ == nullptr) {
      return;
    }
    clear();
    deallocate_node(*start_.node_);
    *start_.node_ = nullptr;
    deallocate_map();
    map_ = nullptr;
  }

  /* 赋值运算符 */
  deque& operator=(const deque& other) {
    if (this != &other) {
      const size_type length = size();
      if (length >= other.size()) {
        // 当前 deque 比 other 大，则将 other 的元素拷贝到 当前 deque，然后
        // erase 多余的元素
        auto newEnd = toystl::copy(other.begin(), other.end(), begin());
        erase(newEnd, end());
      } else {
        // 当前 deque 比 other 小
        // 则将 other 的一部分拷贝到当前的 deque，然后将剩下的元素插入到 cend()
        // 之前
        iterator mid = other.start_ + static_cast<difference_type>(length);
        toystl::copy(other.start_, mid, start_);
        insert(finish_, mid, other.finish_);
      }
    }

    return *this;
  }

  deque& operator=(deque&& other) {
    if (this != &other) {
      clear();
      start_ = toystl::move(other.start_);
      finish_ = toystl::move(other.finish_);
      map_ = other.map_;
      mapSize_ = other.mapSize_;
      other.map_ = nullptr;
      other.mapSize_ = 0;
    }

    return *this;
  }

  deque& operator=(std::initializer_list<T> ilist) {
    deque tmp(ilist);
    swap(tmp);

    return *this;
  }

  /* assign */
  void assign(size_type count, const_reference value) {
    fill_assign(count, value);
  }  // TO DO

  // 类模板的成员模板
  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last) {
    copy_assign(first, last, iterator_category(first));
  }

  void assign(std::initializer_list<T> ilist) {
    copy_assign(ilist.begin(), ilist.end(), toystl::forward_iterator_tag());
  }

  allocator_type get_allocator() { return allocator_type(); }

  /* 元素访问相关操作 */

  // deque::at() 会进行边界检查，安全的访问
  reference at(size_type position) {
    if (position >= size()) {
      throw std::out_of_range("deque");
    }

    return start_[position];
  }

  const_reference at(size_type position) const {
    if (position >= size()) {
      throw std::out_of_range("deque");
    }

    return start_[position];
  }

  // deque::operator[] 不会进行边界检查，如果越界了，程序会发生段错误
  reference operator[](size_type position) { return start_[position]; }

  const_reference operator[](size_type position) const {
    return start_[position];
  }

  reference front() { return *start_; }

  const_reference front() const { return *start_; }

  reference back() { return *(finish_ - 1); }

  const_reference back() const { return *(finish_ - 1); }

  /* 迭代器相关操作 */

  iterator begin() noexcept { return start_; }

  const_iterator begin() const noexcept { return start_; }

  const_iterator cbegin() const noexcept { return start_; }

  iterator end() noexcept { return finish_; }

  const_iterator end() const noexcept { return finish_; }

  const_iterator cend() const noexcept { return finish_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

  const_reverse_iterator crend() const noexcept { return rend(); }

  /* 容量相关 */

  bool empty() const noexcept { return finish_ == start_; }

  /**
   * @description: deque 中元素的个数
   * @param  {*}
   * @return {*}
   */
  size_type size() const noexcept { return finish_ - start_; }

  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  // /**
  //  * @description: 请求移除所有未使用的缓冲区
  //  * @param  {*}
  //  * @return {*}
  //  */
  // void shrink_to_fit() {
  //     // 至少会留下头部缓冲区
  //     for (auto current = map_; current < start_.node_; ++current) {
  //         node_allocator::deallocate(*current, deque_buf_size());
  //         // deallocate_node(*current);
  //         *current = nullptr;
  //     }

  //     for(auto current = finish_.node_ + 1; current < map_ + mapSize_;
  //     ++current) {
  //         node_allocator::deallocate(*current, deque_buf_size());
  //         // deallocate_node(*current);
  //         *current = nullptr;
  //     }
  // }

  /* 修改容器相关操作 */

  // deque 的 clear，删除所有元素，并释放内存
  // 最终需要保留一个缓冲区，这是 deque 的策略，也是 deque 的初始状态
  void clear() {
    // 以下针对头尾之外的每一个缓冲区（他们一定是饱满的）。析构中间缓冲区中的对象
    for (map_pointer node = start_.node_ + 1; node < finish_.node_; ++node) {
      node_allocator::destroy(*node, *node + deque_buf_size());
      deallocate_node(*node);
    }

    if (start_.node_ != finish_.node_) {
      // 至少有两个缓冲区
      // 将头缓冲区的目前所有元素析构
      toystl::destroy(start_.current_, start_.last_);
      // 将尾缓冲区的目前所有元素析构
      toystl::destroy(finish_.first_, finish_.current_);
      deallocate_node(finish_.first_);
    } else {
      // 只有一个缓冲区
      // 将此唯一缓冲区内的所有元素析构
      toystl::destroy(start_.current_, finish_.current_);
    }

    // shrink_to_fit(); // 留下一个缓冲区，回收其他缓冲区的内存空间
    finish_ = start_;
  }

  /* insert */
  /* 以下实现 insert。在某点（之前）插入元素。返回被插入元素的迭代器 */
  iterator insert(iterator position, const_reference value) {
    if (position.current_ == begin().current_) {
      push_front(value);  // TO DO
      return begin();
    } else if (position.current_ == end().current_) {
      push_back(value);
      return end() - 1;
    } else {
      return insert_aux(position, value);
    }
  }

  iterator insert(iterator position, const value_type&& value) {
    // if (position.current_ == cbegin().current_) {
    //     emplace_front(toystl::move(value));
    //     return begin();
    // } else if (position.current_ == cend().current_) {
    //     emplace_back(toystl::move(value));
    //     return end() - 1;·
    // } else {
    //     return insert_aux(position, toystl::move(value));
    // }
    return emplace(position, std::move(value));
  }

  void insert(iterator position, size_type count, const_reference value) {
    fill_insert(position, count, value);
  }

  template <class InputIterator,
            typename std::enable_if<
                toystl::is_input_iterator<InputIterator>::value, int>::type = 0>
  void insert(iterator position, InputIterator first, InputIterator last) {
    // using is_Integral = is_integral<InputIterator>;
    insert_dispatch(position, first, last, iterator_category(first));
    // insert_dispatch(position, first, last, is_Integral());
  }

  void insert(iterator position, std::initializer_list<T> ilist) {
    insert(position, ilist.begin(), ilist.end());
  }

  /* empalce */
  template <class... Args>
  iterator emplace(iterator position, Args&&... args) {
    if (position.current_ == start_.current_) {
      emplace_front(toystl::forward<Args>(args)...);
      return start_;
    } else if (position.current_ = finish_.current_) {
      emplace_back(toystl::forward<Args>(args)...);
      return finish_ - 1;
    }
    return insert_aux(position, toystl::forward<Args>(args)...);
  }

  /* erase */
  iterator erase(iterator position) {
    iterator next = position;
    ++next;
    difference_type elems_before = position - start_;
    if (elems_before < (size() / 2)) {  // position 靠近头部，则拷贝前面的元素
      // copy_backward 函数：将 [first, last) 范围内的元素复制到 以 d_first
      // 为终点（不包括 d_last）的范围内。
      toystl::copy_backward(begin(), position, next);
      pop_front();
    } else {  // position 更靠近尾部，则拷贝后面的元素
      toystl::copy(next, end(), position);
      pop_back();
    }

    return start_ + elems_before;
  }

  // 清除区间 [first, last) 上的元素
  iterator erase(iterator first, iterator last) {
    if (first == begin() && last == end()) {  // 如果清除区间就是整个 deque
      clear();                                // 直接调用 clear 就行
      return finish_;
    } else {
      difference_type n = last - first;                 // 清除区间的长度
      difference_type elementBefore = first - begin();  // 清除区间前方的元素
      difference_type elementAfter = end() - last;  // 清除区间后方的元素

      if (elementBefore < elementAfter) {
        // 前面的元素少，拷贝前面的元素
        toystl::copy_backward(begin(), first, last);
        auto newStart = begin() + n;
        destroy(begin(), newStart);
        for (auto node = start_.node_; node < newStart.node_; ++node) {
          deallocate_node(*node);
        }
        start_ = newStart;
      } else {
        // 后面的元素少，拷贝后面的元素
        toystl::copy(last, end(), first);
        auto newFinish = end() - n;
        destroy(newFinish, end());
        for (auto node = newFinish.node_ + 1; node <= finish_.node_; ++node) {
          deallocate_node(*node);
        }
        finish_ = newFinish;
      }

      return begin() + elementBefore;
    }
  }

  /* push_back */
  void push_back(const value_type& value) {
    if (finish_.current_ != finish_.last_ - 1) {
      // 最后缓冲区中尚有两个（含）以上的元素备用空间
      // 如果最后就剩下一个一个元素备用空间，那么就会引发新缓冲区的配置
      node_allocator::construct(finish_.current_, value);
      ++finish_.current_;
    } else {
      // 最后一个缓冲区只剩下一个可用空间,则在尾部分配一个新的缓冲区
      reserve_map_at_back();  // TO DO
      *(finish_.node_ + 1) = allocate_node();
      node_allocator::construct(finish_.current_, value);
      finish_.set_node(finish_.node_ + 1);
      finish_.current_ = finish_.first_;
    }
  }

  void push_back(const value_type&& value) {
    emplace_back(toystl::move(value));
  }

  /* emplace_back */
  template <class... Args>
  void emplace_back(Args&&... args) {
    if (finish_.current_ != finish_.last_ - 1) {
      node_allocator::construct(finish_.current_,
                                toystl::forward<Args>(args)...);
      ++finish_.current_;
    } else {
      // 最后一个缓冲区只剩下一个可用空间，则在尾部分配一个新的缓冲区。
      reserve_map_at_back();
      *(finish_.node_ + 1) = allocate_node();
      node_allocator::construct(finish_.current_,
                                toystl::forward<Args>(args)...);
      finish_.set_node(finish_.node_ + 1);
      finish_.current_ = finish_.first_;
    }
  }

  /* pop_back */
  void pop_back() {
    if (finish_.current_ != finish_.first_) {
      // 最后缓冲区有一个或更多元素
      --finish_.current_;
      node_allocator::destroy(finish_.current_);
    } else {
      // 最后缓冲区没有任何元素
      deallocate_node(finish_.first_);      // 释放最后一个缓冲区
      finish_.set_node(finish_.node_ - 1);  // 调整 finish 的状态，使指向
      finish_.current_ = finish_.last_ - 1;  // 上一个缓冲区的最后一个元素
      node_allocator::destroy(finish_.current_);  // 将该元素析构
    }
  }

  /* push_front */
  void push_front(const_reference value) {
    if (start_.current_ != start_.first_) {
      // 第一缓冲区中尚有备用空间
      node_allocator::construct(start_.current_ - 1, value);
      --start_.current_;
    } else {
      // 第一个缓冲区中已经没有空间了，则在头部分配一个新的缓冲区
      reserve_map_at_front();                 // TO DO
      *(start_.node_ - 1) = allocate_node();  // 配置一个新的节点（缓冲区）
      start_.set_node(start_.node_ - 1);
      start_.current_ = start_.last_ - 1;
      node_allocator::construct(start_.current_, value);
    }
  }

  void push_front(const T&& value) { emplace_front(toystl::move(value)); }

  /* emplace_front */
  // 在头部就地构建元素
  template <class... Args>
  void emplace_front(Args&&... args) {
    if (start_.current_ != start_.first_) {
      node_allocator::construct(start_.current_ - 1,
                                toystl::forward<Args>(args)...);
      --start_.current_;
    } else {
      reserve_map_at_front();
      *(start_.node_ - 1) = allocate_node();
      start_.set_node(start_.node_ - 1);
      start_.current_ = start_.last_ - 1;
      node_allocator::construct(start_.current_,
                                toystl::forward<Args>(args)...);
    }
  }

  /* pop_front */
  void pop_front() {
    if (start_.current_ != start_.last_ - 1) {
      // 第一缓冲区有两个（或更多）元素
      node_allocator::destroy(start_.current_);
      ++start_.current_;
    } else {
      // 第一个缓冲区中只有一个元素
      // 头元素在缓冲区尾部，需要更新 start_ 的信息
      node_allocator::destroy(start_.current_);
      deallocate_node(start_.first_);     // 释放第一缓冲区
      start_.set_node(start_.node_ + 1);  // 调整 start_ 的状态
      start_.current_ =
          start_.first_;  // 使 current_ 指向下一个缓冲区的第一个元素
    }
  }

  /* resize */
  // 通过 erase 和 insert 来实现
  void resize(size_type count, const_reference value) {
    const auto len = size();
    if (count < len) {
      erase(begin() + count, end());
    } else {
      insert(end(), count - len, value);
    }
  }

  void resize(size_type count) { resize(count, T()); }

  void swap(deque& other) {
    toystl::swap(start_, other.start_);
    toystl::swap(finish_, other.finish_);
    toystl::swap(map_, other.map_);
    toystl::swap(mapSize_, other.mapSize_);
  }

 private:
  // 初始化一个空的 deque
  void empty_initialize() { create_map_and_nodes(); }

  void fill_initialize(size_type count, const_reference value) {
    create_map_and_nodes(count);
    // 为每个节点的缓冲区设定初值
    for (auto it = start_.node_; it < finish_.node_; ++it) {
      toystl::uninitialized_fill(*it, *it + deque_buf_size(), value);
    }
    // 最后一个节点的设定稍有不同（因为尾端可能有备用空间，不必设置初值）
    toystl::uninitialized_fill(finish_.first_, finish_.current_, value);
  }

  template <class InputIterator>
  void copy_initialize(InputIterator first, InputIterator last,
                       input_iterator_tag) {
    const size_type n = toystl::distance(first, last);
    create_map_and_nodes(n);
    for (; first != last; ++first) {
      emplace_back(*first);
    }
  }

  template <class ForwardIterator>
  void copy_initialize(ForwardIterator first, ForwardIterator last,
                       forward_iterator_tag) {
    size_type n = distance(first, last);
    create_map_and_nodes(n);
    // 空间肯定是够的
    for (auto it = start_.node_; it < finish_.node_; ++it) {
      auto next = first;
      toystl::advance(next, deque_buf_size());
      toystl::uninitialized_copy(first, next, *it);
      first = next;
    }

    toystl::uninitialized_copy(first, last, finish_.first_);
  }

  void fill_assign(size_type n, const value_type& value) {
    if (n > size()) {
      // 如果赋值的 deque 的大小大于原 deque 的大小
      toystl::fill(begin(), end(), value);  // TO DO
      insert(end(), n - size(), value);
    } else {
      erase(begin() + n, end());
      toystl::fill(begin(), end(), value);
    }
  }

  template <class InputIterator>
  void copy_assign(InputIterator first, InputIterator last,
                   input_iterator_tag) {
    auto first1 = begin();
    auto last1 = end();
    for (; first != last && first1 != last1; ++first, ++first1) {
      *first1 = *first;
    }
    if (first1 != last1) {
      erase(first1, last1);
    } else {
      insert_dispatch(finish_, first, last, input_iterator_tag());
    }
  }

  template <class ForwardIterator>
  void copy_assign(ForwardIterator first, ForwardIterator last,
                   forward_iterator_tag) {
    const size_type len1 = size();
    const size_type len2 = toystl::distance(first, last);
    if (len1 < len2) {
      auto next = first;
      toystl::advance(next, len1);
      toystl::copy(first, next, start_);
      insert_dispatch(finish_, next, last, forward_iterator_tag());
    } else {
      erase(toystl::copy(first, last, start_), finish_);
    }
  }

  void fill_insert(iterator pos, size_type n, const_reference value) {
    if (pos.current_ == start_.current_) {
      iterator new_start = reserve_elements_at_front(n);
      toystl::uninitialized_fill(new_start, start_, value);
      start_ = new_start;
    } else if (pos.current_ == finish_.current_) {
      iterator new_finish = reserve_elements_at_back(n);
      toystl::uninitialized_fill(finish_, new_finish, value);
      finish_ = new_finish;
    } else {
      insert_aux(pos, n, value);  // TO DO
    }
  }

  /**
   * @description: 创建并且组织好 deque 结构
   * @param  {*} 元素个数
   * @return {*}
   */
  /*  */
  void create_map_and_nodes(size_type numElements = 0) {
    // 缓冲区个数 = (元素个数 / 每个缓冲区的容量) + 1
    size_type numNodes = numElements / deque_buf_size() + 1;
    // 一个 map 最多要管理几个节点。最少是 8 个，最多是 会多配置两个缓冲区
    mapSize_ = toystl::max(numNodes + 2, detail::min_map_size_);
    try {
      map_ = allocate_map();
    } catch (...) {
      map_ = nullptr;
      mapSize_ = 0;
      throw;
    }

    // 《STL 源码剖析》 P154
    // 以下令 nstart 和 nfinish 指向 map 所拥有的全部节点的最中央区段
    // 保持在最中央，可使头尾两端的扩充能量一样大。每个节点可对应一个缓冲区。
    T** nStart = map_ + (mapSize_ - numNodes) / 2;
    T** nFinish = nStart + numNodes - 1;

    // TO DO : commit or rollback. Done
    // for(auto it = nStart; it <= nFinish; ++it) {
    //     *it = allocate_node();
    // }

    // commit or rollback
    map_pointer cur;
    try {
      for (cur = nStart; cur <= nFinish; ++cur) {
        *cur = node_allocator::allocate(deque_buf_size());
      }
    } catch (...) {
      while (cur != nStart) {
        --cur;
        node_allocator::deallocate(*cur, deque_buf_size());
        *cur = nullptr;
      }
      throw;
    }

    // 分配缓冲区，并设置迭代器
    // 注意：只有 [nStart, nFinish) 范围分配了缓冲区
    // 其他 map 位置还没有分配缓冲区。当需要的时候，动态扩展。
    start_.set_node(nStart);
    finish_.set_node(nFinish);
    start_.current_ = start_.first_;
    finish_.current_ = finish_.first_ + numElements % deque_buf_size();
  }

  /* 分配 map 的内存空间 */
  T** allocate_map() {
    // 在此基础上将各个指针置为 nullptr
    // return map_allocator::allocate(mapSize_);

    map_pointer mp = nullptr;
    mp = map_allocator::allocate(mapSize_);
    for (size_type i = 0; i < mapSize_; ++i) {
      *(mp + i) = nullptr;
    }

    return mp;
  }

  /* 回收 map 的内存空间  */
  void deallocate_map() { map_allocator::deallocate(map_, mapSize_); }

  /* 分配缓冲区的内存空间 */
  T* allocate_node() { return node_allocator::allocate(deque_buf_size()); }

  /* 回收缓冲区的内存空间 */
  void deallocate_node(pointer ptr) {
    node_allocator::deallocate(ptr, deque_buf_size());
  }

  /* 在前面预留 n 个元素的位置 */
  iterator reserve_elements_at_front(size_type n) {
    // 当前缓冲区剩下的位置个数
    size_type nBefore = start_.current_ - start_.first_;
    if (n > nBefore) {
      // 空间不够，需要分配新的缓冲区
      new_elements_at_front(n - nBefore);  // TO DO
    }

    return start_ - difference_type(n);
  }

  iterator reserve_elements_at_back(size_type n) {
    size_type nAfter = finish_.last_ - finish_.current_ - 1;
    if (n > nAfter) {
      // 空间不够，需要分配新的缓冲区
      new_elements_at_back(n - nAfter);  // TO DO
    }

    return finish_ + difference_type(n);
  }

  /* 在前面预留 n 个的缓冲区 */
  void reserve_map_at_front(size_type n = 1) {
    // 如果 map 前端的节点备用空间不足
    if (n > static_cast<size_type>(start_.node_ - map_)) {
      // 符合以上条件则必须重换一个 map（配置更大的，拷贝原来的，释放原来的）
      reallocate_map(n, true);
    }
  }

  /* 在后面预留 n 个缓冲区 */
  void reserve_map_at_back(size_type n = 1) {
    // 如果 map 后端的节点备用空间不足
    if (n > mapSize_ - (finish_.node_ - map_ + 1)) {
      // 符合以上条件则必须重换一个 map（配置更大的，拷贝原来的，释放原来的）
      reallocate_map(n, false);
    }
  }

  /* 调整 map */
  void reallocate_map(size_type nodesToAdd, bool addToFront) {
    size_type oldNumNodes = finish_.node_ - start_.node_ + 1;  // 原缓冲区的个数
    size_type newNumNodes = oldNumNodes + nodesToAdd;  // 新缓冲区的个数

    T** newStart;
    if (mapSize_ >
        2 * newNumNodes) {  // 现在 map 有足够的位置，所以只需移动元素即可
      // 将新的 [start_, finish_) 放在中间
      // 如果是在前面增加元素，还要将 newStart 向后移动 nodesToAdd 个位置
      newStart =
          map_ + (mapSize_ - newNumNodes) / 2 + (addToFront ? nodesToAdd : 0);
      if (newStart < start_.node_) {
        toystl::copy(start_.node_, finish_.node_ + 1, newStart);
      } else {
        toystl::copy_backward(start_.node_, finish_.node_ + 1,
                              newStart + oldNumNodes);
      }
    } else {  // 如果增加缓冲区后的个数的 2 倍大于等于
              // mapSize_，则需要重新配置空间。此时，会导致所有迭代器失效。
      size_type newMapSize_ = mapSize_ + toystl::max(mapSize_, nodesToAdd) + 2;
      mapSize_ = newMapSize_;
      auto newMap = allocate_map();
      newStart =
          newMap + (mapSize_ - newNumNodes) / 2 + (addToFront ? nodesToAdd : 0);
      toystl::copy(start_.node_, finish_.node_ + 1, newStart);
      deallocate_map();
      map_ = newMap;
    }

    // 重新设置迭代器 start_ 和 finish_
    start_.set_node(newStart);
    finish_.set_node(newStart + oldNumNodes - 1);
  }

  /* 这个函数的功能：在 deque 的任意位置（非头非尾）插入一个元素 */
  // 参看 《STL 源码剖析》 P166。具体的过程看 画的图。
  template <class... Args>
  iterator insert_aux(iterator position, Args&&... args) {
    const size_type elems_before = position - start_;
    value_type value_copy = value_type(toystl::forward<Args>(args)...);
    if (elems_before < (size() / 2)) {
      // 在前半段插入
      emplace_front(front());
      auto front1 = start_;
      ++front1;
      auto front2 = front1;
      ++front2;
      position = start_ + elems_before;
      auto pos = position;
      ++pos;
      toystl::copy(front2, pos, front1);
    } else {
      // 在后半段插入
      emplace_back(back());
      auto back1 = finish_;
      --back1;
      auto back2 = back1;
      --back2;
      position = start_ + elems_before;
      toystl::copy(position, back2, back1);
    }
    *position = toystl::move(value_copy);
    return position;
  }

  /* 在 position 位置之前插入 count 个 value  */
  void insert_aux(iterator position, size_type count, const value_type& value) {
    const difference_type elems_before = position - start_;
    size_type len = size();
    value_type value_copy = value;
    if (elems_before < difference_type(len / 2)) {
      iterator newstart = reserve_elements_at_front(count);
      iterator oldstart = start_;
      position = start_ + elems_before;

      if (elems_before >= difference_type(count)) {
        iterator start_n = start_ + difference_type(count);
        uninitialized_copy(start_, start_n, newstart);
        start_ = newstart;
        toystl::copy(start_n, position, oldstart);
        toystl::fill(position - difference_type(count), position, value_copy);
      } else {
        // __uninitialized_copy_fill(_M_start, __pos, __new_start,
        //               _M_start, __x_copy);
        iterator mid2 = uninitialized_copy(start_, position, newstart);
        uninitialized_fill(mid2, start_, value_copy);
        start_ = newstart;
        toystl::fill(oldstart, position, value_copy);
      }
    } else {
      iterator newfinish = reserve_elements_at_back(count);
      iterator oldfinish = finish_;
      const difference_type elems_after = difference_type(len) - elems_before;
      position = finish_ - elems_after;

      if (elems_after > difference_type(count)) {
        iterator finish_n = finish_ - difference_type(count);
        uninitialized_copy(finish_n, finish_, finish_);
        finish_ = newfinish;
        toystl::copy_backward(finish_n, finish_, oldfinish);
        toystl::fill(position, position + difference_type(count), value_copy);
      } else {
        // __uninitialized_fill_copy
        uninitialized_fill(finish_, position + difference_type(count),
                           value_copy);
        uninitialized_copy(position, finish_,
                           position + difference_type(count));
        finish_ = newfinish;
        toystl::fill(position, oldfinish, value_copy);
      }
    }
  }

  void insert_aux(iterator position, iterator first, iterator last,
                  size_type n) {
    const difference_type elems_before = position - start_;
    size_type len = size();
    if (static_cast<size_type>(elems_before) < len / 2) {
      iterator newstart = reserve_elements_at_front(n);
      iterator oldstart = start_;
      position = start_ + elems_before;
      if (elems_before >= difference_type(n)) {
        iterator start_n = start_ + difference_type(n);
        uninitialized_copy(start_, start_n, newstart);
        start_ = newstart;
        toystl::copy(start_n, position, oldstart);
        toystl::copy(first, last, position - difference_type(n));
      } else {
        iterator mid = first + (n - elems_before);
        // __uninitialized_copy_copy(_M_start, __pos, __new_start,
        //               _M_start, __x_copy);
        iterator mid1 = uninitialized_copy(start_, position, newstart);
        uninitialized_copy(first, mid, mid1);
        start_ = newstart;
        toystl::copy(mid, last, oldstart);
      }
    } else {
      iterator newfinish = reserve_elements_at_back(n);
      iterator oldfinish = finish_;
      const difference_type elems_after = difference_type(len) - elems_before;
      position = finish_ - elems_after;

      if (elems_after > difference_type(n)) {
        iterator finish_n = finish_ - difference_type(n);
        uninitialized_copy(finish_n, finish_, finish_);
        finish_ = newfinish;
        toystl::copy_backward(position, finish_n, oldfinish);
        toystl::copy(first, last, position);
      } else {
        iterator mid = first;
        advance(mid, elems_after);
        iterator mid1 = uninitialized_copy(mid, last, finish_);
        uninitialized_copy(position, finish_, mid1);
        finish_ = newfinish;
        toystl::copy(first, mid, position);
      }
    }
  }

  template <class IIter>
  void insert_dispatch(iterator position, IIter first, IIter last,
                       input_iterator_tag) {
    std::copy(first, last, toystl::inserter(*this, position));  // TO DO
  }

  template <class FIter>
  void insert_dispatch(iterator position, FIter first, FIter last,
                       forward_iterator_tag) {
    size_type n = static_cast<size_type>(toystl::distance(first, last));
    if (position.current_ == start_.current_) {
      iterator newstart = reserve_elements_at_front(n);
      uninitialized_copy(first, last, newstart);
      start_ = newstart;
    } else if (position.current_ == finish_.current_) {
      iterator newfinish = reserve_elements_at_back(n);
      uninitialized_copy(first, last, finish_);
      finish_ = newfinish;
    } else {
      insert_aux(position, first, last, n);
    }
  }

  // template <class Integer>
  // void insert_dispatch(iterator position, Integer n, Integer x, true_type) {
  //     fill_insert(position, static_cast<size_type>(n),
  //     static_cast<value_type>(x));
  // }

  // template <class InputIterator>
  // void insert_dispatch(iterator position, InputIterator first, InputIterator
  // lastm false_type) {
  //     insert(position, first, last, iterator_category(first));
  // }

  void new_elements_at_front(size_type new_elems) {
    size_type new_nodes = (new_elems + deque_buf_size() - 1) / deque_buf_size();
    reserve_map_at_front(new_nodes);
    size_type i;
    try {
      for (i = 1; i <= new_nodes; ++i) {
        *(start_.node_ - i) = allocate_node();
      }
    } catch (...) {
      for (size_type j = 1; j < i; ++j) {
        deallocate_node(*(start_.node_ - j));
        throw;
      }
    }
  }

  void new_elements_at_back(size_type new_elems) {
    size_type new_nodes = (new_elems + deque_buf_size() - 1) / deque_buf_size();
    reserve_map_at_back(new_nodes);
    size_type i;
    try {
      for (i = 1; i <= new_nodes; ++i) {
        *(finish_.node_ + i) = allocate_node();
      }
    } catch (...) {
      for (size_type j = 1; j < i; ++j) {
        deallocate_node(*(finish_.node_ + j));
        throw;
      }
    }
  }

};  // class deque

/* 非成员函数 */
template <class T, class Allocator>
bool operator==(const deque<T, Allocator>& left,
                const deque<T, Allocator>& right) {
  return left.size() == right.size() &&
         toystl::equal(left.cbegin(), left.cend(), right.cbegin());
}

template <class T, class Allocator>
bool operator!=(const deque<T, Allocator>& left,
                const deque<T, Allocator>& right) {
  return !(left == right);
}

template <class T, class Allocator>
bool operator<(const deque<T, Allocator>& left,
               const deque<T, Allocator>& right) {
  return toystl::lexicographical_compare(left.cbegin(), left.cend(),
                                         right.cbegin(), right.cend());
}

template <class T, class Allocator>
bool operator<=(const deque<T, Allocator>& left,
                const deque<T, Allocator>& right) {
  return !(right < left);
}

template <class T, class Allocator>
bool operator>(const deque<T, Allocator>& left,
               const deque<T, Allocator>& right) {
  return right < left;
}

template <class T, class Allocator>
bool operator>=(const deque<T, Allocator>& left,
                const deque<T, Allocator>& right) {
  return !(right > left);
}

/* 特化 std::swap 算法 */
template <class T, class Allocator>
void swap(const deque<T, Allocator>& left, const deque<T, Allocator>& right) {
  left.swap(right);
}
}  // namespace toystl

#endif  // TOYSTL_SRC_DEQUE_H_
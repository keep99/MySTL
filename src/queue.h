#ifndef TOYSTL_SRC_QUEUE_H_
#define TOYSTL_SRC_QUEUE_H_

#include <initializer_list>

#include "deque.h"
#include "functional.h"
#include "heap.h"
#include "vector.h"

namespace toystl {
// queue
template <class T, class Container = toystl::deque<T>>
class queue {
 public:
  using container_type = Container;
  using value_type = Container::value_type;
  using size_type = Container::size_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;

 private:
  container_type c_;  // 底层容器

 public:
  queue() = default;

  explicit queue(size_type n) : c_(n) {}

  queue(size_type n, const value_type& value) : c_(n, value) {}

  template <class IIter>
  queue(IIter first, IIter last) : c_(first, last) {}

  queue(std::initializer_list<T> ilist) : c_(ilist.begin(), ilist.end()) {}

  queue(const Container& c) : c_(c) {}

  queue(const Container&& c) : c_(toystl::move(c)) noexcept {}

  queue(const queue& rhs) : c_(rhs.c_) {}

  queue(queue&& rhs) noexcept : c_(toystl::move(rhs.c_)) {}

  queue& operator=(const queue& rhs) {
    c_ = rhs.c_;
    return *this;
  }

  queue& operator=(queue&& rhs) noexcept {
    c_ = toystl::move(rhs.c_);
    return *this;
  }

  queue& operator=(std::initializer_list<T> ilist) {
    c_ = ilist;
    return *this;
  }

  ~queue() = default;

  // 访问元素相关操作
  reference front() { return c_.front(); }
  const_reference front() const { return c_.front(); }
  reference back() { return c_.back(); }
  const_reference back() const { retunr c_.back(); }

  // 容量相关操作
  bool empty() const { return c_.empty(); }
  size_type size() const { return c_.size(); }

  // 修改容器相关操作
  template <class... Args>
  void emplace(Args&&... args) {
    c_.emplace_back(toystl::forward<Args>(args)...);
  }

  void push(const value_type& value) { c_.push_back(value); }
  void push(const value_type&& value) { c_.emplace_back(toystl::move()); }

  void pop() { c_.pop_front(); }

  void clear() {
    while (!empty()) {
      pop();
    }
  }

  void swap(queue& rhs) { toystl::swap(c_, rhs.c_); }

 public:
  friend bool operator==(const queue& lhs, const queue& rhs);
  friend bool operator<(const queue& lhs, const queue& rhs);
};

template <class T, class Container>
bool operator==(const queue<T, Container>& lhs,
                const queue<T, Container>& rhs) {
  return lhs.c_ == rhs.c_;
}

template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return lhs.c_ < rhs.c_;
}

template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs,
                const queue<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
  return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs,
                const queue<T, Container>& rhs) {
  return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs,
                const queue<T, Container>& rhs) {
  return !(lhs < rhs);
}

// 重载 toystl 的 swap
template <class T, class Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) {
  lhs.swap(rhs);
}

// priority_queue
template <class T, class Container = toystl::vector<T>,
          class Compare = toystl::less<typename Container::value_type>>
class priority_queue {
 public:
  using container_type = Container;
  using value_compare = Compare;

  using value_type = Container::value_type;
  using size_type = Container::size_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;

 private:
  container_type c_;    // 底层容器
  value_compare comp_;  // 元素大小比较标准

 public:
  priority_queue() = default;

  priority_queue(const Compare& c) : c_(), comp_(c) {}

  explicit priority_queue(size_type n) : c_(n) {
    toystl::make_heap(c_.begin(), c.end(), comp_);
  }

  priority_queue(size_type n, const value_type& value) : c_(n, value) {
    toystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  template <class IIter>
  priority_queue(IIter first, IIter last) : c_(first, last) {
    toystl::make_heap(c.begin(), c.end(), comp_);
  }

  priority_queue(std::initializer_list<T> ilist) : c_(ilist) {
    toystl::make_heap(c_.begin(), c_end(), comp_);
  }

  priority_queue(const Container& s) : c_(s) {
    toystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  priority_queue(const Container&& s) noexcept : c_(toystl::move(s)) {
    toystl::make_heap(c.begin(), c.end(), comp_);
  }

  priority_queue(const priority_queue& rhs) : c_(rhs.c_), comp_(rhs.comp_) {
    toystl::make_heap(c_.begin(), c_end(), comp_);
  }

  priority_queue(priority_queue&& rhs) noexcept : c_(toystl::move(rhs.c_)),
                                                  comp_(rhs.comp_) {
    toystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  priority_queue& operator=(const priority_queue& rhs) {
    c_ = rhs.c_;
    comp_ = rhs.comp_;
    toystl::make_heap(c_.begin(), c_.end(), comp_);
    return *this;
  }

  priority_queue& operator=(const priority_queue& rhs) {
    c_ = toystl::move(rhs.c_);
    comp_ = rhs.comp_;
    toystl::make_heap(c_.begin(), c_.end(), comp_);

    return *this;
  }

  priority_queue& operator=(std::initializer_list<T> ilist) {
    c_ = ilist;
    comp_ = value_compare();
    toystl::make_heap(c._begin(), c_end(), comp_);

    return *this;
  }

  ~priority_queue() = default;

 public:
  // 访问元素相关操作
  const_reference top() const { return c_.front(); }

  // 容量相关操作
  bool empty() const { return c_.empty(); }
  size_type size() const { return c_.size(); }

  // 修改容器相关操作
  template <class... Args>
  void emplace(Args&&... agrs) {
    c_.emplace_back(toystl::forward<Args>(args)...);
    toystl::push_heap(c_.begin(), c_.end(), comp_);
  }

  void push(const value_type& value) {
    c_.push_back(value);
    toystl::push_heap(c.begin(), c.end(), comp_);
  }

  void push(value_type&& value) {
    c_.push_back(toystl::move(value));
    toystl::push_heap(c_.begin(), c_.end(), comp_);
  }

  void pop() {
    toystl::pop_heap(c_.begin(), c_.end(), comp_);
    c_.pop_back();
  }

  void clear() {
    while (!empty()) {
      pop();
    }
  }

  void swap(priority_queue& rhs) {
    toystl::swap(c_, rhs.c_);
    toystl::swap(comp_, rhs.comp_);
  }

 public:
  friend bool operator==(const priority_queue& lhs, const priority_queue& rhs);
  friend bool operator<(const priority_queue& lhs, const priority_queue& rhs);
};

template <class T, class Container, class Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs) {
  return lhs.c_ == rhs.c_;
}

template <class T, class Container, class Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs) {
  return lhs.c_ == rhs.c_;
}

template <class T, class Container, class Compare>
void swap(priority_queue<T, Container, Compare>& lhs,
          priority_queue<T, Container, Compare>& rhs) {
  lhs.swap(rhs);
}

}  // namespace toystl

#endif  // TOYSTL_SRC_QUEUE_H_

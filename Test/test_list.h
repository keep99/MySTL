#ifndef TOYSTL_TEST_TEST_LIST_H_
#define TOYSTL_TEST_TEST_LIST_H_

#include <list>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "list.h"
#include "test_helper.h"

namespace toystl {
namespace listtest {
class Testlist : public ::testing::Test {
 protected:
  // using Kitten = typename toystl::TestHelper::nontrivial;
  typedef typename toystl::testhelper::nontrivial Kitten;
  std::list<int> std_list_of_id{1, 2, 3, 4};
  std::list<Kitten> std_list_of_kitten;
  toystl::list<Kitten> abc_list_of_kitten;

  void ExpectEqual() const {
    EXPECT_EQ(abc_list_of_kitten.empty(), std_list_of_kitten.empty());
    EXPECT_EQ(abc_list_of_kitten.size(), std_list_of_kitten.size());

    auto first1 = std_list_of_kitten.begin(), last1 = std_list_of_kitten.end();
    auto first2 = abc_list_of_kitten.begin(), last2 = abc_list_of_kitten.end();
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
      EXPECT_EQ(*first1, *first2);
    }

    EXPECT_EQ(first1, last1);
    EXPECT_EQ(first2, last2);
  }
};

TEST_F(Testlist, ConstructorDefault) { ExpectEqual(); }

TEST_F(Testlist, ConstructorWithSize) {
  auto size = 73;
  abc_list_of_kitten = toystl::list<Kitten>(size);
  std_list_of_kitten = std::list<Kitten>(size);
  ExpectEqual();
}

TEST_F(Testlist, ConstructorWithSizeAndValue) {
  auto size = 73;
  auto value = Kitten(size);
  abc_list_of_kitten = toystl::list<Kitten>(size, value);
  std_list_of_kitten = std::list<Kitten>(size, value);
  ExpectEqual();
}

// 策略不一样，非错误
TEST_F(Testlist, ConstructorWithInitializerList) {
  auto list = {Kitten{1}, Kitten{2}, Kitten{3}};
  abc_list_of_kitten = toystl::list<Kitten>(list);
  std_list_of_kitten = std::list<Kitten>(list);
  ExpectEqual();
  abc_list_of_kitten = toystl::list<Kitten>{Kitten{1}, Kitten{2}, Kitten{3}};
  std_list_of_kitten = std::list<Kitten>{Kitten{1}, Kitten{2}, Kitten{3}};
  ExpectEqual();
}

TEST_F(Testlist, EmplaceBack) {
  for (auto& i : std_list_of_id) {
    std_list_of_kitten.emplace_back(i);
    abc_list_of_kitten.emplace_back(i);
    ExpectEqual();
  }
}

TEST_F(Testlist, PushBack) {
  for (auto& i : std_list_of_id) {
    std_list_of_kitten.push_back(Kitten(i));
    abc_list_of_kitten.push_back(Kitten(i));
    ExpectEqual();
  }
}

TEST_F(Testlist, FrontAndBack) {
  for (const auto& i : std_list_of_id) {
    std_list_of_kitten.emplace_back(i);
    abc_list_of_kitten.emplace_back(i);
    EXPECT_EQ(abc_list_of_kitten.front(), std_list_of_kitten.front());
    EXPECT_EQ(abc_list_of_kitten.back(), std_list_of_kitten.back());
  }
}

TEST_F(Testlist, Resize) {
  for (int i = 0; i != 37; ++i) {
    std_list_of_kitten.emplace_back(i);
    abc_list_of_kitten.emplace_back(i);
  }
  std_list_of_kitten.resize(73, std_list_of_kitten.back());
  abc_list_of_kitten.resize(73, abc_list_of_kitten.back());
  ExpectEqual();
  std_list_of_kitten.resize(37);
  abc_list_of_kitten.resize(37);
  ExpectEqual();
  std_list_of_kitten.resize(1);
  abc_list_of_kitten.resize(1);
  ExpectEqual();
  std_list_of_kitten.resize(0);
  abc_list_of_kitten.resize(0);
  ExpectEqual();
}

TEST_F(Testlist, PopBack) {
  for (const auto& i : std_list_of_id) {
    std_list_of_kitten.emplace_back(i);
    abc_list_of_kitten.emplace_back(i);
  }
  while (!std_list_of_kitten.empty()) {
    EXPECT_EQ(abc_list_of_kitten.back(), std_list_of_kitten.back());
    EXPECT_EQ(abc_list_of_kitten.front(), std_list_of_kitten.front());
    std_list_of_kitten.pop_back();
    abc_list_of_kitten.pop_back();
  }
}

TEST_F(Testlist, BeginAndEnd) {
  for (const auto& i : std_list_of_id) {
    abc_list_of_kitten.emplace_back(i);
    std_list_of_kitten.emplace_back(i);
  }
  auto iter_abc = abc_list_of_kitten.begin();
  auto iter_std = std_list_of_kitten.begin();
  while (iter_abc != abc_list_of_kitten.end()) {
    EXPECT_EQ(*iter_abc, *iter_std);
    EXPECT_EQ(iter_abc->Id(), iter_std->Id());
    ++iter_abc;
    ++iter_std;
  }
}

TEST_F(Testlist, RangeFor) {
  for (const auto& i : std_list_of_id) {
    abc_list_of_kitten.emplace_back(i);
    std_list_of_kitten.emplace_back(i);
  }
  auto iter = std_list_of_kitten.begin();
  for (auto& x : abc_list_of_kitten) {
    EXPECT_EQ(x, *iter++);
  }
}

TEST_F(Testlist, Equal) {
  auto new_list_of_kitten = decltype(abc_list_of_kitten)();
  for (const auto& i : std_list_of_id) {
    abc_list_of_kitten.emplace_back(i);
    new_list_of_kitten.emplace_back(i);
  }
  EXPECT_TRUE(abc_list_of_kitten == abc_list_of_kitten);
  EXPECT_TRUE(new_list_of_kitten == abc_list_of_kitten);
  EXPECT_FALSE(abc_list_of_kitten != abc_list_of_kitten);
  EXPECT_FALSE(new_list_of_kitten != abc_list_of_kitten);
}

TEST_F(Testlist, Swap) {
  auto list_a = {1, 2, 3, 4};
  auto list_b = {5, 6};
  auto a = toystl::list<int>(list_a);
  auto b = toystl::list<int>(list_b);
  auto size_a = a.size();
  auto size_b = b.size();
  auto end_of_a = a.back();
  auto end_of_b = b.back();
  a.swap(b);
  EXPECT_EQ(a.size(), size_b);
  // EXPECT_EQ(a.capacity(), size_b);
  EXPECT_EQ(a.back(), end_of_b);
  EXPECT_EQ(b.size(), size_a);
  // EXPECT_EQ(b.capacity(), size_a);
  EXPECT_EQ(b.back(), end_of_a);
}

TEST_F(Testlist, Reverse) {
  for (const auto& i : std_list_of_id) {
    std_list_of_kitten.emplace_back(i);
    abc_list_of_kitten.emplace_back(i);
  }

  std_list_of_kitten.reverse();
  abc_list_of_kitten.reverse();
  ExpectEqual();
}
}  // namespace listtest
}  // namespace toystl

#endif  // TOYSTL_TEST_TEST_LIST_H_
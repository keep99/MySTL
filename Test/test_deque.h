#ifndef MYSTL_TEST_TEST_DEQUE_H_
#define MYSTL_TEST_TEST_DEQUE_H_

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_helper.h"
#include "deque.h"

#include <deque>

namespace toystl {
    namespace dequetest {
        class TestDeque : public ::testing::Test {
        protected:
            // using Kitten = typename toystl::TestHelper::nontrivial;
            typedef typename toystl::TestHelper::nontrivial Kitten;
            std::deque<int> std_deque_of_id{ 1, 2, 3, 4 };
            std::deque<Kitten> std_deque_of_kitten;
            toystl::deque<Kitten> abc_deque_of_kitten;

            void ExpectEqual() const {
                EXPECT_EQ(abc_deque_of_kitten.empty(),
                        std_deque_of_kitten.empty());
                EXPECT_EQ(abc_deque_of_kitten.size(),
                        std_deque_of_kitten.size());
                for (int i = 0; i < static_cast<int>(abc_deque_of_kitten.size()); i++) {
                    EXPECT_EQ(abc_deque_of_kitten[i],
                                std_deque_of_kitten[i]) << i;
                }
            }
        };
        
        TEST_F(TestDeque, ConstructorDefault) {
            ExpectEqual();
        }
        

        TEST_F(TestDeque, ConstructorWithSize) {
            auto size = 73;
            abc_deque_of_kitten = toystl::deque<Kitten>(size);
            std_deque_of_kitten = std::deque<Kitten>(size);
            ExpectEqual();
        }

        TEST_F(TestDeque, ConstructorWithSizeAndValue) {
            auto size = 73;
            auto value = Kitten(size);
            abc_deque_of_kitten = toystl::deque<Kitten>(size, value);
            std_deque_of_kitten = std::deque<Kitten>(size, value);
            ExpectEqual();
        }
        
        // 策略不一样，非错误
        TEST_F(TestDeque, ConstructorWithInitializerDeque) {
            auto deque = { Kitten{1}, Kitten{2}, Kitten{3} };
            abc_deque_of_kitten = toystl::deque<Kitten>(deque);
            std_deque_of_kitten = std::deque<Kitten>(deque);
            ExpectEqual();
            abc_deque_of_kitten = toystl::deque<Kitten>{ Kitten{1}, Kitten{2}, Kitten{3} };
            std_deque_of_kitten = std::deque<Kitten>{ Kitten{1}, Kitten{2}, Kitten{3} };
            ExpectEqual();
        }

        TEST_F(TestDeque, EmplaceBack) {
            for (auto& i : std_deque_of_id) {
                std_deque_of_kitten.emplace_back(i);
                abc_deque_of_kitten.emplace_back(i);
                ExpectEqual();
            }
        }

        TEST_F(TestDeque, PushBack) {
            for (auto& i : std_deque_of_id) {
                std_deque_of_kitten.push_back(Kitten(i));
                abc_deque_of_kitten.push_back(Kitten(i));
                ExpectEqual();
            }
        }

        TEST_F(TestDeque, FrontAndBack) {
            for (const auto& i : std_deque_of_id) {
                std_deque_of_kitten.emplace_back(i);
                abc_deque_of_kitten.emplace_back(i);
                EXPECT_EQ(abc_deque_of_kitten.front(),
                        std_deque_of_kitten.front());
                EXPECT_EQ(abc_deque_of_kitten.back(),
                        std_deque_of_kitten.back());
            }
        }
        
        TEST_F(TestDeque, Resize) {
            for (int i = 0; i != 37; ++i) {
                std_deque_of_kitten.emplace_back(i);
                abc_deque_of_kitten.emplace_back(i);
            }
            std_deque_of_kitten.resize(73, std_deque_of_kitten.back());
            abc_deque_of_kitten.resize(73, abc_deque_of_kitten.back());
            ExpectEqual();
            std_deque_of_kitten.resize(37);
            abc_deque_of_kitten.resize(37);
            ExpectEqual();
            std_deque_of_kitten.resize(1);
            abc_deque_of_kitten.resize(1);
            ExpectEqual();
            std_deque_of_kitten.resize(0);
            abc_deque_of_kitten.resize(0);
            ExpectEqual();
        }
        
        TEST_F(TestDeque, PopBack) {
            for (const auto& i : std_deque_of_id) {
                std_deque_of_kitten.emplace_back(i);
                abc_deque_of_kitten.emplace_back(i);
            }
            while (!std_deque_of_kitten.empty()) {
                EXPECT_EQ(abc_deque_of_kitten.back(),
                        std_deque_of_kitten.back());
                EXPECT_EQ(abc_deque_of_kitten.front(),
                        std_deque_of_kitten.front());
                std_deque_of_kitten.pop_back();
                abc_deque_of_kitten.pop_back();
            }
        }

        TEST_F(TestDeque, At) {
            int j = 0;
            for (const auto& i : std_deque_of_id) {
                std_deque_of_kitten.emplace_back(i);
                abc_deque_of_kitten.emplace_back(i);
                EXPECT_EQ(abc_deque_of_kitten.at(j),
                        std_deque_of_kitten.at(j));
                j++;
            }
        }

        TEST_F(TestDeque, BeginAndEnd) {
            for (const auto& i : std_deque_of_id) {
                abc_deque_of_kitten.emplace_back(i);
                std_deque_of_kitten.emplace_back(i);
            }
            auto iter_abc = abc_deque_of_kitten.begin();
            auto iter_std = std_deque_of_kitten.begin();
            while (iter_abc != abc_deque_of_kitten.end()) {
                EXPECT_EQ(*iter_abc, *iter_std);
                EXPECT_EQ(iter_abc->Id(), iter_std->Id());
                ++iter_abc;
                ++iter_std;
            }
        }
        
        TEST_F(TestDeque, RangeFor) {
            for (const auto& i : std_deque_of_id) {
                abc_deque_of_kitten.emplace_back(i);
                std_deque_of_kitten.emplace_back(i);
            }
            auto iter = std_deque_of_kitten.begin();
            for (auto& x : abc_deque_of_kitten) {
                EXPECT_EQ(x, *iter++);
            }
        }
        
        TEST_F(TestDeque, Equal) {
            auto new_deque_of_kitten = decltype(abc_deque_of_kitten)();
            for (const auto& i : std_deque_of_id) {
                abc_deque_of_kitten.emplace_back(i);
                new_deque_of_kitten.emplace_back(i);
            }
            EXPECT_TRUE(abc_deque_of_kitten == abc_deque_of_kitten);
            EXPECT_TRUE(new_deque_of_kitten == abc_deque_of_kitten);
            EXPECT_FALSE(abc_deque_of_kitten != abc_deque_of_kitten);
            EXPECT_FALSE(new_deque_of_kitten != abc_deque_of_kitten);
        }

        TEST_F(TestDeque, Swap) {
            auto deque_a = {1, 2, 3, 4};
            auto deque_b = {5, 6};
            auto a = toystl::deque<int>(deque_a);
            auto b = toystl::deque<int>(deque_b);
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

        TEST_F(TestDeque, Clear) {
            auto deque_a = {1, 2, 3, 4};
            auto deque_b = {5, 6};
            auto a = toystl::deque<int>(deque_a);
            auto b = std::deque<int>(deque_b);
            a.clear();
            b.clear();
            ExpectEqual();
        }
    }
}


#endif
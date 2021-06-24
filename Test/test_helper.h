/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-06-22 21:00:49
 * @LastEditTime: 2021-06-23 05:14:58
 * @LastEditors: Chen.Yu
 */
#ifndef _TESTHELPER_H_
#define _TESTHELPER_H_

#include <iostream>
#include <memory>

namespace toystl {
    namespace TestHelper {
        template <class Container>
        void print_container(Container& container, const std::string& name = "") {
            std::cout << "Container " << name << " :";
            for(auto val : container) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

        template <class Container1, class Container2>
        bool container_equal(Container1& con1, Container2& con2) {
            auto first1 = con1.begin(), last1 = con1.begin();
            auto first2 = con2.begin(), last2 = con2.begin();
            for(; first1 != last1 && first2 != last2; ++first1, ++first2) {
                if(*first1 != *first2) {
                    return false;
                }
            }

            return first1 == last1 && first2 == last2;
        }

        class nontrivial {
            friend bool operator==(const nontrivial& lhs, const nontrivial& rhs);
            friend bool operator!=(const nontrivial& lhs, const nontrivial& rhs);
        
        public:
            nontrivial() : i_(new int(0)), j_(0) {}
            nontrivial(int i, int j = 0) : i_(new int(i)), j_(j) {}
            nontrivial(const nontrivial& rhs) {
                if(this != &rhs) {
                    i_.reset(new int(*rhs.i_));
                    j_ = rhs.j_;
                }
            }

            nontrivial operator=(const nontrivial& rhs) {
                if (&rhs != this) {
                    i_.reset(new int(*rhs.i_));
                    j_ = rhs.j_;
                }
                return *this;
            }

            ~nontrivial() = default;

            void print() const {
                printf("(%d, %d) ", *i_, j_);
            }

            int Id() const { return i_ ? *i_ : -1; }

        private:
            std::unique_ptr<int> i_;
            int j_;
        };

        bool operator==(const nontrivial& lhs, const nontrivial& rhs);
        bool operator!=(const nontrivial& lhs, const nontrivial& rhs);

        // inline void display_obj(const nontrivial& obj) {
        //     obj.print();
        // }

        // inline void display_int(const int& i) {
        //     std::cout << i << " ";
        // }

        // inline void display_double(const double& i) {
        //     std::cout << i << " ";
        // }

        // template <typename Key, typename T>
        // struct display_map {
        //     void operator()(const std::pair<Key, T> p) {
        //             std::cout << "[k:" << p.first << " v:" << p.second << "], ";
        //     }
        // };

        // template <typename T>
        // struct display {
        //     void operator()(const T& i) {
        //         std::cout << i << " ";
        //     }
        // };
    }
}
#endif
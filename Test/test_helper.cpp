/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-06-22 23:41:25
 * @LastEditTime: 2021-06-23 00:19:10
 * @LastEditors: Chen.Yu
 */
#include "test_helper.h"

namespace toystl {
    namespace TestHelper {
        bool operator==(const nontrivial& lhs, const nontrivial& rhs) {
            return *lhs.i_ == *rhs.i_ && lhs.j_ == rhs.j_;
        }

        bool operator!=(const nontrivial& lhs, const nontrivial& rhs) {
            return !(lhs == rhs);
        }
    }
}
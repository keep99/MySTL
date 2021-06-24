/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-06-20 23:41:11
 * @LastEditTime: 2021-06-24 19:41:11
 * @LastEditors: Chen.Yu
 */
#include "test_vector.h"
#include "test_list.h"
#include "test_deque.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


int main(int argc, char** argv)
{   
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
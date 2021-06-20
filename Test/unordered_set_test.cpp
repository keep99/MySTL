#include <iostream>

#include "../src/unordered_set.h"

using namespace MySTL;

int main()
{
    // int a[] = { 5,4,3,2,1 };
    // MySTL::unordered_set<int> us1;
    // MySTL::unordered_set<int> us2(520);
    // std::cout << us2.size() << std::endl;
    // MySTL::unordered_set<int> us3(520, MySTL::hash<int>());
    // std::cout << us3.size() << std::endl;
    // MySTL::unordered_set<int> us5(a, a + 5);
    // std::cout << us5.size() << std::endl;
    // MySTL::unordered_set<int> us6(a, a + 5, 100);
    // std::cout << us6.size() << std::endl;
    // MySTL::unordered_set<int> us7(a, a + 5, 100, MySTL::hash<int>());
    // MySTL::unordered_set<int> us8(a, a + 5, 100, MySTL::hash<int>(), MySTL::equal_to<int>());
    // std::cout << us8.size() << std::endl;
    // MySTL::unordered_set<int> us9(us5);
    // std::cout << us9.size() << std::endl;
    // MySTL::unordered_set<int> us10(MySTL::move(us5));
    // std::cout << us10.size() << std::endl;
    // MySTL::unordered_set<int> us11;
    // us11 = us6;
    // std::cout << us11.size() << std::endl;
    // MySTL::unordered_set<int> us12;
    // us12 = MySTL::move(us6);
    // std::cout << us12.size() << std::endl;


    int a[] = { 5,4,3,2,1 };
    MySTL::unordered_multiset<int> us1;
    MySTL::unordered_multiset<int> us2(520);
    std::cout << us2.size() << std::endl;
    MySTL::unordered_multiset<int> us3(520, MySTL::hash<int>());
    std::cout << us3.size() << std::endl;
    MySTL::unordered_multiset<int> us5(a, a + 5);
    std::cout << us5.size() << std::endl;
    MySTL::unordered_multiset<int> us6(a, a + 5, 100);
    std::cout << us6.size() << std::endl;
    MySTL::unordered_multiset<int> us7(a, a + 5, 100, MySTL::hash<int>());
    MySTL::unordered_multiset<int> us8(a, a + 5, 100, MySTL::hash<int>(), MySTL::equal_to<int>());
    std::cout << us8.size() << std::endl;
    MySTL::unordered_multiset<int> us9(us5);
    std::cout << us9.size() << std::endl;
    MySTL::unordered_multiset<int> us10(MySTL::move(us5));
    std::cout << us10.size() << std::endl;
    MySTL::unordered_multiset<int> us11;
    us11 = us6;
    std::cout << us11.size() << std::endl;
    MySTL::unordered_multiset<int> us12;
    us12 = MySTL::move(us6);
    std::cout << us12.size() << std::endl;

}
#include <iostream>

#include "../src/unordered_map.h"
#include "../src/vector.h"

using namespace MySTL;

int main()
{
    unordered_map<int, int> m;
    std::cout << m.size() << std::endl;
    m[1] = 2;
    std::cout << m.size() << std::endl;
    pair<const int, int> p(1,1);
    m.insert(p);
    std::cout << m.size() << std::endl;
    std::cout << m[1] << std::endl;
    
    unordered_map<int, int> m_copy;
    m_copy = m;
    std::cout << m_copy.size() << std::endl;

    unordered_multimap<int, int> m1;
    std::cout << m1.size() << std::endl;
    // m1.insert(pair<int, int>(1,2));
    std::cout << m1.size() << std::endl;
    
    vector<int> arr = {1,1,2,2,3,4,4,4,5,6,7,6,6};
    unordered_map<int, int> m2;
    for(auto a : arr) {
        m2[a]++;
    }

    for(auto a : m2) {
        std::cout << a.first << " 出现了几次：" << a.second << std::endl;
    }
    
}

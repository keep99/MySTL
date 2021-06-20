#include <iostream>

#include "../src/deque.h"

using namespace MySTL;

template<typename InputIterator>
void print(InputIterator first,InputIterator last)
{
	while(first!=last)
	{
		std::cout<<*first<<"\t";
		++first;
	}
	std::cout<<std::endl;
}

int main()
{
    deque<int> d(10, 0);

    print(d.begin(), d.end());

	std::cout << "[===============================================================]" << std::endl;
	std::cout << "[----------------- Run container test : deque ------------------]" << std::endl;
	std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
	int a[] = { 1,2,3,4,5 };
	MySTL::deque<int> d1;
	std::cout << "d1" <<std::endl;
	print(d1.begin(), d1.end());
	MySTL::deque<int> d2(5);
	print(d2.begin(), d2.end()); // 0 0 0 0 0 
	MySTL::deque<int> d3(5, 1);
	print(d3.begin(), d3.end()); // 1 1 1 1 1
	MySTL::deque<int> d4(a, a + 5); // 1 2 3 4 5
	print(d4.begin(), d4.end());
	MySTL::deque<int> d5(d2); 
	print(d5.begin(), d5.end()); // 0 0 0 0 0 
	MySTL::deque<int> d6(std::move(d2)); 
	print(d6.begin(), d6.end()); // 0 0 0 0 0 
	MySTL::deque<int> d7;
	d7 = d3;
	std::cout << "d7" << std::endl;
	print(d7.begin(), d7.end()); // 1 1 1 1 1
	MySTL::deque<int> d8;
	d8 = std::move(d3); 
	print(d8.begin(), d8.end()); // 1 1 1 1 1
	MySTL::deque<int> d9{ 1,2,3,4,5,6,7,8,9 };
	print(d9.begin(), d9.end()); // 1,2,3,4,5,6,7,8,9
	MySTL::deque<int> d10;
	d10 = { 1,2,3,4,5,6,7,8,9 }; // 1,2,3,4,5,6,7,8,9
	print(d10.begin(), d10.end());


	std::cout << "=================" << std::endl;
	deque<int> tmp = {1,2,3,4,5};
	std::cout << tmp[0] << std::endl;
	std::cout << tmp[3] << std::endl;
	// std::cout << tmp.at(5) << std::endl;
	std::cout << tmp.size() << std::endl; // 5
	std::cout << tmp.back() << std::endl; // 5
	std::cout << tmp.front() << std::endl; // 1
	tmp.push_back(1);
	print(tmp.begin(), tmp.end());
	tmp.pop_back();
	print(tmp.begin(), tmp.end());
	tmp.push_front(1);
	print(tmp.begin(), tmp.end());
	tmp.pop_front();
	print(tmp.begin(), tmp.end());
	tmp.clear();							
	std::cout << tmp.size() << std::endl;
	
}

/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-06-18 03:47:45
 * @LastEditTime: 2021-06-18 03:47:45
 * @LastEditors: Chen.Yu
 */
#include <iostream>

#include "../src/list.h"

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
	std::cout << "splice" << std::endl;
	list<int> lisint;
	lisint.push_back(1);
	lisint.push_back(2);
	lisint.push_back(3);
	lisint.push_back(4);
	lisint.push_back(5);
	lisint.push_back(6);
	print(lisint.begin(),lisint.end());


	list<int> other;
	other.push_back(101);
	other.push_back(102);
	other.push_back(103);
	other.push_back(104);
	other.push_back(105);
	other.push_back(106);
	other.pop_back();
	other.pop_front();
	print(other.begin(),other.end());

	lisint.splice(lisint.begin(), other);	
	print(lisint.begin(),lisint.end());
	print(other.begin(),other.end());

	
	std::cout << "[===============================================================]" << std::endl;
	std::cout << "[------------------ Run container test : list ------------------]" << std::endl;
	std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
	int a[] = { 1,2,3,4,5 };
	MySTL::list<int> l1;
	std::cout << "=====" << std::endl;
	MySTL::list<int> l2(5);
	std::cout << "list的大小是：" << l2.size() << std::endl;
	print(l2.begin(),l2.end());  //  0 0 0 0 0 
	MySTL::list<int> l3(5, 1); 
	std::cout << "l3" << std::endl; 
	print(l3.begin(),l3.end()); // 1 1 1 1 1 
	MySTL::list<int> l4(a, a + 5); 
	print(l4.begin(),l4.end()); // 1 2 3 4 5 
	MySTL::list<int> l5(l2);
	print(l5.begin(),l5.end()); // 0 0 0 0 0 
	MySTL::list<int> l6(std::move(l4));  
	print(l6.begin(),l6.end()); // 1 2 3 4 5 
	MySTL::list<int> l7{ 1,2,3,4,5,6,7,8,9 };
	print(l7.begin(),l7.end()); // 1,2,3,4,5,6,7,8,9
	MySTL::list<int> l8;
	l8 = l3; 
	print(l8.begin(),l8.end()); // 1 1 1 1 1
	MySTL::list<int> l9;
	l9 = std::move(l3); 
	print(l9.begin(),l9.end()); // 1 1 1 1 1
	MySTL::list<int> l10;
	l10 = { 1, 2, 2, 3, 5, 6, 7, 8, 9 };
	l10.erase(l10.begin());
	print(l10.begin(),l10.end());
	std::cout << "l1o.size" << l10.size() <<std::endl;
	l10.clear();
	std::cout << "l1o.size" << l10.size() <<std::endl;
	
	std::cout << "sort" << std::endl;
	list<int> sort1 = {1,3,5,7,9};
	print(sort1.begin(),sort1.end());
	list<int> sort2 = {2,4,6,8,10};
	print(sort2.begin(),sort2.end());
	sort1.merge(sort2);
	print(sort1.begin(), sort1.end());
	sort1.reverse();
	print(sort1.begin(), sort1.end());

	std::cout << "===" << std::endl;
	return 0;
}
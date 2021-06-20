#include "../src/set.h"
#include <iostream>
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
	set<int> setint;

	setint.insert(2);
	print(setint.begin(),setint.end());
	setint.insert(2);
	print(setint.begin(),setint.end());
	setint.insert(3);
	print(setint.begin(),setint.end());
	setint.insert(1);
	setint.insert(6);
	setint.insert(5);
	setint.insert(10);

	print(setint.begin(),setint.end());

	set<int>::iterator it = setint.begin();
	std::cout<<*it<<std::endl;
	++it;
	std::cout<<*it<<std::endl;
	setint.erase(it);
	print(setint.begin(),setint.end());

	//===================== multiset =====================//
	std::cout << "//===================== multiset =====================//" << std::endl;
	multiset<int> multi_setint;

	multi_setint.insert(2);
	print(multi_setint.begin(),multi_setint.end());
	multi_setint.insert(2);
	print(multi_setint.begin(),multi_setint.end());
	multi_setint.insert(3);
	print(multi_setint.begin(),multi_setint.end());
	multi_setint.insert(1);
	multi_setint.insert(6);
	multi_setint.insert(5);
	multi_setint.insert(10);

	print(multi_setint.begin(),multi_setint.end());

	multiset<int>::iterator it1 = multi_setint.begin();
	std::cout<<*it1<<std::endl;
	++it1;
	std::cout<<*it1<<std::endl;
	multi_setint.erase(it1);
	print(multi_setint.begin(),multi_setint.end());

}
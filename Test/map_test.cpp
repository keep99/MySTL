#include "../src/map.h"
#include <iostream>
using namespace MySTL;


template<typename InputIterator>
void print(InputIterator first,InputIterator last)
{
	while(first!=last)
	{
		std::cout<<first->first<<"\t";
		std::cout<<first->second<<"\t";
		++first;
	}
	std::cout<<std::endl;
}

int main()
{	
	map<int, int> mapint;

	pair<const int, int> p(1,1);
	pair<const int, int> p2(2,2);
	pair<const int, int> p3(3,3);
	mapint.insert(p);
	mapint.insert(p2);
	mapint.insert(p3);
	print(mapint.begin(),mapint.end());
	
	// mapint.insert(3);
	// print(mapint.begin(),mapint.end());
	// std::cout << "3" << std::endl;
	// mapint.insert(1);
	// std::cout << "1" << std::endl;
	// mapint.insert(6);
	// mapint.insert(5);
	// mapint.insert(10);

	// print(mapint.begin(),mapint.end());

	// map<int, int>::iterator it = mapint.begin();
	// std::cout<<*it<<std::endl;
	// ++it;
	// std::cout<<*it<<std::endl;
	// mapint.erase(it);
	// print(mapint.begin(),mapint.end());


	return 0;
}
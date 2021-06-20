#include <iostream>

#include "../src/vector.h"
#include "../src/algo.h"

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
	vector<int> vec(4,2);
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;
	print(vec.begin(),vec.end());

	vec[2]=3;
	print(vec.begin(),vec.end());

	vec.front() = 1;
	print(vec.begin(),vec.end());

	vec.back()=4;
	print(vec.begin(),vec.end());

	vec.push_back(5);
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;

	vec.pop_back();
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;
	
	vec.insert(vec.begin(),0);
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;

	vec.insert(vec.end(),3,-1);
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;


	vec.erase(vec.begin()+1);
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;


	vec.erase(vec.end()-3,vec.end());
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;

	vec.clear();
	print(vec.begin(),vec.end());
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;

	std::cout << "shrink_to_fit" << std::endl;
	vec.shrink_to_fit();
	std::cout<<"vec元素个数:"<<vec.size()<<std::endl;
	std::cout<<"vec容量:"<<vec.capacity()<<std::endl;

	vector<int> arr = {2,3,7,4,9};
	print(arr.begin(),arr.end());
	sort(arr.begin(), arr.end());
	print(arr.begin(),arr.end());

	vector<int> arr1;
	arr1.reserve(3);
	std::cout<<"vec元素个数:"<<arr1.size()<<std::endl;
	std::cout<<"vec容量:"<<arr1.capacity()<<std::endl;
	print(arr1.begin(),arr1.end());
	arr1.emplace_back(1);
	std::cout<<"vec元素个数:"<<arr1.size()<<std::endl;
	std::cout<<"vec容量:"<<arr1.capacity()<<std::endl;
	print(arr1.begin(),arr1.end());

	std::cout << "[===============================================================]\n";
	std::cout << "[----------------- Run container test : vector -----------------]\n";
	std::cout << "[-------------------------- API test ---------------------------]\n";
	int a[] = { 1,2,3,4,5 };
	MySTL::vector<int> v1;
	MySTL::vector<int> v2(10);
	MySTL::vector<int> v3(10, 1);
	MySTL::vector<int> v4(a, a + 5);
	print(v4.begin(),v4.end());
	MySTL::vector<int> v5(v2);
	print(v5.begin(),v5.end());
	MySTL::vector<int> v6(std::move(v2));
	print(v6.begin(),v6.end());
	MySTL::vector<int> v7{ 1,2,3,4,5,6,7,8,9 };
	MySTL::vector<int> v8, v9, v10;
	v8 = v3;
	print(v8.begin(),v8.end());
	v9 = std::move(v3);
	print(v9.begin(),v9.end());
	v9[0] = 100;
	print(v9.begin(),v9.end());
	std::cout << "v9.size : " << v9.size() <<std::endl;
	std::cout << "v9.capacity : " << v9.capacity() <<std::endl;
	v10 = { 1,2,3,4,5,6,7,8,9 };

	vector<int> for_test_push_back;
	for(int i = 1; i <= 10; ++i) {
		for_test_push_back.push_back(i);
	}
	std::cout << "for_test_push_back.size : " << for_test_push_back.size() <<std::endl;
	std::cout << "for_test_push_back.capacity : " << for_test_push_back.capacity() <<std::endl;

	return 0;
}
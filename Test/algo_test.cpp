#include "../src/algo.h"
#include "../src/vector.h"

#include <iostream>
#include <list>
using namespace MySTL;


bool is_odd(int i) { return i & 1; }
bool is_even(int i) { return !(i & 1); }

int main()
{	
	// pass
	std::cout<<"测试 copy"<<std::endl;
	std::cout<<"测试1"<<std::endl;
	const char ccs[5]={'a','b','c','d','e'};
	char ccd[5];
	copy(ccs,ccs+5,ccd);
	for(int i = 0; i < 5; ++i) {
		std::cout << ccd[i] << std::endl;
	}

	std::cout<<"测试2"<<std::endl;
	const wchar_t wccs[5]={'a','b','c','d','e'};
	wchar_t wccd[5];
	copy(wccs,wccs+5,wccd);
	for(int i = 0; i < 5; ++i) {
		std::cout << wccd[i] << std::endl;
	}
	
	std::cout<<"测试3"<<std::endl;
	int is[5]={1,2,3,4,5};
	int os[5];
	copy(is,is+5,os);
	for(int i = 0; i < 5; ++i) {
		std::cout << os[i] << std::endl;
	}
	
	std::cout<<"测试4"<<std::endl;
	const int cis[5]={1,2,3,4,5};
	int cos[5];
	copy(cis,cis+5,cos);
	for(int i = 0; i < 5; ++i) {
		std::cout << cos[i] << std::endl;
	}

	std::cout<<"测试5"<<std::endl;
	vector<int> vecint(is,is+5);
	MySTL::copy(vecint.begin(),vecint.end(),cos);
	for(int i = 0; i < 5; ++i) {
		std::cout << cos[i] << std::endl;
	}
	std::cout << std::endl;

	std::cout << "测试 max min" << std::endl;
	int a = 1, b = 2;
	std::cout << "a 和 b 中较小的是：" << min(a, b) << std::endl;
	std::cout << "a 和 b 中较大的是：" << max(a, b) << std::endl;
	std::cout << std::endl;

	// pass
	std::cout << "测试 iter_swap" << std::endl;
	vector<int> v = {1,2,3,4,5};
	for(int i = 0; i < 5; ++i) {
		std::cout << v[i] << std::endl;
	}
	iter_swap(v.begin(), v.begin() + 1);
	for(int i = 0; i < 5; ++i) {
		std::cout << v[i] << std::endl;
	}
	std::cout << std::endl;

	// pass
	std::cout << "测试 copy_backward" << std::endl;
	int arr1[] = { 1,2,3,4,5 };
	vector<int> exp{ 0,0,0,0,0,6,7,8,9,10 };
	int act[] = { 0,0,0,0,0,6,7,8,9,10 };
	MySTL::copy_backward(arr1, arr1 + 5, exp.begin() + 5);
	MySTL::copy_backward(arr1, arr1 + 5, act + 5);
	for(int i = 0; i < 10; ++i) {
		std::cout << exp[i] << std::endl;
	}
	for(int i = 0; i < 10; ++i) {
		std::cout << act[i] << std::endl;
	}
	std::cout << std::endl;

	// pass
	std::cout << "测试 copy_if" << std::endl;
	int arr2[] = { 1,2,3,4,5,6,7,8,9,10 };
	int exp1[5], act1[5];
	MySTL::copy_if(arr2, arr2 + 10, exp1, is_odd);
	MySTL::copy_if(arr2, arr2 + 10, act1, is_odd);
	for(int i = 0; i < 5; ++i) {
		std::cout << exp1[i] << " ";
	}
	std::cout << std::endl;
	for(int i = 0; i < 5; ++i) {
		std::cout << act1[i] << " ";
	}
	std::cout << std::endl;
	MySTL::copy_if(arr2, arr2 + 10, exp1, is_even);
	MySTL::copy_if(arr2, arr2 + 10, act1, is_even);
	for(int i = 0; i < 5; ++i) {
		std::cout << exp1[i] << " ";
	}
	std::cout << std::endl;
	for(int i = 0; i < 5; ++i) {
		std::cout << act1[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "测试 copy_n" << std::endl;
	int arr3[] = { 1,2,3,4,5,6,7,8,9,10 };
	int exp2[10], act2[10];
	MySTL::copy_n(arr3, 10, exp2);
	MySTL::copy_n(arr3, 10, act2);
	for(int i = 0; i < 10; ++i) {
		std::cout << exp2[i] << " ";
	}
	std::cout << std::endl;
	for(int i = 0; i < 10; ++i) {
		std::cout << act2[i] << " ";
	}
	std::cout << std::endl;
	MySTL::copy_n(arr3 + 5, 5, exp2);
	MySTL::copy_n(arr3 + 5, 5, act2);
	for(int i = 0; i < 10; ++i) {
		std::cout << exp2[i] << " ";
	}
	std::cout << std::endl;
	for(int i = 0; i < 10; ++i) {
		std::cout << act2[i] << " ";
	}
	std::cout << std::endl;

	return 0;
}
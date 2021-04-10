/*
 * @Description: type_traits 负责萃取型别特性
 * @Author: Chen.Yu
 * @Date: 2021-04-02 20:52:32
 * @LastEditTime: 2021-04-10 23:17:35
 * @LastEditors: Chen.Yu
 */
#ifndef _TYPE_TRAITS_H_
#define _TYPE_TRAITS_H_

#include <cstddef> // nullptr_t

namespace MySTL {
	// integral_constant 是包装特定类型的静态常量。
	template <class T, T v>
	struct integral_constant {
		using value_type = T;
		using type 	     = integral_constant;

		static constexpr T value = v;

		constexpr explicit operator value_type() const noexcept {
			return value;
		}

		constexpr value_type operator()() const noexcept {
			return value;
		}
	};
	
	using true_type   =   integral_constant<bool, true>;
	using false_type  =   integral_constant<bool, false>;

	// is_same
	template <class T, class U>
	struct is_same : false_type {};

	template <class T>
	struct is_same<T, T> : true_type {};

	template <class T, class U> 
	bool is_same_v = is_same<T, U>::value;

	// 移除最顶层的 const
	template <class T>
	struct remove_cast {
		using type = T;
	};

	template <class T>
	struct remove_cast<const T>
	{
		using type = T;
	};

	template <class T>
	using remove_const_t = typename remove_const<T>::type;

	// 移除最顶层 volatile
    template <class T>
    struct remove_volatile {
        using type = T;
    };

    template <class T>
    struct remove_volatile<volatile T> {
        using type = T;
    };

    template <class T>
	using remove_volatile_t = typename remove_volatile<T>::type; // C++ 14

	//移除最顶层 const、 volatile 或 两者
	template <class T>
	struct remove_cv {
		using type = typename remove_volatile<typename remove_const<T>::type>::type; 
	};

	template <class T>
	using remove_cv_t = typename remove_cv<T>::type;

	// is_void
	template <class T>
	struct is_void : public is_same<void, typename remove_cv<T>::type> {};

	template <class T>
	bool is_void_v = is_void<T>::value;

	//is_null_pointer
	template <class T>
	struct is_null_pointer : public is_same<std::nullptr_t, typename remove_cv<T>::type> {};

	template <class T>
	bool is_null_pointer_v = is_null_pointer<T>::value;

	// is_integral 是否为整数
	template <class T>
	struct is_integral						: public false_type {};

	//模板的特例化
	template <>
	struct is_integral<bool>				: public true_type {};

	template <>
    struct is_integral<char>                : public true_type {};

    template <>
    struct is_integral<unsigned char >      : public true_type {};

    template <>
    struct is_integral<signed char>         : public true_type {};

    template <>
    struct is_integral<short>               : public true_type {};

    template <>
    struct is_integral<unsigned short>      : public true_type {};

    template <>
    struct is_integral<int>                 : public true_type {};

    template <>
    struct is_integral<unsigned int>        : public true_type {};

    template <>
    struct is_integral<long>                : public true_type {};

    template <>
    struct is_integral<unsigned long>       : public true_type {};

    template <>
    struct is_integral<long long>           : public true_type {};

    template <>
    struct is_integral<unsigned long long > : public true_type {};

	// is_floating_point 是否为 浮点数
	template <class T>
	struct is_floating_point : public integral_constant<bool, 
												     	is_same<float, typename remove_cv<T>::type>::value ||
														is_same<double, typename remove_cv<T>::type>::value || 
														is_same<long double, typename remove_cv<T>::type>::value> {};
				
	template <class T>
	bool is_floating_point_v = is_floating_point<T>::value;

	//is_array 是否为数组类型
	template <class T>
	struct is_array : public false_type {};

	template <class T>
	struct is_array<T[]> : public true_type {};

	template <class T, std::size_t N>
    struct is_array<T[N]> : public true_type {};

    template <class T> 
	bool is_array_v = is_array<T>::value; 


	//is_pointer 判断 T是否是指向对象或函数的指针
	template <class T>
	struct is_pointer_helper : public false_type {};

	template <class T>
	struct is_pointer_helper<T*> : public true_type {};

	template <class T>
	struct is_pointer : public is_pointer_helper<typename remove_cv<T>::type> {};

	template <class T>
	bool is_pointer_v = is_pointer<T>::value; 

	// is_lvalue_reference 判断 T 是否为左值引用类型
	template <class T>
	struct is_lvalue_reference : public false_type {};

	template <class T>
	struct is_lvalue_reference<T&> : public true_type {};

	template <class T>
	bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

	// is_rvalue_reference 判断 T 是否是右值引用类型
	template <class T>
    struct is_rvalue_reference : public false_type {};

    template <class T>
    struct is_rvalue_reference<T&&> : public true_type {};

    template <class T> bool is_rvalue_reference_v = is_rvalue_reference<T>::value; // C++ 17

	//is_reference 判断是否是引用类型
	template <class T>
    struct is_reference : public false_type {};

    template <class T>
    struct is_reference<T&> : public true_type {};

    template <class T>
    struct is_reference<T&&> : public true_type {};

    template <class T> bool is_reference_v = is_reference<T>::value; // C++ 17

	// is_const
    // is_const_v
    // 判断 T 为 const 限定类型（即 const 或 const volatile ）。
    template <class T>
    struct is_const : public false_type {};

    template <class T>
    struct is_const<const T> : public true_type {};

    template <class T> bool is_const_v = is_const<T>::value; // C++ 17

    // is_volatile
    // is_volatile_v
    // 判断 T 为 volatile 限定类型（即 volatile 或 const volatile ）。
    template <class T>
    struct is_volatile : public false_type {};

    template <class T>
    struct is_volatile<volatile T> : public true_type {};

    template <class T> bool is_volatile_v = is_volatile<T>::value; // C++ 17

    // remove_reference
    // 去除引用，得到类型 T
    template <class T>
    struct remove_reference {
        using type = T;
    };

    template <class T>
    struct remove_reference<T&> {
        using type = T;
    };

    template <class T>
    struct remove_reference<T&&> {
        using type = T;
    };

    template <class T> 
	using remove_reference_t = typename remove_reference<T>::type;

	/**
     * add_lvalue_reference
     */
    // 获取 T 的左值引用类型。
    template <class T>
    struct add_lvalue_reference {
        using type = T&;
    };

    template <class T>
    struct add_lvalue_reference<T&> {
        using type = T&;
    };

    template <class T>
    struct add_lvalue_reference<T&&> {
        using type = T&;

    };

	template <class T>
	using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

    /**
     * add_rvalue_reference
     */
    // 获取 T 的右值引用类型。
    template <class T>
    struct add_rvalue_reference {
        using type = T&&;
    };

    template <class T>
    struct add_rvalue_reference<T&> {
        using type = T&&;
    };

    template <class T>
    struct add_rvalue_reference<T&&> {
        using type = T&&;

    };

    template <class T> 
	using add_rvalue_reference_t = typename add_rvalue_reference<T>::type; 

	// remove_pointer
    // remove_pointer_t
    // 移除 T 的指针属性
    template <class T>
    struct remove_pointer {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T*> {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T* const> {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T* volatile> {
        using type = T;
    };

    template <class T>
    struct remove_pointer<T* const volatile> {
        using type = T;
    };

    template <class T> 
	using remove_pointer_t = typename remove_pointer<T>::type;


	// type_traits
	template<class T>
	struct __type_traits {
		using this_dummy_member_must_be_first = true_type;

		using has_trivial_default_constructor = false_type;
		using has_trivial_copy_constructor 	  = false_type;
		using has_trivial_assignment_operator = false_type;
		using has_trivial_destructor 		  = false_type;
		using is_POD_type					  = false_type;
	};

	template <>
    struct __type_traits<bool> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

	template <>
    struct __type_traits<char> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<signed char> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned char> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<short> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned short> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<int> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned int> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<long long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<unsigned long long> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<float> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<double> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    template <>
    struct __type_traits<long double> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

    // 原生指针偏特化版本的 type traits
    template <class T>
    struct __type_traits<T*> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };


    template <class T>
    struct __type_traits<const T*> {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor    = true_type;
        using has_trivial_assignment_operator = true_type;
        using has_trivial_destructor          = true_type;
        using is_POD_type                     = true_type;
    };

}

#endif
#ifndef _TYPE_TRAITS_H_
#define _TYPE_TRAITS_H_

#include <cstddef> // nullptr_t

namespace toystl {
    template <class T, T v>
    struct m_integral_constant
    {
        static constexpr T value = v;
    };

    template <bool b>
    using m_bool_constant = m_integral_constant<bool, b>;

    using true_type = m_bool_constant<true>;
    using false_type = m_bool_constant<false>;

    // struct true_type {
        
    // };

    // struct false_type {

    // };


	/********************************** type_traits **********************************/
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


    /********************************** is_integral **********************************/
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

    /********************************** remove_reference **********************************/
    template <class T>
    struct remove_reference
    { using type = T; };

    template <class T>
    struct remove_reference<T&>
    { using type = T; };
    
    template <class T>
    struct remove_reference<T&&>
    { using type = T; };

    

}

#endif
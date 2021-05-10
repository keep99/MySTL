/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-25 16:24:28
 * @LastEditTime: 2021-05-10 18:40:20
 * @LastEditors: Chen.Yu
 */
#ifndef _ALGO_H_
#define _ALGO_H_
#include <cstddef>
#include "algobase.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "functional.h"
#include "type_traits.h"
#include "tempbuf.h"
#include "heap.h"

namespace MySTL {
    /*****************************************************************************************/
    // all_of
    // 检查[first, last)内是否 全部元素 都满足一元操作 unary_pred 为 true 的情况，满足则返回 true
    /*****************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    bool all_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) {
            if (!unary_pred(*first)) {
                return false;
            }
        }

        return true;
    }


    /*****************************************************************************************/
    // any_of
    // 检查[first, last)内是否 存在某个元素 满足一元操作 unary_pred 为 true 的情况，满足则返回 true
    /*****************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    bool any_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                return true;
            }
        }

        return false;
    }


    /*****************************************************************************************/
    // none_of
    // 检查[first, last)内是否 全部元素都不满足 一元操作 unary_pred 为 true 的情况，满足则返回 true
    /*****************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                return false;
            }
        }

        return true;
    }


    /*****************************************************************************************/
    // count
    // 对[first, last)区间内的元素与给定值进行比较，缺省使用 operator==，返回元素相等的个数
    /*****************************************************************************************/    
    template <class InputIter, class T>
    size_t count(InputIter first, InputIter last, const T& value) {
        size_t n = 0;
        for (; first != last; ++first) {
            if (*first == value) {
                n++;
            }
        }

        return n;
    }


    /*****************************************************************************************/
    // count_if
    // 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
    /*****************************************************************************************/
    template <class InputIter, class T, class Compare>
    bool count_if(InputIter first, InputIter last, const T& value, Compare comp) {
        size_t n = 0;
        for (; first != last; ++first) {
            if (comp(*first)) {
                n++;
            }
        }
        
        return true;
    }

    /*****************************************************************************************/
    // find
    // 在[first, last)区间内找到等于第一个 value 的元素，返回指向该元素的迭代器
    /*****************************************************************************************/
    template <class InputIter, class T>
    InputIter
    find(InputIter first, InputIter last, const T& value) {
        while (first != last && *first != value) {
            first++;
        }

        return first;
    }


    /*****************************************************************************************/
    // find_if
    // 在[first, last)区间内找到第一个令一元操作 unary_pred 为 true 的元素并返回指向该元素的迭代器
    /*****************************************************************************************/   
    template <class InputIter, class UnaryPredicate>
    InputIter
    find_if(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        while (first != last && !unary_pred(*first)) {
            first++;
        }

        return first;
    }


    /*****************************************************************************************/
    // find_if_not
    // 在[first, last)区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
    /*****************************************************************************************/
    template <class InputIter, class UnaryPredicate>
    InputIter
    find(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        while (first != last && unary_pred(*first)) {
            first++;
        }

        return first;
    }


    /*****************************************************************************************/
    // find_end
    // 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回 last1
    /*****************************************************************************************/
    // 以下是 forward_iterator_tag 版本
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1
    find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2,
                      forward_iterator_tag, forward_iterator_tag)
    {
        if(first2 == last2) {
            return last1;
        }
        else {
            ForwardIter1 result = last1;
            while(1) {
                // 以下利用 search() 查找某个子序列的首次出现点。找不到的话就返回 last1
                ForwardIter1 new_result = MySTL::search(first1, last1, first2, last2);
                if(new_result == last1) {   // 没找到
                    return result;
                }
                else {
                    result = new_result;
                    first1 = new_result;    // 调动一个标兵，准备下一次查找行动
                    ++first1;
                }
            }
        }
    }

    // 以下是 bidirectional_iterator_tag 版本（可以逆序查找）
    template <class BidirectIter1, class BidirectIter2>
    BidirectIter1
    find_end_dispatch(BidirectIter1 first1, BidirectIter1 last1,
                      BidirectIter2 first2, BidirectIter2 last2,
                      bidirectional_iterator_tag, bidirectional_iterator_tag)
    {
        using reviter1 = reverse_iterator<ForwardIter1>;
        using reviter2 = reverse_iterator<ForwardIter2>;

        reviter1 rlast1(first1);
        reviter2 rlast2(first2);
        reviter1 rresult = MySTL::search(reviter1(last1), rlast1, reviter2(last2), rlast2);
        if(rresult == rlast1) { // 没找到
            return last1;
        }
        else { // 找到了
            BidirectIter1 result = rresult.base();  // 转会正常迭代器（非逆序）
            MySTL::advance(result, -MySTL::distance(first2, last2)); // 调整回到子序列的起头处
            return result;
        }
    }

    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1
    find_end(ForwardIter1 first1, ForwardIter1 last1,
             ForwardIter2 first2, ForwardIter2 last2)
    {
        using Category1 = typename iterator_traits<ForwardIter1>::iterator_category;
        using Category2 = typename iterator_traits<ForwardIter2>::iterator_category;

        return MySTL::find_end_dispatch(first1, last1, first2, last2, Category1(), Category2());
    }


    template <class ForwardIter1, class ForwardIter2, class Compare>
    ForwardIter1
    find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2, Compare comp,
                      forward_iterator_tag, forward_iterator_tag)
    {
        if(first2 == last2) {
            return last1;
        }
        else {
            ForwardIter1 result = last1;
            while(1) {
                // 以下利用 search() 查找某个子序列的首次出现点。找不到的话就返回 last1
                ForwardIter1 new_result = MySTL::search(first1, last1, first2, last2, comp);
                if(new_result == last1) {   // 没找到
                    return result;
                }
                else {
                    result = new_result;
                    first1 = new_result;    // 调动一个标兵，准备下一次查找行动
                    ++first1;
                }
            }
        }
    }

    // 以下是 bidirectional_iterator_tag 版本（可以逆序查找）
    template <class BidirectIter1, class BidirectIter2, class Compare>
    BidirectIter1
    find_end_dispatch(BidirectIter1 first1, BidirectIter1 last1,
                      BidirectIter2 first2, BidirectIter2 last2, Compare comp,
                      bidirectional_iterator_tag, bidirectional_iterator_tag)
    {
        using reviter1 = reverse_iterator<ForwardIter1>;
        using reviter2 = reverse_iterator<ForwardIter2>;

        reviter1 rlast1(first1);
        reviter2 rlast2(first2);
        reviter1 rresult = MySTL::search(reviter1(last1), rlast1, reviter2(last2), rlast2, comp);
        if(rresult == rlast1) { // 没找到
            return last1;
        }
        else { // 找到了
            BidirectIter1 result = rresult.base();  // 转会正常迭代器（非逆序）
            MySTL::advance(result, -MySTL::distance(first2, last2)); // 调整回到子序列的起头处
            return result;
        }
    }
    
    // 重载函数版本，使用 comp 代替比较操作
    template <class ForwardIter1, class ForwardIter2, class Compare>
    ForwardIter1
    find_end(ForwardIter1 first1, ForwardIter1 last1,
             ForwardIter2 first2, ForwardIter2 last2, Compare comp)
    {
        using Category1 = typename iterator_traits<ForwardIter1>::iterator_category;
        using Category2 = typename iterator_traits<ForwardIter2>::iterator_category;

        return MySTL::find_end_dispatch(first1, last1, first2, last2, comp, Category1(), Category2());
    }


    /*****************************************************************************************/
    // find_first_of
    // 在[first1, last1)中查找[first2, last2)中的某些元素，返回指向第一次出现的元素的迭代器
    /*****************************************************************************************/
    template <class InputIter, class ForwardIter>
    InputIter
    find_first_of(InputIter first1, InputIter last1,
                  ForwardIter first2, ForwardIter last2)
    {
        for(; first1 != last1; ++first1) {
            for(ForwardIter* iter = first2; iter != last2; ++iter) {
                if(*iter == *first1) {
                    return first1;
                }
            }
        }

        return last1;
    }

    template <class InputIter, class ForwardIter, class Compare>
    InputIter
    find_first_of(InputIter first1, InputIter last1,
                  ForwardIter first2, ForwardIter last2, Compare comp)
    {
        for(; first1 != last1; ++first1) {
            for(ForwardIter* iter = first2; iter != last2; ++iter) {
                if(comp(*iter, *first1)) {
                    return first1;
                }
            }
        }

        return last1;
    }


    /*****************************************************************************************/
    // for_each
    // 使用一个函数对象 f 对[first, last)区间内的每个元素执行一个 operator() 操作，但不能改变元素内容
    // f() 可返回一个值，但该值会被忽略
    /*****************************************************************************************/
    template <class InputIter, class Function>
    Function for_each(InputIter first, InputIter last, Function f) {
        for(; first != last; ++first) {
            f(*first);
        }

        return f;
    }


    /*****************************************************************************************/
    // search
    // 在[first1, last1)中查找[first2, last2)的首次出现点
    // 如果找不到，返回迭代器 last1
    /*****************************************************************************************/
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1
    search(ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2)
    {
        auto d1 = MySTL::distance(first1, last1);
        auto d2 = MySTL::distance(first2, last2);
        if (d1 < d2) {
            return last1;
        }

        auto current1 = first1;
        auto current2 = first2;

        while (current2 != last2) {  // 遍历整个第二序列
            if (*current1 == *current2) { // 如果这个元素相同
                ++current1;               // 调整以比较下一个元素
                ++current2;
            }
            else {                        // 如果这个元素不同了
                if (d1 == d2) {            // 如果两个序列一样长
                    return last1;         // 表示不可能成功了
                }
                else {                    // 两个序列不一样长（至此肯定是序列一大于序列二）
                    current1 = ++first1;  // 调整第一序列的标兵
                    current2 = first2;    // 准备在新起点上再找一次
                    --d1;                 // 已经排除了序列一的一个元素，所以序列一的长度要减一
                }
            }
        }

        return first1;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class ForwardIter1, class ForwardIter2, class Compare>
    ForwardIter1
    search(ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2, Compare comp)
    {
        auto d1 = MySTL::distance(first1, last1);
        auto d2 = MySTL::distance(first2, last2);
        if (d1 < d2) {
            return last1;
        }

        auto current1 = first1;
        auto current2 = first2;

        while (current2 != last2) {  // 遍历整个第二序列
            if (comp(*current1, *current2)) { // 如果这个元素相同
                ++current1;               // 调整以比较下一个元素
                ++current2;
            }
            else {                        // 如果这个元素不同了
                if (d1 == d2) {            // 如果两个序列一样长
                    return last1;         // 表示不可能成功了
                }
                else {                    // 两个序列不一样长（至此肯定是序列一大于序列二）
                    current1 = ++first1;  // 调整第一序列的标兵
                    current2 = first2;    // 准备在新起点上再找一次
                    --d1;                 // 已经排除了序列一的一个元素，所以序列一的长度要减一
                }
            }
        }

        return first1;
    }


    /*****************************************************************************************/
    // search_n
    // 在[first, last)中查找连续 n 个 value 所形成的子序列，返回一个迭代器指向该子序列的起始处
    // 如果找不到，返回迭代器 last1
    /*****************************************************************************************/    
    template <class ForwardIter, class Size, class T>
    ForwardIter
    search_n(ForwardIter first, ForwardIter last, Size n, T value) 
    {
        if (n <= 0) {
            return first;
        }
        else {
            first = MySTL::find(first, last, value); // 首先找到 value 第一次出现点
            while (first != last) {                  // 继续查找余下的元素怒
                Size m = n - 1;                      // value 还应出现 n 次
                ForwardIter i = first;               // 从上次出现点接下去查找
                ++i;
                while ( i != last && m != 0 && *i == value) {
                    ++i;    
                    --m;    // 找到了，“value 应再出现次数” 便可减1
                }
                if (m == 0) {                        // m == 0 表示确实找到了 “元素值出现n次”的子序列
                    return first;
                }
                else {
                    first = MySTL::find(i, last, value);    // 找到value 的 下一个出现点，并且返回循环
                }
            }
        }
        return last;
    }

    // 重载版本使用 函数对象 comp 代替比较操作
    template <class ForwardIter, class Size, class T, class Compare>
    ForwardIter
    search_n(ForwardIter first, ForwardIter last,
             Size n, const T& value, Compare comp)
    {
        if (n <= 0) {
            return first;
        }
        else {
            // 首先先找出 value 第一次出现的点
            while (first != last) {
                if (comp(*first, value))
                    break;
                ++first; // 找到就离开
            }
            // 继续查找余下的元素
            while (first != last) {
                Size m = n - 1; // 还应有 m 个连续元素符合条件
                ForwardIter i = first;
                ++i;
                // 以下循环确定接下来 count - 1 个元素是否都满足条件
                while (i != last && m != 0 && comp(*i, value)) {
                    ++i;
                    --m;
                }
                if(m == 0) { // m == 0 表示确实找到了 n 个满足条件的元素
                    return first;
                }
                else {  // 查找下一个符合条件的元素，然后返回外循环
                    while (i != last) {
                        if (comp(*i, value)) break;
                        ++i;
                    }
                    
                    first = i;
                }
            }
            return last;
        }    
    }

    
    /*****************************************************************************************/
    // adjacent_find
    // 找出第一对匹配的相邻元素，缺省使用 operator== 比较，如果找到返回一个迭代器，指向这对元素的第一个元素
    /*****************************************************************************************/
    // 查找相邻的重复元素，版本一
    template <class ForwardIter>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
        if(first == last) {
            return last;
        }

        ForwardIter next = first;
        while(++next != last) {
            if(*first == *next) return first;
            first = next;
        }

        return last;
    }

    // 查找相邻的重复元素，版本二
    template <class ForwardIter, class Compare>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last, Compare comp) {
        if(first == last) {
            return last;
        }

        ForwardIter next = first;
        while(++next != last) {
            if(comp(*first, *next)) return first;
            first = next;
        }

        return last;
    }


    // Binary search (lower_bound, upper_bound, binary_search, equal_range)
    /*****************************************************************************************/
    // lower_bound
    // 应用于有序区间
    // 在[first, last)中查找 第一个不小于value的元素，并返回指向它的迭代器，若没有则返回 last
    // 换句话说，返回值是 “在不破坏排序状态的原则下，可插入value的第一个位置”
    /*****************************************************************************************/
    template <class ForwardIter, class T>
    ForwardIter 
    lower_bound_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag) {
        auto len = MySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first;
            MySTL::advance(middle, half);
            if(*middle < value) { // 如果中间位置的元素值 < 目标值
                first = middle;   // 这两行使得 first 指向 middle 的下一个位置
                ++first;
                len = len - half - 1;   // 修正 len , 回头测试循环的结束条件
            }
            else {
                len = half;             // 修正 len , 回头测试循环的结束条件
            }
        }

        return first;
    }

    template <class RandomIter, class T>
    RandomIter 
    lower_bound_dispatch(RandomIter first, RandomIter last, const T& value, forward_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first + half;
            if(*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            }
            else {
                len = half;
            }
        }
        return first;
    }

    template <class ForwardIter, class T>
    ForwardIter
    lower_bound(ForwardIter first, ForwardIter last, const T& value) {
        return MySTL::lower_bound_dispatch(first, last, value, iterator_category(first));
    }

    template <class ForwardIter, class T, class Compare>
    ForwardIter 
    lower_bound_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag, Compare comp) {
        auto len = MySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first;
            MySTL::advance(middle, half);
            if(comp(*middle, value)) { // 如果中间位置的元素值 < 目标值
                first = middle;   // 这两行使得 first 指向 middle 的下一个位置
                ++first;
                len = len - half - 1;   // 修正 len , 回头测试循环的结束条件
            }
            else {
                len = half;             // 修正 len , 回头测试循环的结束条件
            }
        }

        return first;
    }

    template <class RandomIter, class T, class Compare>
    RandomIter 
    lower_bound_dispatch(RandomIter first, RandomIter last, const T& value, forward_iterator_tag, Compare comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first + half;
            if(comp(*middle, value)) {
                first = middle + 1;
                len = len - half - 1;
            }
            else {
                len = half;
            }
        }
        return first;
    }

    template <class ForwardIter, class T, class Compare>
    ForwardIter
    lower_bound(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
        return MySTL::lower_bound_dispatch(first, last, value, iterator_category(first), comp);
    }
    

    /*****************************************************************************************/
    // upper_bound
    // 在[first, last)中查找第一个大于value 的元素，并返回指向它的迭代器，若没有则返回 last
    // 换句话说，其返回值是 “在不破坏元素的排序状态的情况下，可插入 value 的最后一个位置”
    /*****************************************************************************************/
    template <class ForwardIter, class T>
    ForwardIter 
    upper_bound_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag) {
        auto len = MySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first;
            MySTL::advance(middle, half);
            if(value < *middle) {
                len = half;
            }
            else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }

        return first;
    }

    template <class RandomIter, class T>
    RandomIter 
    upper_bound_dispatch(RandomIter first, RandomIter last, const T& value, forward_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first + half;
            if(value < *middle) {
                len = half;
            }
            else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    template <class ForwardIter, class T>
    ForwardIter
    upper_bound(ForwardIter first, ForwardIter last, const T& value) {
        return MySTL::upper_bound_dispatch(first, last, value, iterator_category(first));
    }

    template <class ForwardIter, class T, class Compare>
    ForwardIter 
    upper_bound_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag, Compare comp) {
        auto len = MySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first;
            MySTL::advance(middle, half);
            if(comp(value, *middle)) { 
                len = half;
            }
            else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }

        return first;
    }

    template <class RandomIter, class T, class Compare>
    RandomIter 
    upper_bound_dispatch(RandomIter first, RandomIter last, const T& value, forward_iterator_tag, Compare comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;

        while(len > 0) {
            half = len >> 1;
            middle = first + half;
            if(comp(value, *middle)) {
                len = half;
            }
            else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    template <class ForwardIter, class T, class Compare>
    ForwardIter
    upper_bound(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
        return MySTL::upper_bound_dispatch(first, last, value, iterator_category(first), comp);
    }


    /*****************************************************************************************/
    // binary_search
    // 二分查找，如果在[first, last)内有等同于 value 的元素，返回 true，否则返回 false
    // 二分查找依靠 lower_bound 来完成
    /*****************************************************************************************/
    template <class ForwardIter, class T, class Compare>
    bool binary_search(ForwardIter first, ForwardIter last, const T& value) {
        ForwardIter it = lower_bound(first, last, value);
        return it != last && !(value < *it);
    }

    template <class ForwardIter, class T, class Compare>
    bool binary_search(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
        ForwardIter it = lower_bound(first, last, value, comp);
        return it != last && !comp(value < *it);
    }


    /*****************************************************************************************/
    // equal_range
    // 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
    // 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
    /*****************************************************************************************/
    template <class ForwardIter, class T>
    MySTL::pair<ForwardIter, ForwardIter>
    equal_range(ForwardIter first, ForwardIter last, const T& value) {
        return MySTL::equal_range_dispatch(first, last, value, iterator_category(first));
    }

    template <class ForwardIter, class T>
    MySTL::pair<ForwardIter, ForwardIter>
    equal_range_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag) {
        auto len = last - first;
        auto half = len;
        ForwardIter middle, left, right;
        while(len > 0) {            // 整个区间尚未遍历完毕
            half = len >> 1;        // 找出中间位置
            middle = first;         // 这两行设定中央迭代器
            MySTL::advance(middle, half);
            if(*middle < value) {   // 如果中央元素 < 指定值，向右
                first = middle + 1; // 将运作区间缩小（移至后半段）以提高效率
                len = len - half - 1;
            }
            else if (value < *middle) { // 如果中央元素 > 指定值，向左
                len = half;             // 将运作区间缩小（移至前半段）以提高效率
            }
            else {  // 如果中央元素 = 指定值
                // 在前半段中找 lower_bound
                left = MySTL::lower_bound(first, middle, value);
                right = MySTL::upper_bound(middle++, first + len, value);
                return pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return pair<ForwardIter, ForwardIter>(first, first);
    }

    template <class RandomIter, class T>
    MySTL::pair<RandomIter, RandomIter>
    equal_range_dispatch(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while(len > 0) {            // 整个区间尚未遍历完毕
            half = len >> 1;        // 找出中间位置
            middle = first + half;
            if(*middle < value) {   // 如果中央元素 < 指定值，向右
                first = middle + 1; // 将运作区间缩小（移至后半段）以提高效率
                len = len - half - 1;
            }
            else if (value < *middle) { // 如果中央元素 > 指定值，向左
                len = half;             // 将运作区间缩小（移至前半段）以提高效率
            }
            else {  // 如果中央元素 = 指定值
                // 在前半段中找 lower_bound
                left = MySTL::lower_bound(first, middle, value);
                right = MySTL::upper_bound(middle++, first + len, value);
                return pair<RandomIter, RandomIter>(left, right);
            }
        }

        return pair<RandomIter, RandomIter>(first, first);
    }

    template <class ForwardIter, class T, class Compare>
    MySTL::pair<ForwardIter, ForwardIter>
    equal_range(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
        return MySTL::equal_range_dispatch(first, last, value, iterator_category(first), comp);
    }

    template <class ForwardIter, class T, class Compare>
    MySTL::pair<ForwardIter, ForwardIter>
    equal_range_dispatch(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag, Compare comp) {
        auto len = last - first;
        auto half = len;
        ForwardIter middle, left, right;
        while(len > 0) {            // 整个区间尚未遍历完毕
            half = len >> 1;        // 找出中间位置
            middle = first;         // 这两行设定中央迭代器
            MySTL::advance(middle, half);
            if(comp(*middle, value)) {   // 如果中央元素 < 指定值，向右
                first = middle + 1; // 将运作区间缩小（移至后半段）以提高效率
                len = len - half - 1;
            }
            else if (comp(value, *middle)) { // 如果中央元素 > 指定值，向左
                len = half;             // 将运作区间缩小（移至前半段）以提高效率
            }
            else {  // 如果中央元素 = 指定值
                // 在前半段中找 lower_bound
                left = MySTL::lower_bound(first, middle, value, comp);
                right = MySTL::upper_bound(middle++, first + len, value, comp);
                return pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return pair<ForwardIter, ForwardIter>(first, first);
    }

    template <class RandomIter, class T, class Compare>
    MySTL::pair<RandomIter, RandomIter>
    equal_range_dispatch(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag, Compare comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while(len > 0) {            // 整个区间尚未遍历完毕
            half = len >> 1;        // 找出中间位置
            middle = first + half;
            if(comp(*middle, value)) {   // 如果中央元素 < 指定值，向右
                first = middle + 1; // 将运作区间缩小（移至后半段）以提高效率
                len = len - half - 1;
            }
            else if (comp(value, *middle)) { // 如果中央元素 > 指定值，向左
                len = half;             // 将运作区间缩小（移至前半段）以提高效率
            }
            else {  // 如果中央元素 = 指定值
                // 在前半段中找 lower_bound
                left = MySTL::lower_bound(first, middle, value);
                right = MySTL::upper_bound(middle++, first + len, value);
                return pair<RandomIter, RandomIter>(left, right);
            }
        }

        return pair<RandomIter, RandomIter>(first, first);
    }


    /*****************************************************************************************/
    // generate
    // 将函数对象 gen 的运算结果对 [first, last) 内的每个元素赋值
    /*****************************************************************************************/
    template <class ForwardIter, class Generator>
    void generate(ForwardIter first, ForwardIter last, Generator gen) {
        for(; first != last; ++first) {
            *first = gen();
        }
    }


    /*****************************************************************************************/
    // generate_n
    // 用函数对象 gen 连续对 n 个元素赋值
    /*****************************************************************************************/
    template <class ForwardIter, class Size, class Generator>
    void generate_n(ForwardIter first, Size n, Generator gen) {
        for(; n > 0; --n, ++first) {
            *first = gen();
        }
    }


    /*****************************************************************************************/
    // includes
    // 判断序列一 S1 传入的元素是否小于序列二 S2 传入的元素
    /*****************************************************************************************/
    template <class InputIter1, class InputIter2>
    bool includes(InputIter1 first1, InputIter1 last1,
                  InputIter2 first2, InputIter2 last2) 
    {
        while (first1 != last1 && first2 != last2) {
            if(*first2 < *first1) { // 序列二的元素小于序列一的元素
                return false;       // “涵盖” 的情况必然不成立，结束执行
            }
            else if(*first1 < *first2) { // 序列二的元素大于序列一的元素
                ++first1;                // 序列一前进1
            }
            else {                       // *first1 == *first2
                ++first1, ++first2;      // 两序列各自前进1
            }
        }

        return first2 == last2;          // 有一个序列走完了，判断最后一关
    }

    template <class InputIter1, class InputIter2, class Compare>
    bool includes(InputIter1 first1, InputIter1 last1,
                  InputIter2 first2, InputIter2 last2, Compare comp)
    {
        while (first1 != last1 && first2 != last2) {
            if(comp(*first2, *first1)) { // 序列二的元素小于序列一的元素
                return false;       // “涵盖” 的情况必然不成立，结束执行
            }
            else if(comp(*first1, *first2)) { // 序列二的元素大于序列一的元素
                ++first1;                // 序列一前进1
            }
            else {                       // *first1 == *first2
                ++first1, ++first2;      // 两序列各自前进1
            }
        }

        return first2 == last2;          // 有一个序列走完了，判断最后一关
    }


    /*****************************************************************************************/
    // is_heap
    // 检查[first, last)内的元素是否为最大堆（大顶堆），如果是，则返回 true
    // 非 C++ standard
    /*****************************************************************************************/
    template <class RandomIter>
    bool is_heap(RandomIter first, RandomIter last) {
        auto n = MySTL::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child) {
            if (first[parent] < first[child]) { // 判断是否是一个最大堆
                return false;
            }
            if((child & 1) == 0) // 如果 child 为偶数
                ++parent;
        }

        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class RandomIter, class Compare>
    bool is_heap(RandomIter first, RandomIter last, Compare comp) {
        auto n = MySTL::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child) {
            if (comp(first[parent], first[child])) {
                return false;
            }
            if((child & 1) == 0) // 如果 child 为偶数
                ++parent;
        }

        return true;
    }


    /*****************************************************************************************/
    // is_sorted
    // 检查[first, last)内的元素是否不降序，如果是不降序，则返回 true
    /*****************************************************************************************/
    template <class ForwardIter>
    bool is_sorted(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return true;
        }

        auto next = first;
        ++next;
        for(; next != last; first = next, next++) {
            if (*next < *first) {
                return false;
            }
        }

        return true;
    }

    template <class ForwardIter, class Compare>
    bool is_sorted(ForwardIter first, ForwardIter last, Compare comp) {
        if (first == last) {
            return true;
        }

        auto next = first;
        ++next;
        for(; next != last; first = next, next++) {
            if (comp(*next, *first)) {
                return false;
            }
        }

        return true;
    }


    /*****************************************************************************************/
    // median
    // 找出三个值的中间值
    /*****************************************************************************************/
    template <class T>
    const T& median(const T& left, const T& mid, const T& right) {
        if (left < mid) {
            if (mid < right) {          // left < mid < right
                return mid;
            }
            else if (right > left) {    // left < right <= mid
                return right;
            }
            else {                      // right <= left < mid
                return left
            }
        }
        else if(left < right) {         // mid <= left < right
            return left;
        }
        else if(mid < right) {          // mid < right <= left
            return right;
        }
        else {                          // right <= mid <= left
            return mid;
        }
    }

    template <class T, class Compare>
    const T& median(const T& left, const T& mid, const T& right, Compare comp) {
        if (comp(left, mid))
            if (comp(mid, right))
                return mid;
            else if (comp(left, right))
                return right;
            else
                return left;
        else if (comp(left, right))
            return left;
        else if (comp(mid, right))
            return right;
        else
            return mid;
    }


    /*****************************************************************************************/
    // max_element
    // 返回一个迭代器，指向序列中最大的元素
    /*****************************************************************************************/
    template <class ForwardIter>
    ForwardIter max_element(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return first;
        }
        ForwardIter result = first;
        while (++first != last) 
        {
            if (*result < *first) {
                result = first;
            }
        }
        
        return result;
    }

    template <class ForwardIter, class Compare>
    ForwardIter max_element(ForwardIter first, ForwardIter last, Compare comp) {
        if (first == last) {
            return first;
        }
        ForwardIter result = first;
        while (++first != last) 
        {
            if (comp(*result, *first)) {
                result = first;
            }
        }
        
        return result;
    }


    /*****************************************************************************************/
    // min_element
    // 返回一个迭代器，指向序列中最小的元素
    /*****************************************************************************************/
    template <class ForwardIter>
    ForwardIter min_element(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return first;
        }
        ForwardIter result = first;
        while (++first != last) 
        {
            if (*first < *result) {
                result = first;
            }
        }
        
        return result;
    }

    template <class ForwardIter, class Compare>
    ForwardIter min_element(ForwardIter first, ForwardIter last, Compare comp) {
        if (first == last) {
            return first;
        }
        ForwardIter result = first;
        while (++first != last) 
        {
            if (comp(*first, *result)) {
                result = first;
            }
        }
        
        return result;
    }


    /*****************************************************************************************/
    // swap_ranges
    // 将 [first1, last1) 区间内的元素 与 "从 first2 开始，个数相同"的元素 互相交换
    // 交换的区间长度必须相同，两个序列不能互相重叠，否则执行结果未可预期
    // 返回一个迭代器，指向序列二最后一个被交换元素的下一位置
    /*****************************************************************************************/
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter2
    swap_ranges(ForwardIter1 first1, ForwardIter1 last1,
              ForwardIter2 first2) 
    {
        for(; first1 != last1; first1++, first2++) {
            MySTL::iter_swap(first1, first2);
        }

        return first2;
    }


    /*****************************************************************************************/
    // transform
    // 第一个版本以函数对象 unary_op 作用于[first, last)中的每个元素并将结果保存至 result 中
    // 第二个版本以函数对象 binary_op 作用于两个序列[first1, last1)、[first2, last2)的相同位置
    /*****************************************************************************************/
    template <class InputIter, class OutputIter, class UnaryOperation>
    OutputIter
    transform(InputIter first, InputIter last,
              OutputIter result, UnaryOperation unary_op)
    {
        for(; first != last; ++first) {
            *result = unary_op(*first);
        }

        return result;
    }

    template <class InputIter1, class InputIter2, class OutputIter, class BinaryOperation>
    OutputIter
    transform(InputIter1 first1, InputIter1 last,
              InputIter2 first2, OutputIter result, BinaryOperation binary_op) 
    {
        for(; first1 != last1; ++first1, ++first2, ++result) {
            *result = binary_op(*first1, *first2);
        }

        return result;
    }


    /*****************************************************************************************/
    // remove
    // 移除所有与指定 value 相等的元素
    // 并不从容器中删除这些元素，所以 remove 和 remove_if 不适用于 array
    /*****************************************************************************************/    
    template <class ForwardIter, class T>
    ForwardIter remove(ForwardIter first, ForwardIter last, const T& value) {
        first = MySTL::find(first, last, value); // 找出第一个相等的元素
        auto next = first;
        return first == last ? first : MySTL::remove_copy(++next, last, first, value);
    }


    /*****************************************************************************************/
    // remove_copy
    // 移除区间内与指定 value 相等的元素，并将结果复制到以 result 标示起始位置的容器上。
    // 并不真正从 容器中删除元素
    /*****************************************************************************************/
    template <class InputIter, class OutputIter, class T>
    OutputIter
    remove_copy(InputIter first, InputIter last, OutputIter result, const T& value) 
    {
        for(; first != last; ++first) {
            if(*first != value) {
                *result++ = *first;
            }
        }

        return result;
    }

    /*****************************************************************************************/
    // remove_copy_if
    // 移除区间内所有令一元操作 unary_pred 为 true 的元素，并将结果复制到以 result 为起始位置的容器上
    /*****************************************************************************************/
    template <class InputIter, class OutputIter, class UnaryPredicate>
    OutputIter
    remove_copy_if(InputIter first, InputIter last,
                   OutputIter result, UnaryPredicate unary_pred)
    {
        for(; first != last; ++first) {
            if(!unary_pred(*first)) {
                *result = *first;
                ++result;
            }
        }

        return result;
    }

    /*****************************************************************************************/
    // remove_if
    // 移除区间内所有令一元操作 unary_pred 为 true 的元素
    /*****************************************************************************************/
    template <class ForwardIter, class UnaryPredicate>
    ForwardIter
    remove_if(ForwardIter first, ForwardIter last, UnaryPredicate unary_pred)
    {
        first = MySTL::find_if(first, last, unary_pred);
        auto next = first;
        return first == last ? first : MySTL::remove_copy_if(++next, last, first, unary_pred);
    }
    
    /*****************************************************************************************/
    // replace
    // 将区间内所有的 old_value 都以 new_value 替代
    /*****************************************************************************************/
    template <class ForwardIter, class T>
    void replace(ForwardIter first, ForwardIter last,
                 const T& old_value, const T& new_value)
    {
        for(; first != last; ++first)
        {
            if(*first == old_value) {
                *first = new_value;
            }
        }
    }

    
    /*****************************************************************************************/
    // replace_copy
    // 行为与 replace 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
    /*****************************************************************************************/
    template <class InputIter, class OutputIter, class T>
    OutputIter
    replace_copy(InputIter first, InputIter last,
                 OutputIter result, const T& old_value, const T& new_value)
    {
        for(; first != last; ++first, ++result) {
            // 如果旧序列上的元素等于 old_value，就放 new_value 到新序列中
            // 否则就将元素拷贝一份放进新序列中
            *result = *first == old_value ? new_value : *first;
        }
    }


    /*****************************************************************************************/
    // replace_if
    // 将区间内所有令一元操作 unary_pred 为 true 的元素都用 new_value 替代
    /*****************************************************************************************/    
    template <class ForwardIter, class UnaryPredicate, class T>
    void replace_if(ForwardIter first, ForwardIter last, UnaryPredicate unary_pre, const T& new_value) {
        for(; first != last; ++first) {
            if(unary_pre(*first)) {
                *first = new_value;
            }
        }
    }


    /*****************************************************************************************/
    // replace_copy_if
    // 行为与 replace_if 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
    /*****************************************************************************************/
    template <class InputIter, class OutputIter, class UnaryPredicate, class T>
    OutputIter
    replace_copy_if(InputIter first, InputIter last, 
                    OutputIter result, UnaryPredicate unary_pred, const T& new_value)
    {
        for(; first != last; ++first, ++result) {
            *result = unary_pred(*first) ? new_value : *first;
        }

        return result;
    }


    /*****************************************************************************************/
    // reverse
    // 将[first, last)区间内的元素反转
    /*****************************************************************************************/
    template <class BidirectionalIter>
    void reverse_dispatch(BidirectionalIter first, BidirectionalIter last, bidirectional_iterator_tag) {
        while(true) {
            if(first == last || first == --last) {
                return;
            }
            MySTL::iter_swap(first++, last);
        }
    }

    template <class RandomIter>
    void reverse_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
        // 只有 random_access_iterator_tag 才能做到以下的 first < last 判断
        while(first < last) {
            MySTL::iter_swap(first++, --last);
        }
    }

    template <class BidirectionalIter>
    void reverse(BidirectionalIter first, BidirectionalIter last) {
        MySTL::reverse_dispatch(first, last, iterator_category(first));
    }


    /*****************************************************************************************/
    // reverse_copy
    // 行为与 reverse 类似，不同的是将结果复制到 result 所指容器中
    /*****************************************************************************************/
    template <class BidirectionalIter, class OutputIter>
    OutputIter
    reverse_copy(BidirectionalIter first, BidirectionalIter last, OutputIter result)
    {
        while(first != last) {
            --last;
            *result = *last;
            ++result;
        }

        return result;
    }


    /*****************************************************************************************/
    // random_shuffle
    // 将[first, last)内的元素次序随机重排
    // 重载版本使用一个产生随机数的函数对象 rand
    // 参考 gcc 的实现
    /*****************************************************************************************/
    template <class RandomIter>
    void random_shuffle(RandomIter first, RandomIter last) {
        if(first == last) {
            return;
        }

        for(RandomIter i = first + 1; i != last; ++i) {
            MySTL::iter_swap(i, first + (rand() % ((i - first) + 1)));
        }
    }

    // 重载版本使用一个产生随机数的函数对象 rand
    template <class RandomIter, class RandomNumberGenerator>
    void random_shuffle(RandomIter first, RandomIter last, RandomNumberGenerator& rand) {
        if(first == last) {
            return;
        }
        for(auto i = first + 1; i != last; ++i) {
            MySTL::iter_swap(i, first + rand((i - first) + 1));
        }
    }


    /*****************************************************************************************/
    // rotate
    // 将[first, middle)内的元素和 [middle, last)内的元素互换，可以交换两个长度不同的区间
    // 返回 原 first 元素旋转后的迭代器，即 first + (last - n_first)
    // 画图理解算法
    /*****************************************************************************************/
    template <class ForwardIter>
    ForwardIter
    rotate_dispatch(ForwardIter first, ForwardIter middle,
                    ForwardIter last, forward_iterator_tag) 
    {
        ForwardIter first2 = middle;

        do {
            MySTL::swap(*first++, *first2++);
            if(first == middle) {
                middle = first2;
            }
        } while(first2 != last); // 先将后半段移到后面

        /* 上面和下面的过程是相同的，可以做成一个循环，分开是为了记录下返回值 */
        ForwardIter new_middle = first; // 迭代器返回的位置，记录下来
        /* 以下是重复的过程，新的first, 新的middle，重复 */
        first2 = middle;
        while(first2 != last) {
            MySTL::swap(*first++, *first2++);
            if(first == middle)
            {
                middle = first2;
            }
            else if(first2 == last) { // 其实结束条件为 middle 到达 last，就是后半段没有了
                first2 = middle;
            }
        }

        return new_middle;
    }

    // rotate_dispatch 的 bidirectional_iterator_tag 版本
    // 这里的话 双向迭代器，与 forward_iterator_tag相比，可以使用 reverse 算法。
    // 但是不能直接三次旋转，因为 C++ 11 的 rotate 需要 返回原 first 元素旋转后的迭代器。
    // 所以出现了下面的算法
    template <class BidirectIter>
    BidirectIter
    rotate_dispatch(BidirectIter first, BidirectIter middle,
                    BidirectIter last, bidirectional_iterator_tag) 
    {
        if(first == middle) {
            return last;
        }
        if(last == middle) {
            return first;
        }
        
        // 先分段反转
        MySTL::reverse_dispatch(first, middle, bidirectional_iterator_tag());
        MySTL::reverse_dispatch(middle, last, bidirectional_iterator_tag());
        // 再整体反转
        while(first != middle && last != middle) {
            MySTL::swap(*first++, *--last);
            if(first == middle)
            {
                MySTL::reverse_dispatch(middle, last, bidirectional_iterator_tag());
                return last;
            }
            else {
                MySTL::reverse_dispatch(first, middle, bidirectional_iterator_tag());
                return first;
            }
        }
    }

    // rotate_dispatch 的 random_access_iterator_tag 版本
    // TO DO
    // 具体算法讲解参照这篇博客 https://blog.csdn.net/dongyu_1989/article/details/80090640
    template <class RandomIter>
    RandomIter
    rotate_dispatch(RandomIter first, RandomIter middle,
                    RandomIter last, random_access_iterator_tag) 
    {
        if(first == middle) {
            return last;
        }
        if(middle == last) {
            return first;
        }

        auto n = last - first;      // 整体长度
        auto l = middle - first;    // 左边长度
        auto r = n - l;             // 右边长度
        auto result = first + (last - middle); // 返回值，最后 first 指向的元素会被移动到这个位置
        // 如果左边长度等于右边长度的话，直接调用 swap_ranges (可以交换序列1和序列2长度相等的序列)。
        if(l == r) {
            MySTL::swap_ranges(first, middle, middle);
            return result;
        }
        auto cycle_times = gcd(n, l); // 循环位移的遍数，比如 10 , 2。只需要循环遍历 2 次。【1,2,3,4,5,6,7,8,9,10】第一次将 2，4，6，8，10 进行 循环移动，第二次将 1，3，5，7，9 循环移动
        for(auto i = 0; i < cycle_times; ++i) { // 循环 cycle_times 次
            auto tmp = *first; // 先记录下头位置
            auto p = first;
            if(l < r) { // 左边长度小于右边长度
                for(auto j = 0; j < l / cycle_times - 1; ++j) { // 一次循环需要移动几个元素。比如，10，2。j 从 0 到 4，需要移动四个元素
                    if(p > first + r) { 
                        *p = *(p - r);  // 最后把前面的元素放到最后，相当于是一个循环
                        p -= r;
                    }
                    *p = *(p + l); // 把后面的元素往前面移动
                    p += l;
                }
            }
            else {
                for(auto j = 0; j < l / cycle_times - 1; ++j) {
                    if(p < last - l) {
                        *p = *(p + l);
                        p += l;
                    }
                    *p = *(p - r);
                    p -= r;
                }
            }
            *p = tmp;
            ++first;
        }

        return result;
    }

    // 求最大公因子(公约数)，辗转相除法
    template <class EuclideanRingElement>
    EuclideanRingElement gcd(EuclideanRingElement m, EuclideanRingElement n)
    {
        while(n != 0) {
            EuclideanRingElement t = m % n;
            m = n;
            n = t;
        }

        return m;
    }

    // // 辗转相除法一行代码
    // int gcd(int m, int n) {
    //     return n == 0 ? m : gcd(n, m % n);
    // }

    template <class ForwardIter>
    ForwardIter
    rotate(ForwardIter first, ForwardIter middle, ForwardIter last) {
        if(first == middle) {
            return last;
        }
        if(middle == last) {
            return first;
        }

        return MySTL::rotate_dispatch(first, middle, last, iterator_category(first));
    }


    /*****************************************************************************************/
    // rotate_copy
    // 行为与 rotate 类似，不同的是将结果复制到 result 所指的容器中
    // 由于不需要原地在原容器中调整，实现也就简单了许多，只要先把后端内容复制到新容器的前端，再
    // 把前段接续复制到新容器即可
    /*****************************************************************************************/
    template <class ForwardIter, class OutputIter>
    ForwardIter
    rotate_copy(ForwardIter first, ForwardIter middle,
                ForwardIter last, OutputIter result)
    {
        return MySTL::copy(first, middle, MySTL::copy(middle, last, result));
    }
    

    // TO DO, 性能优化
    /*****************************************************************************************/
    // is_permutation 
    // 判断[first1,last1)是否为[first2, last2)的排列组合
    /*****************************************************************************************/
    template <class ForwardIter1, class ForwardIter2, class BinaryPred>
    bool is_permutation_aux(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred)
    {
        if(MySTL::distance(first1, last1) != MySTL::distance(first2, last2)) {
            return false;
        }

        for(auto it = first1; it != last1; ++it) {
            if(MySTL::find(first1, it, [it ,pred](typename iterator_traits<ForwardIter1>::value_type x) { return pred(*it, x); }) != it)
                continue;
            // 统计元素个数
            auto count1 = MySTL::count_if(it, last1, [it,pred](typename iterator_traits<ForwardIter1>::value_type x) { return pred(*it, x); });
            auto count2 = MySTL::count_if(first2, last2, [it,pred](typename iterator_traits<ForwardIter1>::value_type x) { return pred(*it, x); });
            if(count1 != count2) {
                return false;
            }
        }
 
        return true;
    }

    template <class ForwardIter1, class ForwardIter2, class BinaryPred>
    bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred)
    {
        return is_permutation_aux(first1, last1, first2, last2, pred);
    }


    template <class ForwardIter1, class ForwardIter2>
    bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2)
    {   
        using v1 = iterator_traits<ForwardIter1>::value_type;
        using v2 = iterator_traits<ForwardIter2>::value_type;
        static_assert(MySTL::is_same<v1, v2>::value, "the type should be same in MySTL::is_permutation");

        return is_permutation_aux(first1, last1, first2, last2, MySTL::equal_to<v1>()); // TO DO
    }



    /*****************************************************************************************/
    // next_permutation
    // 取得[first, last)所标示序列的下一个排列组合，如果没有下一个排序组合，返回 false，否则返回 true
    /*****************************************************************************************/
    // 思想：
    // 1、找下一个排列，也就是希望一个数更大
    // 2、这样就需要将后面的大数和前面的小数进行交换，就能得到一个更大的数
    // 3、我们还希望下一个数增加的幅度尽可能小，因此，在尽可能靠右的低位查找，也就是从后往前查找。找到后将 尽可能小的大数和前面的小数进行交换
    // 4、将大数换到前面之后，需要将大数后面的所有数重置为升序，升序排列就是最小的排列
    template <class BidirectionalIter>
    bool next_permutation(BidirectionalIter first, BidirectionalIter last)
    {
        // 空或者只有一个元素，没有下一个排列
        BidirectionalIter i = last;
        if(first == last || first == --i) {
            return false;
        }

        for(;;) {
            BidirectionalIter ii = i;
            --i;
            // 锁定两个两个相邻元素 ii 和 i
            if(*i < *ii) { // 如果前一个元素小于后一个元素
                BidirectionalIter j = last; // 令 j 指向尾端
                while(!(*i < *--j)) {}
                MySTL::iter_swap(i， j); // 交换 i,j 所指元素 
                MySTL::reverse(ii, last); // 将 ii 之后（包括 ii）所有元素反转
                return true;
            }

            if(i == first) { // 进行到最前面了
                MySTL::reverse(first, last);
                return false;
            }
        }
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class BidirectionalIter, class Compare>
    bool next_permutation(BidirectionalIter first, BidirectionalIter last, Compare comp)
    {
        // 空或者只有一个元素，没有下一个排列
        BidirectionalIter i = last;
        if(first == last || first == --i) {
            return false;
        }

        for(;;) {
            BidirectionalIter ii = i;
            --i;
            // 锁定两个两个相邻元素 ii 和 i
            if(comp(*i, *ii)) { // 如果前一个元素小于后一个元素
                BidirectionalIter j = last; // 令 j 指向尾端
                while(!comp(*i, *--j)) {}
                MySTL::iter_swap(i， j); // 交换 i,j 所指元素 
                MySTL::reverse(ii, last); // 将 ii 之后（包括 ii）所有元素反转
                return true;
            }

            if(i == first) { // 进行到最前面了
                MySTL::reverse(first, last);
                return false;
            }
        }
    }


    /*****************************************************************************************/
    // prev_permutation
    // 取得[first, last)所标示序列的上一个排列组合，如果没有上一个排序组合，返回 false，否则返回 true
    /*****************************************************************************************/
    template <class BidirectionalIter>
    bool pre_premutation(BidirectionalIter first, BidirectionalIter last) {
        BidirectionalIter i = last;
        i--;
        if(first == last || first == i) {
            return false;
        }
        for(;;) {
            BidirectionalIter ii = i;
            --i;
            if(*ii < *i) {
                BidirectionalIter j = last;
                while(!(*--j < *i)) {}
                MySTL::iter_swap(i, j);
                MySTL::reverse(ii, last);
                return true;
            }
            if(i == last) {
                MySTL::reverse(first, last);
                return false;
            }
        }
    }

    template <class BidirectionalIter, class Compare>
    bool pre_premutation(BidirectionalIter first, BidirectionalIter last, Compare comp) {
        BidirectionalIter i = last;
        i--;
        if(first == last || first == i) {
            return false;
        }
        for(;;) {
            BidirectionalIter ii = i;
            --i;
            if(comp(*ii, *i)) {
                BidirectionalIter j = last;
                while(!(comp(*--j, *i))) {}
                MySTL::iter_swap(i, j);
                MySTL::reverse(ii, last);
                return true;
            }
            if(i == last) {
                MySTL::reverse(first, last);
                return false;
            }
        }
    }

    /*****************************************************************************************/
    // merge
    // 将两个经过排序的集合 S1 和 S2 合并起来置于另一段空间，返回一个迭代器指向最后一个元素的下一位置
    /*****************************************************************************************/
    // 版本1：默认是升序排列
    template <class InputIter1, class InputIter2, class OutputIter>
    OutputIter
    merge(InputIter1 first1, InputIter1 last1,
          InputIter2 first2, InputIter2 last2, OutputIter result)
    {
        while(first1 != last1 && first2 != last2) {
            if(*first2 < *first1) {
                *result = *first2;
                ++first2;
            }
            else {
                *result = first1;
                ++first1;
            }
            ++result;
        }

        return MySTL::copy(first2, last2, MySTL::copy(first1, last1, result));
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIter1, class InputIter2, class OutputIter, class Compare>
    OutputIter
    merge(InputIter1 first1, InputIter1 last1,
          InputIter2 first2, InputIter2 last2, OutputIter result, Compare comp)
    {
        while(first1 != last1 && first2 != last2) {
            if(comp(*first2, *first1)) {
                *result = *first2;
                ++first2;
            }
            else {
                *result = first1;
                ++first1;
            }
            ++result;
        }

        return MySTL::copy(first2, last2, MySTL::copy(first1, last1, result));
    }


    /*****************************************************************************************/
    // inplace_merge
    // 把连接在一起的两个有序序列结合成单一序列并保持有序
    /*****************************************************************************************/
    // 参考http://www.cocoachina.com/articles/135990
    // |********A（< *first_cut）********||********B（>= *first_cut）********||********C（< *first_cut）********||********D（>= *first_cut）********|
    // 然后交换 B 和 C，递归执行下去
    template <class BidirectionalIter, class Distance>
    void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
                              BidirectionalIter last, Distance len1, Distance len2)
    {   
        // 递归结束条件
        if(len1 == 0 || len2 == 0) {
            return;
        }
        if(len1 + len2 == 2) {
            if(*middle < *first)) {
                MySTL::iter_swap(first, middle);
            }

            return;
        }
        
        BidirectionalIter first_cut = first;
        BidirectionalIter second_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        if(len1 > len2)
        {
            len11 = len1 >> 1;
            MySTL::advance(first_cut, len11);
            second_cut = MySTL::lower_bound(middle, last, *first_cut);
            len22 = MySTL::distance(middle, second_cut);
        }
        else 
        {
            len22 = len2 >> 1;
            MySTL::advance(second_cut, len22);
            first_cut = MySTL::upper_bound(first, middle, *second_cut);
            len11 = MySTL::distance(first, first_cut);
        }
        BidirectionalIter new_middle = MySTL::rotate(first_cut, middle, second_cut);
        MySTL::merge_without_buffer(first, first_cut, new_middle, len11, len22);
        MySTL::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22);
    }

    // merge_adaptive
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter1
    merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
                   BidirectionalIter2 first2, BidirectionalIter2 last2,
                   BidirectionalIter1 result)
    {
        if(first1 == last1) {
            return MySTL::copy_backward(first2, last2, result);
        }
        if(first2 == last2) {
            return MySTL::copy_backward(first1, last1, result);
        }

        --last1;
        --last2;
        while(true)
        {
            if(*last2 < *last1)
            {
                *--result = *last1;
                if(first1 == last1) {
                    return MySTL::copy_backward(first2, ++last2, result);
                }
                --last1;
            }
            else 
            {
                **--result = *last2;
                if(first2 == last2) {
                    return MySTL::copy_backward(first1, ++last1, result);
                }
                --last2;
            }
        }
    }

    // 参考 《STL 源码剖析》P405
    template <class BidirectionalIter, class Distance, class Pointer>
    void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
                        BidirectionalIter last, Distance len1, Distance len2,
                        Pointer buffer, Distance buffer_size)
    {   
        
        if(len1 <= len2 && len1 <= buffer_size)
        {
            // case1.缓冲区足够安置序列一
            Pointer buffer_end = MySTL::copy(first, middle, buffer);
            MySTL::merge(buffer, buffer_end, middle, last, first);
        }
        else if(len2 <= buffer_size) 
        {
            // case2.缓冲区足够安置序列二
            Pointer buffer_end = MySTL::copy(middle, last, buffer);
            MySTL::merge_backward(first, middle, buffer, buffer_end, last);

        }
        else 
        {
            // case3.缓冲区空间不足安置任何一个序列
            BidirectionalIter first_cut = first;
            BidirectionalIter second_cut = middle;
            Distance len11 = 0;
            Distance len22 = 0;
            if(len1 > len2)
            {
                len11 = len1 >> 1;
                MySTL::advance(first_cut, len11);
                second_cut = MySTL::lower_bound(middle, last, *first_cut);
                len22 = MySTL::distance(middle, second_cut);
            }
            else 
            {
                len22 = len2 >> 1;
                MySTL::advance(second_cut, len22);
                first_cut = MySTL::upper_bound(first, middle, *second_cut);
                len11 = MySTL::distance(first, first_cut);
            }
            BidirectionalIter new_middle = MySTL::rotate_adaptive(first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);
            // 针对左段，递归调用
            MySTL::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size);
            // 针对右段，递归调用
            MySTL::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size);
        }
    }

    template <class BidirectionalIter, class T>
    void 
    inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle,
                      BidirectionalIter last, T*)
    {
        auto len1 = MySTL::distance(first, middle);
        auto len2 = MySTL::distance(middle, last);
        temporary_buffer<BidirectionalIter, T> buf(first, last);
        if(!buf.begin()) // 内存配置失败，进行合并
        {
            MySTL::merge_without_buffer(first, middle, last, len1, len2);
        }
        else    // 在有暂时缓冲区的情况下进行合并
        {
            MySTL::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size());
        }
    }

    // 版本1：使用 operator< 进行比较
    template <class BidirectionalIter>
    void 
    inplace_merge(BidirectionalIter first, BidirectionalIter middle,
                  BidirectionalIter last)
    {
        if(first == middle || middle == last) {
            return;
        }
        MySTL::inpalce_merge_aux(first, middle, last, value_type(first));
    }
    
    // 参考http://www.cocoachina.com/articles/135990
    // |********A（< *first_cut）********||********B（>= *first_cut）********||********C（< *first_cut）********||********D（>= *first_cut）********|
    // 然后交换 B 和 C，递归执行下去
    template <class BidirectionalIter, class Distance, class Compare>
    void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
                              BidirectionalIter last, Distance len1, Distance len2, Compare comp)
    {   
        // 递归结束条件
        if(len1 == 0 || len2 == 0) {
            return;
        }
        if(len1 + len2 == 2) {
            if(comp(*middle, *first)) {
                MySTL::iter_swap(first, middle);
            }

            return;
        }
        
        BidirectionalIter first_cut = first;
        BidirectionalIter second_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        if(len1 > len2)
        {
            len11 = len1 >> 1;
            MySTL::advance(first_cut, len11);
            second_cut = MySTL::lower_bound(middle, last, *first_cut, comp);
            len22 = MySTL::distance(middle, second_cut);
        }
        else 
        {
            len22 = len2 >> 1;
            MySTL::advance(second_cut, len22);
            first_cut = MySTL::upper_bound(first, middle, *second_cut, comp);
            len11 = MySTL::distance(first, first_cut);
        }
        BidirectionalIter new_middle = MySTL::rotate(first_cut, middle, second_cut);
        MySTL::merge_without_buffer(first, first_cut, new_middle, len11, len22, comp);
        MySTL::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
    }

    // merge_adaptive comp
    template <class BidirectionalIter1, class BidirectionalIter2, class Compare>
    BidirectionalIter1
    merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
                   BidirectionalIter2 first2, BidirectionalIter2 last2,
                   BidirectionalIter1 result, Compare comp)
    {
        if(first1 == last1) {
            return MySTL::copy_backward(first2, last2, result);
        }
        if(first2 == last2) {
            return MySTL::copy_backward(first1, last1, result);
        }

        --last1;
        --last2;
        while(true)
        {
            if(comp(*last2, *last1))
            {
                *--result = *last1;
                if(first1 == last1) {
                    return MySTL::copy_backward(first2, ++last2, result);
                }
                --last1;
            }
            else 
            {
                **--result = *last2;
                if(first2 == last2) {
                    return MySTL::copy_backward(first1, ++last1, result);
                }
                --last2;
            }
        }
    }

    // rotate_adaptive 和 rotate 的功效差不多，只是针对缓冲区的存在，做了优化
    template <class BidirectionalIter1, class BidirectionalIter2, class Distance>
    BidirectionalIter1
    rotate_adaptive(BidirectionalIter1 first, BidirectionalIter1 middle,
                    BidirectionalIter1 last, Distance len1, Distance len2,
                    BidirectionalIter2 buffer, Distance buffer_size)
    {
        BidirectionalIter2 buffer_end;
        if(len1 > len2 && len2 <= buffer_size)
        {
            buffer_end = MySTL::copy(middle, last, buffer);
            MySTL::copy_backward(first, middle, last);
            return MySTL::copy(buffer, buffer_end, first);
        }
        else if(len1 <= buffer_size) {
            buffer_end = MySTL::copy(first, middle, buffer);
            MySTL::copy(middle, last, first);
            return MySTL::copy_backward(buffer, buffer_end, last);
        }
        else {
            return MySTL::rotate(first, middle, last);
        }
    }

    // 参考 《STL 源码剖析》P405
    template <class BidirectionalIter, class Distance, class Pointer, class Compare>
    void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
                        BidirectionalIter last, Distance len1, Distance len2,
                        Pointer buffer, Distance buffer_size, Compare comp)
    {   
        
        if(len1 <= len2 && len1 <= buffer_size)
        {
            // case1.缓冲区足够安置序列一
            Pointer buffer_end = MySTL::copy(first, middle, buffer);
            MySTL::merge(buffer, buffer_end, middle, last, first, comp);
        }
        else if(len2 <= buffer_size) 
        {
            // case2.缓冲区足够安置序列二
            Pointer buffer_end = MySTL::copy(middle, last, buffer);
            MySTL::merge_backward(first, middle, buffer, buffer_end, last, comp);

        }
        else 
        {
            // case3.缓冲区空间不足安置任何一个序列
            BidirectionalIter first_cut = first;
            BidirectionalIter second_cut = middle;
            Distance len11 = 0;
            Distance len22 = 0;
            if(len1 > len2)
            {
                len11 = len1 >> 1;
                MySTL::advance(first_cut, len11);
                second_cut = MySTL::lower_bound(middle, last, *first_cut, comp);
                len22 = MySTL::distance(middle, second_cut);
            }
            else 
            {
                len22 = len2 >> 1;
                MySTL::advance(second_cut, len22);
                first_cut = MySTL::upper_bound(first, middle, *second_cut, comp);
                len11 = MySTL::distance(first, first_cut);
            }
            BidirectionalIter new_middle = MySTL::rotate_adaptive(first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);
            // 针对左段，递归调用
            MySTL::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size, comp);
            // 针对右段，递归调用
            MySTL::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size, comp);
        }
    }

    template <class BidirectionalIter, class T, class Compare>
    void 
    inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle,
                      BidirectionalIter last, T*, Compare comp)
    {
        auto len1 = MySTL::distance(first, middle);
        auto len2 = MySTL::distance(middle, last);
        temporary_buffer<BidirectionalIter, T> buf(first, last);
        if(!buf.begin()) // 内存配置失败，进行合并
        {
            MySTL::merge_without_buffer(first, middle, last, len1, len2, comp);
        }
        else    // 在有暂时缓冲区的情况下进行合并
        {
            MySTL::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size(), comp);
        }
    }

     // 版本2：使用 comp 进行比较
    template <class BidirectionalIter, class Compare>
    void 
    inplace_merge(BidirectionalIter first, BidirectionalIter middle,
                  BidirectionalIter last, Compare comp)
    {
        if(first == middle || middle == last) {
            return;
        }
        MySTL::inpalce_merge_aux(first, middle, last, value_type(first), comp);
    }


    // TO DO, heap 算法
    /*****************************************************************************************/
    // partial_sort
    // 对整个序列做部分排序，保证较小的 N 个元素以递增顺序置于[first, first + N)中
    /*****************************************************************************************/
    template <class RandomIter>
    void partial_sort(RandomIter first, RandomIter middle, RandomIter last)
    {
        MySTL::make_heap(first, middle);
        for(auto i = middle; i < last; ++i) {
            if(*i < *first)
            {
                MySTL::pop_heap_aux(first, middle, i, *i, distance_type(first));
            }
        }

        MySTL::sort_heap(first, middle);
    }

    template <class RandomIter, class Compare>
    void partial_sort(RandomIter first, RandomIter middle, RandomIter last, Compare comp)
    {
        MySTL::make_heap(first, middle);
        for(auto i = middle; i < last; ++i) {
            if(comp(*i, *first))
            {
                MySTL::pop_heap_aux(first, middle, i, *i, distance_type(first), comp);
            }
        }

        MySTL::sort_heap(first, middle, comp);
    }


    /*****************************************************************************************/
    // partial_sort_copy
    // 行为与 partial_sort 类似，不同的是把排序结果复制到 result 容器中
    /*****************************************************************************************/
    template <class InputIter, class RandomIter, class Distance>
    RandomIter
    psort_copy_aux(InputIter first, InputIter last,
                   RandomIter result_first, RandomIter result_last,
                   Distance*)
    {
        if(result_first == result_last)
            return result_last;
        RandomIter result_iter = result_first;
        while(first != last && result_iter != result_last) {
            *result_iter = *first;
            ++result_iter;
            ++first;
        }

        MySTL::make_heap(result_first, result_iter);
        while(first != last)
        {
            if(*first < *result_first) {
                MySTL::adjust_heap(result_first, static_cast<Distance>(0), result_iter - result_first, *first);
            }
            ++first;
        }
        MySTL::sort_heap(result_first, result_iter);
        return result_iter;
    }

    template <class InputIter, class RandomIter>
    RandomIter
    paritial_sort_copy(InputIter first, InputIter last,
                       RandomIter result_first, RandomIter result_last)
    {
        return MySTL::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first));
    }

    template <class InputIter, class RandomIter, class Distance, class Compare>
    RandomIter
    psort_copy_aux(InputIter first, InputIter last,
                   RandomIter result_first, RandomIter result_last,
                   Distance*, Compare comp)
    {
        if(result_first == result_last)
            return result_last;
        RandomIter result_iter = result_first;
        while(first != last && result_iter != result_last) {
            *result_iter = *first;
            ++result_iter;
            ++first;
        }

        MySTL::make_heap(result_first, result_iter, comp);
        while(first != last)
        {
            if(comp(*first, *result_first)) {
                MySTL::adjust_heap(result_first, static_cast<Distance>(0), result_iter - result_first, *first, comp);
            }
            ++first;
        }
        MySTL::sort_heap(result_first, result_iter, comp);
        return result_iter;
    }

    template <class InputIter, class RandomIter, class Compare>
    RandomIter
    paritial_sort_copy(InputIter first, InputIter last,
                       RandomIter result_first, RandomIter result_last,
                       Compare comp)
    {
        return MySTL::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first), comp);
    }


    /*****************************************************************************************/
    // partition
    // 对区间内的元素重排，被一元条件运算判定为 true 的元素会放到区间的前段
    // 该函数不保证元素的原始相对位置
    // 返回值为后半段的首元素
    /*****************************************************************************************/    
    template <class ForwardIter, class UnaryPredicate>
    ForwardIter
    partition(ForwardIter first, ForwardIter last,
            UnaryPredicate unary_pred)
    {
        if(first == last) {
            return first;
        }

        // 先找到第一个不满足条件的位置
        while(unary_pred(*first)) {
            if(++first == last) {
                return first;
            }
        }

        ForwardIter next = first;

        while(++next != last) {
            if(unary_pred(*next)) {
                MySTL::iter_swap(first, next);
                ++first;
            }
        }

        return first;
    }

    template <class BidirectionalIter, class UnaryPredicate>
    BidirectionalIter
    partition(BidirectionalIter first, BidirectionalIter last,
            UnaryPredicate unary_pred)
    {
        while(true) {
            while(first != last && unary_pred(*first)) { // 找到第一个不满足 条件 的 first位置
                ++first;
            }
            if(first == last) {
                break;
            }
            --last;
            while(first != last && !unary_pred(*last)) { //找到最后一个满足 条件的 last 位置
                --last;
            }
            if(first == last) {
                break;
            }
            MySTL::iter_swap(first, last); // 交换
            ++first;
        }

        return first;
    }

    template <class ForwardIter, class UnaryPredicate>
    ForwardIter partition(ForwardIter first, ForwardIter last, UnaryPredicate unary_pre)
    {
        return partition_aux(first, last, unary_pre, iterator_category(first));
    }

    
    /*****************************************************************************************/
    // sort
    // 将[first, last)内的元素以递增的方式排序
    /*****************************************************************************************/
    // 在小数据量的情况下，简单的插入排序要比 快速排序要快 ———— 快速排序需要函数递归调用
    enum { threshold_ = 16 }; 

    //-------------------------------------------------- 默认
    // 找出 2 ^ k <= n 的最大值 k。；例如：n = 7, k = 2; n = 20, k = 4; n = 8; k = 3;
    // 用来控制分割恶化的情况
    template <class Size>
    Size lg2(Size n) 
    {
        Size k = 0;
        for(; n > 1; n >>= 1) {
            ++k;
        }

        return k;
    }

    // 达到的效果是：以此时的 first 为轴，将序列分为左右两半，左半部分的所有元素值都小于等于中枢值，右半部分的所有元素值都大于等于中枢值
    template <class RandomIter, class T>
    RandomIter
    unchecked_partition(RandomIter first, RandomIter last, const T& pivot)
    {
        while(true) {
            while(*first < pivot) { // first 找到 >= pivot 就停下来
                ++first;
            }
            --last; //调整
            while(pivot < *last) {  // last 找到 <= pivot 就停下来
                --last;
            }
            if(!(first < last)) {
                return first;
            }
            MySTL::iter_swap(first, last);
            ++first;
        }
    }

    template <class RandomIter, class Size>
    void intro_sort(RandomIter first, RandomIter last, Size depth_limit) 
    {
        while(last - first > static_cast<int>(threshold_))
        {
            if(depth_limit == 0)    // 至此，递归层次过深
            {
                MySTL::partial_sort(first, last, last); // 改为用 heap_sort
                return;
            }

            --depth_limit;
            auto mid = MySTL::median(*(first), *(first + (last - first) / 2), *(last - 1)); // 三点中值（头、尾、中央），作为中枢轴
            auto cut = MySTL::unchecked_partition(first, last, mid);
            MySTL::intro_sort(cut, last, depth_limit); // 对右半段递归执行 sort
            last = cut; // 现在回到 while 函数，准备对左半段递归进行 sort
        }
    }

    template <class RandomIter, class T>
    void unchecked_linear_insert(RandomIter last, const T& value) 
    {
        RandomIter next = last;
        --next;
        while(value < *next) {
            *last = *next;
            last = next;
            --next;
        }
        *last = value;
    }

    template <class RandomIter>
    void unchecked_insertion_sort(RandomIter first, RandomIter last)
    {
        for (auto i = first; i != last; ++i)
        {
            MySTL::unchecked_linear_insert(i, *i);
        }
    }


    // 插入排序的改进版
    // 先和头进行比较，如果大于头，就不直接一个一个比较了，直接将前面的元素往后拷贝，然后将头置为 value
    template <class RandomIter>
    void insertion_sort(RandomIter first, RandomIter last)
    {
        if(first == last)
            return;
        for(auto i = first + 1; i != last; ++i) {
            auto value = *i;
            if(value < *first) { 
                MySTL::copy_backward(first, i, i + 1);
                *first = value;
            }
            else { // 尾不小于头
                MySTL::unchecked_linear_insert(i, value);
            }
        }
    }

    template <class RandomIter>
    void final_insertion_sort(RandomIter first, RandomIter last)
    {
        if(last - first > static_cast<int>(threshold_))
        {
            MySTL::insertion_sort(first, first + threshold_);
            MySTL::unchecked_insertion_sort(first + threshold_, last);
        }
        else {
            MySTL::insertion_sort(first, last);
        }
    }


    template <class RandomIter>
    void sort(RandomIter first, RandomIter last)
    {
        if(first != last) {
            MySTL::intro_sort(first, last, lg2(last - first) * 2);
            MySTL::final_insertion_sort(first, last);
        }
    }
    //-------------------------------------------------- 默认


    //-------------------------------------------------- 重载版本使用函数对象 comp 代替比较操作
    // 分割函数 unchecked_partition
    template <class RandomIter, class T, class Compared>
    RandomIter
    unchecked_partition(RandomIter first, RandomIter last,
                        const T& pivot, Compared comp)
    {
        while (true)
        {
            while (comp(*first, pivot))
                ++first;
            --last;
            while (comp(pivot, *last))
                --last;
            if (!(first < last))
                return first;
            MySTL::iter_swap(first, last);
            ++first;
        }
    }

    // 内省式排序，先进行 quick sort，当分割行为有恶化倾向时，改用 heap sort
    template <class RandomIter, class Size, class Compared>
    void intro_sort(RandomIter first, RandomIter last,
                    Size depth_limit, Compared comp)
    {
        while (last - first > static_cast<int>(threshold_))
        {
            if (depth_limit == 0)
            {                            // 到达最大分割深度限制
                MySTL::partial_sort(first, last, last, comp);  // 改用 heap_sort
                return;
            }
            --depth_limit;
            auto mid = MySTL::median(*(first), *(first + (last - first) / 2), *(last - 1));
            auto cut = MySTL::unchecked_partition(first, last, mid, comp);
            MySTL::intro_sort(cut, last, depth_limit, comp);
            last = cut;
        }
    }

    // 插入排序辅助函数 unchecked_linear_insert
    template <class RandomIter, class T, class Compared>
    void unchecked_linear_insert(RandomIter last, const T& value, Compared comp)
    {
        auto next = last;
        --next;
        while (comp(value, *next))
        {  // 从尾部开始寻找第一个可插入位置
            *last = *next;
            last = next;
            --next;
        }
        *last = value;
    }

    // 插入排序函数 unchecked_insertion_sort
    template <class RandomIter, class Compared>
    void unchecked_insertion_sort(RandomIter first, RandomIter last,
                                Compared comp)
    {
        for (auto i = first; i != last; ++i)
        {
            MySTL::unchecked_linear_insert(i, *i, comp);
        }
    }

    // 插入排序函数 insertion_sort
    template <class RandomIter, class Compared>
    void insertion_sort(RandomIter first, RandomIter last, Compared comp)
    {
        if (first == last)
            return;
        for (auto i = first + 1; i != last; ++i)
        {
            auto value = *i;
            if (comp(value, *first))
            {
                MySTL::copy_backward(first, i, i + 1);
                *first = value;
            }
            else
            {
                MySTL::unchecked_linear_insert(i, value, comp);
            }
        }
    }

    // 最终插入排序函数 final_insertion_sort
    template <class RandomIter, class Compared>
    void final_insertion_sort(RandomIter first, RandomIter last, Compared comp)
    {
        if (last - first > static_cast<int>(threshold_))
        {
            MySTL::insertion_sort(first, first + threshold_, comp);
            MySTL::unchecked_insertion_sort(first + threshold_, last, comp);
        }
        else
        {
            MySTL::insertion_sort(first, last, comp);
        }
    }
    
    template <class RandomIter, class Compare>
    void sort(RandomIter first, RandomIter last, Compare comp)
    {
        if(first != last) {
            MySTL::intro_sort(first, last, lg2(last - first) * 2, comp);
            MySTL::final_insertion_sort(first, last, comp);
        }
    }
    //--------------------------------------------------


    /*****************************************************************************************/
    // nth_element
    // 对序列重排，使得所有小于第 n 个元素的元素出现在它的前面，大于它的出现在它的后面
    /*****************************************************************************************/
    template <class RandomIter>
    void nth_element(RandomIter first, RandomIter nth, RandomIter last) 
    {
        if(nth == last) {
            return;
        }

        while(last - first > 3) {
            auto cut = MySTL::unchecked_partition(first, last, MySTL::median(*first,
                                *(first + (last - first) / 2),
                                *(last - 1)));
            if(cut <= nth) {
                first = cut;
            }
            else {
                last = cut;
            }
        }
        MySTL::insertion_sort(first, last);
    }

    template <class RandomIter, class Compare>
    void nth_element(RandomIter first, RandomIter nth, RandomIter last, Compare comp) 
    {
        if(nth == last) {
            return;
        }

        while(last - first > 3) {
            auto cut = MySTL::unchecked_partition(first, last, MySTL::median(*first,
                                *(first + (last - first) / 2),
                                *(last - 1)), comp);
            if(cut <= nth) {
                first = cut;
            }
            else {
                last = cut;
            }
        }
        MySTL::insertion_sort(first, last, comp);
    }


    /*****************************************************************************************/
    // unique_copy
    // 从[first, last)中将元素复制到 result 上，序列必须有序，如果有重复的元素，只会复制一次
    /*****************************************************************************************/
    // unique_copy_dispatch 的 forward_iterator_tag 版本
    template <class InputIter, class ForwardIter>
    ForwardIter 
    unique_copy_dispatch(InputIter first, InputIter last, ForwardIter result, forward_iterator_tag)
    {
        *result = *first;
        while(++first != last) {
            if(*result != *first) {
                *++result = *first;
            }
        }

        return ++result;
    }

    // unique_copy_dispatch 的 output_iterator_tag 版本
    // output_iterator_tag 是 唯写，不能进行读操作 *result != *first
    template <class InputIter, class ForwardIter>
    ForwardIter 
    unique_copy_dispatch(InputIter first, InputIter last, ForwardIter result, forward_iterator_tag)
    {
        auto value = *first;
        *result = value;
        while(++first != last) {
            if(value != *first) {
                value = *first;
                *++result = value;
            }
        }
    }


    template <class InputIter, class OutputIter>
    OutputIter
    unique_copy(InputIter first, InputIter last, OutputIter result) 
    {
        if(first == last) {
            return result;
        }

        return MySTL::unique_copy_dispatch(first, last, result, iterator_category(result));
    }

    // unique_copy_dispatch 的 output_iterator_tag 版本
    // output_iterator_tag 是 唯写，不能进行读操作 *result != *first
    template <class InputIter, class ForwardIter, class Compare>
    ForwardIter 
    unique_copy_dispatch(InputIter first, InputIter last, ForwardIter result, forward_iterator_tag, Compare comp)
    {
        auto value = *first;
        *result = value;
        while(++first != last) {
            if(!comp(value, *first)) {
                value = *first;
                *++result = value;
            }
        }
    }

    // unique_copy_dispatch 的 forward_iterator_tag 版本
    template <class InputIter, class ForwardIter, class Compare>
    ForwardIter 
    unique_copy_dispatch(InputIter first, InputIter last, ForwardIter result, forward_iterator_tag, Compare comp)
    {
        *result = *first;
        while(++first != last) {
            if(!comp(*result, *first)) {
                *++result = *first;
            }
        }

        return ++result;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template <class InputIter, class OutputIter, class Compare>
    OutputIter
    unique_copy(InputIter first, InputIter last, OutputIter result, Compare comp) 
    {
        if(first == last) {
            return result;
        }

        return MySTL::unique_copy_dispatch(first, last, result, iterator_category(result), comp);
    }


    /*****************************************************************************************/
    // unique
    // 移除[first, last)内重复的元素，序列必须有序，和 remove 类似，它也不能真正的删除重复元素
    /*****************************************************************************************/
    template <class ForwardIter>
    ForwardIter unique(ForwardIter first, ForwardIter last)
    {
        first = MySTL::adjacent_find(first, last);
        return MySTL::unique_copy(first, last, first);
    }

    // 重载版本
    template <class ForwardIter, class Compare>
    ForwardIter unique(ForwardIter first, ForwardIter last, Compare comp)
    {
        first = MySTL::adjacent_find(first, last, comp);
        return MySTL::unique_copy(first, last, first, comp);
    }

} // namespace MySTL

#endif
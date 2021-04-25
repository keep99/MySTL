/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-25 16:24:28
 * @LastEditTime: 2021-04-25 18:37:25
 * @LastEditors: Chen.Yu
 */
#ifndef _ALGO_H_
#define _ALGO_H_
#include <cstddef>
#include "algobase.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"

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
    // 在[first, last)区间内找到等于 value 的元素，返回指向该元素的迭代器
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
    find(InputIter first, InputIter last, UnaryPredicate unary_pred) {
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

    /*****************************************************************************************/
    // lower_bound
    // 在[first, last)中查找 第一个不小于value的元素，并返回指向它的迭代器，若没有则返回 last
    /*****************************************************************************************/
    


}




#endif
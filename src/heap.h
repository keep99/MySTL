#ifndef _HEAP_H
#define _HEAP_H

#include <iterator_base.h>

#include <cstddef>

namespace MySTL {
    /*****************************************************************************************/
    // push_heap
    // 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
    /*****************************************************************************************/
    // 上溯
    template <class RandomIter, class Distance, class T>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value)
    {
        auto parent = (holeIndex - 1) / 2; // 找出父节点
        while(holeIndex > topIndex && *(first + parent) < value) { 
            // 当前尚未到达顶端，且父节点小于新值
            // 由于以上使用 operator< , 所以 heap 为 大顶堆
            *(first + holeIndex) = *(first + parent); // 令洞值为父值
            holeIndex = parent;                       // 调整洞号，向上提升父节点
            parent = (holeIndex - 1) / 2;             // 新洞的父节点
        }
        *(first + holeIndex) = value;                 // 令洞值为新值，完成插入操作
    }

    template <class RandomIter, class Distance>
    void push_heap_d(RandomIter first, RandomIter last, Distance*)
    {
        MySTL::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
    }

    template <class RandomIter>
    void push_heap(RandomIter first, RandomIter last) 
    {
        MySTL::push_heap_d(first, last, distance_type(first));
    }

    template <class RandomIter, class Distance, class T, class Compare>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value, Compare comp)
    {
        auto parent = (holeIndex - 1) / 2; // 找出父节点
        while(holeIndex > topIndex && comp(*(first + parent), value)) { 
            // 当前尚未到达顶端，且父节点小于新值
            // 由于以上使用 operator< , 所以 heap 为 大顶堆
            *(first + holeIndex) = *(first + parent); // 令洞值为父值
            holeIndex = parent;                       // 调整洞号，向上提升父节点
            parent = (holeIndex - 1) / 2;             // 新洞的父节点
        }
        *(first + holeIndex) = value;                 // 令洞值为新值，完成插入操作
    }

    template <class RandomIter, class Distance, class Compare>
    void push_heap_d(RandomIter first, RandomIter last, Distance*, Compare comp)
    {
        MySTL::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), comp);
    }

    template <class RandomIter, class Compare>
    void push_heap(RandomIter first, RandomIter last, Compare comp) 
    {
        MySTL::push_heap_d(first, last, distance_type(first), comp);
    }

    /*****************************************************************************************/
    // pop_heap
    // 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
    /*****************************************************************************************/
    // 分为下溯和上溯两个过程
    // 下溯是把左右子节点中更大的值往上搬，但是没有和那个被 堆顶覆盖的 value 进行比较
    // 上溯，被堆顶覆盖的元素不一定就能被放到叶子节点，其实还需要再进行一次上溯操作
    // 参考 《STL 源码剖析》 P176
    template <class RandomIter, class T, class Distance>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value)
    {
        Distance topIndex = holeIndex;
        Distance rchild = 2 * holeIndex + 2; // 洞节点的右子节点
        while(rchild < len) {
            if(*(first + rchild) < *(first + rchild - 1)) {
                // 比较洞节点的左右两个子节点的值，找出较大的那个，以 rchild 代表较大的子节点
                rchild--;
            }
            
            // 下沉，令较大子节点为洞值，再令洞号下移至较大子节点处
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = 2 * (rchild + 1);
        }

        if(rchild == len)
        {
            // 没有右子节点，只有左子节点
            // 令左子节点的值为洞值，再令洞号下移至左子节点处
            *(first + holeIndex) = *(first + (rchild - 1));
            holeIndex = rchild - 1;
        }

        // 再执行一次上溯过程
        MySTL::push_heap_aux(first, holeIndex, topIndex, value);
    }


    template <class RandomIter, class T, class Distance>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*)
    {
        *result = *first;   // 设定尾值为首值
        MySTL::adjust_heap(first, static_cast<Distance>(0), last - first, value);
    }

    template <class RandomIter>
    void pop_heap(RandomIter first, RandomIter last)
    {
        MySTL::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
    }


    template <class RandomIter, class T, class Distance, class Compare>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compare comp)
    {
        Distance topIndex = holeIndex;
        Distance rchild = 2 * holeIndex + 2; // 洞节点的右子节点
        while(rchild < len) {
            if(comp(*(first + rchild), *(first + rchild - 1))) {
                // 比较洞节点的左右两个子节点的值，找出较大的那个，以 rchild 代表较大的子节点
                rchild--;
            }
            
            // 下沉，令较大子节点为洞值，再令洞号下移至较大子节点处
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = 2 * (rchild + 1);
        }

        if(rchild == len)
        {
            // 没有右子节点，只有左子节点
            // 令左子节点的值为洞值，再令洞号下移至左子节点处
            *(first + holeIndex) = *(first + (rchild - 1));
            holeIndex = rchild - 1;
        }

        // 再执行一次上溯过程
        MySTL::push_heap_aux(first, holeIndex, topIndex, value, comp);
    }


    template <class RandomIter, class T, class Distance, class Compare>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*, Compare comp)
    {
        *result = *first;   // 设定尾值为首值
        MySTL::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
    }

    template <class RandomIter, class Compare>
    void pop_heap(RandomIter first, RandomIter last, Compare comp)
    {
        MySTL::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first), comp);
    }

    /*****************************************************************************************/
    // sort_heap
    // 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
    /*****************************************************************************************/
    template <class RandomIter>
    void sort_heap(RandomIter first, RandomIter last)
    {
        while(last - first > 1) {
            MySTL::pop_heap(first, last--);
        }
    }

    template <class RandomIter, class Compare>
    void sort_heap(RandomIter first, RandomIter last, Compare comp)
    {
        while(last - first > 1) {
            MySTL::pop_heap(first, last--, comp);
        }
    }

    /*****************************************************************************************/
    // make_heap
    // 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
    /*****************************************************************************************/
    template <class RandomIter, class Distance>
    void make_heap_aux(RandomIter first, RandomIter last, Distance*)
    {   
        // 如果长度为 0 或者 1，不用重排列
        if(last - first < 2) {
            return;
        }
        Distance len = last - first;
        auto holeIndex = (len - 2) / 2;
        while(true) {
            MySTL::adjust_heap(first, holeIndex, len, *(first + holeIndex));
            if(holeIndex == 0) {
                return;
            }
            holeIndex--;
        }
    }
    
    template <class RandomIter>
    void make_heap(RandomIter first, RandomIter last)
    {
        MySTL::make_heap_aux(first, last, distance_type(first));
    }

    template <class RandomIter, class Distance, class Compare>
    void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compare comp)
    {   
        // 如果长度为 0 或者 1，不用重排列
        if(last - first < 2) {
            return;
        }
        Distance len = last - first;
        auto holeIndex = (len - 2) / 2;
        while(true) {
            MySTL::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
            if(holeIndex == 0) {
                return;
            }
            holeIndex--;
        }
    }
    
    template <class RandomIter, class Compare>
    void make_heap(RandomIter first, RandomIter last, Compare comp)
    {
        MySTL::make_heap_aux(first, last, distance_type(first));
    }


} // namespace MySTL


#endif
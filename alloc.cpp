#include "alloc.h"
#include <string.h> //memcpy

namespace MySTL {
    char* alloc::start_free = nullptr;
    char* alloc::end_free = nullptr;
    size_t alloc::heap_size = 0;

    alloc::obj* volatile alloc::free_list[alloc::_NFREELIST] = {
        nullptr, nullptr, nullptr, nullptr, 
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
    };

    void* alloc::allocate(size_t bytes) {
        if (bytes > _MAX_BYTES) {
            return mallocAlloc::allocate(bytes);
        }

        size_t index = FREELIST_INDEX(bytes);
        obj* my_free_list = free_list[index];

        if (my_free_list == nullptr) {
            void* r = refill(ROUND_UP(bytes));
            return r;
        }

        free_list[index] = my_free_list->free_list_next;
        return my_free_list;
    }

    void alloc::deallocate(void* ptr, size_t bytes) {
        if (bytes > _MAX_BYTES) {
            mallocAlloc::deallocate(ptr);
            return;
        }

        size_t index = FREELIST_INDEX(bytes);
        obj* node = static_cast<obj*>(ptr);
        node->free_list_next = free_list[index];
        free_list[index] = node;
    }

    //该实现参考 SGI STL 源码
    void* alloc::reallocate(void* ptr, size_t old_sz, size_t new_sz) {
        auto maxBytes = static_cast<size_t>(_MAX_BYTES);
        // 新旧内存空间的大小都大于 128 bytes，则第一级内存空间配置器
        if(old_sz > maxBytes && new_sz > maxBytes) {
            return mallocAlloc::reallocate(ptr, new_sz);
        }
        // 如果新旧值上调到 8 的倍数后，是同一个数，则不需要调整，直接返回原内存空间的指针即可
        if(ROUND_UP(old_sz) == ROUND_UP(new_sz)) {
            return ptr;
        }

        void* result = allocate(new_sz);
        size_t copy_sz = new_sz > old_sz ? old_sz : new_sz;
        ::memcpy(result, ptr, copy_sz);
        deallocate(ptr, old_sz);
        return result;
    }

    //返回一个大小为n的对象，并且有时候会为适当的freelist增加节点
    //假设bytes已经上调为8的倍数
    void *alloc::refill(size_t bytes) {
        //记录获取的区块数量
        size_t nobjs = _NOBJS;
     
        //从内存池中取nobjs个区块作为freelist的新节点
        char *chunk = chunk_alloc(bytes, nobjs);
        obj * volatile *my_list = 0;
        obj *result = 0;
        obj *current_obj = 0, *next_obj = 0;
     
        if (nobjs == 1) {
            //只获取了一个区块
            return chunk;
        }

        my_list = free_list + FREELIST_INDEX(bytes);

        result = (obj *)(chunk);
     
        //剩下的空间分离出来
        *my_list = next_obj = (obj *)(chunk + bytes);

        for (int i = 1;; ++i) {
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj +bytes);
            if (nobjs - 1 == i) {
            current_obj->free_list_next = 0;
            break;
            }

            current_obj->free_list_next = next_obj;
        }

        return result;
    }
  
    //内存池(一大块空闲的空间) bytes已经上调为8的倍数
    char *alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
        char *result = 0;
        size_t bytes_need = bytes * nobjs;
        size_t bytes_left  = end_free - start_free;
     
        if (bytes_left > bytes_need) {
            result = start_free;
            start_free = start_free + bytes_need;
            return result;
        } else if (bytes_left >= bytes){ //只能提供小于nobjs个区间的内存
            nobjs = bytes_left / bytes;
            bytes_need = nobjs * bytes;
            result = start_free;
            start_free = start_free + bytes_need;
            return result;
        } else { //一个区块的无法提供
            //每次申请两倍的新内存+
            size_t bytes_to_get = 2 * bytes_need + ROUND_UP(heap_size >> 4);
            //试着让内存池中的残余零头还有利用价值
            if (bytes_left > 0) {
                obj * volatile * my_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj *)start_free)->free_list_next = *my_list;
                *my_list = (obj *)start_free;
            }

            start_free = (char *)malloc(bytes_to_get);
            if (nullptr == start_free) {//heap内存不足
                obj * volatile * my_list = 0, *p = 0;
                /*
                * 在freelist上寻找未使用的大区块。
                */

                for (int i = 0; i <= _MAX_BYTES; i += _ALIGN) {
                    my_list = free_list + FREELIST_INDEX(i);
                    p = *my_list;
                    if (nullptr != p) {
                    *my_list = p->free_list_next;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return chunk_alloc(bytes, nobjs);
                    }
                }
                end_free = nullptr; //没有内存可用
                start_free = static_cast<char*>(mallocAlloc::allocate(bytes_to_get));
            }

            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(bytes, nobjs);
        }
    }
}
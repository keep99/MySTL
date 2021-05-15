/*
 * @Description: 
 * @Author: Chen.Yu
 * @Date: 2021-04-09 21:11:59
 * @LastEditTime: 2021-04-09 21:12:54
 * @LastEditors: Chen.Yu
 */
#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <cstdlib> // size_t
#include <functional> //std::function

namespace MySTL {
    //第一级空间配置器
    class mallocAlloc {
    public:
        //内存不足处理例程，客户端自定义
        using OutOfMemoryHander = std::function<void()>;

        static void* allocate(size_t bytes) {
            void* result = ::malloc(bytes);
            if (result == nullptr) {
                result = outOfMemoryInMalloc(bytes);
            }

            return result;
        }

        static void deallocate(void* ptr) {
            ::free(ptr);
        }

        static void* reallocate(void* ptr, size_t bytes) {
            void* result = ::realloc(ptr, bytes);
            if (result == nullptr) {
                result = outOfMemoryInRealloc(ptr, bytes);
            }
            
            return result;
        }

    private:
        //分配内存失败时的自定义处理函数
        static OutOfMemoryHander handler;    

        static void* outOfMemoryInMalloc(size_t bytes) {
            while (true) {
                if(handler == nullptr) {
                    throw std::bad_alloc();
                }
                handler();
                void* result = ::malloc(bytes);
                if (result != nullptr) {
                    return result;
                }                
            }
        }

        static void* outOfMemoryInRealloc(void *ptr, size_t bytes) {
            while (true) {
                if (handler == nullptr) {
                    throw std::bad_alloc();
                }

                handler();
                void* result = ::realloc(ptr, bytes);
                if (result != nullptr) {
                    return result;
                }
            }
        }
    };

    //第二级空间配置器
    class alloc {
    private:
        enum {_ALIGN = 8};
        enum {_MAX_BYTES = 128};
        enum {_NFREELIST = _MAX_BYTES / _ALIGN};
        enum {_NOBJS = 20};

        union obj
        {
            union obj* free_list_next;
            char client_data[1];
        };

        static obj* volatile free_list[_NFREELIST];

        static char* start_free;
        static char* end_free;
        static size_t heap_size;

        static size_t FREELIST_INDEX(size_t bytes) {
            return ((bytes + _ALIGN - 1) / _ALIGN - 1);
        }

        static size_t ROUND_UP(size_t bytes) {
            return ((bytes+ _ALIGN - 1) & ~(_ALIGN - 1));
        }

        static void* refill(size_t bytes);

        static char* chunk_alloc(size_t bytes, size_t& nobjs);
    
    public:
        static void* allocate(size_t bytes);
        static void deallocate(void* ptr, size_t bytes);
        static void* reallocate(void* ptr, size_t old_sz, size_t new_sz);
    };

} //end of namespace MySTL

#endif
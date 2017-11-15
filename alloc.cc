#include "alloc.h"

namespace mystl {

    //static data member的定义和初始值设定
    char *alloc::start_free = 0;
    char *alloc::end_free = 0;
    size_t alloc::heap_size = 0;

    alloc::obj *alloc::free_list[alloc::NFREELISTS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    void *alloc::allocate(size_t n) {
        if (n > MAX_BYTES) { //大于128就调用第一级配置器
            return malloc(n);
        }

        obj **my_free_list, *result;
        my_free_list = free_list + FREELIST_INDEX(n); //寻找16个free-list中适当的一个;
        result = *my_free_list;

        if (result == 0) {
            //没有找到可用的free-list,准备重新填充free-list
            void *r = refill(ROUND_UP(n));
            return r;
        }

        *my_free_list = result->free_list_link; //重新调整free-list
        return result;
    }

    void alloc::deallocate(void *p, size_t n) {
        if (n > MAX_BYTES) { //大于128就调用第一级配置器
            free(p);
            return;
        }
        //寻找对应的free-list
        obj **my_free_list;
        my_free_list = free_list + FREELIST_INDEX(n);
        obj *q = static_cast<obj *>(p);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

    void *alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
        deallocate(p, old_sz);
        p = allocate(new_sz);
        return p;
    }

    //返回一个大小为n的对象，并且有时候会为适当的free-list增加节点
    //假设n已经上调至8的倍数
    void* alloc::refill(size_t n) {
        int nobjs = 20;
        //调用chunk_alloc()，尝试取得nobjs个区块最为free-list的新节点
        //注意nobjs是传引用
        char *chunk = chunk_alloc(n, nobjs); //从内存池取
        obj **my_free_list;
        obj *result;
        obj *current_obj, *next_obj;

        if (nobjs == 1) { //取出的空间只够一个对象使用,free-lista无新节点
            return chunk;
        }
        //否则准备调整新节点
        my_free_list = free_list + FREELIST_INDEX(n);
        result = (obj *)(chunk); //返回给客端
        *my_free_list = next_obj = (obj *)(chunk + n); //导引free-list指向新配置的空间
        for (int i = 1; ; ++i) {
            current_obj = next_obj;
            next_obj = (obj *)(char *)(next_obj + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = 0;
                break;
            } else {
                current_obj->free_list_link = next_obj;
            }
        }
        return result;
    }

    //假设size已经适当上调至8的倍数
    //主要nobjs是传引用
    char *alloc::chunk_alloc(size_t size, int& nobjs) { //内存池
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free; //内存池剩余空间

        if (bytes_left >= total_bytes) {
            //内存池剩余空间完全满足需求量
            result = start_free;
            start_free += total_bytes;
            return result;
        } else if (bytes_left >= size) {
            //剩余空间不完全满足需求量，但足够供应一个以上的区块
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        } else {
            //内存池剩余空间连一个区块的大小都无法满足
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            //以下试着让内存池中的参与碎片还有利用价值
            if (bytes_left > 0) {
                obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj *)(start_free))->free_list_link = *my_free_list;
                *my_free_list = (obj *)(start_free);
            }

            //配置heap空间，用来补充内存池
            start_free = (char *)(malloc(bytes_to_get));
            if (!start_free) {
                //heap空间不足，malloc失败
                obj **my_free_list, *p;
                for (int i = size; i <= MAX_BYTES; i += ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != 0) {
                        //调整free-list以释放未使用区块
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        //递归调动自己，调整nobjs
                        return (chunk_alloc(size, nobjs));
                    }
                }
                end_free = 0; //没有可用内存了
                //调用第一级配置器，利用out-of-memory机制
                start_free = static_cast<char *>(malloc(bytes_to_get));
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            //低估调用自己，调整nobjs
            return chunk_alloc(size, nobjs);
        }
    }

} //namespace mystl

#ifndef MYSTL_ALLOC_H_
#define MYSTL_ALLOC_H_

#include <cstdlib>

namespace mystl {

//具备次配置力（内存池）的空间配置器
class alloc {

private:
    enum { ALIGN = 8 }; //小型区块的上调边界
    enum { MAX_BYTES = 128 }; //小型区块的上限
    enum { NFREELISTS = MAX_BYTES / ALIGN }; //free-lists个数
private:
    static size_t ROUND_UP(size_t bytes) { //将butes上调至8的倍数
        return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
    }
private:
    union obj { //free-list节点
        union obj *free_list_link;
        char client_data[1];
    };
private:
    //16个free-lists
    static obj *free_list[NFREELISTS];
    //根据区块大小决定使用第n个free-list, n从0开始计算
    static size_t FREELIST_INDEX(size_t bytes) {
        return ((bytes + ALIGN - 1) / ALIGN - 1);
    }
    //返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
    static void *refill(size_t n);
    //配置一大块空间，可容纳nobjs个大小为size的区块
    //如果配置nobjs个区块有所不便，nobjs可能会降低
    static char *chunk_alloc(size_t size, int &nobjs);
private:
    static char *start_free; //内存池起始位置，只在chunk_alloc变化
    static char *end_free; //内存池结束位置，只在chunk_alloc变化
    static size_t heap_size;

public:
    static void *allocate(size_t bytes);
    static void deallocate(void *p, size_t n);
    static void *reallocate(void *p, size_t old_sz, size_t new_sz);
};

}//namespace

#endif

#ifndef MYSTL_MEMORY_H_
#define MYSTL_MEMORY_H_

#include <utility>

namespace mystl {

// 默认删除器
template<typename T>
struct default_delete {
    void operator()(T* ptr) {
        if (ptr) delete ptr;
    }
}; // struct default_delete

template <typename T>
struct default_delete<T[]> {
    void operator()(T* ptr) {
        if (ptr) delete[] ptr;
    }
}; // struct default_delete

// unique_ptr
template<typename T, typename D = default_delete<T>>
class unique_ptr {
public:
    typedef T                element_type;
    typedef D                deleter_type;
    typedef element_type*    pointer;
public:
    // 构造析构相关
    explicit unique_ptr(T *data = nullptr) : data_(data) {}
    unique_ptr(T* data, deleter_type del) : data_(data), deleter(del) {}
    // TODO
private:
    inline void clear() {
        deleter(data_);
        data_ nullptr;
    }
private:
    element_type* data_; // 实际指针
    deleter_type deleter; // 删除器
}; // class unique_ptr

} // namespace

#endif

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

    unique_ptr(unique_ptr&& up) : data_ (nullptr) {
        std::swap(data_, up.data_);
    }
    unique_ptr& operator=(unique_ptr&& up) {
        if (&up != this) {
            clear();
            std::swap(*this, up);
        }
        return *this;
    }

    unique_ptr(const unique_ptr&) = delete; // 不允许复制
    unique_ptr& operator=(const unique_ptr&) = delete;

    ~unique_ptr() { claer(); }
public:
    const pointer get() const { return data_; }
    pointer get() { return data_; }
    deleter_type& get_deleter() { return deleter; }
    const deleter_type& get_deleter() const { return deleter; }

    operator bool() const { return get() != nullptr; }

    pointer release() {
        T *p = nullptr;
        std::swap(p, data_);
        return p;
    }
    void reset(pointer p = pointer()) {
        clear();
        data_ = p;
    }
    void swap(unique_ptr& up) { std::swap(data_, up.data_); }

    const element_type& operator*() const { return *data_; }
    const pointer oerator->() const { return data_; }
    element_type& operator*() { return *data_; }
    pointer operator->() { return data_; }
private:
    inline void clear() {
        deleter(data_);
        data_ = nullptr;
    }
private:
    element_type* data_; // 实际指针
    deleter_type deleter; // 删除器
}; // class unique_ptr

} // namespace

#endif

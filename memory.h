#ifndef MYSTL_MEMORY_H_
#define MYSTL_MEMORY_H_

#include "ref.h"

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

    ~unique_ptr() { clear(); }
public:
    const pointer get() const { return data_; }
    pointer get() { return data_; }
    deleter_type& get_deleter() { return deleter; }
    const deleter_type& get_deleter() const { return deleter; }

    // 重载bool运算符
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
    const pointer operator->() const { return data_; }
    element_type& operator*() { return *data_; }
    pointer operator->() { return data_; }
private:
    jikinline void clear() {
        deleter(data_);
        data_ = nullptr;
    }
private:
    element_type* data_; // 实际指针
    deleter_type deleter; // 删除器
}; // class unique_ptr

template<typename T, typename D>
void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) {
    x.swap(y);
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator==(const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs) {
    return lhs.get() == rhs.get();
}

template<typename T, typename D>
bool operator==(const unique_ptr<T, D>& up, std::nullptr_t p) {
    return up.get() == p;
}

template <typename T, typename D>
bool operator==(std::nullptr_t p, const unique_ptr<T, D>& up){
    return up.get() == p;
}

template <typename T1, typename D1, typename T2, typename D2>
bool operator != (const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs){
    return !(lhs == rhs);
}

template <typename T, typename D>
bool operator != (const unique_ptr<T, D>& up, std::nullptr_t p){
    return up.get() != p;
}

template <typename T, typename D>
bool operator != (std::nullptr_t p, const unique_ptr<T, D>& up){
    return up.get() != p;
}

template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// shared_ptr
template<typename T>
class shared_ptr {
public:
    typedef T element_type;
private:
    template<typename Type>
    using ref_t = mystl::ref_t<Type>;
public:
    explicit shared_ptr(T* p = nullptr) : ref_(new ref_t<T>(p)) {}
    template<typename D>
    shared_ptr(T* p, D del) : ref_(new ref_t<T>(p, del)) {}

    shared_ptr(const shared_ptr& sp) {
        copy_ref(sp.ref_);
    }

    shared_ptr& operator=(const shared_ptr& sp) {
        if (this != &sp) {
            decrease_ref();
            copy_ref(sp.ref_);
        }
        return *this;
    }

    ~shared_ptr() { decrease_ref(); }

    const element_type& operator*() const { return *(get()); }
    const element_type* operator->() const { return get(); }
    element_type& operator*() { return *(get()); }
    element_type* operator->() { return get(); }

    const element_type* get() const { return ref_->get_data(); }
    element_type* get() { return ref_->get_data(); }
    size_t use_count() const { return ref_->count(); }

    // 重载bool运算符
    operator bool() const { return get() != nullptr; }
private:
    void decrease_ref() { 
        if (ref_->get_data()) {
            --(*ref_);
            if (use_count() == 0)
            delete ref_;
        }
    }
    void copy_ref(ref_t<T>* r) {
        ref_ = r;
        ++(*ref_);
    }
private:
    ref_t<T>* ref_;
}; // class shared_ptr

template<typename T1, typename T2>
bool operator==(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs) {
	return lhs.get() == rhs.get();
}
template<typename T>
bool operator==(const shared_ptr<T>& sp, std::nullptr_t p) {
	return sp.get() == p;
}
template<typename T>
bool operator==(std::nullptr_t p, const shared_ptr<T>& sp){
	return sp == p;
}
template<typename T1, typename T2>
bool operator!=(const shared_ptr<T1>& lhs, const shared_ptr<T2>& rhs){
	return !(lhs == rhs);
}
template<typename T>
bool operator!=(const shared_ptr<T>& sp, std::nullptr_t p){
	return !(sp == p);
}
template<typename T>
bool operator!=(std::nullptr_t p, const shared_ptr<T>& sp){
	return !(sp == p);
}

template<typename T, typename...Args>
shared_ptr<T> make_shared(Args... args){
	return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace mystl

#endif
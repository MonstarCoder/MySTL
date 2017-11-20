#ifndef MYSTL_VTCTOR_H
#define MYSTL_VTCTOR_H

#include "allocator.h"
#include "typetraits.h"
#include "iterator.h"

#include <algorithm> //for fill_n
#include <memory> //for uninitialized_copy unititialized_fill
#include <cstddef> //for ptrdiff_t
#include <initializer_list> //for initializer_list
#include <iostream> //for ostream
#include <utility> //for swap

namespace mystl {

template<typename T, typename Alloc = allocator<T>>
class vector {

public:
    //vector的嵌套类型定义
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;
    typedef value_type&         reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

protected:
    typedef Alloc data_allocator; //空间配置器

    iterator start_; //表示目前使用空间的头
    iterator finish_; //表示目前使用空间的尾（后）
    iterator end_of_storage_; // 表示目前可用空间的尾（后）

    void insert_aux(iterator position, const value_type& x);

    void deallocate();

    iterator allocate_and_fill(const size_type n, const value_type& value);

    iterator allocate_and_copy(iterator first, iterator last);

    void fill_initialize(size_type n, const value_type& value);

public:
    //构造，析构，复制，移动相关函数
    vector() : start_(0), finish_(0), end_of_storage_(0) {}
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    vector(int n, const value_type& value) { fill_initialize(n, value); }
    vector(long n, const value_type& value) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); }
    vector(iterator first, iterator last);
    vector(std::initializer_list<value_type> values) //列表初始化
        :vector(values.begin(), values.end()) {}
    vector(const vector& vec);
    vector(vector&& vec);
    vector& operator=(vector vec); //swap实现，此赋值运算符既是拷贝也是移动赋值运算符
    //vector& operator=(const vector& vec); //拷贝
    //vector& operator=(vector&& vec) noexcept; //移动
    ~vector() {
        destroy(start_, finish_); //全局函数
        deallocate(); //成员函数
    }

    //比较相关操作
    bool operator==(const vector& vec) const;
    bool operator!=(const vector& vec) const;

    //迭代器和容量相关
    iterator begin() { return start_; }
    const_iterator cbegin() const { return start_; }
    iterator end() { return finish_; }
    const_iterator cend() const { return finish_; }
    iterator size() const { return static_cast<size_type>(end() - begin()); }
    size_type capacity() const {
        return static_cast<size_type>(end_of_storage_ - begin());
    }
    bool empty() const { return begin() == end(); }
    void resize(size_type n);
    void reserve(size_type n);
    void shrink_to_fit(); //调整容器容量大小为size

    //访问元素相关
    reference operator[](size_type n) { return *(begin() + n); }
    reference at(size_type n) const;
    reference front() { return *(begin()); }
    reference back() { return *(end() - 1); }
    pointer data() { return start_; }

    //操作容器相关
    void clear();
    void swap(vector& vec);
    void push_back(const value_type& value);
    void emplace_back(const value_type& value);
    void pop_back();
    iterator emplace(iterator position, const value_type& value);
    iterator insert(iterator position, const value_type& val);
    iterator insert(iterator position, const size_type& n, const value_type& value);
    iterator insert(iterator position, iterator first, iterator last);
    iterator insert(iterator position, std::initializer_list<value_type> values);
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    void assign(iterator first, iterator last);
    void assign(std::initializer_list<value_type> value);
    void assign(size_type n, value_type& value);

public:
    //重载输出运算符
    friend std::ostream& operator<<(std::ostream &os, const vector& vec) {
        for (iterator first = vec.begin(); first != vec.end(); ++first) {
            os << *first << " ";
        }
        return os;
    }
};

template<typename T, typename Alloc>
std::ostream& operator<<(std::ostream &os, const vector<T, Alloc>& vec); //友元声明

//**********构造，析构，拷贝，移动相关函数**********
template<typename T, typename Alloc>
void vector<T, Alloc>::deallocate() {
    if (start_)
        data_allocator::deallocate(start_, end_of_storage_ - start_);
}

template<typename T, typename Alloc>
void vector<T, Alloc>::fill_initialize(size_type n, const value_type& value) {
    start_ = allocate_and_fill(n, value);
    finish_ = start_ + n;
    end_of_storage_ = finish_;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::allocate_and_fill(const size_type n, const value_type& value) {
    iterator result = data_allocator::allocate(n);
    std::uninitialized_fill_n(result, n, value);
    return result;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::allocate_and_copy(iterator first, iterator last) {
    iterator result = data_allocator::allocate(first - last);
    std::uninitialized_copy(first, last, result);
    return result;
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(iterator first, iterator last) {
    if (first == last) return;
    auto n = std::distance(first, last);
    start_ = allocate_and_copy(n, first, last);
    finish_ = start_ + n;
    end_of_storage_ = finish_;
}

template<typename T, typename Alloc> //拷贝构造函数
vector<T, Alloc>::vector(const vector& vec) {
    start_ = allocate_and_copy(vec.begin(), vec.end());
    finish_ = start_ + std::distance(vec.begin(), vec.end());
    end_of_storage_ = finish_;
    //swap(vec);
}

template<typename T, typename Alloc> //移动构造函数
vector<T, Alloc>::vector(vector&& vec) {
    //start_ = vec.begin();
    //finish_ = vec.end();
    //end_of_storage_ = start_ + vec.capacity();
    //vec.begin() = vec.end() = vec.capacity()= 0;
    //
    //start_ = vec.start_;
    //finish_ = vec.finish_;
    //end_of_storage_ = vec.end_of_storage_;
    //vec.start_ = vec.finish_ = vec.end_of_storage_ = 0;
    swap(vec); //使用swap实现，简单安全
}

/*
template<typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& vec) {
    auto copy = vec;
    swap(copy); //同样使用swap
    return *this;
}

template<typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& vec) noexcept {
    if (this != &vec) {
        swap(vec);
       // vec.start_ = vec.finish_ = vec.end_of_storage_ = 0;
    }
    return *this;
}
*/

template<typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector vec) {
    swap(*this, vec);
    return *this;
}

//**********比较相关操作**********





//**********迭代器和容量相关**********

//**********访问元素相关**********

//**********操作容器相关**********
template<typename T, typename Alloc>
void vector<T, Alloc>::swap(vector& vec) {
    using std::swap;
    swap(start_, vec.start_);
    swap(finish_, vec.finish_);
    swap(end_of_storage_, vec.end_of_storage_);
}

} //namespace mystl

#endif

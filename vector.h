#ifndef MYSTL_VTCTOR_H
#define MYSTL_VTCTOR_H

#include "allocator.h"
#include "typetraits.h"
#include "iterator.h"

#include <iterator> //for std::iterator
#include <algorithm> //for fill_n, max
#include <memory> //for uninitialized_copy unititialized_fill
#include <cstddef> //for ptrdiff_t
#include <initializer_list> //for initializer_list
#include <iostream> //for ostream
#include <utility> //for forward, swap
#include <stdexcept> //for out_of_range

namespace mystl {

template<typename T, typename Alloc = alloc>
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
    typedef allocator<T, Alloc> data_allocator; //空间配置器

    iterator start_; //表示目前使用空间的头
    iterator finish_; //表示目前使用空间的尾（后）
    iterator end_of_storage_; // 表示目前可用空间的尾（后）

    void insert_aux(iterator position, const value_type& x);
    void insert_aux(iterator position, const size_type& n, const value_type& x);

    void deallocate();

    iterator allocate_and_fill(const size_type n, const value_type& value);

    iterator allocate_and_copy(iterator first, iterator last);
    iterator allocate_and_copy(const_iterator first, const_iterator last);

    void fill_initialize(size_type n, const value_type& value);

public:
    //构造，析构，复制，移动相关函数
    vector() : start_(0), finish_(0), end_of_storage_(0) {}
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    vector(int n, const value_type& value) { fill_initialize(n, value); }
    vector(long n, const value_type& value) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); } //防止出现vector<T> vec = n
    vector(iterator first, iterator last);
    vector(std::initializer_list<value_type> values) //列表初始化
        :vector(values.begin(), values.end()) {}
    vector(const vector& vec);
    vector(vector&& vec);
    vector& operator=(vector vec); //swap实现，此赋值运算符既是拷贝也是移动赋值运算符
    //vector& operator=(const vector& vec); //拷贝
    //vector& operator=(vector&& vec) noexcept; //移动
    ~vector() {
        data_allocator::destroy(start_, finish_); //
        deallocate(); //成员函数
    }

    //比较相关操作
    bool operator==(const vector& vec) const;
    bool operator!=(const vector& vec) const;

    //迭代器和容量相关
    iterator begin() { return start_; }
    const_iterator begin() const { return start_; }
    const_iterator cbegin() const { return start_; }
    iterator end() { return finish_; }
    const_iterator end() const { return finish_; }
    const_iterator cend() const { return finish_; }
    size_type size() const { return static_cast<size_type>(end() - begin()); }
    size_type capacity() const {
        return static_cast<size_type>(end_of_storage_ - cbegin());
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
    template<typename... Args>
    void emplace_back(Args&&... args);
    void pop_back();
    template<typename... Args>
    iterator emplace(iterator position, Args&&... args);
    iterator insert(iterator position, const value_type& value);
    iterator insert(iterator position, const size_type& n, const value_type& value);
    iterator insert(iterator position, iterator first, iterator last);
    iterator insert(iterator position, std::initializer_list<value_type> lst);
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    void assign(iterator first, iterator last);
    void assign(std::initializer_list<value_type> lst);
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
    iterator result = data_allocator::allocate(last - first);
    std::uninitialized_copy(first, last, result);
    return result;
}
 //重载allocate_and_copy
template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::allocate_and_copy(const_iterator first, const_iterator last) {
    iterator result = data_allocator::allocate(last - first);
    std::uninitialized_copy(first, last, result);
    return result;
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(iterator first, iterator last) {
    if (first == last) return;
    auto n = last - first;
    start_ = allocate_and_copy(first, last);
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
template<typename T, typename Alloc>
bool vector<T, Alloc>::operator==(const vector& vec) const {
    if (size() != vec.size()) {
        return false;
    }
    else {
        auto ptr1 = start_;
        auto ptr2 = vec.start_;
        for (; ptr1 != finish_ && ptr2 != vec.finish_; ++ptr1, ++ptr2) {
            if (*ptr1 != *ptr2)
                return false;
        }
        return true;
    }
}

template<typename T, typename Alloc>
bool vector<T, Alloc>::operator!=(const vector& vec) const {
    return !(*this == vec);
}

//**********迭代器和容量相关**********
template<typename T, typename Alloc>
void vector<T, Alloc>::resize(size_type n) {
    if (n < size()) {
        erase(begin() + n, end());
    } else {
        insert(end(), n - size(), T());
    }
}

template<typename T, typename Alloc>
void vector<T, Alloc>::reserve(size_type n) {
    if (capacity() < n) {
        const size_type old_size = size();
        iterator tmp = allocate_and_copy(start_, finish_);
        data_allocator::destroy(start_, finish_);
        deallocate();
        start_ = tmp;
        finish_ = tmp + old_size;
        end_of_storage_ = start_ + n;
    }
}

template<typename T, typename Alloc>
void vector<T, Alloc>::shrink_to_fit() {
    auto vec = *this;
    swap(vec);
}

//**********访问元素相关**********
template<typename T, typename Alloc>
typename vector<T, Alloc>::reference
vector<T, Alloc>::at(size_type n) const {
    return *(begin() + n);
}

//**********操作容器相关**********
template<typename T, typename Alloc>
void vector<T, Alloc>::swap(vector& vec) {
    using std::swap;
    swap(start_, vec.start_);
    swap(finish_, vec.finish_);
    swap(end_of_storage_, vec.end_of_storage_);
}

template<typename T, typename Alloc>
void vector<T, Alloc>::clear() {
    erase(begin(), end());
}

template<typename T, typename Alloc>
void vector<T, Alloc>::push_back(const value_type& value) {
    if (finish_ != end_of_storage_) {
        construct(finish_, value);
        ++finish_;
    } else {
        insert_aux(end(), value);
    }
}

template<typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const value_type& value) {
    if (finish_ != end_of_storage_) { //还有剩余内存
        construct(finish_, *(finish_ -1));
        ++finish_;
        value_type val_copy = value;
        std::copy_backward(position, finish_ - 2, finish_ -1);
        *position = val_copy;
    } else { //内存不足，重新分配（原来的2倍）
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = std::uninitialized_copy(start_, position, new_start);
            construct(new_finish, value);
            ++new_finish;
            new_finish = std::uninitialized_copy(position, finish_, new_finish);
        } catch(...) {
            data_allocator::destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        data_allocator::destroy(begin(), end());
        deallocate();
        start_ = new_start;
        finish_ = new_finish;
        end_of_storage_ = new_start + len;
    }
}

//重载insert_aux
template<typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const size_type& n, const value_type& value) {
    if ((finish_  + n) <= end_of_storage_) { //还有剩余内存
        construct(finish_, *(finish_ -n));
        finish_ += n;
        value_type val_copy = value;
        std::copy_backward(position, finish_ - 2, finish_ -1);
        for (auto i = 0; i != n; ++i) {
            *position = val_copy;
            ++position;
        }
    } else { //内存不足，重新分配（原来的加上max(old_size, n)）
        const size_type old_size = size();
        const size_type len = old_size != 0 ? std::max(n, old_size) : 1;
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = std::uninitialized_copy(start_, position, new_start);
            for (auto i = 0; i < n; ++i) {
                construct(new_finish, value);
                ++new_finish;
            }
            new_finish = std::uninitialized_copy(position, finish_, new_finish);
        } catch(...) {
            data_allocator::destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }
        data_allocator::destroy(begin(), end());
        deallocate();
        start_ = new_start;
        finish_ = new_finish;
        end_of_storage_ = new_start + len;
    }
}

template<typename T, typename Alloc>
void vector<T, Alloc>::pop_back() {
    --finish_;
    data_allocator::destroy(finish_);
}

template<typename T, typename Alloc>
template<typename... Args>
void vector<T, Alloc>::emplace_back(Args&&... args) { //先构造value_type,然后调用push_back
    push_back(value_type(std::forward<Args>(args)...)); //c++11 forward转发
}

template<typename T, typename Alloc>
template<typename... Args>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::emplace(iterator position, Args&&... args) {
    if (position < cbegin() || position > cend()) {
        throw std::out_of_range("vector::emplace() - parameter \"position\" is out of bound");
    }
    auto n = position - cbegin();
    insert(position, value_type(std::forward<Args>(args)...));
    return cbegin() + n;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator position, const value_type& value) {
    auto n = position -begin();
    insert_aux(position, value);
    return begin() + n;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator position, const size_type& n, const value_type& value) {
    if (n == 0) return 0;
    auto tmp = position - begin();
    insert_aux(position, n, value);
    return begin() + tmp;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator position, iterator first, iterator last) {
    if (first == last) return 0;
    auto n = position - begin();
    for (auto iter = first; iter != last; ++iter) {
        insert(position, *iter);
    }
    return begin() + n;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator position, std::initializer_list<value_type> lst) {
    return insert(position, lst.begin(), lst.end());
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::erase(iterator position) {
    if (position + 1 != cend()) {
        std::copy(position + 1, finish_, position);
    }
    --finish_;
    data_allocator::destroy(finish_);
    return position;
}

template<typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::erase(iterator first, iterator last) {
    auto i = std::copy(last, finish_, first);
    data_allocator::destroy(i, finish_);
    finish_ = finish_ - (last - first);
    return first;
}

template<typename T, typename Alloc>
void vector<T, Alloc>::assign(iterator first, iterator last) {
    clear();
    insert(start_, first, last);
}

template<typename T, typename Alloc>
void vector<T, Alloc>::assign(std::initializer_list<value_type> lst) {
    assign(lst.begin(), lst.end());
}

template<typename T, typename Alloc>
void vector<T, Alloc>::assign(size_type n, value_type& value) {
    clear();
    insert(start_, n, value);
}

} //namespace mystl

#endif

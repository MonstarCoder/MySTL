#ifndef MYSTL_STRING_H_
#define MYSTL_STRING_H_

#include "allocator.h"
#include "construct.h"
#include "pair.h"
#include "iterator.h"

#include <cstddef>
#include <cstring>
#include <utility> // for std::swap
#include <memory> // for copy
#include <type_traits> // for is_integral
#include <iostream>

namespace mystl {

class string {
public:
    typedef char            value_type;
    typedef char*           iterator;
    typedef const char*     const_iterator;
    typedef char&           reference;
    typedef const char&     const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    static const size_t     s_max = -1; // size_t的最大可能值
protected:
    char* start_;
    char* finish_;
    char* end_of_storage_;

    typedef mystl::allocator<char, alloc>      data_allocator;
public:
    // 构造，复制，析构相关
    string() : start_(0), finish_(0), end_of_storage_(0) {}
    string(const string& str);
    string(string&& str);
    string(const string& str, size_type pos, size_type n = s_max);
    string(const char* s);
    string(const char* s, size_type n);
    string(size_type n, char c);
    template<typename InputIterator>
    string(InputIterator first, InputIterator last);

    string& operator=(const string& str);
    string& operator=(string&& str);
    string& operator=(const char* s);
    string& operator=(char c);

    ~string();

    // 访问，操作元素相关
    iterator begin() { return start_; }
    const_iterator begin() const { return start_; }
    const_iterator cbegin() const { return start_; }
    iterator end() { return finish_; }
    const_iterator end() const { return finish_; }
    const_iterator cend() const { return finish_; }

    size_type size() const { return finish_ - start_; }
    size_type length() const { return finish_ - start_; }
    size_type capacity() const { return end_of_storage_ - start_; }
    bool empty() const { return start_ == finish_; }

    void resize(size_type n);
    void resize(size_type n, char c);
    void reserve(size_type n = 0);
    void shrink_to_fit() {
        data_allocator::deallocate(finish_, end_of_storage_ - finish_);
        end_of_storage_ = finish_;
    }
    void clear() {
        destroy(start_, finish_);
        start_ = finish_;
    }

    char& operator[] (size_type pos) { return *(start_ + pos); }
    const char& operator[] (size_type pos) const { return *(start_ - pos); }
    char& back() { return *(finish_ -1); }
    const char& back() const { return *(finish_ -1); }
    char& front() { return *(start_); }
    const char& front() const { return *(start_); }

    string& insert(size_type pos, const char* s);
    string& insert(size_type pos, const char* s, size_type n);
    string& insert(size_type pos, size_type n, char c);
    string& insert(size_type pos, const string& str);
    string& insert(size_type pos, const string& str, size_type subpos,
                   size_type sublen = s_max);
    iterator insert(iterator p, size_type n, char c);
    iterator insert(iterator p, char c);
    template<typename InputIterator>
    iterator insert(iterator p, InputIterator first, InputIterator last);

    string& append(const char* s);
    string& append(const char* s, size_type n);
    string& append(const string& str);
    string& append(const string& str, size_type subpos, size_type sublen = s_max);
    string& append(size_type n, char c);
    template<typename InputIterator>
    string& append(InputIterator first, InputIterator last);

    string& operator+= (const string& str);
    string& operator+= ( const char* s );
    string& operator+= (char c);

    void push_back(char c) { insert(finish_, c); }

    string& erase(size_type pos = 0, size_type len = s_max);
    iterator erase(iterator p);
    iterator erase(iterator first, iterator last);

    void pop_back() { erase(finish_ - 1, finish_); }

    string& replace(size_type pos, size_type len, const string& str);
    string& replace(iterator it1, iterator it2, const string& str);
    string& replace(size_type pos, size_type len, const string& str, size_type subpos,
                    size_type sublen = s_max);
    string& replace(size_type pos, size_type len, const char* s);
    string& replace(iterator it1, iterator it2, const char* s);
    string& replace(size_type pos, size_type len, const char* s, size_type n);
    string& replace(iterator it1, iterator it2, const char* s, size_type n);
    string& replace(size_type pos, size_type len, size_type n, char c);
    string& replace(iterator it1, iterator it2, size_type n, char c);
    template<typename InputIterator>
    string& replace(iterator it1, iterator it2, InputIterator first, InputIterator last);

    string& assign(const string& str);
    string& assign(const string& str, size_type subpos, size_type sublen = s_max);
    string& assign(const char* s);
    string& assign(const char* s, size_type n);
    string& assign(size_type n, char c);
    template<typename InputIterator>
    string& assign(InputIterator first, InputIterator last);

    void swap(string& str) {
        std::swap(start_, str.start_);
        std::swap(finish_, str.finish_);
        std::swap(end_of_storage_, str.end_of_storage_);
    }

    size_type copy(char* s, size_type len, size_type pos = 0) {
        auto ptr = std::uninitialized_copy(start_ + pos, start_ + pos + len, s);
        return static_cast<size_type>(ptr - s);
    }

    string substr(size_type pos = 0, size_type len = s_max) const {
        len = equal_smax(len, size(), pos);
        return string(begin() + pos, begin() + pos + len);
    }

    int compare(const string& str) const;
    int compare(size_type pos, size_type len, const string& str) const;
    int compare(size_type pos, size_type len, const string& str,
                size_type subpos, size_type sublen = s_max) const;
    int compare(const char* s) const;
    int compare(size_type pos, size_type len, const char* s) const;
    int compare(size_type pos, size_type len, const char* s, size_type n) const;

    size_type find(const string& str, size_type pos = 0) const;
    size_type find(const char* s, size_type pos = 0) const;
    size_type find(const char* s, size_type pos, size_type n) const;
    size_type find(char c, size_type pos = 0) const;
    size_type find_first_of(const string& str, size_type pos = 0) const;
    size_type find_first_of(const char* s, size_type pos = 0) const;
    size_type find_first_of(const char* s, size_type pos, size_type n) const;
    size_type find_first_of(char c, size_type pos = 0) const;
    size_type find_last_of(const string& str, size_type pos = s_max) const;
    size_type find_last_of(const char* s, size_type pos = s_max) const;
    size_type find_last_of(const char* s, size_type pos, size_type n) const;
    size_type find_last_of(char c, size_type pos = s_max) const;
    size_type find_first_not_of(const string& str, size_type pos = 0) const;
    size_type find_first_not_of(const char* s, size_type pos = 0) const;
    size_type find_first_not_of(const char* s, size_type pos, size_type n) const;
    size_type find_first_not_of(char c, size_type pos = 0) const;
    size_type find_last_not_of(const string& str, size_type pos = s_max) const;
    size_type find_last_not_of(const char* s, size_type pos = s_max) const;
    size_type find_last_not_of(const char* s, size_type pos, size_type n) const;
    size_type find_last_not_of(char c, size_type pos = s_max) const;
private:
    void string_aux(size_type n, char c, std::true_type);
    template<typename InputIterator>
    void string_aux(InputIterator first, InputIterator last, std::false_type);
    void deallocate() {
        if (start_)
            data_allocator::deallocate(start_, end_of_storage_ - start_);
    }
    void allocate_and_fill(size_type n, value_type c);
    template<typename InputIterator>
    void allocate_and_copy(InputIterator first, InputIterator last);
    // 插入剩余空间不足时的辅助函数
    template<typename InputIterator>
    iterator insert_copy_aux(iterator p, InputIterator first, InputIterator last);
    iterator insert_fill_aux(iterator p, size_type n, value_type c);
    // 如果n等于s_max,则改变其值
    size_type equal_smax(size_type n, size_type end, size_type start) const;
    int compare_aux(size_type pos, size_type len, const_iterator cit,
                    size_type subpos, size_type sublen) const;
    size_type find_aux(const_iterator cit, size_type pos,
                       size_type len, size_type cond) const;
public:
    friend std::ostream& operator<<(std::ostream& os, const string& str);
    friend std::istream& operator>>(std::istream& is, string& str);
    friend string operator+(const string& lhs, const string& rhs);
    friend string operator+(const string& lhs, const char* rhs);
    friend string operator+(const char* lhs, const string& rhs);
    friend string operator+(const string& lhs, char rhs);
    friend string operator+(char lhs, const string& rhs);
    friend bool operator==(const string& lhs, const string& rhs);
    friend bool operator==(const string& lhs, const char* rhs);
    friend bool operator==(const char* lhs, const string& rhs);
    friend bool operator!=(const string& lhs, const string& rhs);
    friend bool operator!=(const string& lhs, const char* rhs);
    friend bool operator!=(const char* lhs, const string& rhs);
    friend bool operator<(const string& lhs, const string& rhs);
    friend bool operator<(const string& lhs, const char* rhs);
    friend bool operator<(const char* lhs, const string& rhs);
    friend bool operator<=(const string& lhs, const string& rhs);
    friend bool operator<=(const string& lhs, const char* rhs);
    friend bool operator<=(const char* lhs, const string& rhs);
    friend bool operator>(const string& lhs, const string& rhs);
    friend bool operator>(const string& lhs, const char* rhs);
    friend bool operator>(const char* lhs, const string& rhs);
    friend bool operator>=(const string& lhs, const string& rhs);
    friend bool operator>=(const string& lhs, const char* rhs);
    friend bool operator>=(const char* lhs, const string& rhs);

    friend void swap(string& x, string& y);
    friend std::istream& getline(std::istream& is, string& str, char delim);
    friend std::istream& getline(std::istream& is, string& str);
}; // class string

template<typename InputIterator>
string::string(InputIterator first, InputIterator last) {
    // 处理指针和数字间区别的函数
    string_aux(first, last, typename std::is_integral<InputIterator>::type());
}

template<typename InputIterator>
typename string::iterator
string::insert_copy_aux(iterator p, InputIterator first, InputIterator last) {
    size_type len_insert = last - first;
    size_type old_capacity = capacity();
    auto res = std::max(old_capacity, len_insert);
    // 新的容量分配
    auto new_capacity = old_capacity + res;
    iterator new_start_ = data_allocator::allocate(new_capacity);
    iterator new_finish_ = std::uninitialized_copy(start_, p, new_start_);
    new_finish_ = std::uninitialized_copy(first, last, new_finish_);
    auto tmp = new_finish_;
    new_finish_ = std::uninitialized_copy(p, finish_, new_finish_);

    destroy(start_, finish_);
    deallocate();
    start_ = new_start_;
    finish_ = new_finish_;
    end_of_storage_ = start_ + new_capacity;
    return tmp;
}

template<typename InputIterator>
typename string::iterator
string::insert(iterator p, InputIterator first, InputIterator last) {
    if (first == last) return 0;
    size_type len_left = capacity() - size();
    size_type len_insert = std::distance(first, last);
    if (len_left >= len_insert) {
        for (auto it = finish_ - 1; it >= p; --it) {
            *(it + len_insert) = *(it);
        }
        std::uninitialized_copy(first, last, p);
        finish_ += len_insert;
        return (p + len_insert);
    } else {
        return insert_copy_aux(p, first, last);
    }
}

template<typename InputIterator>
string& string::append(InputIterator first, InputIterator last) {
    insert(end(), first, last);
    return *this;
}

template<typename InputIterator>
string&
string::replace(iterator it1, iterator it2, InputIterator first, InputIterator last) {
    auto ptr = erase(it1, it2);
    insert(ptr, first, last);
    return *this;
}

template<typename InputIterator>
string& string::assign(InputIterator first, InputIterator last) {
    erase(start_, finish_);
    insert(start_, first, last);
    return *this;
}

template<typename InputIterator>
void string::allocate_and_copy(InputIterator first, InputIterator last) {
    start_ = data_allocator::allocate(last - first);
    finish_ = std::uninitialized_copy(first, last, start_);
    end_of_storage_ = finish_;
}

template<typename InputIterator>
void string::string_aux(InputIterator first, InputIterator last, std::false_type) {
    allocate_and_copy(first, last);
}

} // namespace mystl

#endif

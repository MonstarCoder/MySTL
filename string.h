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
#include <iostream>

namespace mystl {

class string {
public:
    typedef char                        value_type;
    typedef char*                       iterator;
    typedef const char*                 const_iterator;
    typedef char&                       reference;
    typedef const char&                 const_reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;
    static const size_t s_max = -1; // size_t的最大可能值
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
    string(const char*);
    string(const char* s, size_type n);
    string(size_type n, char c);
    template<typename InputIterator>
    string(InputIterator first, InputIterator last);

    string& operator=(const string& str);
    string& operator=(string& str);
    string& operator=(const char* s);
    string& operator=(char* s);

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
        // TODO
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
    // TODO

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


} // namespace mystl

#endif

#ifndef MYSTL_STRING_H_
#define MYSTL_STRING_H_

#include "allocator.h"
#include "construct.h"
#include "pair.h"
#include "iterator.h"

#include <cstddef>
#include <cstring>

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
protected:
    char* start_;
    char* finish_;
    char* end_of_storage_;

    typedef mystl::allocator<char, Alloc>      data_allocator;
public:
    // 构造，复制，析构相关
    string() : start_ = 0, finish_ = 0, end_of_storage_ = 0 {}
    string(const string& str);
    string(string&& str);
    string(const string& str, size_type pos, size_type n);
    string(const char*);
    string(const char* s, size_type n);
    string(size_type n, char c);
    template<typename InputIterator>
    string(InputIterator first, InputIterator last);

    string& operator=(const string& str);
    string& operator=(string& str);
    string& operator=(const char* c);
    string& operator=(char* c);

    ~string();

    // 访问，操作元素相关




}; // class string




} // namespace mystl

#endif

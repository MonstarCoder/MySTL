#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include "alloc.h"
#include "construct.h"

#include <cstddef> //for size_t, ptrdiff_t

namespace mystl {

template<typename T, typename Alloc>
class allocator {
public:
    typedef T            value_type;
    typedef T*           pointer;
    typedef const T*     const_pointer;
    typedef T&           reference;
    typedef const T&     const_reference;
    typedef size_t       size_type;
    typedef ptrdiff_t    difference_type;
public:
    static T *allocate();
    static T *allocate(size_t n);
    static void deallocate(T *p);
    static void deallocate(T *p, size_t n);

    //static void construct(T *p);
    //static void construct(T *p, const T& value);
    //static void destroy(T *p);
    //static void destroy(T *first, T *last);
};

template<typename T, typename Alloc>
T *allocator<T, Alloc>::allocate() {
    return static_cast<T *>(Alloc::allocate(sizeof(T)));
}

template<typename T, typename Alloc>
T *allocator<T, Alloc>::allocate(size_t n) {
    if (n == 0) return 0;
    return static_cast<T *>(Alloc::allocate(sizeof(T) * n));
}

template<typename T, typename Alloc>
void allocator<T, Alloc>::deallocate(T *p) {
    Alloc::deallocate(static_cast<void *>(p), sizeof(T));
}

template<typename T, typename Alloc>
void allocator<T, Alloc>::deallocate(T *p, size_t n) {
    if (n != 0)
        Alloc::deallocate(static_cast<void *>(p), sizeof(T) * n);
}
/*
template<typename T, typename Alloc>
void allocator<T, Alloc>::construct(T* p) {
    new(p) T();
}

template<typename T, typename Alloc>
void allocator<T, Alloc>::construct(T *p, const T& value) {
   // construct(p, value);
    new(p) T(value);
}

template<typename T, typename Alloc>
void allocator<T, Alloc>::destroy(T *p) {
    //mystl::destroy(p);
    p->~T();
}

template<typename T, typename Alloc>
void allocator<T, Alloc>::destroy(T *first, T *last) {
    //mystl::destroy(first, last);
    for (; first != last; ++first) {
        first->~T();
    }
}
*/
} //namespace mystl

#endif

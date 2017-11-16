#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include "alloc.h"
#include "construct.h"

#include <cstddef> //for size_t, ptrdiff_t

namespace mystl {

template<typename T>
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

    static void construct(T *p);
    static void construct(T *p, const T& value);
    static void destroy(T *p);
    static void destroy(T *first, T *last);
};

template<typename T>
T *allocator<T>::allocate() {
    return static_cast<T *>(alloc::allocate(sizeof(T)));
}

template<typename T>
T *allocator<T>::allocate(size_t n) {
    if (n == 0) return 0;
    return static_cast<T *>(alloc::allocate(sizeof(T) * n));
}

template<typename T>
void allocator<T>::deallocate(T *p) {
    alloc::deallocate(static_cast<void *>(p), sizeof(T));
}

template<typename T>
void allocator<T>::deallocate(T *p, size_t n) {
    if (n == 0) return 0;
    alloc::deallocate(static_cast<void *>(p), sizeof(T) * n);
}

template<typename T>
void allocator<T>::construct(T* p) {
    new(p) T();
}

template<typename T>
void allocator<T>::construct(T *p, const T& value) {
   // construct(p, value);
    new(p) T(value);
}

template<typename T>
void allocator<T>::destroy(T *p) {
    destroy(p);
    //p->~T();
}

template<typename T>
void allocator<T>::destroy(T *first, T *last) {
    destroy(first, last);
    //for (; first != last; ++first) {
        //first->T();
    //}
}

} //namespace mystl

#endif

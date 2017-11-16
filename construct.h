#ifndef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

#include <new>
#include "typetraits.h"

namespace mystl {

template<typename T1, typename T2>
inline void construct(T1 *p, const T2& value) {
    new(p) T1(value); //placement new
}

//destroy的第一版本，接受一个指针
template<typename T>
inline void destroy(T *p) {
    p->~T(); //调用~T()
}

//destoy的第二个版本，接受两个迭代器
//利用_type_traits<>求取适当的措施
template<typename ForwardIterator>
inline void _destroy(ForwardIterator first, ForwardIterator last, _true_type) {  }

template<typename ForwardIterator>
inline void _destroy(ForwardIterator first, ForwardIterator last, _false_type) {
    for ( ; first < last; ++first)
        destroy(&*first);
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    typedef typename _type_traits<ForwardIterator>::has_trivial_destructor trivial_destructor;
    _destroy(first, last, trivial_destructor()); //注意，传入的trivial_destructor是一个临时对象
}

} //namespace mystl

#endif

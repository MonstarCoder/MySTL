#ifndef MYSTL_MAP_H_
#define MYSTL_MAP_H_

#include "rbtree.h"
#include "iterator.h"
#include "pair.h"

#include <utility>

namespace mystl {

template<typename Key, typename T,
         typename Compare = std::less<Key>,
         typename Alloc = alloc>
class map {
public:
    typedef Key                   key_type;
    typedef T                     data_type;
    typedef T                     mapped_type;
    typedef pair<const Key, T>    value_type;
    typedef Compare               key_compare;
private:
    typedef rb_tree<key_type, value_type,



}; // class map

} // namespace mystl

#endif

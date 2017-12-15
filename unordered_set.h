#ifndef MYSTL_UNORDERED_SET_H_
#define MYSTL_UNORDERED_SET_H_

#include "hashtable.h"
#include "pair.h"

#include <functional> // for hash, equal_to

namespace mystl {

// 前置声明
template<typename Value, typename HashFcn = std::hash<Value>,
         typename EqualKey = std::equal_to<Value>, typename Alloc = alloc>
class unordered_set;

template<typename Value, typename HashFcn, typename EqualKey, typename Alloc>
bool operator==(const unordered_set<Value, HashFcn, EqualKey, Alloc>&,
                const unordered_set<Value, HashFcn, EqualKey, Alloc>&);

template<typename Value, typename HashFcn, typename EqualKey, typename Alloc>
bool operator<(const unordered_set<Value, HashFcn, EqualKey, Alloc>&,
                const unordered_set<Value, HashFcn, EqualKey, Alloc>&);

// 用于ExtractKey提取key
template<typename T>
struct identity {
    const T& operator() (const T& x) const { return x; }
}; // struct identity

template<typename Value, typename HashFcn, typename EqualKey, typename Alloc>
class unordered_set {
private:
    typedef hashtable<Value, Value, HashFcn, identity<Value>,
                      EqualKey, Alloc> ht;
    ht rep; // 以hashtable为底层容器
public:
typedef typename ht::key_type           key_type;
typedef typename ht::value_type         value_type;
typedef typename ht::hasher             hasher;
typedef typename ht::key_equal          key_equal;

// 注意: reference, pointer, iterator都为const, 因为不能修改hashtable
// 内部的元素, 否则会导致hashtable失效
typedef typename ht::size_type          size_type;
typedef typename ht::difference_type    difference_type;
typedef typename ht::const_pointer      pointer;
typedef typename ht::const_pointer      const_pointer;
typedef typename ht::const_reference    reference;
typedef typename ht::const_reference    const_reference;
typedef typename ht::const_iterator     iterator;
typedef typename ht::const_iterator     const_iterator;

public:
    unordered_set() : rep(100, hasher(), key_equal()) {}
    explicit unordered_set(size_type n) : rep(n, hasher(), key_equal()) {}
    unordered_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    unordered_set(size_type n, const hasher& hf, const key_equal& eql)
        : rep(n, hf, eql) {}

    template<typename InputIterator>
    unordered_set(InputIterator f, InputIterator l)
        : rep(100, hasher(), key_equal()) { rep.insert_unique(f, l); }
    template<typename InputIterator>
    unordered_set(InputIterator f, InputIterator l, size_type n)
        : rep(n, hasher(), key_equal()) { rep.insert_unique(f, l); }
    template<typename InputIterator>
    unordered_set(InputIterator f, InputIterator l, size_type n, const hasher& hf)
        : rep(n, hf, key_equal()) { rep.insert_unique(f, l); }
    template<typename InputIterator>
    unordered_set(InputIterator f, InputIterator l, size_type n,
                  const hasher& hf, const key_equal& eql)
        : rep(n, hf, eql) { rep.insert_unique(f, l); }
    // 返回hash相关函数
    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }
public:
    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(unordered_set& us) { rep.swap(us.rep); }

    friend bool operator== <>(const unordered_set&, const unordered_set&);
    friend bool operator< <>(const unordered_set&, const unordered_set&);
    iterator begin() const { return rep.begin(); }
    iterator end() const { return rep.end(); }

public:
    pair<iterator, bool> insert(const value_type& obj) {
      pair<typename ht::iterator, bool> p = rep.insert_unique(obj);
      return pair<iterator, bool>(p.first, p.second);
    }
template<typename InputIterator>
void insert(InputIterator f, InputIterator l) { rep.insert_unique(f,l); }

    pair<iterator, bool> insert_noresize(const value_type& obj) {
        pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
        return pair<iterator, bool>(p.first, p.second);
    }

    iterator find(const key_type& key) const { return rep.find(key); }

    size_type count(const key_type& key) const { return rep.count(key); }

    pair<iterator, iterator> equal_range(const key_type& key) const {
        return rep.equal_range(key); }

    size_type erase(const key_type& key) {return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear(); }

public:
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const {
        return rep.elems_in_bucket(n); }
}; // class unordered_set

template<typename Value, typename HashFcn, typename EqualKey, typename Alloc>
inline bool operator==(const unordered_set<Value, HashFcn, EqualKey, Alloc>& u_set1,
                       const unordered_set<Value, HashFcn, EqualKey, Alloc>& u_set2) {
    return u_set1.rep == u_set2.rep;
}

template<typename Value, typename HashFcn, typename EqualKey, typename Alloc>
inline bool operator<(const unordered_set<Value, HashFcn, EqualKey, Alloc>& u_set1,
                       const unordered_set<Value, HashFcn, EqualKey, Alloc>& u_set2) {
    return u_set1.rep < u_set2.rep;
}

// swap特例化
template <typename Val, typename HashFcn, typename EqualKey, typename Alloc>
inline void swap(unordered_set<Val, HashFcn, EqualKey, Alloc>& u_set1,
                 unordered_set<Val, HashFcn, EqualKey, Alloc>& u_set2) {
    u_set1.swap(u_set2);
}

} // namespace mystl

#endif

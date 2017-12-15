#ifndef MYSTL_UNORDERED_MAP_H_
#define MYSTL_UNORDERED_MAP_H_

#include "hashtable.h"
#include "pair.h"

#include <functional>

namespace mystl {

// 前置声明
template<typename Key, typename T, typename HashFcn = std::hash<Key>,
         typename EqualKey = std::equal_to<Key>, typename Alloc = alloc>
class unordered_map;

template<typename Key, typename T, typename HashFcn, typename EqualKey, typename Alloc>
bool operator==(const unordered_map<Key, T, HashFcn, EqualKey, Alloc>&,
                const unordered_map<Key, T, HashFcn, EqualKey, Alloc>&);

template<typename Key, typename T, typename HashFcn, typename EqualKey, typename Alloc>
bool operator<(const unordered_map<Key, T, HashFcn, EqualKey, Alloc>&,
                const unordered_map<Key, T, HashFcn, EqualKey, Alloc>&);

// 用于ExtractKey提取key
template<typename T>
struct select1st {
    const typename T::first_type& operator() (const T& x) const { return x.first; }
}; // struct select1st

template<typename Key, typename T, typename HashFcn, typename EqualKey, typename Alloc>
class unordered_map {
private:
    typedef hashtable<pair<const Key, T>, Key, HashFcn,
                      select1st<pair<const Key, T>>, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef typename                        ht::key_type key_type;
    typedef T                               data_type;
    typedef T                               mapped_type;
    typedef typename ht::value_type         value_type;
    typedef typename ht::hasher             hasher;
    typedef typename ht::key_equal          key_equal;

    typedef typename ht::size_type          size_type;
    typedef typename ht::difference_type    difference_type;
    typedef typename ht::pointer            pointer;
    typedef typename ht::const_pointer      const_pointer;
    typedef typename ht::reference          reference;
    typedef typename ht::const_reference    const_reference;

    typedef typename ht::iterator           iterator;
    typedef typename ht::const_iterator     const_iterator;
public:
    unordered_map() : rep(100, hasher(), key_equal()) {}
    explicit unordered_map(size_type n) : rep(n, hasher(), key_equal()) {}
    unordered_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    unordered_map(size_type n, const hasher& hf, const key_equal& eql)
        : rep(n, hf, eql) {}

    template <typename InputIterator>
    unordered_map(InputIterator f, InputIterator l)
        : rep(100, hasher(), key_equal()) { rep.insert_unique(f, l); }
    template <typename InputIterator>
    unordered_map(InputIterator f, InputIterator l, size_type n)
        : rep(n, hasher(), key_equal()) { rep.insert_unique(f, l); }
    template <typename InputIterator>
    unordered_map(InputIterator f, InputIterator l, size_type n,
                  const hasher& hf)
        : rep(n, hf, key_equal()) { rep.insert_unique(f, l); }
    template <typename InputIterator>
    unordered_map(InputIterator f, InputIterator l, size_type n,
                  const hasher& hf, const key_equal& eql)
        : rep(n, hf, eql) { rep.insert_unique(f, l); }

    // 返回hash相关函数
    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(unordered_map& hs) { rep.swap(hs.rep); }
    friend bool operator== <> (const unordered_map&, const unordered_map&);

    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }
    const_iterator begin() const { return rep.begin(); }
    const_iterator end() const { return rep.end(); }

public:
  // 不允许插入key相同的元素
    pair<iterator, bool> insert(const value_type& obj) {
        return rep.insert_unique(obj); }

    template <typename InputIterator>
    void insert(InputIterator f, InputIterator l) { rep.insert_unique(f,l); }

    pair<iterator, bool> insert_noresize(const value_type& obj) {
        return rep.insert_unique_noresize(obj); }

    iterator find(const key_type& key) { return rep.find(key); }
    const_iterator find(const key_type& key) const { return rep.find(key); }

    // 如果key存在则返回对应的元素, 否则新建一个key
    T& operator[](const key_type& key) {
        return rep.find_or_insert(value_type(key, T())).second;
    }

    size_type count(const key_type& key) const { return rep.count(key); }

    pair<iterator, iterator> equal_range(const key_type& key) {
        return rep.equal_range(key); }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
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
}; // class unordered_map

template <typename Key, typename T, typename HashFcn, typename EqualKey, typename Alloc>
inline bool operator==(const unordered_map<Key, T, HashFcn, EqualKey, Alloc>& hm1,
                       const unordered_map<Key, T, HashFcn, EqualKey, Alloc>& hm2) {
return hm1.rep == hm2.rep;
}

} // namespace mystl

#endif

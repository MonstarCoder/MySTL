#ifndef MYSTL_MAP_H_
#define MYSTL_MAP_H_

#include "rbtree.h"
#include "iterator.h"
#include "pair.h"

#include <utility>

namespace mystl {

// 应用模板特定实例的声明
template<typename Key, typename T, typename Compare, typename Alloc>
class map;

template<typename Key, typename T, typename Compare, typename Alloc>
bool operator==(const map<Key, T, Compare, Alloc>&, const map<Key, T, Compare, Alloc>&);

template<typename Key, typename T, typename Compare, typename Alloc>
bool operator<(const map<Key, T, Compare, Alloc>&, const map<Key, T, Compare, Alloc>&);

// 用于KeyOfValue()，提取key
template<typename T>
struct select1st {
    // 参见pair定义
    const typename T::first_type& operator() (const T& x) const { return x.first; }
}; // struct select1st

template<typename Key, typename T,
         typename Compare = std::less<Key>,
         typename Alloc = alloc>
class map {
public:
    typedef Key                   key_type;
    typedef T                     data_type;
    typedef T                     mapped_type;
    typedef pair<const Key, T>    value_type; // 元素类型，保证key不被修改
    typedef Compare               key_compare;

// 用于调用元素比较函数（实际未使用）
class value_compare {
friend class map<Key, T, Compare, Alloc>;
protected :
    Compare comp;
    value_compare(Compare c) : comp(c) {}
public:
    bool operator()(const value_type& x, const value_type& y) const {
    return comp(x.first, y.first);
    }
}; // class value_comp

private:
    // 以红黑树为底层容器
    typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc> rep_type;
    rep_type t;
public:
    typedef typename rep_type::pointer pointer;                  // STL标准强制要求
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::reference reference;              // STL标准强制要求
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::iterator iterator;                // STL标准强制要求
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;  // STL标准强制要求

    map() : t(Compare()) {}
    explicit map(const Compare& comp) : t(comp) {}

    template<typename InputIterator>
    map(InputIterator first, InputIterator last)
        : t(Compare()) { t.insert_unique(first, last); }

    template<typename InputIterator>
    map(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) { t.insert_unique(first, last); }

    map(const map<Key, T, Compare, Alloc>& x) : t(x.t) {}

    map<Key, T, Compare, Alloc>& operator=(const map<Key, T, Compare, Alloc>& x) {
        t = x.t;
        return *this;
    }

    key_compare key_comp() const { return t.key_comp(); }

    value_type value_comp() const { return value_compare(t.key_comp()); } // 实际未使用

    iterator begin() { return t.begin(); }
    const_iterator begin() const { return t.begin(); }
    iterator end() { return t.end(); }
    const_iterator end() const { return t.end(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    // 下标访问操作符，如果key不存在则会新建一个
    T& operator[](const key_type& k) {
        return (*((insert(value_type(k, T()))).first)).second;
    }

    pair<iterator, bool> insert(const value_type& x) { return t.insert_unique(x); }

    iterator insert(iterator position, const value_type& x) {
        return t.insert_unique(position, x);
    }

    template<typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        t.insert_unique(first, last);
    }

    void erase(iterator position) { t.erase(position); }

    size_type erase(const key_type& x) { return t.erase(x); }

    void erase(iterator first, iterator last) { t.erase(first, last); }

    void clear() { t.clear(); }

    iterator find(const key_type& x) { return t.find(x); }
    const_iterator find(const key_type& x) const { return t.find(x); }

    iterator lower_bound(const key_type& x) { return t.lower_bound(x); }
    const_iterator lower_bound(const key_type& x) const {
        return t.lower_bound(x);
    }

    iterator upper_bound(const key_type& x) { return t.upper_bound(x); }
    const_iterator upper_bound(const key_type& x) const {
        return t.upper_bound(x);
    }

    pair<iterator, iterator> equal_range(const key_type& x) const {
        return t.equal_range(x);
    }

    friend bool operator== <>(const map&, const map&);
    friend bool operator< <>(const map&, const map&);
}; // class map

template <typename Key, typename T, typename Compare, typename Alloc>
inline bool operator==(const map<Key, T, Compare, Alloc>& x,
                       const map<Key, T, Compare, Alloc>& y)
{
    return x.t == y.t;
}

template <typename Key, typename T, typename Compare, typename Alloc>
inline bool operator<(const map<Key, T, Compare, Alloc>& x,
                      const map<Key, T, Compare, Alloc>& y)
{
    return x.t < y.t;
}

// 模板特例化，将全局的swap实现为使用map私有的swap以提高效率
template <typename Key, typename T, typename Compare, typename Alloc>
inline void swap(map<Key, T, Compare, Alloc>& x,
                 map<Key, T, Compare, Alloc>& y)
{
    x.swap(y);
}

} // namespace mystl

#endif

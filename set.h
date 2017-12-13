#ifndef MYSTL_SET_H_
#define MYSTL_SET_H_

#include "rbtree.h"
#include "iterator.h"
#include "pair.h"

namespace mystl {

// 特定模板实例的声明
template<typename Key, typename Compare, typename Alloc>
class set;

template<typename Key, typename Compare, typename Alloc>
bool operator==(const set<Key, Compare, Alloc>&, const set<Key, Compare, Alloc>&);

template<typename Key, typename Compare, typename Alloc>
bool operator<(const set<Key, Compare, Alloc>&, const set<Key, Compare, Alloc>&);

// 用于KeyOfValue()，提取key
template<typename T>
struct identity {
    const T& operator() (const T& x) const { return x; }
}; // struct identity

template<typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
class set {
public:
    // set中key和value相同
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
private:
    typedef rb_tree<key_type, value_type,
                    identity<value_type>, key_compare, Alloc> rep_type;
    rep_type t; // 以红黑树为实际容器
public:
    typedef typename rep_type::const_pointer pointer;            // STL标准强制要求
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;        // STL标准强制要求
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::const_iterator iterator;          // STL标准强制要求
    typedef typename rep_type::const_iterator const_iterator;
    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;  // STL标准强制要求

    set() : t(Compare()) {}
    explicit set(const Compare& comp) : t(comp) {}

    template<typename InputIterator>
    set(InputIterator first, InputIterator last)
        : t(Compare()) { t.insert_unique(first, last); }

    template<typename InputIterator>
    set(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp()) { t.insert_unique(first, last); }

    set(const set<Key, Compare, Alloc>& x) : t(x.t) {}

    set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
        t = x.t;
        return *this;
    }

    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return t.key_comp(); }

    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    bool empty() const { return t.empty; }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }

    typedef pair<iterator, bool> pair_itretor_bool;

    pair<iterator, bool> insert(const value_type& x) {
        pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
        return pair<iterator, bool>(p.first, p.second);
    }

    iterator insert(iterator position, const value_type& x) {
        typedef typename rep_type::iterator rep_iterator;
        return t.insert_unique((rep_iterator&)position, x);
    }

    template<typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        t.insert_unique(first, last);
    }

    void erase(iterator position) {
        typedef typename rep_type::iterator rep_iterator;
        t.erase((rep_iterator&)position);
    }

    size_type erase(const key_type& x) {
        return t.erase(x);
    }

    void erase(iterator first, iterator last) {
        typedef typename rep_type::iterator rep_iterator;
        t.erase((rep_iterator&)first, (rep_iterator&)last);
    }

    void clear() { t.clear(); }

    iterator find(const key_type& x) const { return t.find(x); }

    size_type count(const key_type& x) const { return t.count(x); }

    iterator lower_bound(const key_type& x) const {
        return t.lower_bound(x);
    }

    iterator upper_bound(const key_type& x) const {
        return t.upper_bound(x);
    }

    pair<iterator, iterator> equal_range(const key_type& x) const {
        return t.equal_range(x);
    }

    friend bool operator== <>/*<Key, Compare, Alloc>*/(const set&, const set&);
    friend bool operator< <>/*<Key, Compare, Alloc>*/(const set&, const set&);
}; // class set

template <typename Key, typename Compare, typename Alloc>
inline bool operator==(const set<Key, Compare, Alloc>& x,
                       const set<Key, Compare, Alloc>& y) {
  return x.t == y.t;
}

template <typename Key, typename Compare, typename Alloc>
inline bool operator<(const set<Key, Compare, Alloc>& x,
                      const set<Key, Compare, Alloc>& y) {
  return x.t < y.t;
}

// 模板特例化
// 将全局的swap实现为set私有的swap将提高效率
template<typename Key, typename Compare, typename Alloc>
inline void swap(set<Key, Compare, Alloc>& x,
        set<Key, Compare, Alloc>& y) {
    x.swap(y);
}

} // namespace mystl

#endif

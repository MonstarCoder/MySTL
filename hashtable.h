#ifndef MYSTL_HASHTABLE_H_
#define MYSTL_HASHTABLE_H_

#include "vector.h"
#include "iterator.h"
#include "allocator.h"
#include "construct.h"
#include "pair.h"

#include <cstddef>

#include <functional>
#include <algorithm>

namespace mystl {

// 哈希表的链表节点
template<typename Value>
struct _hashtable_node {
    _hashtable_node* next;
    Value val;
}; // struct _hashtable_node

// 前置声明
template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc = alloc>
class hashtable;

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct _hashtable_iterator;

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct _hashtable_const_iterator;

template<typename Value, typename Key, typename HashFcn,
         typename ExtractKey, typename EqualKey, typename Alloc>
bool operator==(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>&,
                          const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>&);

// 哈希表的iterator
template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct _hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>  _hashtable;
    typedef _hashtable_iterator<Value, Key, HashFcn,
                                      ExtractKey, EqualKey, Alloc>       iterator;
    typedef _hashtable_const_iterator<Value, Key, HashFcn,
                                      ExtractKey, EqualKey, Alloc>       const_iterator;
    typedef _hashtable_node<Value>                                       node;
    typedef forward_iterator_tag                                         iterator_category;
    typedef ptrdiff_t                                                    difference_type;
    typedef size_t                                                       size_type;
    typedef Value&                                                       reference;
    typedef Value*                                                       pointer;

    node* cur; // 当前位置，即线性表中的链表节点
    _hashtable* ht; // 线性表中的位置

    _hashtable_iterator(node* n, _hashtable* tab) : cur(n), ht(tab) {}
    _hashtable_iterator() {}

    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }

    iterator& operator++();
    iterator operator++(int);

    bool operator==(const iterator& it) const { return cur == it.cur; }
    bool operator!=(const iterator& it) const { return cur != it.cur; }
}; // struct _hashtable_iterator

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey, typename Alloc>
struct _hashtable_const_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>  _hashtable;
    typedef _hashtable_iterator<Value, Key, HashFcn,
                                      ExtractKey, EqualKey, Alloc>       iterator;
    typedef _hashtable_const_iterator<Value, Key, HashFcn,
                                      ExtractKey, EqualKey, Alloc>       const_iterator;
    typedef _hashtable_node<Value>                                       node;
    typedef forward_iterator_tag                                         iterator_category;
    typedef ptrdiff_t                                                    difference_type;
    typedef size_t                                                       size_type;
    typedef const Value&                                                 reference;
    typedef const Value*                                                 pointer;

    const node* cur; // 当前位置，即线性表中的链表节点
    const _hashtable* ht; // 线性表中的位置

    _hashtable_const_iterator(const node* n, const _hashtable* tab) : cur(n), ht(tab) {}
    _hashtable_const_iterator() {}

    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }

    const_iterator& operator++();
    const_iterator operator++(int);

    bool operator==(const const_iterator& it) const { return cur == it.cur; }
    bool operator!=(const const_iterator& it) const { return cur != it.cur; }
}; // struct _hashtable_const_iterator

// 素数表
static const int num_primes = 28;
static const unsigned long prime_list[num_primes] = {
    53,         97,           193,         389,       769,
    1543,       3079,         6151,        12289,     24593,
    49157,      98317,        196613,      393241,    786433,
    1572869,    3145739,      6291469,     12582917,  25165843,
    50331653,   100663319,    201326611,   402653189, 805306457,
    1610612741, 3221225473ul, 4294967291ul
};

// 返回大于n的最小素数
inline unsigned long next_prime(unsigned long n) {
    const unsigned long* first = prime_list;
    const unsigned long* last = prime_list + num_primes;
    const unsigned long* pos = std::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

// Value:       节点的value类型
// Key:         节点的key类型
// HashFcn:     hash function
// ExtractKey:  从节点中取出键值的方法
// EqualKey:    判断键值是否相同的方法
// Alloc:       allocator, 默认为alloc（见前置声明）
template<typename Value, typename Key, typename HashFcn,
         typename ExtractKey, typename EqualKey, typename Alloc>
class hashtable {
public:
    typedef Key                  key_type;
    typedef Value                value_type;
    typedef HashFcn              hasher;
    typedef EqualKey             key_equal;
    typedef size_t               size_type;
    typedef ptrdiff_t            difference_type;
    typedef value_type*          pointer;
    typedef const value_type*    const_pointer;
    typedef value_type&          reference;
    typedef const value_type&    const_reference;

    hasher hash_funct() const { return hash; }
    key_equal key_eq() const { return equals; }
private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef _hashtable_node<Value> node;
    typedef allocator<node, Alloc> node_allocator;

    vector<node*, Alloc> buckets;
    size_type num_elements;
public:
    typedef _hashtable_iterator<Value, Key, HashFcn, ExtractKey,
                                      EqualKey, Alloc> iterator;
    typedef _hashtable_const_iterator<Value, Key, HashFcn, ExtractKey,
                                      EqualKey, Alloc> const_iterator;

    friend struct _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend struct _hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
public:
    // 构造析构相关
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql, const ExtractKey& ext)
        : hash(hf), equals(eql), get_key(ext), num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(const hashtable& ht)
        : hash(ht.hash), equals(ht.equals), get_key(ht.get_key), num_elements(0) {
        copy_from(ht);
    }

    hashtable& operator=(const hashtable& ht) {
        if (&ht != this) {
            clear();
            hash = ht.hash;
            equals = ht.equals;
            get_key = ht.get_key;
            copy_from(ht);
        }
        return *this;
    }

    ~hashtable() { clear(); }

    // 访问、修改元素相关
    size_type size() const { return num_elements; }
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return size() == 0; }

    void swap(hashtable& ht) {
        std::swap(hash, ht.hash);
        std::swap(equals, ht.equals);
        std::swap(get_key, ht.get_key);
        buckets.swap(ht.buckets);
        std::swap(num_elements, ht.num_elements);
    }

    iterator begin() {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return iterator(buckets[n], this);
        return end();
    }

    iterator end() { return iterator(0, this); }

    const_iterator begin() const {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return const_iterator(buckets[n], this);
        return end();
    }

    const_iterator end() const { return const_iterator(0, this); }

    friend bool operator== <>(const hashtable&, const hashtable&);

public:
    // 线性表中的节点数
    size_type bucket_count() const { return buckets.size(); }

    // 线性表最多能分配的节点数
    size_type max_bucket_count() const { return prime_list[num_primes - 1]; }

    // 返回指定bucket有多少value
    size_type elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for (node* cur = buckets[bucket]; cur; cur = cur->next)
            result += 1;
        return result;
    }

    // 插入操作, 不允许重复
    pair<iterator, bool> insert_unique(const value_type& obj) {
        // 首先判断容量是否够用, 否则就重新配置
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    // 插入操作, 允许重复
    iterator insert_equal(const value_type& obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }

    pair<iterator, bool>
    insert_unique_noresize(const value_type& obj);

    iterator insert_equal_noresize(const value_type& obj);

    template <typename InputIterator>
    void insert_unique(InputIterator f, InputIterator l) {
        insert_unique(f, l, iterator_category(f));
    }

    template <typename InputIterator>
    void insert_equal(InputIterator f, InputIterator l) {
        insert_equal(f, l, iterator_category(f));
    }

    template <typename InputIterator>
    void insert_unique(InputIterator f, InputIterator l,
                       input_iterator_tag) {
        for ( ; f != l; ++f)
            insert_unique(*f);
    }

    template <typename InputIterator>
    void insert_equal(InputIterator f, InputIterator l,
                      input_iterator_tag) {
        for ( ; f != l; ++f)
            insert_equal(*f);
    }

    template <typename ForwardIterator>
    void insert_unique(ForwardIterator f, ForwardIterator l,
                       forward_iterator_tag) {
        size_type n = std::distance(f, l);
        resize(num_elements + n);
        for ( ; n > 0; --n, ++f)
            insert_unique_noresize(*f);
    }

    template <typename ForwardIterator>
    void insert_equal(ForwardIterator f, ForwardIterator l,
                      forward_iterator_tag) {
        size_type n = std::distance(f, l);
        resize(num_elements + n);
        for ( ; n > 0; --n, ++f)
            insert_equal_noresize(*f);
    }

    reference find_or_insert(const value_type& obj);

    // 查找指定key
    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first;
        for (first = buckets[n];
             first && !equals(get_key(first->val), key);
             first = first->next)
        {  }
        return iterator(first, this);
    }

    const_iterator find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        const node* first;
        for (first = buckets[n];
             first && !equals(get_key(first->val), key);
             first = first->next)
        {  }
      return const_iterator(first, this);
    }

    // 返回key元素的个数
    size_type count(const key_type& key) const {
        const size_type n = bkt_num_key(key);
        size_type result = 0;

        for (const node* cur = buckets[n]; cur; cur = cur->next)
            if (equals(get_key(cur->val), key))
                ++result;
        return result;
    }

    pair<iterator, iterator> equal_range(const key_type& key);
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

    // 删除元素
    size_type erase(const key_type& key);
    void erase(const iterator& it);
    void erase(iterator first, iterator last);

    void erase(const const_iterator& it);
    void erase(const_iterator first, const_iterator last);

    void resize(size_type num_elements_hint);
    void clear();

    private:
    size_type next_size(size_type n) const { return next_prime(n); }

    // 预留空间, 并进行初始化
    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*) 0);
        num_elements = 0;
    }

    size_type bkt_num_key(const key_type& key) const {
        return bkt_num_key(key, buckets.size());
    }

    // 获取obj映射位置, 要经过一个mod过程
    size_type bkt_num(const value_type& obj) const {
        return bkt_num_key(get_key(obj));
    }

    size_type bkt_num_key(const key_type& key, size_t n) const {
        return hash(key) % n;
    }

    size_type bkt_num(const value_type& obj, size_t n) const {
        return bkt_num_key(get_key(obj), n);
    }

    // 分配空间并进行构造
    node* new_node(const value_type& obj) {
        node* n = node_allocator::allocate();
        n->next = 0;
        try {
            construct(&n->val, obj);
            return n;
        }
        catch(...) {
            node_allocator::deallocate(n);
            throw;
        }
    }

    // 析构并释放空间
    void delete_node(node* n) {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }

    void erase_bucket(const size_type n, node* first, node* last);
    void erase_bucket(const size_type n, node* last);

    void copy_from(const hashtable& ht);
}; // class hashtable

// 以下是struct _hashtable_iterator的实现
template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
_hashtable_iterator<V, K, HF, ExK, EqK, A>&
_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() {
    const node* old = cur;
    cur = cur->next;              // 当前链表结点的下一个结点, 如果不为0
                                 // 那么它就是我们要的
    // 链表结点恰好是最后一个结点, 我们要在线性表的下一个表格的链表中查找
    if (!cur)
    {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
 }

template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
inline _hashtable_iterator<V, K, HF, ExK, EqK, A>
_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
    iterator tmp = *this;
    ++*this;      // 触发operator ++()
    return tmp;
}

// const情况同上
template <typename V, class K, class HF, class ExK, class EqK, class A>
_hashtable_const_iterator<V, K, HF, ExK, EqK, A>&
_hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++() {
    const node* old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template <typename V, class K, class HF, class ExK, class EqK, class A>
inline _hashtable_const_iterator<V, K, HF, ExK, EqK, A>
_hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
}

// 以下是class hashtable的实现
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
bool operator==(const hashtable<V, K, HF, Ex, Eq, A>& ht1,
                const hashtable<V, K, HF, Ex, Eq, A>& ht2) {
    typedef typename hashtable<V, K, HF, Ex, Eq, A>::node node;
    if (ht1.buckets.size() != ht2.buckets.size())
        return false;
    for (int n = 0; n < ht1.buckets.size(); ++n) {
        node* cur1 = ht1.buckets[n];
        node* cur2 = ht2.buckets[n];
        for ( ; cur1 && cur2 && cur1->val == cur2->val;
                cur1 = cur1->next, cur2 = cur2->next)
        {}
        if (cur1 || cur2)
            return false;
    }
    return true;
}

//　特例化swap
template <typename Val, typename Key, typename HF, typename Extract, typename EqKey, typename A>
inline void swap(hashtable<Val, Key, HF, Extract, EqKey, A>& ht1,
                 hashtable<Val, Key, HF, Extract, EqKey, A>& ht2) {
    ht1.swap(ht2);
}

// 在不需要重新调整容量的情况下插入元素, key不可以重复
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj) {
    // 获取待插入元素在hashtable中的索引
    const size_type n = bkt_num(obj);

    node* first = buckets[n];

    for (node* cur = first; cur; cur = cur->next)
        // 如果keu重复, 在不进行插入, 并告知用户插入失败
        if (equals(get_key(cur->val), get_key(obj)))
            return pair<iterator, bool>(iterator(cur, this), false);

    // 插入结点
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return pair<iterator, bool>(iterator(tmp, this), true);
}

// 在不需要重新调整容量的情况下插入元素, key可以重复
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
typename hashtable<V, K, HF, Ex, Eq, A>::iterator
hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];

    for (node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj))) {
            node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);
}

// 用于支持hash_map操作
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
typename hashtable<V, K, HF, Ex, Eq, A>::reference
hashtable<V, K, HF, Ex, Eq, A>::find_or_insert(const value_type& obj) {
    resize(num_elements + 1);

    size_type n = bkt_num(obj);
    node* first = buckets[n];

    for (node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj)))
            return cur->val;

    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return tmp->val;
}

// 查找满足key的区间
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator,
     typename hashtable<V, K, HF, Ex, Eq, A>::iterator>
hashtable<V, K, HF, Ex, Eq, A>::equal_range(const key_type& key) {
    typedef pair<iterator, iterator> pii;
    const size_type n = bkt_num_key(key);

    for (node* first = buckets[n]; first; first = first->next) {
        if (equals(get_key(first->val), key)) {
            for (node* cur = first->next; cur; cur = cur->next)
                if (!equals(get_key(cur->val), key))
                    return pii(iterator(first, this), iterator(cur, this));
            for (size_type m = n + 1; m < buckets.size(); ++m)
                if (buckets[m])
                    return pii(iterator(first, this),
                            iterator(buckets[m], this));
            return pii(iterator(first, this), end());
        }
    }
    return pii(end(), end());
}

template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator,
     typename hashtable<V, K, HF, Ex, Eq, A>::const_iterator>
hashtable<V, K, HF, Ex, Eq, A>::equal_range(const key_type& key) const {
    typedef pair<const_iterator, const_iterator> pii;
    const size_type n = bkt_num_key(key);

    for (const node* first = buckets[n] ; first; first = first->next) {
        if (equals(get_key(first->val), key)) {
            for (const node* cur = first->next; cur; cur = cur->next)
                if (!equals(get_key(cur->val), key))
                    return pii(const_iterator(first, this),
                            const_iterator(cur, this));
            for (size_type m = n + 1; m < buckets.size(); ++m)
                if (buckets[m])
                    return pii(const_iterator(first, this),
                            const_iterator(buckets[m], this));
            return pii(const_iterator(first, this), end());
        }
    }
    return pii(end(), end());
}

// 删除指定元素
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
typename hashtable<V, K, HF, Ex, Eq, A>::size_type
hashtable<V, K, HF, Ex, Eq, A>::erase(const key_type& key) {
    // 计算映射位置
    const size_type n = bkt_num_key(key);
    node* first = buckets[n];
    size_type erased = 0;

    // 开始查找并删除
    if (first) {
        node* cur = first;
        node* next = cur->next;
        while (next) {
            if (equals(get_key(next->val), key)) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                ++erased;
                --num_elements;
            }
            else {
                cur = next;
                next = cur->next;
            }
        }
        if (equals(get_key(first->val), key)) {
            buckets[n] = first->next;
            delete_node(first);
            ++erased;
            --num_elements;
        }
    }
    return erased;
}

template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::erase(const iterator& it) {
    if (node* const p = it.cur) {
        const size_type n = bkt_num(p->val);
        node* cur = buckets[n];

        if (cur == p) {
            buckets[n] = cur->next;
            delete_node(cur);
            --num_elements;
        }
        else {
            node* next = cur->next;
            while (next) {
                if (next == p) {
                    cur->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                }
                else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

// 删除指定区间的元素
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::erase(iterator first, iterator last) {
    size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();
    size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();

    if (first.cur == last.cur)
        return;
    else if (f_bucket == l_bucket)
        erase_bucket(f_bucket, first.cur, last.cur);
    else {
        erase_bucket(f_bucket, first.cur, 0);
        for (size_type n = f_bucket + 1; n < l_bucket; ++n)
            erase_bucket(n, 0);
        if (l_bucket != buckets.size())
            erase_bucket(l_bucket, last.cur);
    }
}

template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
inline void
hashtable<V, K, HF, Ex, Eq, A>::erase(const_iterator first,
        const_iterator last) {
    erase(iterator(const_cast<node*>(first.cur),
          const_cast<hashtable*>(first.ht)),
          iterator(const_cast<node*>(last.cur),
          const_cast<hashtable*>(last.ht)));
}

template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
inline void
hashtable<V, K, HF, Ex, Eq, A>::erase(const const_iterator& it) {
    erase(iterator(const_cast<node*>(it.cur),
          const_cast<hashtable*>(it.ht)));
}

// 调整hashtable的容量
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
    const size_type old_n = buckets.size();

    // 如果新调整的大小小于当前大小, 不进行更改
    if (num_elements_hint > old_n) {
        const size_type n = next_size(num_elements_hint);

        // 如果已经到达hashtable的容量的极限, 那么也不进行更改
        if (n > old_n) {
            // 建立新的线性表来扩充容量
            vector<node*, A> tmp(n, (node*) 0);
            try {
                // 先面开始copy
                for (size_type bucket = 0; bucket < old_n; ++bucket) {
                    node* first = buckets[bucket];
                    while (first) {
                        size_type new_bucket = bkt_num(first->val, n);
                        buckets[bucket] = first->next;
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        first = buckets[bucket];
                    }
                }
                buckets.swap(tmp);
            }
            catch(...) {
                for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                    while (tmp[bucket]) {
                        node* next = tmp[bucket]->next;
                        delete_node(tmp[bucket]);
                        tmp[bucket] = next;
                    }
                }
                throw;
            }
        }
    }
}

// 删除指定映射位置的所有元素
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::erase_bucket(const size_type n,
                                                  node* first, node* last) {
    node* cur = buckets[n];
    if (cur == first)
        erase_bucket(n, last);
    else {
        node* next;
        for (next = cur->next; next != first; cur = next, next = cur->next)
            ;
        while (next) {
            cur->next = next->next;
            delete_node(next);
            next = cur->next;
            --num_elements;
        }
    }
}

template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::erase_bucket(const size_type n, node* last) {
    node* cur = buckets[n];
    while (cur != last) {
        node* next = cur->next;
        delete_node(cur);
        cur = next;
        buckets[n] = cur;
        --num_elements;
    }
}

// 清空hashtable, 但是不释放vector的内存
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::clear() {
    for (size_type i = 0; i < buckets.size(); ++i) {
        node* cur = buckets[i];
        while (cur != 0) {
            node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = 0;
    }
    num_elements = 0;
}

// 复制另一个hashtable给当前hashtable
template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable& ht) {
    // 首先清空当前hashtable
    buckets.clear();
    // 预留足够容量
    buckets.reserve(ht.buckets.size());
    // 完成初始化操作, 这是hashtable的先验条件
    buckets.insert(buckets.end(), ht.buckets.size(), (node*) 0);
    try {
        // 开始copy操作
        for (size_type i = 0; i < ht.buckets.size(); ++i) {
            if (const node* cur = ht.buckets[i]) {
                node* copy = new_node(cur->val);
                buckets[i] = copy;

                for (node* next = cur->next; next; cur = next, next = cur->next) {
                    copy->next = new_node(next->val);
                    copy = copy->next;
                }
            }
        }
        num_elements = ht.num_elements;
    }
    catch(...) {
        clear();
        throw;
    }
}

} // namespace mystl

#endif

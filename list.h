#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "typetraits.h"
#include "iterator.h"

#include <cstddef> // for size_t, ptrdiff_t
#include <iterator> // for distance
#include <iostream>
#include <initializer_list>
#include <stdexcept>

namespace mystl {

//**********list的node结构**********
template<typename T>
struct _list_node {
    typedef _list_node* node_pointer;
    node_pointer prev; // 双向节点
    node_pointer next;
    T data;
}; // struct _list_node

//**********list iterator**********
template<typename T, typename Ref, typename Ptr>
struct _list_iterator {
    typedef _list_iterator<T, T&, T*>     iterator; // 指向节点数据的迭代器
    typedef _list_iterator<T, Ref, Ptr>   self; // 指向list节点的迭代器
    typedef bidirectional_iterator_tag    iterator_category;
    typedef T                             value_type;
    typedef Ptr                           pointer;
    typedef Ref                           reference;
    typedef _list_node<T>*                link_type;
    typedef size_t                        size_type;
    typedef ptrdiff_t                     difference_type;

    link_type                             node; // 指向节点的指针

    // constructor
    _list_iterator(link_type x) : node(x) {}
    _list_iterator() {}
    _list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }

    reference operator*() const { return (*node).data; } // 返回的是实际维护的数据
    pointer operator->() const { return &(this->operator*()); } // 返回的是指针

    // 前置递增和递减
    self& operator++() {
        node = node->next;
        return *this;
    }
    self& operator--() {
        node = node->prev;
        return *this;
    }
    // 后置递增和递减
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self operator+(int n) {
        self tmp = *this;
        while (n-- > 0) {
            tmp = tmp.node->next;
        }
        return tmp;
    }
    self operator-(int n) {
        self tmp = *this;
        while (n-- > 0) {
            tmp = tmp.node->prev;
        }
        return tmp;
    }
}; // struct _list_iterator

//**********class list**********
template<typename T, typename Alloc = alloc>
class list {
protected:
    typedef _list_node<T>        list_node;
    typedef allocator<list_node, Alloc>        list_node_allocator; // 空间配置器
public:
    typedef list_node*           link_type;
    typedef T                    value_type;
    typedef value_type*          pointer;
    typedef const value_type*    const_pointer;
    typedef value_type&          reference;
    typedef const value_type&    const_reference;
    typedef size_t               size_type;
    typedef ptrdiff_t            difference_type;
public:
    typedef _list_iterator<T, T&, T*>                iterator;
    typedef _list_iterator<T, const T&, const T*>    const_iterator;
protected:
    link_type node; // 链表头结点，本身不保存数据

public:
    // 构造，析构，复制相关
    list() { empty_initialize(); } // 产生空的链表
    ~list() {
        clear();
        put_node(node); // 释放头结点
    }

    list(size_type n, const T& value) { fill_initialize(n, value); }
    list(int n, const T& value) { fill_initialize(n, value); }
    list(long n, const T& value) { fill_initialize(n, value); }

    explicit list(size_type n) { fill_initialize(n, T()); } // explicit防止出现list<int> = n

    template<typename InputIterator>
    list(InputIterator first, InputIterator last) { range_initialize(first, last); }

    list(const list<T, Alloc>& x) { // 复制构造
        range_initialize(x.begin(), x.end());
    }

    list& operator=(const list<T, Alloc>& x);

    // 迭代器和容量相关
    iterator begin() { return node->next; }
    const_iterator begin() const { return node->next; }
    const_iterator cbegin() const { return node->next; }
    iterator end() { return node; }
    const_iterator end() const { return node; }
    const_iterator cend() const { return node; }

    bool empty() const { return node->next == node; }
    size_type size() const {
        size_type result = 0;
        result = std::distance(begin(), end());
        return result;
    }

    // 访问，比较元素
    reference max_size() const { return static_cast<size_type>(-1); }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }

    template<typename l, typename alloc>
    friend bool operator==(const list<l, alloc>& x, const list<l, alloc>& y);
    template<typename l, typename alloc>
    friend bool operator!=(const list<l, alloc>& x, const list<l, alloc>& y);
    // 链表比较大小使用的hi字典顺序
    //inline bool operator<(const list<T, Alloc>& x, const list<T, Alloc>& y);

    // 操作容器相关
    void swap(list<T, Alloc>& x) { std::swap(node, x.node); }

    iterator insert(iterator position, const T& x);
    iterator insert(iterator position);
    template<typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);
    void insert(iterator position, size_type n, const T& x);
    void insert(iterator position, int n, const T& x);
    void insert(iterator position, long n, const T& x);

    void push_front(const T& x) { insert(begin(), x); } // 在链表前段插入节点
    void push_back(const T& x) { insert(end(), x); } // 在链表后插入节点

    iterator erase(iterator position); // 删除指定节点
    iterator erase(iterator first, iterator last); // 删除一个区间的节点

    void resize(size_type new_size, const T& x);
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear();
    void pop_front() { erase(begin()); }
    void pop_back() { erase(--end()); }

    // 将链表x移动到position之前
    void splice(iterator position, list& x);
    // 将链表中i指向的内容移动到position之前
    void splice(iterator position, list&, iterator i);
    // 将[first, last)元素移动到position之前
    void splice(iterator position, list&, iterator first, iterator last);

    void remove(const T& value);
    void unique();
    void merge(list& x);
    void reverse();
    void sort();

    // 仿函数应用，重载unique(), merge(), sort()
    template<typename Predicate> void remove_if(Predicate); // 仿函数（函数对象）
    template<typename BinaryPredicate> void unique(BinaryPredicate);
    template<typename StrictWeakOrdering> void merge(list&, StrictWeakOrdering);
    template<typename StrictWeakOrdering> void sort(StrictWeakOrdering);

protected:
    // 分配一个新节点，不进行构造
    link_type get_node() { return list_node_allocator::allocate(); }

    // 释放指定节点，不进行析构
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    // 创建节点，分配内存后进行构造
    link_type create_node(const T& x) {
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }

    // 析构节点元素，并释放内存
    void destroy_node(link_type p) {
        destroy(&(p->data));
        put_node(p);
    }

    // 产生空链表
    void empty_initialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }

    // 创建值为value的n个节点的链表
    void fill_initialize(size_type n, const T& value) {
        empty_initialize();
        insert(begin(), n, value);
    }

    // 以一个区间初始化链表
    template<typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        empty_initialize();
        insert(begin(), first, last);
    }

    // 将[first, last)区间插入到psition之前
    // 如果last == position,则不进行操作
    void transfer(iterator position, iterator first, iterator last) {
        if (position != last) {
            last.node->prev->next = position.node;
            first.node->prev->next = last.node;
            position.node->prev->next = first.node;
            link_type tmp = position.node->prev;
            position.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = tmp;
        }
    }
}; // class list

//**********构造，析构，复制相关**********
template<typename T, typename Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& x) {
    if (this != &x) {
        iterator first1 = begin();
        iterator last1 = end();
        const_iterator first2 = x.begin();
        const_iterator last2 = x.end();
        while (first1 != last1 && first2 != last2)
            *first1++ = *first2++;
        if (first2 == last2)
            erase(first1, last1);
        else
            insert(last1, first2, last2);
    }
    return *this;
}

//**********访问比较元素**********
template<typename T, typename Alloc>
bool operator==(const list<T, Alloc>& x, const list<T, Alloc>& y) {
    typedef typename list<T, Alloc>::link_type link_type;
    link_type e1 = x.node;
    link_type e2 = y.node;
    link_type n1 = e1->next;
    link_type n2 = e2->next;
    for ( ; n1 != e1 && n2 != e2; n1 = n1->next, n2 = n2->next)
        if (n1->data != n2->data)
            return false;
    return n1 == e1 && n2 == e2;
}

template<typename T, typename Alloc>
bool operator!=(const list<T, Alloc>& x, const list<T, Alloc>& y) {
    return !(x == y);
}

//**********操作容器相关**********
template<typename T, typename Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::insert(iterator position, const T& x) {
    link_type tmp = create_node(x);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
}

template<typename T, typename Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::insert(iterator position) {
    return insert(position, T());
}

template<typename T, typename Alloc>
template<typename InputIterator>
void list<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last) {
    for ( ; first != last; ++first)
        insert(position, *first);
}

template<typename T, typename Alloc>
void list<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    for ( ; n > 0; --n)
        insert(position, x);
}

template<typename T, typename Alloc>
void list<T, Alloc>::insert(iterator position, int n, const T& x) {
    for ( ; n > 0; --n)
        insert(position, x);
}

template<typename T, typename Alloc>
void list<T, Alloc>::insert(iterator position, long n, const T& x) {
    for ( ; n > 0; --n)
        insert(position, x);
}

template<typename T, typename Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::erase(iterator position) {
    link_type next_node = position.node->next;
    link_type prev_node = position.node->prev;
    prev_node->next = next_node;
    next_node->prev = prev_node;
    destroy_node(position.node);
    return next_node;
}

template<typename T, typename Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::erase(iterator first, iterator last) {
    while (first != last) erase(first++);
    return last;
}

//重新设置容量大小，如果当前容量大于新容量，则析构多余元素
//否则增加容量至指定大小，并填充为元素x
template<typename T, typename Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x) {
    iterator i = begin();
    size_type len = 0;
    for ( ; i != end() && len < new_size; ++i, ++len)
        ;
    if (len == new_size)
        erase(i, end());
    else
        insert(end(), new_size - len, x);
}

template<typename T, typename Alloc>
void list<T, Alloc>::clear() {
    link_type cur = node->next;
    while (cur != node) {
        link_type tmp = cur;
        cur = cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
}

//将链表x移动到position之前
template<typename T, typename Alloc>
void list<T, Alloc>::splice(iterator position, list& x) {
    if (!x.empty())
        transfer(position, x.begin(), x.end());
}

//将链表中i指向的内容移动到position之前
template<typename T, typename Alloc>
void list<T, Alloc>::splice(iterator position, list& x, iterator i) {
    iterator j = i;
    ++j;
    if (position == i || position == j) return;
    transfer(position, i, j);
}

//将[first, last)元素移动到position之前
template<typename T, typename Alloc>
void list<T, Alloc>::splice(iterator position, list& x, iterator first, iterator last) {
    if (first != last)
        transfer(position, first, last);
}

//移除指定值的所有节点，时间复杂度为O(n)
template<typename T, typename Alloc>
void list<T, Alloc>::remove(const T& value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        if (*first == value) erase(first);
        first = next;
    }
}

//移除容器内所有相邻的重复节点，时间复杂度为O(n)
template<typename T, typename Alloc>
void list<T, Alloc>::unique() {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
        if (*first == *next)
            erase(next);
        else
            first = next;
        next = first;
    }
}

//假设当前容器和x都已有序，保证两容器合并和仍然有序
template<typename T, typename Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) {
            iterator next = first2;
            trandfer(first1, first2, ++next);
            first2 = next;
        }
        else
            ++first1;
    }
    if (first2 != last2) transfer(last1, first2, last2);
}

//将链表倒置：
//遍历链表，每次取出一个节点，并插入到链表起始点
template<typename T, typename Alloc>
void list<T, Alloc>::reverse() {
    if (node->next == node || node->next->next == node) return;
    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        transfer(begin(), old, first);
    }
}

//按照升序排序
template<typename T, typename Alloc>
void list<T, Alloc>::sort() {
    if (node->next == node || node->next->next == node) return;
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }
    for (int i = 1; i < fill; ++i)
        counter[i].merge(counter[i - 1]);
    swap(counter[fill - 1]);
}

//给定一个仿函数，如果仿函数为真则进行相应的元素移除
template<typename T, typename Alloc>
template<typename Predicate>
void list<T, Alloc>::remove_if(Predicate pred) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        if (pred(*first)) erase(first);
        first = next;
    }
}

//根据仿函数，决定如何移除相邻的重复节点
template<typename T, typename Alloc>
template<typename BinaryPredicate>
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
        if (binary_pred(*first, *next))
            erase(next);
        else
            first = next;
        next = first;
    }
}

//假设当前容器和x已经有序，将x合并到当前容器中，并保证在comp仿函数判定下仍然有效
template<typename T, typename Alloc>
template<typename StrictWeakOrdering>
void list<T, Alloc>::merge(list<T, Alloc>& x, StrictWeakOrdering comp) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first2)) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
        }
        else
            ++first1;
    }
    if (first2 != last2) transfer(last1, first2, last2);
}

//根据仿函数comp进行排序
template<typename T, typename Alloc>
template<typename StrictWeakOrdering>
void list<T, Alloc>::sort(StrictWeakOrdering comp) {
    if (node->next == node || node->next->next == node) return;
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry, comp);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }
    for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1], comp);
    swap(counter[fill - 1]);
}

} // namespace mystl

#endif

#ifndef MUSTL_DEQUE_H_
#define MYSTL_DEQUE_H_

#include "allocator.h"
#include "iterator.h"
#include "alloc.h"
#include "construct.h"

#include <iostream>
#include <algorithm> // for max, equal,lexicographical_compare
#include <iterator> // for distance
#include <memory> // for copy
#include <cstddef> // for ptrdiff_t
#include <utility> // for swap
#include <stdexcept>

namespace mystl {

//**********防止不同编译器在处理常量表达式时的bug**********
inline size_t _deque_buf_size(size_t n, size_t sz) {
    return n != 0 ? n : (sz < 512 ? static_cast<size_t>(512 / sz) : static_cast<size_t>(1));
}

//**********deque iterator**********
template<typename T, typename Ref, typename Ptr, size_t BufSiz>
struct _deque_iterator {
    typedef _deque_iterator<T, T&, T*, BufSiz>                iterator;
    typedef _deque_iterator<T, const T&, const T*, BufSiz>    const_iterator;
    static size_t buffer_size() { return _deque_buf_size(BufSiz, sizeof(T)); }

    typedef random_access_iterator_tag    iterator_category; // STL标准强制要求
    typedef T                             value_type;        // STL标准强制要求
    typedef Ptr                           pointer;           // STL标准强制要求
    typedef Ref                           reference;         // STL标准强制要求
    typedef size_t                        size_type;
    typedef ptrdiff_t                     difference_type;    // STL标准强制要求
    typedef T**                           map_pointer;

    typedef _deque_iterator               self;

    //保存容器中的节点
    T* cur;  // 指向当前缓冲区的元素
    T* first; // 当前缓冲区的起点
    T* last; // 当前缓冲区的终点

    map_pointer node; // 指向map

    _deque_iterator(T* x, map_pointer y)
        : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    _deque_iterator() : cur(0), first(0), last(0), node(0) {}
    _deque_iterator(const iterator& x)
        : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

    reference operator*() const { return *cur; }
    pointer operator->() const { return &(operator*()); }

    difference_type operator-(const self& x) const { //相同node之间才可直接-
        return static_cast<difference_type>(buffer_size()) * (node - x.node - 1) +
            (cur - first) + (x.last - x.cur);
    }

    self& operator++() {
        ++cur;
        if (cur == last) { // 如果达到所在缓冲区的尾端，则切换至下一个节点
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if (cur == first) { // 如果达到所在缓冲区的头端，则切换至前一个节点
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < static_cast<difference_type>(buffer_size())) {
            cur += n;
        }
        else {
            difference_type node_offset =
                offset > 0 ? offset / static_cast<difference_type>(buffer_size())
                : -static_cast<difference_type>((-offset - 1) / buffer_size()) -1;
            //切换至正确的节点
            set_node(node + node_offset);
            //切换至正确的元素
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) {
        return *this += -n;
    }

    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return !(*this == x); }
    bool operator<(const self& x) const {
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }

    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + static_cast<difference_type>(buffer_size());
    }
}; // struct _deque_iterator

//**********class deque**********
template<typename T, typename Alloc = alloc, size_t  BufSiz = 0>
class deque {
public:
    typedef T                    value_type;
    typedef value_type*          pointer;
    typedef const value_type*    const_pointer;
    typedef value_type&          reference;
    typedef const value_type&    const_reference;
    typedef size_t               size_type;
    typedef ptrdiff_t            difference_type;

public:
    typedef _deque_iterator<T, T&, T*, BufSiz>                iterator;
    typedef _deque_iterator<T, const T&, const T*, BufSiz>    const_iterator;

protected:
    typedef pointer* map_pointer;
    typedef allocator<value_type, Alloc>    data_allocator; // deque空间配置器
    typedef allocator<pointer, Alloc>       map_allocator;

    // 获取缓冲区最大存储元素数量
    static size_type buffer_size() {
        return _deque_buf_size(BufSiz, sizeof(value_type));
    }

    static size_type initial_map_size() { return 8; }

protected:
    iterator start; // 起始缓冲区
    iterator finish; // 最后一个缓冲区
    map_pointer map; // 指向map的指针
    size_type map_size; // map容量

public:
    // 构造，析构和移动，复制相关等
    deque() : start(), finish(), map(0), map_size(0) {
        create_map_and_nodes(0);
    }

    deque(const deque& x) : start(), finish(), map(0), map_size(0) {
        create_map_and_nodes(x.size());
        std::uninitialized_copy(x.begin(), x.end(), start);
    }

    // 移动构造函数
    deque(deque&& x) : start(x.start), finish(x.finish), map(x.map), map_size(x.map_size) {}

    deque(size_type n, const value_type& value)
        : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    deque(int n, const value_type& value)
        : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    deque(long n, const value_type& value)
        : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    explicit deque(size_type n) : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value_type());
    }

    template<typename InputIterator>
    deque(InputIterator first, InputIterator last)
        : start(), finish(), map(0), map_size(0) {
        range_initialize(first, last, iterator_category(first));
    }

    ~deque() {
        destroy(start, finish);
        destroy_map_and_nodes();
    }

    deque& operator=(const deque& x) {
        if (&x != this) {
            const size_type len = size();
            if (len >= x.size()) {
                erase(std::copy(x.begin(), x.end(), start), finish);
            }
            else {
                const_iterator mid = x.begin() + static_cast<difference_type>(len);
                std::copy(x.begin(), mid, start);
                insert(finish, mid, x.end());
            }
        }
        return *this;
    }

    /* 既是拷贝，也是移动赋值运算符？
    deque& operator=(const deque x) {
        *this.swap(x);
    }
    */

public:
    // 访问容器元素和大小相关
    iterator begin() { return start; }
    iterator end() { return finish; }
    const_iterator begin() const { return start; }
    const_iterator end() const { return finish; }

    reference operator[](size_type n) {
        return start[static_cast<difference_type>(n)];
    }
    const_reference operator[](size_type n) const {
        return start[static_cast<difference_type>(n)];
    }

    reference front() { return *start; }
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    const_reference front() const { return *start; }
    const_reference back() const {
        const_iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    size_type size() const { return finish - start; }
    size_type max_size() const { return static_cast<size_type>(-1); }
    bool empty() const { return finish == start; }

public:
    // 操作容器相关
    void swap(deque& x) { //　交换其内部维护的iterator即可
        std::swap(start, x.start);
        std::swap(finish, x.finish);
        std::swap(map, x.map);
        std::swap(map_size, x.map_size);
    }

    void push_back(const value_type& t) {
        if (finish.cur != finish.last -1) {
            construct(finish.cur, t);
            ++finish.cur;
        } else { // 容量已满要申请新内存
            push_back_aux(t);
        }
    }

    void push_front(const value_type& t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        } else {
            push_front_aux(t);
        }
    }

    void pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;
            destroy(finish.cur);
        } else {
            pop_back_aux();
        }
    }

    void pop_front() {
        if (start.cur != start.last -1) {
            destroy(start.cur);
            ++start.cur;
        } else {
            pop_front_aux();
        }
    }

    iterator insert(iterator position, const value_type& x);
    iterator insert(iterator position);
    void insert(iterator pos, size_type n, const value_type& x);
    void insert(iterator pos, int n, const value_type& x);
    void insert(iterator pos, long n, const value_type& x);
    template<typename InputIterator>
    void insert(iterator pos, InputIterator first, InputIterator last);

    void resize(size_type new_size, const value_type& x);
    void resize(size_type new_size);

    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);

    void clear();

// 比较元素相关
public:
    bool operator==(const deque<T, Alloc, 0>& x) const {
        return size() == x.size() && equal(begin(), end(), x.begin());
    }
    bool operator!=(const deque<T, Alloc, 0>& x) const {
        return !(*this == x);
    }
    bool operator<(const deque<T, Alloc, 0>& x) const {
        return std::lexicographical_compare(begin(), end(), x.begin(), x.end()); // 按std字典序
    }

protected:
    void create_map_and_nodes(size_type n);
    void destroy_map_and_nodes();
    void fill_initialize(size_type n, const value_type& value);
    template<typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last, input_iterator_tag);

protected:
    void push_back_aux(const value_type& t);
    void push_front_aux(const value_type& t);
    void pop_back_aux();
    void pop_front_aux();

    iterator insert_aux(iterator pos, const value_type& x);
    void insert_aux(iterator pos, size_type n, const value_type& x);
    template<typename ForwardIterator>
    void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last, size_type n);

    //　在起始缓冲区预留大小为n的空间，如果缓冲区不足则重新分配
    iterator reserve_elements_at_front(size_type n) {
        size_type tmp = start.cur - start.first;
        if (n > tmp)
            new_elements_at_front(n - tmp);
        return start - static_cast<difference_type>(n);
    }

    iterator reserve_elements_at_back(size_type n) {
        size_type tmp = (finish.last - finish.cur) - 1;
        if (n > tmp)
            new_elements_at_back(n - tmp);
        return finish + static_cast<difference_type>(n);
    }

    void new_elements_at_front(size_type new_elements);
    void new_elements_at_back(size_type new_elements);

    void destroy_nodes_at_front(iterator before_start);
    void destroy_nodes_at_back(iterator after_finish);

protected:
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add > start.node - map)
            reallocate_map(nodes_to_add, true);
    }

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front);

    void deallocate_map(size_type nodes_to_add, bool add_at_front);

    pointer allocate_node() { // 分配内存，不进行构造
        return data_allocator::allocate(buffer_size());
    }

    void deallocate_node(pointer n) {
        data_allocator::deallocate(n, buffer_size());
    }
}; // class deque

//**********构造，析构和复制相关等**********
template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type n) {
    size_type num_nodes = n / buffer_size() + 1;
    map_size = std::max(initial_map_size(), num_nodes + 2); // 最小为8
    map = map_allocator::allocate(map_size);
    // 将[nstart, nfinish)分配在map的中间
    map_pointer nstart = map + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;

    //　分配节点空间
    map_pointer cur;
    try {
        for (cur = nstart; cur <= nfinish; ++cur)
            *cur = allocate_node();
    }
    catch(...) {
        for(map_pointer n = nstart; n < cur; ++n)
            deallocate_node(*n);
        map_allocator::deallocate(map, map_size);
        throw;
    }

    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    finish.cur = finish.first + n % buffer_size();
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::destroy_map_and_nodes() {
    for (map_pointer cur = start.node; cur <= finish.node; ++cur)
        deallocate_node(*cur);
    map_allocator::deallocate(map, map_size);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::fill_initialize(size_type n, const value_type& value) {
    create_map_and_nodes(n);
    map_pointer cur;
    try {
        for (cur = start.node; cur < finish.node; ++cur)
            std::uninitialized_fill(*cur, *cur + buffer_size(), value);
        std::uninitialized_fill(finish.first, finish.cur, value);
    }
    catch(...) {
        for (map_pointer n = start.node; n < cur; ++n)
            destroy(*n, *n + buffer_size());
        destroy_map_and_nodes();
        throw;
    }
}

template<typename T, typename Alloc, size_t BufSiz>
template<typename InputIterator>
void deque<T, Alloc, BufSiz>::range_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
    create_map_and_nodes(0);
    for (; first != last; ++first)
        push_back(*first);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::push_back_aux(const value_type& t) {
    value_type t_copy = t;
    reserve_map_at_back();
    *(finish.node + 1) = allocate_node();
    try {
        construct(finish.cur, t_copy);
        finish.set_node(finish.node + 1);
        finish.cur = finish.first;
    }
    catch(...) {
        deallocate_node(*(finish.node + 1));
        throw;
    }
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::push_front_aux(const value_type& t) {
    value_type t_copy = t;
    reserve_map_at_front();
    *(start.node - 1) = allocate_node();
    try {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        construct(start.cur, t_copy);
    }
    catch(...) {
    start.set_node(start.node + 1);
    start.cur = start.first;
    deallocate_node(*(start.node - 1));
    throw;
    }
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::pop_back_aux() {
    deallocate_node(finish.first);
    finish.set_node(finish.node -1);
    finish.cur = finish.last - 1;
    destroy(finish.cur);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::pop_front_aux() {
    destroy(start.cur);
    deallocate_node(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator
deque<T, Alloc, BufSiz>::insert_aux(iterator pos, const value_type& x) {
    difference_type index = pos - start;
    value_type x_copy = x;
    if (index < size() / 2) {
        push_front(front());
        iterator front1 = start;
        ++front1;
        iterator front2 = front1;
        ++front2;
        pos = start + index;
        iterator pos1 = pos;
        ++pos1;
        std::copy(front2, pos1, front1);
    } else {
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + index;
        std::copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::insert_aux(iterator pos, size_type n, const value_type& x) {
    const difference_type elems_before = pos - start;
    size_type length = size();
    value_type x_copy = x;
    if (elems_before < length / 2) {
        iterator new_start = reserve_elements_at_front(n);
        iterator old_start = start;
        pos = start + elems_before;
        try {
            if (elems_before >= static_cast<difference_type>(n)) {
                iterator start_n = start + static_cast<difference_type>(n);
                std::uninitialized_copy(start, start_n, new_start);
                start = new_start;
                std::copy(start_n, pos, old_start);
                std::fill(pos - static_cast<difference_type>(n), pos, x_copy);
            } else {
                __uninititalized_copy_fill(start, pos, new_start, start, x_copy);
                start = new_start;
                std::fill(old_start, pos, x_copy);
            }
        }
        catch(...) {
            destroy_nodes_at_front(new_start);
            throw;
        }
    } else {
        iterator new_finish = reserve_elements_at_back(n);
        iterator old_finish = finish;
        const difference_type elems_after = static_cast<difference_type>(length) - elems_before;
        pos = finish - elems_after;
        try {
            if (elems_after > static_cast<difference_type>(n)) {
                iterator finish_n = finish - static_cast<difference_type>(n);
                uninitialized_copy(finish_n, finish, finish);
                finish = new_finish;
                std::copy_backward(pos, finish_n, old_finish);
                std::fill(pos, pos + static_cast<difference_type>(n), x_copy);
            } else {
                __uninititalized_copy_fill(finish, pos + static_cast<difference_type>(n),
                        x_copy, pos, finish);
                finish = new_finish;
                std::fill(pos, old_finish, x_copy);
            }
        } catch(...) {
            destroy_nodes_at_back(new_finish);
            throw;
        }
    }
}

template<typename T, typename Alloc, size_t BufSiz>
template<typename ForwardIterator>
void deque<T, Alloc, BufSiz>::insert_aux(iterator pos, ForwardIterator first
        , ForwardIterator last, size_type n) {
    const difference_type elems_before = pos - start;
    size_type length = size();
    if (elems_before < length / 2) {
        iterator new_start = reserve_elements_at_front(n);
        iterator old_start = start;
        pos = start + elems_before;
        try {
            if (elems_before >= static_cast<difference_type>(n)) {
                iterator start_n = start + static_cast<difference_type>(n);
                std::uninitialized_copy(start, start_n, new_start);
                start = new_start;
                std::copy(start_n, pos, old_start);
                std::copy(first, last, pos - static_cast<difference_type>(n));
            } else {
                ForwardIterator mid = first;
                std::advance(mid, static_cast<difference_type>(n) - elems_before);
                __uninititalized_copy_copy(start, pos, first, mid, new_start);
                start = new_start;
                std::copy(mid, last, old_start);
            }
        } catch(...) {
            destroy_nodes_at_front(new_start);
            throw;
        }
    } else {
        iterator new_finish = reserve_elements_at_back(n);
        iterator old_finish = finish;
        const difference_type elems_after = static_cast<difference_type>(length) - elems_before;
        pos = finish - elems_after;
        try {
            if (elems_after > static_cast<difference_type>(n)) {
                iterator finish_n = finish - static_cast<difference_type>(n);
                std::uninitialized_copy(finish_n, finish, finish);
                finish = new_finish;
                std::copy_backward(pos, finish_n, old_finish);
                std::copy(first, last, pos);
            } else {
                ForwardIterator mid = first;
                std::advance(mid, elems_after);
                __uninititalized_copy_copy(mid, last, pos, finish, finish);
                finish = new_finish;
                std::copy(first, mid, pos);
            }
        } catch(...) {
            destroy_nodes_at_back(new_finish);
            throw;
        }
    }
}

// 在deque前端分配新结点
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements)
{
    size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
    reserve_map_at_front(new_nodes);
    size_type i;
    try {
        for (i = 1; i <= new_nodes; ++i)
           *(start.node - i) = allocate_node();
    }
    catch(...) {
        for (size_type j = 1; j < i; ++j)
            deallocate_node(*(start.node - j));
        throw;
    }
}

// 在deque末尾分配新结点
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {
    size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
    reserve_map_at_back(new_nodes);
    size_type i;
    try {
        for (i = 1; i <= new_nodes; ++i)
            *(finish.node + i) = allocate_node();
    }
    catch(...) {
        for (size_type j = 1; j < i; ++j)
            deallocate_node(*(finish.node + j));
        throw;
    }
}

// 释放[before_start.node, start.node)的结点
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start)
{
    for (map_pointer n = before_start.node; n < start.node; ++n)
        deallocate_node(*n);
}

// 释放(finish.node, after_finish.node]的结点
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish)
{
    for (map_pointer n = after_finish.node; n > finish.node; --n)
        deallocate_node(*n);
}

// 重新配置map, 不会对缓冲区进行操作, map维护的是指向缓冲区的指针
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add
        ,bool add_at_front)
{
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;
    map_pointer new_nstart;
    if (map_size > 2 * new_num_nodes) {
        new_nstart = map + (map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < start.node)
        std::copy(start.node, finish.node + 1, new_nstart);
    else
        std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
    }
    else {
        size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
        std::copy(start.node, finish.node + 1, new_nstart);
        map_allocator::deallocate(map, map_size);
        map = new_map;
        map_size = new_map_size;
    }
    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_num_nodes - 1);
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator
deque<T, Alloc, BufSiz>::insert(iterator position, const value_type& x) {
    if (position.cur == start.cur) {
        push_front(x);
        return start;
    } else if (position.cur == finish.cur) {
        push_back(x);
        iterator tmp = finish;
        --tmp;
        return tmp;
    } else {
        return insert_aux(position, x);
    }
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator
deque<T, Alloc, BufSiz>::insert(iterator position) {
    return insert(position, value_type());
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::insert(iterator pos, size_type n, const value_type& x) {
    if (pos.cur == start.cur) {
        iterator new_start = reserve_map_at_front(n);
        std::uninitialized_fill(new_start, start, x);
        start = new_start;
    } else if (pos.cur == finish.cur) {
        iterator new_finish = reserve_elements_at_back(n);
        std::uninitialized_fill(finish, new_finish, x);
        finish = new_finish;
    } else
        insert_aux(pos, n, x);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::insert(iterator pos, int n, const value_type& x) {
    insert(pos, static_cast<size_type>(n), x);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::insert(iterator pos, long n, const value_type& x) {
    insert(pos, static_cast<size_type>(n), x);
}

template<typename T, typename Alloc, size_t BufSiz>
template<typename InputIterator>
void deque<T, Alloc, BufSiz>::insert(iterator pos, InputIterator first, InputIterator last) {
    insert(pos, first, last, iterator_category(first));
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::resize(size_type new_size, const value_type& x) {
    const size_type len = size();
    if (new_size < len)
        erase(start + new_size, finish);
    else
        insert(finish, new_size - len, x);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::resize(size_type new_size) {
    resize(new_size, value_type());
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator
deque<T, Alloc, BufSiz>::erase(iterator pos) {
    iterator next = pos;
    ++next;
    difference_type index = pos - start;
    // 判断待擦除点前的元素个数，哪部分少就移动哪部分
    if (index < (size() >> 1)) {
        //　前面部分元素更少
        std::copy_backward(start, pos, next);
        pop_front();
    } else { // 后面部分更少
        std::copy(next, finish, pos);
        pop_back();
    }
    return start + index;
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator
deque<T, Alloc, BufSiz>::erase(iterator first, iterator last) {
    if (first == start && last == finish) {
        clear();
        return finish;
    } else {
        difference_type n = last - first;
        difference_type elems_before = first - start;
        if (elems_before < (size() - n) / 2) {
            std::copy_backward(start, first, last);
            iterator new_start = start + n;
            destroy(start, new_start);
            for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                data_allocator::deallocate(*cur, buffer_size());
            start = new_start;
        } else {
            std::copy(last, finish, first);
            iterator new_finish = finish - n;
            destroy(new_finish, finish);
            for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                data_allocator::deallocate(*cur, buffer_size());
            finish = new_finish;
        }
        return start + elems_before;
    }
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::clear() {
    //　首先析构起点到终点的所有元素，并释放相应空间
    for (map_pointer node = start.node + 1; node < finish.node; ++node) {
        destroy(*node, *node + buffer_size());
        data_allocator::deallocate(*node, buffer_size());
    }
    // 如果deque本身不为空，析构所有对象，并释放掉结尾内存
    if (start.node != finish.node) {
        destroy(start.cur, start.last);
        destroy(finish.first, finish.cur);
        data_allocator::deallocate(finish.first, buffer_size());
    }
    // 析构所有元素，但不释放空间
    else
        destroy(start, finish.cur);

    finish = start;
}

//**********非成员函数**********
template<typename T, typename Alloc, size_t BufSiz>
bool operator==(const deque<T, Alloc, BufSiz>& x, const deque<T, Alloc, BufSiz>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template<typename T, typename Alloc, size_t BufSiz>
bool operator<(const deque<T, Alloc, BufSiz>& x, const deque<T, Alloc, BufSiz>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

} // namespace mystl

#endif

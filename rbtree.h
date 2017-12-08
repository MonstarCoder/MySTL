#ifndef MYSTL_RBTREE_H_
#define MYSTL_RBTREE_H_

#include "allocator.h"
#include "iterator.h"

#include <utility> // for pair
#include <cstddef>

namespace mystl {

typedef bool _rb_tree_color_type;
const _rb_tree_color_type _rb_tree_red = false; // 红色为false
const _rb_tree_color_type _rb_tree_black = true; // 黑色为true

// 双层节点，此为第一层
struct _rb_tree_node_base {
    typedef _rb_tree_color_type color_type;
    typedef _rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x) {
        while (x->left != 0) x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x) {
        while (x->right != 0) x = x->right;
        return x;
    }
}; // struct _rb_tree_node_base

// 第二层节点
template<typename Value>
struct _rb_tree_node : public _rb_tree_node_base {
    typedef _rb_tree_node<Value>* link_type;
    Value value;
}; // struct _rb_tree_node

//双层迭代器，此为第一层
struct _rb_tree_base_iterator {
    typedef _rb_tree_node_base::base_ptr     base_ptr;
    typedef bidirectional_iterator_tag       iterator_category;
    typedef ptrdiff_t                        difference_type;

    base_ptr node;

    void increment() {
        if (node->right != 0) { // 有右子节点
            node = node->right;
            while (node->left != 0)
                node = node->left;
        } else { // 没有右子节点
            base_ptr y = node->parent;
            while (node == y->right) {
                node = y;
                y = y->parent;
            }
            if (node->right != y) // 应对特殊情况：此时node为根节点
                node = y;
        }
    }

    void decrement() {
        if (node->color == _rb_tree_red && node->parent->parent == node) // node为head节点
            node = node->right;
        else if (node->left != 0) { // 有左子节点
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        } else { // 没有左子节点
            base_ptr y = node->parent;
            while (node == y->left) {
                node = y;
                y = node->parent;
            }
            node = y;
        }
    }
}; // struct _rb_tree_base_iterator

// 第二层迭代器
template<typename Value, typename Ref, typename Ptr>
struct _rb_tree_iterator : public _rb_tree_base_iterator {
    typedef Value                                                   value_type;
    typedef Ref                                                     reference;
    typedef Ptr                                                     pointer;
    typedef _rb_tree_iterator<Value, Value&, Value*>                iterator;
    typedef _rb_tree_iterator<Value, const Value&, const Value*>    const_iterator;
    typedef _rb_tree_iterator<Value, Ref, Ptr>                      self;
    typedef _rb_tree_node<Value>*                                   link_type;

    _rb_tree_iterator() {}
    _rb_tree_iterator(link_type x) { node = x; }
    _rb_tree_iterator(const iterator& it) { node = it.node; }

    reference operator*() const { return link_type(node)->value; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() { increment(); return *this; }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--() { decrement(); return *this; }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
}; // struct _rb_tree_iterator

inline bool operator==(const _rb_tree_base_iterator& x,
        const _rb_tree_base_iterator& y) {
    return x.node == y.node;
}

inline bool operator!=(const _rb_tree_base_iterator& x,
       const _rb_tree_base_iterator& y) {
    return x.node != y.node;
}

// class rb_tree
template<typename Key, typename Value, typename KeyOfValue, typename Compare,
    typename Alloc = alloc>
class rb_tree {
protected:
    typedef void*                               void_pointer;
    typedef _rb_tree_node_base*                 base_ptr;
    typedef _rb_tree_node<Value>                rb_tree_node;
    typedef _rb_tree_color_type                 color_type;

    typedef allocator<rb_tree_node, Alloc>      rb_tree_node_allocator;
public:
    typedef Key                                 key_type;
    typedef Value                               value_type;
    typedef value_type*                         pointer;
    typedef const value_type*                   const_pointer;
    typedef value_type                          reference;
    typedef const value_type&                   const_reference;
    typedef rb_tree_node*                       link_type;
    typedef size_t                              size_type;
    typedef ptrdiff_t                           difference_type;
public:
    typedef _rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef _rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;
protected:
    size_type node_count; // 节点数量
    link_type header;
    Compare key_compare; // 函数对象，用于比较键值大小

protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
        try {
            rb_tree_node_allocator::construct(&tmp->value, x);
        } catch(...) {
            put_node(tmp);
            throw;
        }
        return tmp;
    }

    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }

    void destroy_node(link_type p) {
        rb_tree_node_allocator::destroy(&p->value);
        put_node(p);
    }
protected:
    // 以下三个函数用于取得header成员
    link_type& root() const { return static_cast<link_type&>(header->parent); }
    link_type& leftmost() const { return static_cast<link_type>(header->left); }
    link_type& rightmost() const { return static_cast<link_type>(header->right); }
    // 以下六个函数用于取得节点x的成员
    static link_type& left(link_type x) { return static_cast<link_type>(x->left); }
    static link_type& right(link_type x) { return static_cast<link_type>(x->right); }
    static link_type& parent(link_type x) { return static_cast<link_type>(x->parent); }
    static reference value(link_type x) { return x->value; }
    static const Key& key(link_type x) { return static_cast<KeyOfValue()>(value(x)); }
    static color_type& color(link_type x) { return static_cast<color_type&>(x->color); }
    // 以下六个函数用于取得节点x的成员
    static link_type& left(base_ptr x) { return static_cast<link_type>(x->left); }
    static link_type& right(base_ptr x) { return static_cast<link_type>(x->right); }
    static link_type& parent(base_ptr x) { return static_cast<link_type>(x->parent); }
    static reference value(base_ptr x) { return static_cast<link_type>(x)->value; }
    static const Key& key(base_ptr x) { return static_cast<KeyOfValue()>(value(x)); }
    static color_type& color(base_ptr x) { return static_cast<color_type&>(x->color); }

    static link_type minimum(link_type x) {
        return static_cast<link_type>(_rb_tree_node_base::minimum(x));
    }
    static link_type maximum(link_type x) {
        return static_cast<link_type>(_rb_tree_node_base::maximum(x));
    }
private:
    iterator _insert(base_ptr x, base_ptr y, const value_type& v);
    link_type _copy(link_type x, link_type y);
    void _erase(link_type x);
    void init() {
        header = get_node();
        color(header) = _rb_tree_red;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }
public:
    // 构造，析构相关
    rb_tree(const Compare& comp = Compare())
        : node_count(0), key_compare(comp) { init(); }
    rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x)
        : node_count(0), key_compare(x.key_compare) {
        header = get_node();
        color(header) = _rb_tree_red;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header;
            rightmost() = header;
        } else {
            try {
                root() = _copy(x.root(), header);
            }
            catch(...) {
                put_node(header);
                throw;
            }
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
        node_count = x.node_count;
    }

    ~rb_tree() {
        clear();
        put_node(header);
    }

    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
        operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);
public:
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    const_iterator begin() const { return leftmost(); }
    iterator end() { return header; }
    const_iterator end() const { return header; }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

    void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
        std::swap(header, t.header);
        std::swap(node_count, t.node_count);
        std::swap(key_compare, t.key_compare);
    }
public:
    // 独一无二的插入
    std::pair<iterator, bool> insert_unique(const value_type& x);
    // 可重复的插入
    iterator insert_equal(const value_type& x);

    iterator insert_unique(iterator positin, const value_type& x);
    iterator insert_equal(iterator position, const value_type& x);

    template<typename InputIterator>
    void insert_unique(InputIterator first, InputIterator last);
    template<typename InputIterator>
    void insert_equal(InputIterator first, InputIterator last);

    void erase(iterator positin);
    size_type erase(const key_type& x);
    void erase(iterator first, iterator last);
    void erase(const key_type* first, const key_type* last);
    void clear() {
        if (node_count != 0) {
            _erase(root());
            leftmost() = header;
            root() = 0;
            rightmost() = header;
            node_count = 0;
        }
    }
public:
    // set的各种操作
    iterator find(const key_type& x);
    const_iterator find(const key_type& x) const;
    size_type count(const key_type& x) const;
    iterator lower_bound(const key_type& x);
    const_iterator lower_bound(const key_type& x) const;
    iterator upper_bound(const key_type& x);
    const_iterator upper_bound(const key_type& x) const;
    std::pair<iterator, iterator> equal_range(const key_type& x);
    std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const;
}; // class rb_tree

//以下为全局函数：
//_rb_tree_rotate_left(), _rb_tree_rotate_right(),
//_rb_tree_rebalance(), _rb_tree_rebalance_for_erase()
inline void _rb_tree_rotate_left(_rb_tree_node* x, _rb_tree_node*& root) {
    // x为旋转点
    _rb_tree_node_base* y = x->right;
    x->right = y->left;
    if (y->left != 0)
        y->left->parent = x;
    y->parent = x->parent;
    //TODO
}

} // namespace mystl

#endif

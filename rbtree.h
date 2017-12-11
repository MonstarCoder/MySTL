#ifndef MYSTL_RBTREE_H_
#define MYSTL_RBTREE_H_

#include "allocator.h"
#include "iterator.h"

#include <utility> // for pair
#include <cstddef>
#include <algorithm>

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
public:
    bool _rb_verify() const; // for debugging
}; // class rb_tree

//以下为全局函数：
//_rb_tree_rotate_left(), _rb_tree_rotate_right(),
//_rb_tree_rebalance(), _rb_tree_rebalance_for_erase()
inline void _rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
    // x为旋转点
    _rb_tree_node_base* y = x->right;
    x->right = y->left;
    if (y->left != 0)
        y->left->parent = x;
    y->parent = x->parent;

    if (x == root)
        root = y;
    else if ( x == x->parent->left )
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

inline void _rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
    // x为旋转点
    _rb_tree_node_base* y = x->left;
    x->left = y->right;
    if (y->right != 0)
        y->right->parent = x;
    y->parent = x->parent;

    if (x == root)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    x->right = x;
    x->parent = y;
}

// x为新增节点，重新令树形平衡（改变节点颜色及旋转树形）
inline void _rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base*& root) {
    x->color = _rb_tree_red; // 新节点一定为红
    while (x != root && x->parent->color == _rb_tree_red) { // 父节点为红
        if (x->parent == x->parent->parent->left) { //　父节点为祖父节点的左子节点
            _rb_tree_node_base* y = x->parent->parent->right;
            if (y && y->color == _rb_tree_red) {
                x->parent->color = _rb_tree_black;
                y->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                x = x->parent->parent;
            } else {
                if ( x == x->parent->right ) {
                    x = x->parent;
                    _rb_tree_rotate_left(x, root);
                }
                x->parent->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                _rb_tree_rotate_right(x->parent->parent, root);
            }
        } else { // 父节点为祖父节点的右子节点
            _rb_tree_node_base* y = x->parent->parent->left;
            if (y && y->color == _rb_tree_red) {
                x->parent->color = _rb_tree_black;
                y->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                x = x->parent->parent;
            } else {
                if (x == x->parent->left) {
                    x = x->parent;
                    _rb_tree_rotate_right(x, root);
                }
                x->parent->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                _rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = _rb_tree_black;
}

inline _rb_tree_node_base*
_rb_tree_rebalance_for_erase(_rb_tree_node_base* z,
                             _rb_tree_node_base*& root,
                             _rb_tree_node_base*& leftmost,
                             _rb_tree_node_base*& rightmost) {
    _rb_tree_node_base* y = z;
    _rb_tree_node_base* x = 0;
    _rb_tree_node_base* x_parent = 0;
    if (y->left == 0)
        x = y->right;
    else
        if (y->right == 0)
            x = y->left;
        else {
            y = y->right;
            while (y->left != 0)
                y = y->left;
            x = y->right;
        }
    if (y != z) {
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        }
        else
            x_parent = y;
        if (root == z)
            root = y;
        else if (z->parent->left == z)
            z->parent->left = y;
        else
            z->parent->right = y;
        y->parent = z->parent;
        std::swap(y->color, z->color);
        y = z;
    }
    else {                        // y == z
        x_parent = y->parent;
        if (x) x->parent = y->parent;
        if (root == z)
            root = x;
        else
            if (z->parent->left == z)
                z->parent->left = x;
            else
                z->parent->right = x;
        if (leftmost == z)
            if (z->right == 0)
                leftmost = z->parent;
            else
                leftmost = _rb_tree_node_base::minimum(x);
        if (rightmost == z)
            if (z->left == 0)
                rightmost = z->parent;
            else
                rightmost = _rb_tree_node_base::maximum(x);
    }
    if (y->color != _rb_tree_red) {
        while (x != root && (x == 0 || x->color == _rb_tree_black)) {
            if (x == x_parent->left) {
                _rb_tree_node_base* w = x_parent->right;
                if (w->color == _rb_tree_red) {
                    w->color = _rb_tree_black;
                    x_parent->color = _rb_tree_red;
                    _rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == 0 || w->left->color == _rb_tree_black) &&
                    (w->right == 0 || w->right->color == _rb_tree_black)) {
                    w->color = _rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == 0 || w->right->color == _rb_tree_black) {
                        if (w->left) w->left->color = _rb_tree_black;
                        w->color = _rb_tree_red;
                        _rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = _rb_tree_black;
                    if (w->right) w->right->color = _rb_tree_black;
                    _rb_tree_rotate_left(x_parent, root);
                    break;
                }
            } else {                  // same as above, with right <-> left.
                _rb_tree_node_base* w = x_parent->left;
                if (w->color == _rb_tree_red) {
                    w->color = _rb_tree_black;
                    x_parent->color = _rb_tree_red;
                    _rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == 0 || w->right->color == _rb_tree_black) &&
                    (w->left == 0 || w->left->color == _rb_tree_black)) {
                    w->color = _rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == 0 || w->left->color == _rb_tree_black) {
                        if (w->right) w->right->color = _rb_tree_black;
                        w->color = _rb_tree_red;
                        _rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = _rb_tree_black;
                    if (w->left) w->left->color = _rb_tree_black;
                    _rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x) x->color = _rb_tree_black;
    }
    return y;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return x.size() == y.size() && std::equal_range(x.begin(), x.end(), y.begin());
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                      const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
                 rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    x.swap(y);
}

// 以下为class rb_tree的定义
template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
    if (this != &x) {
        clear();
        node_count = 0;
        key_compare = x.key_compare;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header;
            rightmost() = header;
        }
        else {
            root() = _copy(x.root(), header);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_count = x.node_count;
        }
    }
    return *this;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
_insert(base_ptr x_, base_ptr y_, const Value& v) {
    link_type x = (link_type) x_;
    link_type y = (link_type) y_;
    link_type z;

    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
        z = create_node(v);
        left(y) = z;
        if (y == header) {
            root() = z;
            rightmost() = z;
        }
        else if (y == leftmost())
            leftmost() = z;
        else {
            z = create_node(v);
            right(y) = z;
        if (y == rightmost())
            rightmost() = z;
        }
        parent(z) = y;
        left(z) = 0;
        right(z) = 0;
        _rb_tree_rebalance(z, header->parent);
        ++node_count;
        return iterator(z);
    }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v) {
    link_type y = header;
    link_type x = root();
    while (x != 0) {
        y = x;
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
    }
    return _insert(x, y, v);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v) {
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while (x != 0) {
        y = x;
        comp = key_compare(KeyOfValue()(v), key(x));
        x = comp ? left(x) : right(x);
    }
    iterator j = iterator(y);
    if (comp)
        if (j == begin())
            return std::pair<iterator,bool>(_insert(x, y, v), true);
        else
            --j;
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return std::pair<iterator,bool>(_insert(x, y, v), true);

    return std::pair<iterator,bool>(j, false);
 }

template<typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_unique(iterator position,
                                                              const Val& v) {
    if (position.node == header->left) // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
            return _insert(position.node, position.node, v);
        else
            return insert_unique(v).first;
    else if (position.node == header) // end()
            if (key_compare(key(rightmost()), KeyOfValue()(v)))
                return _insert(0, rightmost(), v);
            else
                return insert_unique(v).first;
    else {
        iterator before = position;
        --before;
        if (key_compare(key(before.node), KeyOfValue()(v))
            && key_compare(KeyOfValue()(v), key(position.node)))
        if (right(before.node) == 0)
            return _insert(0, before.node, v);
        else
            return _insert(position.node, position.node, v);
        else
            return insert_unique(v).first;
    }
}

template<typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_equal(iterator position,
                                                             const Val& v) {
   if (position.node == header->left) // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
            return _insert(position.node, position.node, v);
        else
            return insert_equal(v);
   else if (position.node == header) // end()
            if (!key_compare(KeyOfValue()(v), key(rightmost())))
                return _insert(0, rightmost(), v);
            else
                return insert_equal(v);
   else {
        iterator before = position;
        --before;
        if (!key_compare(KeyOfValue()(v), key(before.node))
            && !key_compare(key(position.node), KeyOfValue()(v)))
            if (right(before.node) == 0)
                return _insert(0, before.node, v);
            else
                return _insert(position.node, position.node, v);
        else
            return insert_equal(v);
   }
 }

template<typename K, typename V, typename KoV, typename Cmp, typename Al>
template<typename II>
void rb_tree<K, V, KoV, Cmp, Al>::insert_equal(II first, II last) {
    for ( ; first != last; ++first)
       insert_equal(*first);
 }

template<typename K, typename V, typename KoV, typename Cmp, typename Al>
template<typename II>
void rb_tree<K, V, KoV, Cmp, Al>::insert_unique(II first, II last) {
    for ( ; first != last; ++first)
        insert_unique(*first);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline void
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
    link_type y = (link_type) _rb_tree_rebalance_for_erase(position.node,
                                                            header->parent,
                                                            header->left,
                                                            header->right);
    destroy_node(y);
    --node_count;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key& x) {
    std::pair<iterator, iterator> p = equal_range(x);
    size_type n = 0;
    distance(p.first, p.second, n);
    erase(p.first, p.second);
    return n;
}

template<typename K, typename V, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<K, V, KeyOfValue, Compare, Alloc>::link_type
rb_tree<K, V, KeyOfValue, Compare, Alloc>::_copy(link_type x, link_type p) {
    link_type top = clone_node(x);
    top->parent = p;
    try {
        if (x->right)
            top->right = _copy(right(x), top);
            p = top;
            x = left(x);

        while (x != 0) {
            link_type y = clone_node(x);
            p->left = y;
            y->parent = p;
            if (x->right)
                y->right = _copy(right(x), y);
            p = y;
            x = left(x);
        }
    }
    catch(...) {
        _erase(top);
        throw;
    }
    return top;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_erase(link_type x) {
    while (x != 0) {
        _erase(right(x));
        link_type y = left(x);
        destroy_node(x);
        x = y;
    }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first,
                                                             iterator last) {
    if (first == begin() && last == end())
        clear();
    else
        while (first != last) erase(first++);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key* first,
                                                             const Key* last) {
    while (first != last) erase(*first++);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    iterator j = iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
    link_type y = header;
    link_type x = root();
    while (x != 0) {
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    }
    const_iterator j = const_iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) const {
    std::pair<const_iterator, const_iterator> p = equal_range(k);
    size_type n = 0;
    std::distance(p.first, p.second, n);
    return n;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    return iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) const {
    link_type y = header;
    link_type x = root();
    while (x != 0)
    if (!key_compare(key(x), k))
        y = x, x = left(x);
    else
        x = right(x);
    return const_iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (key_compare(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);
    return iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) const {
    link_type y = header;
    link_type x = root();
    while (x != 0)
        if (key_compare(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);
    return const_iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
inline std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
                 typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key& k) {
    return std::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
}

template<typename Key, typename Value, typename KoV, typename Compare, typename Alloc>
inline std::pair<typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator,
                 typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator>
rb_tree<Key, Value, KoV, Compare, Alloc>::equal_range(const Key& k) const {
    return std::pair<const_iterator,const_iterator>(lower_bound(k), upper_bound(k));
}

// 计算从node至root的黑节点数量
inline int _black_count(_rb_tree_node_base* node, _rb_tree_node_base* root) {
    if (node == 0)
        return 0;
    else {
        int bc = node->color == _rb_tree_black ? 1 : 0;
        if (node == root)
            return bc;
        else
            return bc + _black_count(node->parent, root); // 累加
    }
}

// 验证这棵树是否满足rb_tree的条件
template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
bool rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_rb_verify() const {
    if (node_count == 0 || begin() == end())
        return node_count == 0 && begin() == end() &&
            header->left == header && header->right == header;

    int len = _black_count(leftmost(), root());
    for (const_iterator it = begin(); it != end(); ++it) {
        link_type x = (link_type) it.node;
        link_type L = left(x);
        link_type R = right(x);

    if (x->color == _rb_tree_red)
        if ((L && L->color == _rb_tree_red) ||
            (R && R->color == _rb_tree_red))
        return false;

    if (L && key_compare(key(x), key(L)))
        return false;
    if (R && key_compare(key(R), key(x)))
        return false;

    if (!L && !R && _black_count(x, root()) != len)
        return false;
    }

    if (leftmost() != _rb_tree_node_base::minimum(root()))
        return false;
    if (rightmost() != _rb_tree_node_base::maximum(root()))
        return false;

    return true;
}

} // namespace mystl

#endif

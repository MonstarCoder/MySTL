#ifndef MYSTL_QUEUE_H_
#define MYSTL_QUEUE_H_

#include "heap.h"
#include "deque.h"
#include "vector.h"

#include <functional> // for less

namespace mystl {

//**********queue**********
template<typename T, typename Sequence = mystl::deque<T>>
class queue {
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;

protected:
    Sequence c;

public:
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference front() { return c.front(); }
    const_reference front() const { return c.front(); }
    reference back() { return c.back(); }
    const_reference back() const { return c.back(); }
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }

    template<typename Type, typename Container>
    //　特定友元关系
    friend bool operator==(const queue<Type, Container>& x, const queue<Type, Container>& y);
    template<typename Type, typename Container>
    //　特定友元关系
    friend bool operator<(const queue<Type, Container>& x, const queue<Type, Container>& y);
}; // class queue

template<typename Type, typename Container>
bool operator==(const queue<Type, Container>& x, const queue<Type, Container>& y) {
    return x.c == y.c;
}

template<typename Type, typename Container>
bool operator<(const queue<Type, Container>& x, const queue<Type, Container>& y) {
    return x.c < y.c;
}

//**********priority_queue**********
template<typename T, typename Sequence = vector<T>,
    typename Compare = std::less<typename Sequence::value_type>>
class priority_queue {
public:
    typedef typename Sequence::value_type        value_type;
    typedef typename Sequence::size_type         size_type;
    typedef typename Sequence::reference         reference;
    typedef typename Sequence::const_reference   const_reference;

protected:
    Sequence c;
    Compare comp;

public:
    priority_queue() : c() {}

    explicit priority_queue(const Compare& x) : c(), comp(x) {}

    template<typename InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x) :
        c(first, last), comp(x) { mystl::make_heap(c.begin(), c.end(), comp); }
    template<typename InputIterator>
    priority_queue(InputIterator first, InputIterator last) :
        c(first, last) { mystl::make_heap(c.begin(), c.end(), comp); }

    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }

    const_reference top() const { return c.front(); }

    void push(const value_type& x) {
        try {
            c.push_back(x);
            mystl::push_heap(c.begin(), c.end(), comp);
        }
        catch(...) {
            c.clear();
            throw;
        }
    }

    void pop() {
        try {
            mystl::pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        }
        catch(...) {
            c.clear();
            throw;
        }
    }
}; // class priority_queue

} // namespace mystl

#endif

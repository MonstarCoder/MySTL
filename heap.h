#ifndef MYSTL_HEAP_H_
#define MYSTL_HEAP_H_

#include "iterator.h" // for distance_type, value_type

namespace mystl {

//**********push_heap**********
template<typename RandomAccessIterator, typename Distance, typename T>
void _push_heap(RandomAccessIterator first, Distance holeIndex,
        Distance topIndex, T value) {
    // 找出待处理元素的父节点
    Distance parent = (holeIndex -1) / 2;
    //如果当前待处理节点的优先级高于其父节点，则将其父节点下滤
    while (holeIndex > topIndex &&  *(first + parent) < value) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        // first开始即有元素，而非第二位置开始
        parent = (holeIndex -1) / 2;
    }

    *(first + holeIndex) = value;
}

template<typename RandomAccessIterator, typename Distance, typename T>
inline void _push_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
        Distance*, T*) {
    _push_heap(first, last, Distance((last - first) -1), Distance(0), T(*(last - 1)));
}

template<typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    _push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void _push_heap(RandomAccessIterator first, Distance holeIndex,
        Distance topIndex, T value, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && comp(*(first + parent), value)) {
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template <typename RandomAccessIterator, typename Compare, typename Distance, typename T>
inline void _push_heap_aux(RandomAccessIterator first,
        RandomAccessIterator last, Compare comp, Distance*, T*) {
    _push_heap(first, Distance((last - first) - 1), Distance(0),
        T(*(last - 1)), comp);
}

// 这个除了用户自己指定优先级决策判别式外和默认的无区别
template <typename RandomAccessIterator, typename Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last,
        Compare comp) {
    _push_heap_aux(first, last, comp, distance_type(first), value_type(first));
}

//**********pop_heap**********
template<typename RandomAccessIterator, typename Distance, typename T>
void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
        Distance len, T value) {
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2; //　洞节点的右子节点

    while (secondChild < len) {
        if (*(first + secondChild) < *(first + (secondChild - 1)))
            --secondChild; // secondChild作为较大的孩子
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * holeIndex + 2;
    }
    // 没有右子节点
    if (secondChild == len) {
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }

    _push_heap(first, holeIndex, topIndex, value); // 上滤percolate up
}

template<typename RandomAccessIterator, typename T, typename Distance>
inline void _pop_heap(RandomAccessIterator  first, RandomAccessIterator last,
        RandomAccessIterator result, T value, Distance*) {
    *result = *first;
    _adjust_heap(first, Distance(0), Distance(last - first), value);
}

template<typename RandomAccessIterator, typename T>
inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
    _pop_heap(first, last, value_type(first));
}

template<typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    _pop_heap_aux(first, last, value_type(first));
}

template<typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
        Distance len, T value, Compare comp) {
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;
    while (secondChild < len) {
        if (comp(*(first + secondChild), *(first + (secondChild -1))))
            --secondChild;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * secondChild + 2;
    }
    if (secondChild == len) {
        *(first + holeIndex) = *(first + (secondChild -1 ));
        holeIndex = secondChild - 1;
    }
    _push_heap(first, holeIndex, topIndex, value, comp);
}

template<typename RandomAccessIterator, typename T, typename Compare, typename Distance>
inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
        RandomAccessIterator result, T value, Compare comp, Distance*) {
    *result = *first;
    _adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}

template<typename RandomAccessIterator, typename T, typename Compare>
inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
        T*, Compare comp) {
    _pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp,
            distance_type(first));
}

template<typename RandomAccessIterator, typename Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last,
        Compare comp) {
    _pop_heap_aux(first, last, value_type(first), comp);
}

//**********make_heap**********
template<typename RandomAccessIterator, typename T, typename Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*,
        Distance*) {
    if (last - first < 2) return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;

    while (true) {
        _adjust_heap(first, parent, len, T(*(first + parent)));
        if (parent == 0) return;
        --parent;
    }
}

template<typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    _make_heap(first, last, value_type(first), distance_type(first));
}

template<typename RandomAccessIterator, typename Compare, typename T, typename Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last,
        Compare comp, T*, Distance*) {
    if (last - first < 2) return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;

    while (true) {
        _adjust_heap(first, parent, len, T(*(first + parent)), comp);
        if (parent == 0) return;
        --parent;
    }
}

template<typename RandomAccessIterator, typename Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last,
        Compare comp) {
    _make_heap(first, last, comp, value_type(first), distance_type(first));
}

//**********sort_heap**********
template<typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    while (last - first > 1) pop_heap(first, last--);
}

template<typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last,
        Compare comp) {
    while (last - first > 1) pop_heap(first, last--, comp);
}

} // namespace mystl

#endif

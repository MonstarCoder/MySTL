#ifndef MYSTL_ALGORITHM_H_
#define MYSTL_ALGORITHM_H_

#include "iterator.h"
#include "allocator.h"
#include "iterator.h"
#include "typetraits.h"

#include <cstring>
#include <functional>
#include <utility>

namespace mystl {
// ********** [fill] **********
// ********** [Algorithm Complexity: O(N)] **********
template<typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
    for (; first != last; ++first)
        *first = value;
}

inline void fill(char *first, char *last, const char& value)
{
    memset(first, static_cast<unsigned char>(value), last - first);
}

inline void fill(wchar_t *first, wchar_t *last, const wchar_t& value)
{
    memset(first, static_cast<unsigned char>(value), (last - first) * sizeof(wchar_t));
}

// ********** [fill_n] **********
// ********** [Algorithm Complexity: O(N)] **********
template<typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
    for (; n > 0; --n, ++first)
        *first = value;
    return first;
}

template<typename Size>
char *fill_n(char *first, Size n, const char& value)
{
    memset(first, static_cast<unsigned char>(value), n);
    return first + n;
}

template<typename Size>
wchar_t *fill_n(wchar_t *first, Size n, const wchar_t& value)
{
    memset(first, static_cast<unsigned char>(value), n * sizeof(wchar_t));
    return first + n;
}

// ********** [min] **********
// ********** [Algorithm Complexity: O(1)] **********
template <typename T> 
const T& min(const T& a, const T& b){
    return !(b < a) ? a : b;
}

template <typename T, typename Compare>
const T& min(const T& a, const T& b, Compare comp){
    return !comp(b, a) ? a : b;
}

// *********** [max] **********
// *********** [Algorithm Complexity: O(1)] **********
template <typename T> 
const T& max(const T& a, const T& b){
    return (a < b) ? b : a;
}

template <typename T, typename Compare>
const T& max(const T& a, const T& b, Compare comp){
    return (copm(a, b)) ? b : a;
}

// ********** [make_heap] **********
// ********** [Algorithm Complexity: O(N)] **********
template<typename RandomAccessIterator, typename Compare>
// heap上溯算法
static void up(RandomAccessIterator first, RandomAccessIterator last,
    // 1.[first, last], 2.headr points the header of the heapdomAccessIterator last, 
    RandomAccessIterator head, Compare comp){
    if (first != last){
        int index = last - head;
        auto parentIndex = (index - 1) / 2;
        for (auto cur = last; parentIndex >= 0 && cur != head; parentIndex = (index - 1) / 2){
            auto parent = head + parentIndex; // get parent
            if (comp(*parent, *cur))
                std::swap(*parent, *cur);
            cur = parent;
            index = cur - head;
        }
    }
}

template<typename RandomAccessIterator, typename Compare>
// heap下降算法
static void down(RandomAccessIterator first, RandomAccessIterator last, 
    // 1.[first, last], 2.headr points the header of the heap
    RandomAccessIterator head, Compare comp){
    if (first != last){
        auto index = first - head;
        auto leftChildIndex = index * 2 + 1;
        for (auto cur = first; leftChildIndex < (last - head + 1) && cur < last; leftChildIndex = index * 2 + 1){
            auto child = head + leftChildIndex; // get the left child
            if ((child + 1) <= last && *(child + 1) > *child) // cur has a right child
                child = child + 1;
            if (comp(*cur, *child))
                std::swap(*cur, *child);
            cur = child;
            index = cur - head;
        }
    }
}

template <typename RandomAccessIterator, typename Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    const auto range = last - first;
    for (auto cur = first + range / 2 - 1; cur >= first; --cur){
        mystl::down(cur, last - 1, first, comp);
        if (cur == first) return;
    }
}

template <typename RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last){
    mystl::make_heap(first, last,
        std::less<typename mystl::iterator_traits<RandomAccessIterator>::value_type>());
}

// ********** [push_heap] **********
// ********** [Algorithm Complexity: O(lgN)] **********
template <typename RandomAccessIterator>
void push_heap(RandomAccessIterator first, RandomAccessIterator last){
    mystl::push_heap(first, last,
        std::less<typename mystl::iterator_traits<RandomAccessIterator>::value_type>());
}

template <typename RandomAccessIterator, typename Compare>
void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    mystl::up(first, last - 1, first, comp);
}

// ********** [pop_heap] **********
// ********** [Algorithm Complexity: O(lgN)] **********
template <typename RandomAccessIterator>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last){
    mystl::pop_heap(first, last, 
        std::less<typename mystl::iterator_traits<RandomAccessIterator>::value_type>());
}

template <typename RandomAccessIterator, typename Compare>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    std::swap(*first, *(last - 1));
    if (last - first >= 2)
        mystl::down(first, last - 2, first, comp);
}

// ********** [sort_heap] ***********
// ********** [Algorithm Complexity: O(N)] ***********
template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last){
    return mystl::sort_heap(first, last,
        std::less<typename mystl::iterator_traits<RandomAccessIterator>::value_type>());
}

template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    for (auto cur = last; cur != first; --cur){
        mystl::pop_heap(first, cur, comp);
    }
}

// ********** [is_heap] **********
// ********** [Algorithm Complexity: O(N)] **********
template <typename RandomAccessIterator>
bool is_heap(RandomAccessIterator first, RandomAccessIterator last){
    return mystl::is_heap(first, last,
        std::less<typename mystl::iterator_traits<RandomAccessIterator>::value_type>());
}

template <typename RandomAccessIterator, typename Compare>
bool is_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
    const auto range = last - first;
    auto index = range / 2 - 1;
    for (auto cur = first + range / 2 - 1; cur >= first; --cur, --index){
        if (*(first + (index * 2 + 1)) > *cur || // left child > cur
            ((first + (index * 2 + 2)) <= last && *(first + (index * 2 + 2)) > *cur)) // right child > cur
            return false;
        if (cur == first)
            break;
    }
    return true;
}

// ********** [for_each] ***********
// ********** [Algorithm Complexity: O(N)] **********
template <typename InputIterator, typename Function>
Function for_each(InputIterator first, InputIterator last, Function fn){
    for (; first != last; ++first)
        fn(*first);
    return fn;
}

// ********** [find] **********
// ********** [Algorithm Complexity: O(N)] ***********
template <typename InputIterator, typename T>
InputIterator find(InputIterator first, InputIterator last, const T& val){
    for (; first != last; ++first){
        if (*first == val)
            break;
    }
    return first;
}

// ********** [find_if] **********
// ********** [Algorithm Complexity: O(N)] ***********
template <typename InputIterator, typename UnaryPredicate>
InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate pred){
    for (; first != last; ++first){
        if (pred(*first))
            break;
    }
    return first;
}

// ********** [count] **********
// ********** [Algorithm Complexity: O(N)] **********
template <typename InputIterator, typename T>
typename iterator_traits<InputIterator>::difference_type
    count(InputIterator first, InputIterator last, const T& val){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for (; first != last; ++first){
        if (*first == val)
            ++n;
    }
    return n;
}

// ********** [count_if] ***********
// ********** [Algorithm Complexity: O(N)] **********
template <typename InputIterator, typename UnaryPredicate>
typename iterator_traits<InputIterator>::difference_type
    count_if(InputIterator first, InputIterator last, UnaryPredicate pred){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for (; first != last; ++first){
        if (pred(*first))
            ++n;
    }
    return n;
}

// ********** [equal] **********
// ********** [Algorithm Complexity: O(N)] **********
template <typename InputIterator1, typename InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2){
    return mystl::equal(first1, last1, first2,
         std::equal_to<typename mystl::iterator_traits<InputIterator1>::value_type>());
}

template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, BinaryPredicate pred){
    for (; first1 != last1; ++first1, ++first2){
        if (!pred(*first1, *first2))
            return false;
    }
    return true;
}

// ********** [advance] ***********
// ********** [Algorithm Complexity: O(N)] **********
namespace {
    template<typename InputIterator, typename Distance>
    void _advance(InputIterator& it, Distance n, input_iterator_tag){
        assert(n >= 0);
        while (n--){
            ++it;
        }
    }
    template<typename BidirectionIterator, typename Distance>
    void _advance(BidirectionIterator& it, Distance n, bidirectional_iterator_tag){
        if (n < 0){
            while (n++){
                --it;
            }
        }else{
            while (n--){
                ++it;
            }
        }
    }
    template<typename RandomIterator, typename Distance>
    void _advance(RandomIterator& it, Distance n, random_access_iterator_tag){
        if (n < 0){
            it -= (-n);
        }else{
            it += n;
        }
    }
}

template <typename InputIterator, typename Distance> 
void advance(InputIterator& it, Distance n){
    typedef typename iterator_traits<InputIterator>::iterator_category iterator_category;
    mystl::_advance(it, n, iterator_category());
}

// ********** [sort] **********
// ********** [Algorithm Complexity: O(NlogN)] **********
namespace {
    template<typename RandomIterator, typename BinaryPredicate>
    typename iterator_traits<RandomIterator>::value_type
        mid3(RandomIterator first, RandomIterator last, BinaryPredicate pred){ // [first, last]
        auto mid = first + (last + 1 - first) / 2;
        if (pred(*mid, *first)){
            swap(*mid, *first);
        }
        if (pred(*last, *mid)){
            swap(*last, *mid);
        }
        if (pred(*last, *first)){
            swap(*last, *first);
        }
        auto ret = *mid;
        swap(*mid, *(last - 1)); // 将mid item换位作为哨兵
        return ret;
    }

    template<typename RandomIterator, typename BinaryPredicate>
    void bubble_sort(RandomIterator first, RandomIterator last, BinaryPredicate pred){
        auto len = last - first;
        for (auto i = len; i != 0; --i){
            bool swaped = false;
            for (auto p = first; p != (first + i - 1); ++p){
                if (pred(*(p + 1), *p)){
                    swap(*(p + 1), *p);
                    swaped = true;
                }
            }
            if (!swaped)
                break;
        }
    }
}

template<typename RandomIterator>
void sort(RandomIterator first, RandomIterator last){
    return sort(first, last, std::less<typename iterator_traits<RandomIterator>::value_type>());
}

template<typename RandomIterator, typename BinaryPredicate>
void sort(RandomIterator first, RandomIterator last, BinaryPredicate pred){
    if (first >= last || first + 1 == last)
        return;
    if (last - first <= 20) // 区间长度小于等于20的采用冒泡排序更快
        return bubble_sort(first, last, pred);
    auto mid = mid3(first, last - 1, pred);
    auto p1 = first, p2 = last - 2;
    while (p1 < p2){
        while (pred(*p1, mid) && (p1 < p2)) ++p1;
        while (!pred(*p2, mid) && (p1 < p2)) --p2;
        if (p1 < p2){
            swap(*p1, *p2);
        }
    }
    swap(*p1, *(last - 2)); // 将作为哨兵的mid item换回原来的位置
    sort(first, p1, pred);
    sort(p1 + 1, last, pred);
}

// ********** [distance] **********
// ********** [Algorithm Complexity: O(N)] **********
template<typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
    _distance(InputIterator first, InputIterator last, input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type dist = 0;
    while (first++ != last){
        ++dist;
    }
    return dist;
}

template<typename RandomIterator>
typename iterator_traits<RandomIterator>::difference_type
    _distance(RandomIterator first, RandomIterator last, random_access_iterator_tag){
    auto dist = last - first;
    return dist;
}

template<typename Iterator>
typename iterator_traits<Iterator>::difference_type
    distance(Iterator first, Iterator last){
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    return _distance(first, last, iterator_category());
}

// ********** [copy] **********
// ********** [Algorithm Complexity: O(N)] **********
template<typename InputIterator, typename OutputIterator>
OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, _true_type){
    auto dist = distance(first, last);
    memcpy(result, first, sizeof(*first) * dist);
    advance(result, dist);
    return result;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, _false_type){
    while (first != last){
        *result = *first;
        ++result;
        ++first;
    }
    return result;
}

template<typename InputIterator, typename OutputIterator, typename T>
OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, T*){
    typedef typename mystl::_type_traits<T>::is_POD_type is_pod;
    return __copy(first, last, result, is_pod());
}

template <typename InputIterator, typename OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result){
    return _copy(first, last, result, value_type(first));
}

template<>
inline char *copy(char *first, char *last, char *result){
    auto dist = last - first;
    memcpy(result, first, sizeof(*first) * dist);
    return result + dist;
}

template<>
inline wchar_t *copy(wchar_t *first, wchar_t *last, wchar_t *result){
    auto dist = last - first;
    memcpy(result, first, sizeof(*first) * dist);
    return result + dist;
}

} // namespace mystl

#endif

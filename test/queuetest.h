#ifndef MYSTL_QUEUE_TEST_H_
#define MYSTL_QUEUE_TEST_H_

#include "testutil.h"

#include "../queue.h"
#include <queue>

#include <cassert>
#include <string>

namespace mystl{
namespace queuetest{
    template<typename T>
    using stdQ = std::queue<T>;
    template<class T>
    using myQ = mystl::queue<T>;

    void testCase1();
    void testCase2();
    void testCase3();
    void testCase4();
    void testCase5();

    template<typename T>
    using stdPQ = std::priority_queue<T>;
    template<class T>
    using myPQ = mystl::priority_queue<T>;

    void testCase6();
    void testCase7();
    void testCase8();
    void testCase9();
    void testCase10();

    void testAllCases();

} // namespace queuetest
} // namespace mystl

#endif

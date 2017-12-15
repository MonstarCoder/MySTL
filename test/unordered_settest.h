#ifndef MYSTL_UNORDERED_SET_TEST_H_
#define MYSTL_UNORDERED_SET_TEST_H_

#include "testutil.h"
#include "../unordered_set.h"

#include <unordered_set>
#include <cassert>

namespace mystl {
namespace unordered_settest{

template<typename T>
using stdUSet = std::unordered_set<T>;
template<typename T>
using myUSet = mystl::unordered_set<T>;

void testCase1();
void testCase2();
void testCase3();
void testCase4();
void testCase5();

void testAllCases();

} // namespace unordered_settest
} // namespace mystl

#endif

#ifndef MYSTL_SET_TEST_H_
#define MYSTL_SET_TEST_H_

#include "testutil.h"
#include "../set.h"

#include <set>
#include <cassert>

namespace mystl {
namespace settest{

template<typename T>
using stdSet = std::set<T>;
template<typename T>
using mySet = mystl::set<T>;

void testCase1();
void testCase2();
void testCase3();
void testCase4();
void testCase5();

void testAllCases();

} // namespace settest
} // namespace mystl

#endif

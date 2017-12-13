#ifndef MYSTL_SET_TEST_H_
#define MYSTL_SET_TEST_H_

#include "testutil.h"
#include "../map.h"
#include "../pair.h"

#include <map>
#include <cassert>

namespace mystl {
namespace maptest{

template<typename T1, typename T2>
using stdMap = std::map<T1, T2>;
template<typename T1, typename T2>
using myMap = mystl::map<T1, T2>;

void testCase1();
void testCase2();
void testCase3();
void testCase4();
void testCase5();

void testAllCases();

} // namespace settest
} // namespace mystl

#endif

#ifndef MYSTL_UNORDERED_SET_TEST_H_
#define MYSTL_UNORDERED_SET_TEST_H_

#include "testutil.h"
#include "../unordered_map.h"
#include "../pair.h"

#include <unordered_map>
#include <cassert>

namespace mystl {
namespace unordered_maptest{

template<typename T1, typename T2>
using stdUMap = std::unordered_map<T1, T2>;
template<typename T1, typename T2>
using myUMap = mystl::unordered_map<T1, T2>;

void testCase1();
void testCase2();
void testCase3();
void testCase4();
void testCase5();

void testAllCases();

} // namespace unordered_maptest
} // namespace mystl

#endif

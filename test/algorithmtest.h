#ifndef MYSTL_ALGORITHM_TEST_H_
#define MYSTL_ALGORITHM_TEST_H_

#include "testutil.h"
#include "../algorithm.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <cstring>
#include <cctype>
#include <random>
#include <vector>
#include <list> 

namespace mystl{
namespace algorithmtest {

void testFill();
void testFillN();
void testMinMax();
void testHeapAlgorithm();
void testForEach();
void testFind();
void testCount();
void testEqual();
void testAdvance();
void testSort();
void testDistance();
void testCopy();

void testAllCases();

} // namespace algorithmtest
} // namespace mystl

#endif

#ifndef MYSTL_LIST_TEST_H_
#define MYSTL_LIST_TEST_H_

#include "testutil.h"

#include "../list.h"

#include <list>
#include <cassert>
#include <functional>
#include <string>
#include <random>

namespace mystl{
namespace listtest{

template<typename T>
using stdList = std::list<T>;
template<typename T>
using myList = mystl::list<T>;

void testCase1();
void testCase2();
void testCase3();
void testCase4();
void testCase5();
void testCase6();
void testCase7();
void testCase8();
void testCase9();
void testCase10();
void testCase11();
void testCase12();
void testCase13();
void testCase14();
void testCase15();

void testAllCases();

} // namespace listtest
} // namespace mystl

#endif

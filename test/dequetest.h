#ifndef MYSTL_DEQUE_TEST_H_
#define MYSTL_DEQUE_TEST_H_

#include "testutil.h"

#include "../deque.h"
#include <deque>

#include <cassert>
#include <string>

namespace mystl{
namespace dequetest{

	template<class T>
	using stdDq = std::deque<T> ;
	template<class T>
	using myDq = mystl::deque<T> ;

	void testCase1();
	void testCase2();
	void testCase3();
	void testCase4();
	void testCase5();
	void testCase6();

	void testAllCases();
	
} // namespace dequetest
} // namespace mystl

#endif

#include <iostream>

#include "./test/vectortest.h"
#include "./test/listtest.h"
#include "./test/dequetest.h"
//#include "./test/queuetest.h"
//#include "./test/settest.h"
//#include "./test/maptest.h"
//#include "./test/unordered_settest.h"
//#include "./test/unordered_maptest.h"
//#include "./test/stringtest.h"
//#include "./test/algorithmtest.h"

using namespace mystl;

int main() {

    mystl::vectortest::testAllCases();
    mystl::listtest::testAllCases();
    mystl::dequetest::testAllCases();
    //mystl::queuetest::testAllCases();
    //mystl::settest::testAllCases();
    //mystl::maptest::testAllCases();
    //mystl::unordered_settest::testAllCases();
    //mystl::unordered_maptest::testAllCases();
    //mystl::stringtest::testAllCases();
    //mystl::algorithmtest::testAllCases();

	return 0;
}

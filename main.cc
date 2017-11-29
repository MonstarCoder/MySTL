#include <iostream>

#include "./test/vectortest.h"
#include "./test/listtest.h"

using namespace mystl;

int main() {

	mystl::vectortest::testAllCases();
	mystl::listtest::testAllCases();

	return 0;
}

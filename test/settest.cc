#include "settest.h"

#include <algorithm>
#include <utility>

namespace mystl{
namespace settest {

template<typename Container1, typename Container2>
bool container_equal(Container1& con1, Container2& con2) {
    std::vector<typename Container1::value_type> vec1, vec2;
    for (auto& item : con1)
        vec1.push_back(item);
    for (auto& item : con2)
        vec2.push_back(item);
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());
    return vec1 == vec2;
}

void testCase1() {
    stdSet<int> st1{1, 2, 3, 4, 5};
    mySet<int> st2(std::begin(st1), std::end(st1));
    assert(container_equal(st1, st2));

    int ia[5] = { 1, 2, 3, 4, 5 };
    stdSet<int> st3(std::begin(ia), std::end(ia));
    mySet<int> st4(std::begin(ia), std::end(ia));
    assert(container_equal(st3, st4));
}

void testCase2() {

}

void testCase3() {

}

void testCase4() {

}

void testCase5() {

}

void testAllCases() {
    testCase1();
    testCase2();
    testCase3();
    testCase4();
    testCase5();
}

} // namespace settest
} // namespace mystl

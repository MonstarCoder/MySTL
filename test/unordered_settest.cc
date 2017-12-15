#include "unordered_settest.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace mystl{
namespace unordered_settest {

template<typename Container1, typename Container2>
bool container_equal(Container1& con1, Container2& con2) {
    std::vector<typename Container1::value_type> vec1, vec2;
    for (auto it = con1.begin(); it != con1.end(); ++it)
        vec1.push_back(*it);
    for (auto it = con2.begin(); it != con2.end(); ++it)
        vec2.push_back(*it);
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());
    return vec1 == vec2;
}

void testCase1() {
    stdUSet<int> ust1(1);
    myUSet<int> ust2(1);
    assert(container_equal(ust1, ust2));

    int ia[5] = { 1, 2, 3, 4, 5 };
    stdUSet<int> ust3(std::begin(ia), std::end(ia));
    myUSet<int> ust4(std::begin(ia), std::end(ia));
    assert(container_equal(ust3, ust4));
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

} // namespace unordered_settest
} // namespace mystl

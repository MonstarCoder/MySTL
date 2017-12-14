#include "maptest.h"
#include "../pair.h"

#include <algorithm>
#include <string>

namespace mystl{
namespace maptest {

template<typename Container1, typename Container2>
bool container_equal(Container1& con1, Container2& con2) {
    auto it1 = con1.begin(), it2 = con2.begin();
    for (; it1 != con1.end() && it2 != con2.end(); ++it1, ++it2) {
        if ((*it1).first != (*it2).first || (*it1).second != (*it2).second)
            return false;
    }
    if (it1 == con1.end() && it2 == con2.end())
        return true;
    /*
    std::vector<typename Container1::value_type> vec1 vec2;
    for (auto& item : con1)
        vec1.push_back(item);
    for (auto& item : con2)
        vec2.push_back(item);
    //std::sort(vec1.begin(), vec1.end());
    //std::sort(vec2.begin(), vec2.end());
    return vec1 == vec2;
    */
}

void testCase1() {
    stdMap<int, std::string> map1;
    map1.insert({1, "marvin"});
    myMap<int, std::string> map2;
    map2.insert(pair<int, std::string>(1, "marvin"));
    assert(container_equal(map1, map2));
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

#include "algorithmtest.h"

namespace mystl {
namespace algorithmtest {
void testFill(){
    std::vector<int> v1(8), v2(8);
    std::fill(v1.begin(), v1.begin() + 4, 5);   //5 5 5 5 0 0 0 0
    std::fill(v1.begin() + 3, v1.end() - 2, 8);   //5 5 5 8 8 8 0 0
    mystl::fill(v2.begin(), v2.begin() + 4, 5);   //5 5 5 5 0 0 0 0
    mystl::fill(v2.begin() + 3, v2.end() - 2, 8);   //5 5 5 8 8 8 0 0

    assert(mystl::test::container_equal(v1, v2));
}

void testFillN(){
    std::vector<int> v1(8, 10), v2(8, 10);
    std::fill_n(v1.begin(), 4, 20);     //20 20 20 20 10 10 10 10
    std::fill_n(v1.begin() + 3, 3, 33);   //20 20 20 33 33 33 10 10
    mystl::fill_n(v2.begin(), 4, 20);     //20 20 20 20 10 10 10 10
    mystl::fill_n(v2.begin() + 3, 3, 33);   //20 20 20 33 33 33 10 10

    assert(mystl::test::container_equal(v1, v2));
}

void testMinMax(){
    assert(mystl::min(1, 2) == 1);
    assert(mystl::min(2, 1) == 1);
    assert(mystl::min('a', 'z') == 'a');
    assert(mystl::min(3.14, 2.72) == 2.72);

    assert(mystl::max(1, 2) == 2);
    assert(mystl::max(2, 1) == 2);
    assert(mystl::max('a', 'z') == 'z');
    assert(mystl::max(3.14, 2.73) == 3.14);
}

void testHeapAlgorithm(){
    // int myints[] = { 10, 20, 30, 5, 15 };
    // std::vector<int> v1(myints, myints + 5);
    // std::vector<int> v2(myints, myints + 5);

    // std::make_heap(v1.begin(), v1.end());
    // mystl::make_heap(v2.begin(), v2.end());
    // assert(mystl::test::container_equal(v1, v2));

    // std::pop_heap(v1.begin(), v1.end()); 
    // v1.pop_back();
    // mystl::pop_heap(v2.begin(), v2.end());
    // v2.pop_back();
    // assert(mystl::test::container_equal(v1, v2));

    // v1.push_back(99);
    // std::push_heap(v1.begin(), v1.end());
    // v2.push_back(99); 
    // mystl::push_heap(v2.begin(), v2.end());
    // assert(mystl::test::container_equal(v1, v2));

    // std::sort_heap(v1.begin(), v1.end());
    // mystl::sort_heap(v2.begin(), v2.end());
    // assert(mystl::test::container_equal(v1, v2));
}

void testForEach(){
    std::vector<int> myvector{ 10, 20, 30 };
    std::vector<int> temp{ 11, 21, 31 };
    mystl::for_each(myvector.begin(), myvector.end(), [&myvector](int& i){
            ++i;
    });

    assert(mystl::test::container_equal(myvector, temp));
}

void testFind(){
    std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    assert(mystl::find(v.begin(), v.end(), 5) != v.end());
    assert(mystl::find(v.begin(), v.end(), 10) == v.end());
    assert(mystl::find_if(v.begin(), v.end(), [](int i){return i < 0; }) == v.end());
}

void testCount(){
    int myints[] = { 10, 20, 30, 30, 20, 10, 10, 20 };   // 8 elements
    int mycount = mystl::count(myints, myints + 8, 10);
    assert(mycount == 3);

    mycount = mystl::count_if(myints, myints + 8, [](int i){return i % 2 == 0; });
    assert(mycount == 8);
}

void testEqual(){
    // int myints[] = { 20, 40, 60, 80, 100 };
    // std::vector<int> v(myints, myints + 5);     //20 40 60 80 100
    // assert(mystl::equal(v.begin(), v.end(), myints));

    // v[3] = 81;
    // assert(!mystl::equal(v.begin(), v.end(), myints, [](int i, int j){return i == j; }));
}

void testAdvance(){
    // std::vector<int> v;
    // std::list<int> l;
    // for (auto i = 0; i != 10; ++i){
    //     v.push_back(i);
    //     l.push_back(i);
    // }
    // auto vit = v.begin();
    // auto lit = l.begin();

    // mystl::advance(vit, 5);
    // mystl::advance(lit, 5);

    // mystl::advance(vit, -5);
    // mystl::advance(lit, -5);
    // assert(*vit == 0 && *lit == 0);
}

void testSort(){
    int arr1[1] = { 0 };
    mystl::sort(std::begin(arr1), std::end(arr1));
    assert(std::is_sorted(std::begin(arr1), std::end(arr1)));

    int arr2[2] = { 1, 0 };
    mystl::sort(std::begin(arr2), std::end(arr2));
    assert(std::is_sorted(std::begin(arr2), std::end(arr2)));

    int arr3[3] = { 2, 1, 3 };
    mystl::sort(std::begin(arr3), std::end(arr3));
    assert(std::is_sorted(std::begin(arr3), std::end(arr3)));

    int arr4[100];
    std::random_device rd;
    for (auto i = 0; i != 10; ++i){
        for (auto& n : arr4){
            n = rd() % 65536;
        }
        mystl::sort(std::begin(arr4), std::end(arr4));
        assert(std::is_sorted(std::begin(arr4), std::end(arr4)));
    }
}

void testDistance(){
    // std::list<int> l(10, 0);
    // std::vector<int> v(10, 0);

    // auto lit = l.begin();
    // mystl::advance(lit, 5);
    // auto vit = v.begin();
    // mystl::advance(vit, 5);

    // assert(mystl::distance(l.begin(), lit) == 5);
    // assert(mystl::distance(v.begin(), vit) == 5);
}

void testCopy(){
    char arr1[] = "hello", res1[6] = { 0 };
    mystl::copy(std::begin(arr1), std::end(arr1), res1);
    assert(mystl::test::container_equal(arr1, res1));

    wchar_t arr2[] = L"hello", res2[6] = { 0 };
    mystl::copy(std::begin(arr2), std::end(arr2), res2);
    assert(mystl::test::container_equal(arr2, res2));

    int arr3[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, res3[10] = { 0 };
    mystl::copy(std::begin(arr3), std::end(arr3), res3);
    assert(mystl::test::container_equal(arr3, res3));

    std::string arr4[3] = { "1", "2", "3" }, res4[3];
    mystl::copy(std::begin(arr4), std::end(arr4), res4);
    assert(mystl::test::container_equal(arr4, res4));
}

void testAllCases() {
    testFill();
    testFillN();
    testMinMax();
    testHeapAlgorithm();
    testForEach();
    testFind();
    testCount();
    testEqual();
    testAdvance();
    testSort();
    testDistance();
    testCopy();
}

} // namespace algorithmtest
} // namespace mystl

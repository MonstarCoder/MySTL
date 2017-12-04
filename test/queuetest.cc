#include "queuetest.h"

namespace mystl{
namespace queuetest{
//**********queue test**********
void testCase1() {
    stdQ<int> q1;
    myQ<int> q2;

    for (auto i = 0; i != 10; ++i){
        q1.push(i);
        q2.push(i);
    }
    for (auto i = 0; i != 10; ++i){
        assert(q1.front() == q2.front());
        q1.pop();
        q2.pop();
    }
}
/*
void testCase2(){
    myQ<int> q1;
    for (auto i = 0; i != 10; ++i)
        q1.push(i);
    auto q2(q1);
    assert(q1 == q2);
    assert(!(q1 != q2));
}
void testCase3(){
    myQ<int> q;
    assert(q.empty());
    assert(q.size() == 0);

    q.push(10);
    q.push(11);
    assert(!q.empty());
    assert(q.size() == 2);
}
void testCase4(){
    myQ<std::string> q;
    q.push("front");
    q.push("back");

    assert(q.front() == "front");
    assert(q.back() == "back");
}
void testCase5(){
    myQ<int> q1, q2;

    q1.push(1); q1.push(2); q1.push(3);
    q2.push(1); q2.push(2);

    assert(q1.size() == 3 && q2.size() == 2);
    q1.swap(q2);
    assert(q1.size() == 2 && q2.size() == 3);
    mystl::swap(q1, q2);
    assert(q1.size() == 3 && q2.size() == 2);
}
*/
//**********priority_queue test**********
void testCase6() {
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -1, -2, -3 };
    stdPQ<int> pq1(std::begin(arr), std::end(arr));
    myPQ<int> pq2(std::begin(arr), std::end(arr));

    while (!pq1.empty() && !pq2.empty()){
        assert(pq1.top() == pq2.top());
        pq1.pop();
        pq2.pop();
    }
    assert(pq1.empty() && pq2.empty());
}
/*
void testCase7(){
    myPQ<std::string> pq;
    assert(pq.empty());

    pq.push("marvin");
    assert(!pq.empty());
}
void testCase8(){
    myPQ<int> pq;
    auto i = 1;
    for (; i != 10; ++i){
        pq.push(i);
        assert(pq.size() == i);
    }
    for (i = pq.size(); i != 0; --i){
        pq.pop();
        assert(pq.size() == (i - 1));
    }
}
void testCase9(){
    stdPQ<int> pq1;
    myPQ<int> pq2;

    pq1.push(30);
    pq1.push(100);
    pq1.push(25);
    pq1.push(40);

    pq2.push(30);
    pq2.push(100);
    pq2.push(25);
    pq2.push(40);

    while (!pq1.empty() && !pq2.empty()){
        assert(pq1.top() == pq2.top());
        pq1.pop();
        pq2.pop();
    }
}
void testCase10(){
    myPQ<int> foo, bar;
    foo.push(15); foo.push(30); foo.push(10);
    bar.push(101); bar.push(202);

    assert(foo.size() == 3 && bar.size() == 2);
    foo.swap(bar);
    assert(foo.size() == 2 && bar.size() == 3);

    mystl::swap(foo, bar);
    assert(foo.size() == 3 && bar.size() == 2);
}
*/
void testAllCases(){
    testCase1();
    //testCase2();
    //testCase3();
    //testCase4();
    //testCase5();
    testCase6();
    //testCase7();
    //testCase8();
    //testCase9();
    //testCase10();
}

} // namespace queuetest
} // namespace mystl

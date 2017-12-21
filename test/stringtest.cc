#include "stringtest.h"

namespace mystl{
namespace stringtest{

void testCase1(){
    const char *ptr = "hello world";

    stdStr s1(ptr);
    myStr s2(ptr);
    assert(mystl::test::container_equal(s1, s2));

    stdStr s3(ptr, 5);
    myStr s4(ptr, 5);
    assert(mystl::test::container_equal(s3, s4));
}

void testCase2(){
    stdStr temp1("hello, world");
    myStr temp2("hello, world");

    stdStr s1(temp1);
    myStr s2(temp2);
    assert(mystl::test::container_equal(s1, s2));

    stdStr s3(std::move(s1));
    myStr s4(std::move(s2));
    assert(mystl::test::container_equal(s3, s4));

    stdStr s5(temp1, 1);
    myStr s6(temp2, 1);
    assert(mystl::test::container_equal(s5, s6));

    stdStr s7(temp1, 0, 5);
    myStr s8(temp2, 0, 5);
    assert(mystl::test::container_equal(s7, s8));
}

void testCase3(){
    stdStr t1("hello, world");
    myStr t2("hello, world");

    stdStr s1;
    s1 = 'a';
    myStr s2;
    s2 = 'a';
    assert(mystl::test::container_equal(s1, s2));

    stdStr s3;
    s3 = "hello";
    myStr s4;
    s4 = "hello";
    assert(mystl::test::container_equal(s3, s4));

    stdStr s5 = t1;
    myStr s6 = t2;
    assert(mystl::test::container_equal(s5, s6));

    stdStr s7;
    s7 = std::move(t1);
    myStr s8;
    s8 = std::move(t2);
    assert(mystl::test::container_equal(s7, s8));
}

void testCase4(){
    myStr str("test string");
    stdStr s(str.begin(), str.end());
    auto i = 0;
    for (myStr::iterator it = str.begin(); it != str.end(); ++it, ++i){
        assert(*it == s[i]);
    }
}

void testCase5(){
    myStr s;
    assert(s.size() == 0);
    assert(s.length() == 0);

    s = "hello, world";
    assert(s.size() == 12);
    assert(s.size() == 12);
}

void testCase6(){
    stdStr s1("hello, world");
    myStr s2("hello, world");

    s1.resize(5);
    s2.resize(5);
    assert(mystl::test::container_equal(s1, s2));

    s1.resize(20, 'z');
    s2.resize(20, 'z');
    assert(mystl::test::container_equal(s1, s2));

    s1.resize(6, 'a');
    s2.resize(6, 'a');
    assert(mystl::test::container_equal(s1, s2));

    s1.resize(100);
    s2.resize(100);
    assert(mystl::test::container_equal(s1, s2));
}

void testCase7(){
    myStr s;
    s.reserve(10);
    assert(s.capacity() == 10);
}

void testCase8(){
    myStr s;
    assert(s.empty());

    s = "hello, world";
    assert(!s.empty());

    s.clear();
    assert(s.empty());
}

void testCase9(){
    myStr s;
    s.resize(10);
    for (auto i = 0; i != s.size(); ++i)
        s[i] = 'a' + i;
    myStr tmp;
    tmp = "abcdefghij";
    assert(mystl::test::container_equal(s, tmp));

    s.back() = 'Z';
    s.front() = 'A';
    myStr tmp2;
    tmp2 = "AbcdefghiZ";
    assert(mystl::test::container_equal(s, tmp2));
}

void testCase10(){
    stdStr s1;
    myStr s2;
    for (auto i = 0; i != 10; ++i){
        s1.push_back('a' + i);
        s2.push_back('a' + i);
    }
    assert(mystl::test::container_equal(s1, s2));
}

void testCase11(){
    stdStr s1;
    myStr s2;

    s1.insert(s1.begin(), 'A');
    s2.insert(s2.begin(), 'A');
    assert(mystl::test::container_equal(s1, s2));

    s1.insert(s1.end(), 2, 'Z');
    s2.insert(s2.end(), 2, 'Z');
    assert(mystl::test::container_equal(s1, s2));

    size_t n = 2;
    s1.insert(2, 10, '@');
    s2.insert(2, 10, '@');
    assert(mystl::test::container_equal(s1, s2));

    s1.insert(0, "hello, world");
    s2.insert(0, "hello, world");
    assert(mystl::test::container_equal(s1, s2));

    s1.insert(s1.size() - 1, "zouxiaohang", 3);
    s2.insert(s2.size() - 1, "zouxiaohang", 3);
    assert(mystl::test::container_equal(s1, s2));

    stdStr s3;
    myStr s4;

    s3.insert(s3.begin(), s1.begin(), s1.end());
    s4.insert(s4.begin(), s2.begin(), s2.end());
    assert(mystl::test::container_equal(s3, s4));

    s3.insert(1, s1);
    s4.insert(1, s2);
    assert(mystl::test::container_equal(s3, s4));

    stdStr t1("marvin");
    myStr t2("marvin");
    s3.insert(s3.size(), t1, 7, t1.size() - 7);
    s4.insert(s4.size(), t2, 7, t2.size() - 7);
    assert(mystl::test::container_equal(s3, s4));
}

void testCase12(){
    stdStr s1;
    myStr s2;

    s1.append(stdStr("abc"));
    s2.append(myStr("abc"));
    assert(mystl::test::container_equal(s1, s2));

    s1.append(stdStr("123456789"), 1, 3);
    s2.append(myStr("123456789"), 1, 3);
    assert(mystl::test::container_equal(s1, s2));

    s1.append("hello");
    s2.append("hello");
    assert(mystl::test::container_equal(s1, s2));

    s1.append("world", 0, 5);
    s2.append("world", 0, 5);
    assert(mystl::test::container_equal(s1, s2));

    s1.append(10, 'A');
    s2.append(10, 'A');
    assert(mystl::test::container_equal(s1, s2));

    stdStr s3; s3.append(s1.begin(), s1.end());
    myStr s4; s4.append(s2.begin(), s2.end());
    assert(mystl::test::container_equal(s3, s4));
}

void testCase13(){
    stdStr s1;
    myStr s2;

    s1 += 'A';
    s2 += 'A';
    assert(mystl::test::container_equal(s1, s2));

    s1 += "hello";
    s2 += "hello";
    assert(mystl::test::container_equal(s1, s2));

    s1 += stdStr("world");
    s2 += myStr("world");
    assert(mystl::test::container_equal(s1, s2));
}

void testCase14(){
    stdStr s1("hello world");
    myStr s2("hello world");

    s1.pop_back();
    s2.pop_back();
    assert(mystl::test::container_equal(s1, s2));
}

void testCase15(){
    stdStr s1("hello world");
    myStr s2("hello world");

    s1.erase(s1.begin() + 1);
    s2.erase(s2.begin() + 1);
    assert(mystl::test::container_equal(s1, s2));

    s1.erase(2, s1.size() - 4);
    s2.erase(2, s2.size() - 4);
    assert(mystl::test::container_equal(s1, s2));

    s1.erase(s1.begin(), s1.end());
    s2.erase(s2.begin(), s2.end());
    assert(mystl::test::container_equal(s1, s2));
}

void testAllCases(){
    testCase1();
    testCase2();
    testCase3();
    testCase4();
    testCase5();
    testCase6();
    testCase7();
    testCase8();
    testCase9();
    testCase10();
    testCase11();
    testCase12();
    testCase13();
    testCase14();
    testCase15();
}

} // namespace stringtest
} // namespace mystl

#ifndef MYSTL_PROFILER_H
#define MYSTL_PROFILER_H

#include <sys/time.h> //for gettimeofday()
#include <sys/resource.h> //for getrusage()

#include <iostream>

namespace mystl {
namespace profiler {

class ProfilerInstance {
public:
    static void start(); //开始计时
    static void finish(); //结束计时
    static void print_time(); //打印用时
private:
    static struct timeval start_; //系统结构
    static struct timeval finish_; //系统结构
};


} //namespace profiler
} //namespace mystl

#endif


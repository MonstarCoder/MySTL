#ifndef MYSTL_PROFILER_H
#define MYSTL_PROFILER_H

#include <sys/time.h> //for gettimeofday()
#include <sys/resource.h> //for getrusage()
#include <stddef.h> //for size_t

#include <iostream>

namespace mystl {
namespace profiler {

class ProfilerInstance {
public:
    static void start(); //开始计时
    static void finish(); //结束计时
    static void print_time(); //打印用时
    static size_t memory(); //查询当前程序的内存使用量
private:
    static struct timeval start_; //for gettimeofday
    static struct timeval finish_; //for gettimeofday
    static struct rusage usage; //for getrusage
};


} //namespace profiler
} //namespace mystl

#endif


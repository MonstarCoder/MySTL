#include <iostream>
#include <vector>
#include <array>

#include "../vector.h"
#include "profiler.h"

int main() {
//**********mystl::vector**********
    mystl::vector<int> myvec;
    mystl::profiler::ProfilerInstance::start();
    int i = 0;
    for (; i != 100000; ++i) {
        myvec.push_back(i);
    }
    mystl::profiler::ProfilerInstance::finish();
    std::cout << "mystl::vector(100000):" << std::endl;
    mystl::profiler::ProfilerInstance::print_time();

//**********std::vector**********
    std::vector<int> stdvec;
    mystl::profiler::ProfilerInstance::start();
    i = 0;
    for (; i != 100000; ++i) {
        stdvec.push_back(i);
    }
    mystl::profiler::ProfilerInstance::finish();
    std::cout << "std::vector(100000):" << std::endl;
    mystl::profiler::ProfilerInstance::print_time();

//**********std::array**********
    std::array<int, 100000> stdarray;
    mystl::profiler::ProfilerInstance::start();
    i = 0;
    for (; i != 100000; ++i) {
        stdarray[i] = i;
    }
    mystl::profiler::ProfilerInstance::finish();
    std::cout << "std::array(100000):" << std::endl;
    mystl::profiler::ProfilerInstance::print_time();

//**********memory()**********
    std::cout << "当前最大驻留集：" << mystl::profiler::ProfilerInstance::memory()
        << " kb" << std::endl;
}

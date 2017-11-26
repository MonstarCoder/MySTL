#include "profiler.h"

namespace mystl{
namespace profiler {

struct timeval ProfilerInstance::start_;
struct timeval ProfilerInstance::finish_;

void ProfilerInstance::start() {
    gettimeofday(&start_, NULL);
}

void ProfilerInstance::finish() {
    gettimeofday(&finish_, NULL);
}

void ProfilerInstance::print_time() {
    unsigned long timer;
    timer = 1000000 * (finish_.tv_sec - start_.tv_sec) + finish_.tv_usec - start_.tv_usec;
    timer /= 1000;
    std:: cout << "used time: " << timer << "ms" << std::endl;
}

} //namespace profiler
} //namespace mystl

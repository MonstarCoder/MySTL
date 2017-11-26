#include "profiler.h"

namespace mystl{
namespace profiler {

struct timeval ProfilerInstance::start_;
struct timeval ProfilerInstance::finish_;
struct rusage ProfilerInstance::usage;

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

size_t ProfilerInstance::memory() {
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; //kb
}

} //namespace profiler
} //namespace mystl

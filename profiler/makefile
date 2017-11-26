vectorprofiler : vectorprofiler.o alloc.o profiler.o
	g++ -std=c++11 -g -o vectorprofiler vectorprofiler.o alloc.o \
		profiler.o

vectorprofiler.o : vectorprofiler.cc
	g++ -std=c++11 -g -c vectorprofiler.cc
alloc.o : ../impl/alloc.cc ../alloc.h
	g++ -std=c++11 -g -c ../impl/alloc.cc
profilerinstance.o : profiler.cc profiler.h
	g++ -std=c++11 -g -c profiler.cc

.PHONY : clean
clean :
	-rm vectorprofiler vectorprofiler.o alloc.o profiler.o


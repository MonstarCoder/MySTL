args = main.o alloc.o vectortest.o listtest.o dequetest.o queuetest.o \
	   settest.o

a.out : $(args)
	g++ -std=c++11 -g -o a.out $(args)

main.o : main.cc
	g++ -std=c++11 -g -c main.cc
alloc.o : ./impl/alloc.cc alloc.h
	g++ -std=c++11 -g -c ./impl/alloc.cc
vectortest.o : ./test/vectortest.cc ./test/vectortest.h vector.h \
	allocator.h construct.h ./test/testutil.h
	g++ -std=c++11 -g -c ./test/vectortest.cc
listtest.o : ./test/listtest.cc ./test/listtest.h list.h \
	allocator.h construct.h ./test/testutil.h
	g++ -std=c++11 -g -c ./test/listtest.cc
dequetest.o : ./test/dequetest.cc ./test/dequetest.h deque.h \
	allocator.h construct.h ./test/testutil.h
	g++ -std=c++11 -g -c ./test/dequetest.cc
queuetest.o : ./test/queuetest.cc ./test/queuetest.h queue.h heap.h \
	allocator.h construct.h ./test/testutil.h
	g++ -std=c++11 -g -c ./test/queuetest.cc
settest.o : ./test/settest.cc ./test/settest.h set.h rbtree.h \
	allocator.h construct.h ./test/testutil.h
	g++ -std=c++11 -g -c ./test/settest.cc

.PHONY : clean
clean :
	-rm  a.out $(args)

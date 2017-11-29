args = main.o alloc.o vectortest.o listtest.o

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
	
.PHONY : clean
clean :
	-rm  a.out $(args)

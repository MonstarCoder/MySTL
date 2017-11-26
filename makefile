vector = main.o alloc.o vectortest.o

vectortest : $(vector)
	g++ -std=c++11 -g -o vectortest $(vector)

main.o : main.cc
	g++ -std=c++11 -g -c main.cc
alloc.o : ./impl/alloc.cc alloc.h
	g++ -std=c++11 -g -c ./impl/alloc.cc
vectortest.o : ./test/vectortest.cc ./test/vectortest.h vector.h allocator.h construct.h
	g++ -std=c++11 -g -c ./test/vectortest.cc
	
.PHONY : clean
clean :
	-rm  vectortest $(vector)

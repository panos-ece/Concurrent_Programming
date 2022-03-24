all: fifo.o coroutines.o 
	gcc -Wall -g fifo.o coroutines.o -o all

fifo.o: fifo.c coroutines.h
	gcc -Wall -g -c fifo.c

coroutines.o: coroutines.c coroutines.h
	gcc -Wall -g -c coroutines.c

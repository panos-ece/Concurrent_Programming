all: primetest.o threads.o round_robin.o coroutines.o
	gcc -Wall -g primetest.o threads.o round_robin.o coroutines.o -o all
	
primetest.o: primetest.c threads.h round_robin.h coroutines.h
	gcc -Wall -g  -c primetest.c 

threads.o: threads.c threads.h round_robin.h coroutines.h
	gcc -Wall -g -c threads.c

round_robin.o: round_robin.c round_robin.h coroutines.h
	gcc -Wall -g -c round_robin.c

coroutines.o: coroutines.c coroutines.h
	gcc -Wall -g -c coroutines.c

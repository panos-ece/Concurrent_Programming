all: primetest.o my_sem.o
	gcc -Wall -g primetest.o my_sem.o -lpthread  -o all

primetest.o: primetest.c my_sem.h
	gcc -Wall -g -c primetest.c

my_sem.o: my_sem.c my_sem.h
	gcc -Wall -g -c my_sem.c

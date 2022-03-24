all: primetest.o mutex_error.o
	gcc -Wall -g primetest.o mutex_error.o -lpthread  -o all

primetest.o: primetest.c mutex_error.h
	gcc -Wall -g -c primetest.c

mutex_error.o: mutex_error.c mutex_error.h
	gcc -Wall -g -c mutex_error.c

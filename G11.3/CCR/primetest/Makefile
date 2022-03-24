all: primetest_ccr.o mutex_error.o 
	gcc -Wall -g primetest_ccr.o mutex_error.o -lpthread  -o all

primetest_ccr.o: primetest_ccr.c mutex_error.h ccr.h
	gcc -Wall -g -c primetest_ccr.c

mutex_error.o: mutex_error.c mutex_error.h
	gcc -Wall -g -c mutex_error.c

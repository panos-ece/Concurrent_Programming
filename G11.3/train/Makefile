all: train.o mutex_error.o
	gcc -Wall -g train.o mutex_error.o -lpthread  -o all

train.o: train.c mutex_error.h
	gcc -Wall -g -c train.c

mutex_error.o: mutex_error.c mutex_error.h
	gcc -Wall -g -c mutex_error.c

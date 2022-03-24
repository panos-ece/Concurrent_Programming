all: train_ccr.o mutex_error.o 
	gcc -Wall -g train_ccr.o mutex_error.o -lpthread  -o all

train_ccr.o: train_ccr.c mutex_error.h ccr.h
	gcc -Wall -g -c train_ccr.c

mutex_error.o: mutex_error.c mutex_error.h
	gcc -Wall -g -c mutex_error.c

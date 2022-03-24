all: bridge.o mutex_error.o
	gcc -Wall -g bridge.o mutex_error.o -lpthread  -o all

bridge.o: bridge.c mutex_error.h
	gcc -Wall -g -c bridge.c

mutex_error.o: mutex_error.c mutex_error.h
	gcc -Wall -g -c mutex_error.c

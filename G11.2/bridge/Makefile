all: bridge.o my_sem.o
	gcc -Wall -g bridge.o my_sem.o -lpthread  -o all

bridge.o: bridge.c my_sem.h
	gcc -Wall -g -c bridge.c

my_sem.o: my_sem.c my_sem.h
	gcc -Wall -g -c my_sem.c

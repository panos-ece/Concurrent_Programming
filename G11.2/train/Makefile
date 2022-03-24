all: train.o my_sem.o
	gcc -Wall -g train.o my_sem.o -lpthread  -o all

train.o: train.c my_sem.h
	gcc -Wall -g -c train.c

my_sem.o: my_sem.c my_sem.h
	gcc -Wall -g -c my_sem.c

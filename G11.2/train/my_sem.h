#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef MY_SEM
#define MY_SEM

	key_t mysem_create(int val);
	void mysem_down(int key);
	int mysem_up(int key);
	void mysem_destroy(int key); 

#endif 

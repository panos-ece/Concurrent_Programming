#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define SIZE 64

char pipe_buffer[SIZE];
int write_flag = 0;
int read_flag = 0;
int flag = 0;

void pipe_init(int size) {
    int i;
    
    for(i = 0; i < size; i++) {
        pipe_buffer[i] = '\0';
    }
}

void pipe_write(char c) {
	int check_yield;
	//printf("From input_stream on pipe_write %c\n",c);
	
	pipe_buffer[write_flag] = c;
        
	while(1) {
		if(read_flag == write_flag + 1) {
			check_yield = sched_yield();
			if(check_yield == -1) {
			fprintf(stderr,"Error in yield\n");
			exit(EXIT_FAILURE);
			}
		}
		else {
			break;
		}
	}
	write_flag++;
	//printf("bytes is in write %d\n",write_flag);
}

int pipe_read(char *c) {
	int check_yield;
	char output_stream;
	//printf("c = %c\n", *c);
	if((flag == 69) && (read_flag == write_flag - 1)) {
		return(0);
	}
	
	//printf("bytes is in read %d\n",read_flag);
	output_stream = *c;
	while(1) {
		if(write_flag == read_flag + 1) {
			check_yield = sched_yield();
			if(check_yield == -1) {
				fprintf(stderr,"Error in yield\n");
				exit(EXIT_FAILURE);
			}
		}
		else {
			break;
		}
	}
	
	read_flag++;
	//printf("From pipe_read %c \n",output_stream);
	
	printf("%c",output_stream);
	
	return(1);
}

void *write_thread(void *c) {
	char input_stream;
	while(1) {
		if((write_flag < SIZE)) {
			if(fgets(&input_stream,2,stdin) != NULL) {
				pipe_write(input_stream);
			}
			else {
                //printf("I am there\n");
				write_flag++;
				flag = 69;
				break;
			}
		}
		else {
			if(read_flag != 0) {
				write_flag = 0;
			}
		}
	}
	return(NULL);
}

void *read_thread(void *c) {
	int check_read = -1;
	
	while(1) {
		if((read_flag != 0 || write_flag != 0) && read_flag < SIZE ) {
			check_read = pipe_read(&pipe_buffer[read_flag]);
		}
		else { 
			if(write_flag != 0 ) {
				read_flag = 0; 
			}
		}
		if(check_read == 0) {
			//printf("I am in flag\n");
			flag = 666;
			break;
		}
	}
	
	return(NULL);
}

void pipe_close() {
	int i;
	for(i = 0; i < SIZE; i++) {
		pipe_buffer[i] = '\0';
	}
}


int main(int argc, char *argv[]) {
	int check_pthread1, check_pthread2;
	pthread_t thread1, thread2;
	
	pipe_init(SIZE);
	
	check_pthread1 = pthread_create(&thread1,NULL,write_thread,NULL);
	if(check_pthread1 != 0 ) {
		fprintf(stderr,"Error in creation of thread1\n");
		perror("Error :");
		exit(EXIT_FAILURE);
	}
	
	check_pthread2 = pthread_create(&thread2,NULL,read_thread,NULL);
	if(check_pthread2 != 0) {
		fprintf(stderr,"Error in creation of thread2\n");
		perror("Error :");
		exit(EXIT_FAILURE);
	}
	
	while(1) {
		if(flag == 666) {
			break;
		}
	}
	//printf("I am exiting\n");
	pipe_close();
	
    return(0);
}

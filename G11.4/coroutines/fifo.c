#include "coroutines.h"
#define SIZE 64

char buffer[SIZE];
int size = 0;

ucontext_t main_context, producer,consumer;


void buffer_init() {
	int i;
	
	for(i = 0; i < SIZE; i++) {
		buffer[i] = '\0';
	}
}

void file_read(int *input_fd) {
	ssize_t check_read;
	char c;
	
	while(1) {
		check_read = read(*input_fd,&c,1);
		if(check_read == -1) {
			fprintf(stderr,"Error in line %d\n",__LINE__);
			perror("ERROR:");
			exit(EXIT_FAILURE);
		}
		else if(check_read == 0) {
			break;
		}
		
		if(size < SIZE) {
			buffer[size++] = c;
		}
		else {
			mycoroutines_switch(&producer,&consumer,__LINE__);
			buffer[size++] = c;
		}
	}
	if(size > 0) {
		mycoroutines_switch(&producer,&consumer,__LINE__);
		
		mycoroutines_switch(&producer,&main_context,__LINE__);
		//printf("hello\n");
	}
	else {
		mycoroutines_switch(&producer,&main_context,__LINE__);
	}
}

void file_write(int *output_fd) {
	ssize_t check_write;
	char c;
	int i = 0;
	
	while(1) {
		
		if(size > 0) {
			c = buffer[i++];
			check_write = write(*output_fd,&c,1);
			if(check_write == -1) {
				fprintf(stderr,"Error in line %d\n",__LINE__);
				perror("ERROR:");
				exit(EXIT_FAILURE);
			}
			size--;
		}
		else {
			//printf("I am there\n");
			mycoroutines_switch(&consumer,&producer,__LINE__);
			//printf("I am here\n");
			i = 0;
		}
	}
}

int main(int argc, char *argv[]) {
	int input_fd, output_fd, check_close,check_system;
	char diff[10000];
	
	buffer_init();
	
	input_fd = open(argv[1],O_RDONLY,S_IRWXU);
	if(input_fd == -1) {
		fprintf(stderr,"Error in line %d\n",__LINE__);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	
	output_fd = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
	if(output_fd == -1) {
		fprintf(stderr,"Error in line %d\n",__LINE__);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	
	mycoroutines_init(&main_context,__LINE__);
	
	mycoroutines_create(&producer,&consumer,file_read,&input_fd,__LINE__);
	mycoroutines_create(&consumer,&producer,file_write,&output_fd,__LINE__);
	//printf("I am there\n");
	mycoroutines_switch(&main_context,&producer,__LINE__);
	//printf("I am here\n");
	
	check_close = close(input_fd);
	if(check_close == -1) {
		fprintf(stderr,"Error in line %d\n",__LINE__);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	
	check_close = close(output_fd);
	if(check_close == -1) {
		fprintf(stderr,"Error in line %d\n",__LINE__);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	
	mycoroutines_destroy(&producer,__LINE__);
	mycoroutines_destroy(&consumer,__LINE__);
	
	sprintf(diff,"diff %s %s",argv[1],argv[2]);
	check_system = system(diff);
	if(check_system == -1) {
		fprintf(stderr,"Error in line %d\n",__LINE__);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	
	mycoroutines_destroy(&main_context,__LINE__);
	
	return(0);
}

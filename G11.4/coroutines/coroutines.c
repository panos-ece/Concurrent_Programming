#include "coroutines.h"

void mycoroutines_init(ucontext_t *main_context, long line) {
	int check_getcontext;
	check_getcontext = getcontext(main_context);
	if(check_getcontext == -1) {
		fprintf(stderr,"Error in line %li\n",line);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
}

void mycoroutines_create(ucontext_t *new_co,ucontext_t *link_context, void(*body)(),void *arg,long line) {
	int check_getcontext;
	
	check_getcontext = getcontext(new_co);
	if(check_getcontext == -1) {
		fprintf(stderr,"Error in line %li\n",line);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	new_co->uc_stack.ss_sp = malloc(SIGSTKSZ);
	if(new_co->uc_stack.ss_sp == NULL) {
		fprintf(stderr,"Error in malloc allocation in line %li\n",line);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
	
	new_co->uc_stack.ss_size = SIGSTKSZ;
	new_co->uc_link = link_context;
	makecontext(new_co,body,1,arg);
}

void mycoroutines_switch(ucontext_t *old_co,ucontext_t *new_co,long line) {
	int check_swap;
	
	check_swap = swapcontext(old_co,new_co);
	if(check_swap == -1) {
		fprintf(stderr,"Error in line %li\n",line);
		perror("ERROR:");
		exit(EXIT_FAILURE);
	}
}

void mycoroutines_destroy(ucontext_t *co,long line) {
	
	free(co->uc_stack.ss_sp);
	co->uc_stack.ss_sp = NULL;
}

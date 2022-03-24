#include "mutex_error.h"

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr, long line) {
    int check_init;
    
    check_init = pthread_mutex_init(mutex,attr);
    if(check_init != 0) {
        fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n", line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
    }
}

void mutex_lock(pthread_mutex_t *mutex, long line) {
	int check_lock;
	
	check_lock = pthread_mutex_lock(mutex);
	if(check_lock != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n", line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

void mutex_unlock(pthread_mutex_t *mutex, long line) {
	int check_unlock;
	
	check_unlock = pthread_mutex_unlock(mutex);
	if(check_unlock != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n",line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

void mutex_destroy(pthread_mutex_t *mutex, long line) {
	int check_destoy;
	
	check_destoy = pthread_mutex_destroy(mutex);
	if(check_destoy != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n",line);
		perror(ANSI_COLOR_YELLOW "Error:" ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

void cond_init(pthread_cond_t *cond,const pthread_condattr_t *attr, long line) {
    int check_init;
    
    check_init = pthread_cond_init(cond,attr);
    if(check_init != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n",line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
    }
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex, long line) {
	int check_wait;
	
	check_wait = pthread_cond_wait(cond,mutex);
	if(check_wait != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n", line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

void cond_signal(pthread_cond_t *cond, long line) {
	int check_signal;
	
	check_signal = pthread_cond_signal(cond);
	if(check_signal != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n",line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

void cond_broadcast(pthread_cond_t *cond, long line) {
	int check_broadcast;
	
	check_broadcast = pthread_cond_broadcast(cond);
	if(check_broadcast != 0) {
		fprintf(stderr, ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n",line);
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

void cond_destroy(pthread_cond_t *cond, long line) {
	int check_destoy;
	
	check_destoy = pthread_cond_destroy(cond);
	if(check_destoy != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in line %li" ANSI_COLOR_RESET "\n",line);
		perror(ANSI_COLOR_YELLOW "Error" ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}

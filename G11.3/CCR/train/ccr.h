#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mutex_error.h"

#define CCR_DECLARE(label) \
		pthread_mutex_t mutex_##label, mutex_q_##label; \
		pthread_cond_t cond_q1_##label, cond_q2_##label; \
		int q1_counter_##label, q2_counter_##label;
		
#define CCR_INIT(label)  \
		mutex_init(&mutex_##label,NULL,__LINE__); \
		mutex_init(&mutex_q_##label,NULL,__LINE__);\
		cond_init(&cond_q1_##label,NULL,__LINE__); \
		cond_init(&cond_q2_##label,NULL,__LINE__); \
		q1_counter_##label = 0; \
		q2_counter_##label = 0; 
		
#define CCR_EXEC(label, cond, body) \
		mutex_lock(&mutex_##label,__LINE__); \
		mutex_lock(&mutex_q_##label,__LINE__); \
		/*printf("Hello from thread %li\n",pthread_self());*/ \
		while(!cond) { \
			q1_counter_##label++;\
			if(q2_counter_##label > 0) { \
				q2_counter_##label--; \
				cond_signal(&cond_q2_##label,__LINE__); \
			} \
			else { \
				mutex_unlock(&mutex_##label,__LINE__); \
			} \
			/*printf("Thread with id %li blocked\n",pthread_self());*/ \
			cond_wait(&cond_q1_##label,&mutex_q_##label,__LINE__); \
			/*printf("Thread %li waking up\n",pthread_self());*/ \
			if(q1_counter_##label > 0) { \
				q1_counter_##label--; \
				cond_signal(&cond_q1_##label,__LINE__); \
			} \
			else if(q2_counter_##label > 0) { \
				q2_counter_##label--; \
				cond_signal(&cond_q2_##label,__LINE__); \
			} \
			else { \
				mutex_unlock(&mutex_##label,__LINE__); \
			} \
			q2_counter_##label++; \
			cond_wait(&cond_q2_##label,&mutex_q_##label,__LINE__); \
		} \
		body; \
		if(q1_counter_##label > 0) { \
			q1_counter_##label--; \
			cond_signal(&cond_q1_##label,__LINE__); \
		} \
		else if(q2_counter_##label > 0) { \
			q2_counter_##label--; \
			cond_signal(&cond_q2_##label,__LINE__); \
		} \
		else { \
			mutex_unlock(&mutex_##label,__LINE__); \
			/*printf("open mutex\n"); */\
		} \
		mutex_unlock(&mutex_q_##label,__LINE__); 

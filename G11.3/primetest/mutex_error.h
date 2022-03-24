#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef MUTEX_ERROR
#define MUTEX_ERROR

	void mutex_lock(pthread_mutex_t *mutex, long line);
	void mutex_unlock(pthread_mutex_t *mutex, long line);
	void mutex_destroy(pthread_mutex_t *mutex, long line);
	void cond_wait(pthread_cond_t *cond, pthread_mutex_t * mutex, long line);
	void cond_signal(pthread_cond_t *cond, long line);
	void cond_broadcast(pthread_cond_t *cond, long line);
	void cond_destroy(pthread_cond_t *cond, long line);
	
#endif

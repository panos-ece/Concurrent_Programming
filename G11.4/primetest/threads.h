#include "round_robin.h"



void mythreads_init(long line);
int mythreads_create(ucontext_t thread,void(*body)(),void *arg,long line);
int mythreads_yield(long line);
int mythreads_join(ucontext_t thread,long line);
int mythreads_destroy(ucontext_t thread_context,int thread_id,long line);
void mythreads_sem_init(sem_t *s,int val,int id);
int mythreads_sem_down(sem_t *s,long line);
int mythreads_sem_up(sem_t *s);
void mythreads_sem_destroy(sem_t *s);

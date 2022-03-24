#include "coroutines.h"

typedef struct {
	int val;
	int id;
} sem_t;

typedef struct list {
	int state;
	int thread_id;
	int sem_id;
	int join;
	ucontext_t context_id;
	struct list *next;
	struct list *prev;
} list_t;

extern list_t *head;

extern struct sigaction act;
extern struct itimerval timer;


extern ucontext_t handler_context;

extern sigset_t signal_mask;


void list_init();
list_t *list_search();
list_t *list_insert_first(ucontext_t context_id);
void list_delete_last();
int change_state(int thread_id,int val); 
void scheduler();

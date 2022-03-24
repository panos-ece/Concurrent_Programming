#include "round_robin.h"



void list_init() {
	head = (list_t *)malloc(sizeof(list_t));
	if(head == NULL) {
		fprintf(stderr,"Error in allocation space for list\n");
		exit(EXIT_FAILURE);
	}
	head->next = head;
	head->prev = head;
}

list_t  *list_search() {
	list_t *current, *selected_thread; 
    //list_t *last_zero_state = NULL;
	
	for(current = head->next; current != head; current = current->next) {
		//if(current->state == 1) {
			selected_thread = current;
		//}
	}
	
	/*for(current = selected_thread; current != head; current = current->next) {
        if(current->state == 0) {
            last_zero_state = current;
        }
    }
	
	if(last_zero_state != NULL) {
        selected_thread->prev->next = selected_thread->next;
        selected_thread->next->prev = selected_thread->prev;
        
        selected_thread->prev = last_zero_state;
        selected_thread->next = last_zero_state->next;
        last_zero_state->next->prev = selected_thread;
        last_zero_state->next = selected_thread;
    }*/
	
	return(selected_thread);
	
}

list_t *list_insert_first(ucontext_t context_id) {
	list_t *new_node;
	
	new_node = (list_t*)malloc(sizeof(list_t));
	if(new_node == NULL) {
		fprintf(stderr,"Error while insert at the beginning of the list\n");
		exit(EXIT_FAILURE);
	}
	
	new_node->state = 1; //the beginning state for each thread is ready.
	new_node->context_id = context_id;
	
	new_node->next = head->next;
	new_node->prev = head;
	new_node->next->prev = new_node;
	new_node->prev->next = new_node;
	
	return(new_node);
}

void list_delete_last() {
	list_t *selected_thread;
	
    selected_thread  = head->prev;
	
	selected_thread->next->prev = selected_thread->prev;
	selected_thread->prev->next = selected_thread->next;
	
	free(selected_thread);
	
}

int change_state(int thread_id, int val) {
	list_t *current;
	for(current = head->next; ((current != head) && (current->thread_id != thread_id)); current = current->next) {
	}
	
	if(current != head) {
		current->state = val; // blocked state
		return(0);
	}
	else {
		return(-1);
	}
}

void scheduler() {
	int state, state1, check_sigprocmask, thread_id, sem_id;
	int sig;
	ucontext_t thread_context;
	list_t *stored_context, *ready_to_run_context, *current;
	
	while(1) {
        
		
		check_sigprocmask = sigprocmask(SIG_BLOCK,&signal_mask,NULL);
		if(check_sigprocmask != 0) {
			fprintf(stderr,"Error in blocking sigprocmask\n");
			perror("ERROR: ");
			exit(EXIT_FAILURE);
		}
		//sigwait(&signal_mask,&sig);
		printf("SCHEDULER!!\n");
		
        stored_context = list_search();
        state1 = stored_context->state;
		
        do{
			sigwait(&signal_mask,&sig);
            state = stored_context->state;
            thread_context = stored_context->context_id;
            sem_id = stored_context->sem_id;
            thread_id = stored_context->thread_id;
            
            list_delete_last();
            if(state1 == 666) {
				printf("Coroutine destroyed. Thread id is %d\n",thread_id - 1);
                mycoroutines_destroy(&thread_context,__LINE__);
            }
            else {
                stored_context = list_insert_first(thread_context);
                stored_context->thread_id = thread_id;
                stored_context->sem_id = sem_id;
                stored_context->state = state;
            }
            
            for(current = head->next; current != head; current = current->next) {
                printf("Thread_id is %d\n",current->thread_id);
            }
            
            stored_context = list_search();
            printf("Searched thread is %d\n",stored_context->thread_id);
            
            
        }while(stored_context->state == 0);
           
        ready_to_run_context = stored_context;
        printf("next thread is %d\n",ready_to_run_context->thread_id);
		timer.it_value.tv_usec = 60;
		
		/*check_sigprocmask = sigprocmask(SIG_UNBLOCK,&signal_mask,NULL);
		if(check_sigprocmask != 0) {
			fprintf(stderr,"Error in unblocking sigprocmask\n");
			perror("ERROR: ");
			exit(EXIT_FAILURE);
		}*/
		mycoroutines_switch(&handler_context,&(ready_to_run_context->context_id),__LINE__);
	}
	
}

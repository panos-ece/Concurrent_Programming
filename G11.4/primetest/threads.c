#include "threads.h"

struct sigaction act = { {0} };
struct itimerval timer = { {0} };

sigset_t signal_mask;
ucontext_t handler_context;

list_t *head;

void handler(int sig);

void mythreads_init(long line) {
	int check_timer, check_sigaction, check_set;
	list_t *result;
	ucontext_t init_context;
	
	list_init();
	mycoroutines_init(&init_context,line);
	
	check_set = sigemptyset(&signal_mask);
	if(check_set != 0) {
		fprintf(stderr,"Error in empty set\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
	check_set = sigaddset(&signal_mask,SIGALRM);
	if(check_set != 0) {
		fprintf(stderr,"Error in setting the set\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
	
	act.sa_handler = handler;
	act.sa_flags = SA_RESTART;
	
	check_sigaction = sigaction(SIGALRM,&act,NULL);
	if(check_sigaction != 0) {
		fprintf(stderr,"Error in sigaction\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
	
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 60;
	
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 60;
	
	
	result = list_insert_first(init_context);
	result->sem_id = -1;
	result->thread_id = 0;
	result->join = -1;
	
	mycoroutines_create(&handler_context,NULL,scheduler,NULL,line);
    
    printf("Init finished\n");
    
	check_timer = setitimer(ITIMER_REAL,&timer,NULL);
	if(check_timer != 0) {
		fprintf(stderr,"Error in starting the timer\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
}

int mythreads_create(ucontext_t thread,void(*body)(),void *arg,long line) {
	list_t *result, *current;
	static int counter = 1;
	
    mycoroutines_create(&thread,&handler_context,body,arg,line);
	result = list_insert_first(thread);
	result->sem_id = -1;
	result->thread_id = counter;
	result->join = -1;
	printf("\nHello from create thread %d\n",result->thread_id);
	counter++;
	
    for(current = head->next; current != head; current = current->next) {
            printf("Thread_id in create is %d\n",current->thread_id);
    }
    
	return(0);
}

int mythreads_yield(long line) {
	list_t  *stored_context;
	
	stored_context = list_search();
	printf("Running thread is %d\n",stored_context->thread_id);
	mycoroutines_switch(&(stored_context->context_id),&handler_context,line);
	
	return(0);
}

int mythreads_join(ucontext_t thread,long line) {
	list_t *current_context, *selected_thread;
	
	for(current_context = head->next; current_context != head; current_context = current_context->next) {
		if(current_context->state == 1) {
			selected_thread = current_context;
		}
	}
	selected_thread->join = 0;
	change_state(selected_thread->thread_id,0);
	
	for(current_context = head->next; current_context != head; current_context = current_context->next) {
		if(&(current_context->context_id) == &thread) {
			current_context->join = 0;
		}
	}
	
	mycoroutines_switch(&(selected_thread->context_id),&handler_context,line);
	
	
	return(0);
}

int mythreads_destroy(ucontext_t thread_context,int thread_id,long line) {
	int result;
	result = change_state(thread_id,666);
	if(result != -1) {
		mycoroutines_switch(&thread_context,&handler_context,line);
	}
	else {
		fprintf(stderr,"Unable to locate and change the state of the thread\n");
		fprintf(stderr,"I am returing from destroy function...\n");
	}
	
	//mycoroutines_destroy(thread,line);
	
	return(0);
	
}

void mythreads_sem_init(sem_t *s,int val,int id) {
	s->val = val;
	s->id = id;
}

int mythreads_sem_down(sem_t *s,long line) {
	list_t *running_context;
	int sig;
	int check_sigprocmask; 
    
    printf("DOWN!\n");
	
	check_sigprocmask = sigprocmask(SIG_BLOCK,&signal_mask,NULL);
	if(check_sigprocmask != 0) {
		fprintf(stderr,"Error in blocking sigprocmask in down\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
	sigwait(&signal_mask,&sig);
	timer.it_value.tv_usec = 60;
	s->val--;
	if(s->val < 0) {
		running_context = list_search();
        
        printf("thread in down is %d\n", running_context->thread_id);
        printf("State is %d\n",running_context->state);
        
		change_state(running_context->thread_id,0);
		running_context->sem_id = s->id;
        
        printf("thread sem_id is %d and state is %d\n",running_context->sem_id,running_context->state);
        
		mycoroutines_switch(&(running_context->context_id),&handler_context,line);
	}
	//printf("SIG_UNBLOCK\n");
	/*check_sigprocmask = sigprocmask(SIG_UNBLOCK,&signal_mask,NULL);
	if(check_sigprocmask != 0) {
		fprintf(stderr,"Error in unblocking sigprocmask in down\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}*/
	//printf("Signals taken\n");
	return(0);
}

int mythreads_sem_up(sem_t *s) {
	list_t *current;
	int check_sigprocmask;
	int sig; 
    
    printf("UP!!\n");
	
	check_sigprocmask = sigprocmask(SIG_BLOCK,&signal_mask,NULL);
	if(check_sigprocmask != 0) {
		fprintf(stderr,"Error in blocking sigprocmask in up\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
	sigwait(&signal_mask,&sig);
	//timer.it_value.tv_usec = 60;
	printf("value and id for up sem is %d and %d\n",s->val,s->id);
	s->val++;
	if(s->val <= 0) {
		for(current = head->next; current != head; current = current->next) {
            //printf("current->sem_id is %d and current->state is %d\n",current->sem_id,current->state);
			if((current->state == 0) && (current->sem_id  == s->id)) {
                printf("Thread for up is %d\n",current->thread_id);
				change_state(current->thread_id,1);
				current->sem_id = -1;
                printf("threads state is %d and sem_id is %d\n",current->state,current->sem_id);
                break;
			}
		}
	}
	
	/*check_sigprocmask = sigprocmask(SIG_UNBLOCK,&signal_mask,NULL);
	if(check_sigprocmask != 0) {
		fprintf(stderr,"Error in unblocking sigprocmask in up\n");
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}*/
	
	return(0);
}

void mythreads_sem_destroy(sem_t *s) {
	s = NULL;
}



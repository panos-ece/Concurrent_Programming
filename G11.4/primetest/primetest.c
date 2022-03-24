/*Program that checks if a number is prime or not, using semaphores for concurrency.
 * Kofopoulos - Lymperis Stamatis 2548
 * Pavlidis Panagiotis 2608 
 */
#include "threads.h"

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"


int numbers, num_threads;
int num_workers;
int wait_workers = 0;
int wait_main = 0;
int finished_numbers = 0;

sem_t sem_main;
sem_t sem_workers;
sem_t sem_mutex;

struct number_data {
	int numb;
	int flag;
	int prime;
	int counter;
};

typedef struct number_data number_dataT;


number_dataT *number_array;
number_dataT *workers_array;

ucontext_t *contexts;

extern void handler(int sig) {
    
    printf("\nHi from handler!!\n");
	handler_context.uc_sigmask = signal_mask;
    printf("Hello before switch\n");
    sigprocmask(SIG_BLOCK,&(handler_context.uc_sigmask),NULL);
	
	mythreads_yield(__LINE__);
	printf("GOD HELP ME\n");
}

int primetest(int v) {
	int i;

	for(i = 1; i <= v/2; i++) {
		if((i != 1) && (i != v) && (v % i == 0)) {
			return(0);
		}
	}

	if(v == 0) {
		return(0);
	}
	//if prime returns 1.
	return(1);
}

void primesearch(void *v) {
	int i;
	int num_counter;
	number_dataT *check_number;
	//pthread_t thread;
	int check_up;
	
	//thread = pthread_self();
	
	check_number = (number_dataT *) v;

	printf("Hello from worker from check_number number: %d  %d\n", check_number->counter,
		workers_array[check_number->counter].counter);
	check_number->flag = 0;
	
	//Creates and blocks all workers. Lasr worker unblocks main.
	mythreads_sem_down(&sem_mutex,__LINE__);
	num_workers++;
	if(num_workers == num_threads) {
		printf("All workers are created and wait for assignments.\n");
		if(wait_main != 0) {
			wait_main--;
			check_up = mythreads_sem_up(&sem_main);
			if(check_up != 0) {
				printf(ANSI_COLOR_YELLOW "Unused up call when create all workers." ANSI_COLOR_RESET "\n");
			}
		}
	}
	
	wait_workers++;
	check_up = mythreads_sem_up(&sem_mutex);
	if(check_up != 0) {
		printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex after create all workers." ANSI_COLOR_RESET "\n");
	}
	printf("Worker thread blocked\n");
	mythreads_sem_down(&sem_workers,__LINE__);
    
    printf("Hello  worker from check_number number: %d  %d\n", check_number->counter,
		workers_array[check_number->counter].counter);
    
	while(1) {

		//If worker is waked and his flag has changed.
		if(check_number->flag != 0) {

			//Termination of worker.
			if(check_number->flag == 666) {
				mythreads_sem_down(&sem_mutex,__LINE__);
				printf("Terminating... thread: %d\n", check_number->counter);
				wait_workers--;
				check_up = mythreads_sem_up(&sem_mutex);
				if(check_up != 0) {
					printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in flag 666." ANSI_COLOR_RESET "\n");
				}
				if(wait_workers == 0) {
					check_up = mythreads_sem_up(&sem_main);
					if(check_up != 0) {
						printf(ANSI_COLOR_YELLOW "Unused up call while going to terminate main." ANSI_COLOR_RESET "\n");
					}
				}							//Number notification for main
				mythreads_destroy(contexts[check_number->counter + 1],check_number->counter + 1,__LINE__);
			}
			
			printf("The number for checking is: %d, for thread: %d\n", check_number->numb, check_number->counter);

			/*We find the pos of the number which is to be examined in the number_array.*/
			i = 0;
			while(i<numbers){
				if((number_array[i].numb == check_number->numb) && (number_array[i].prime == -1)
						&& (number_array[i].counter == check_number->counter)){
					num_counter = i;
					break;
				}
				i++;
			}
				
			printf("array number is %d and check_number is %d from thread %d\n",number_array[num_counter].numb,
				   check_number->numb, check_number->counter);
			
			number_array[num_counter].prime = primetest(check_number->numb);
			
		}

		//If workers flag is 0 when he is waked. Wrong worker wakes and sleeps again.
		else {
			mythreads_sem_down(&sem_mutex,__LINE__);
			printf("check_number is %d with id %d\n",check_number->numb,check_number->counter);
			wait_workers--;

			check_up = mythreads_sem_up(&sem_workers);
			if(check_up != 0) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_workers while wrong wake up." ANSI_COLOR_RESET "\n");
			}
			wait_workers++;
			check_up = mythreads_sem_up(&sem_mutex);
			if(check_up != 0) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in else." ANSI_COLOR_RESET "\n");
			}
			mythreads_sem_down(&sem_workers,__LINE__);
			//When waked again resets in order to retry the recheck if he has data to process.
			continue;
		}

		mythreads_sem_down(&sem_mutex,__LINE__);
		if(wait_main != 0) {
			printf("I am going to unblock main after checking the number with id %d\n",check_number->counter);
			wait_main--;
			check_up = mythreads_sem_up(&sem_main);
			if(check_up != 0) {
			 	printf(ANSI_COLOR_YELLOW "Unused up call in sem_main before block workers after doing their job." ANSI_COLOR_RESET "\n");
			}
		}

		wait_workers++;
		check_number->flag = 0;
		finished_numbers++;
		printf("finished numbers is %d, wait_workers is: %d and i'm going to block %d\n",finished_numbers, wait_workers, check_number->counter);
		
		//When all the numbers have been checked the last worker wakes main in order to enter finish mode.
		if((wait_workers == num_threads) && (finished_numbers == numbers)) {
			check_up = mythreads_sem_up(&sem_main);
			if(check_up != 0) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_main for printing numbers." ANSI_COLOR_RESET "\n");
			}
		}

		check_up = mythreads_sem_up(&sem_mutex);
		if(check_up != 0) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex before block workers after doing their job." ANSI_COLOR_RESET "\n");
		}
		mythreads_sem_down(&sem_workers,__LINE__);
	}
}

int main(int argc, char *argv[]) {
	int iret1,i,j;
	int check_up;
	
	num_threads = atoi(argv[1]);
	numbers = atoi(argv[2]);
	ucontext_t thread1[num_threads];
	
	contexts = thread1;
	
	mythreads_init(__LINE__);
	
	mythreads_sem_init(&sem_main,0,0);
	mythreads_sem_init(&sem_workers,0,1);
	mythreads_sem_init(&sem_mutex,1,2);

	//creates the array of structs in which we save the data
	number_array = (number_dataT *)malloc(numbers*sizeof(number_dataT));
	if(number_array == NULL) {
		fprintf(stderr,"Error in malloc of struct\n");
		exit(EXIT_FAILURE);
	}

	//assigns in the fields of struct their initial values.
	for(i = 0; i < numbers; i++) {
		printf("Give number to check if it is prime or not: ");
		scanf(" %d",&number_array[i].numb);
		number_array[i].flag = 0;
		number_array[i].prime = -1;
	}

	//creates the array of workers
	workers_array = (number_dataT *)malloc(num_threads*sizeof(number_dataT));
	if(workers_array == NULL) {
		fprintf(stderr,"Error in malloc of struct workers\n");
		//free(number_array);
		exit(EXIT_FAILURE);
	}
	
	//Initialize the flags and threads IDs of workers.
	for(i = 0; i < num_threads; i++) {
		workers_array[i].flag = -1;
		workers_array[i].counter = i;		//ID of threads.
	}

	/* creates the workers... Each worker is appointed in an empty struct in which
	the main copies the data from the number_array.*/ 
	for(i = 0; i < num_threads; i++) {
		iret1 = mythreads_create(thread1[i],primesearch,&workers_array[i],__LINE__);
		if(iret1 != 0 ) {
			fprintf(stderr,"Error - The pthread_create returns code %d\n",iret1);
			perror("Error :");
			//free(number_array);
			//free(workers_array);
			exit(EXIT_FAILURE);
		}
	}
	
	/*Assign the job for every available worker.*/
	i = 0;
	while(i < numbers) {
		//block main.
        printf("Mutex blocked\n");
		mythreads_sem_down(&sem_mutex,__LINE__);
		if(wait_workers != num_threads) {
			wait_main++;
			check_up = mythreads_sem_up(&sem_mutex);
			if(check_up != 0) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in main" ANSI_COLOR_RESET "\n");
			}
			printf("Before block main, wait is: %d\n", wait_main);
			mythreads_sem_down(&sem_main,__LINE__);
            printf("I am there\n");
			mythreads_sem_down(&sem_mutex,__LINE__);
            printf("I am here\n");
		}
		//assign job.
		for(j = 0; j < num_threads; j++) {
			if((workers_array[j].flag == 0) && (j < numbers)) {
				if((i < numbers)) {
					workers_array[j].numb = number_array[i].numb;
					number_array[i].counter = workers_array[j].counter;
					workers_array[j].flag = -1;
					i++;
					//wake worker if needed.
					if(wait_workers != 0) {
						wait_workers--;
						check_up = mythreads_sem_up(&sem_workers);
						if(check_up != 0) {
							printf(ANSI_COLOR_YELLOW "Unused up call while waking up workers from main." ANSI_COLOR_RESET "\n");
						}
					}
				}
			}
		}
		printf("For-loop ended.\n");
		check_up = mythreads_sem_up(&sem_mutex);
		if(check_up != 0) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex after for." ANSI_COLOR_RESET "\n");
		}
	}

	printf("Main waits all workers to become available.\n");
	mythreads_sem_down(&sem_main,__LINE__);


	for(i = 0; i < numbers; i++) {
		if(number_array[i].prime == 1) {
			printf("The number %d is prime!\n",number_array[i].numb);
		}
		else if(number_array[i].prime == 0) {
			printf("The number %d is not prime!\n",number_array[i].numb);
		}
		else {
			printf("Error in number %d!\n",number_array[i].numb);
		}
	}

	/*Notify workers to terminate.*/
	for(i = 0; i < num_threads; i++) {
		workers_array[i].flag = 666;
	}

	for(i = 0; i < num_threads; i++) {
		check_up = mythreads_sem_up(&sem_workers);
		if(check_up != 0) {
			printf(ANSI_COLOR_YELLOW "Unused up call while waking up the workers to terminate." ANSI_COLOR_RESET "\n");
		}
	}
	
	mythreads_sem_down(&sem_main,__LINE__);

	printf("Main thread will terminate! :)\n");

	mythreads_sem_destroy(&sem_mutex);
	mythreads_sem_destroy(&sem_workers);
	mythreads_sem_destroy(&sem_main);
	//free(workers_array);
	//free(number_array);
	
	/*workers_array = NULL;
	nuber_array = NULL;*/
	
	return(0);
}

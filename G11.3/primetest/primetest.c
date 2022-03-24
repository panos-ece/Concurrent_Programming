/*Program that checks if a number is prime or not, using semaphores for concurrency.
 * Kofopoulos - Lymperis Stamatis 2548
 * Pavlidis Panagiotis 2608 
 */
#include "mutex_error.h"
#include <sched.h>

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t workers_cond = PTHREAD_COND_INITIALIZER;


int numbers, num_threads;
int num_workers;
int wait_workers = 0;
int wait_main = 0;
int finished_numbers = 0;

struct number_data {
	int numb;
	int flag;
	int prime;
	int counter;
};

typedef struct number_data number_dataT;


number_dataT *number_array;
number_dataT *workers_array;

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

void *primesearch(void *v) {
	int i;
	int num_counter;
	number_dataT *check_number;
	pthread_t thread;
	
	thread = pthread_self();
	
	check_number = (number_dataT *) v;

	printf("Hello from worker from check_number number: %d   %lu   %d\n", check_number->counter,
		   thread, workers_array[check_number->counter].counter);
	check_number->flag = 0;
	
	//Creates and blocks all workers. Last worker unblocks main.
	mutex_lock(&mutex,__LINE__);
	
	num_workers++;
	if(num_workers == num_threads) {
		printf("All workers are created and wait for assignments.\n");
		if(wait_main != 0) {
			wait_main--;
			cond_signal(&main_cond,__LINE__);
		}
	}
	
	wait_workers++;
	cond_wait(&workers_cond,&mutex,__LINE__);

	while(1) {
		mutex_unlock(&mutex,__LINE__);
		//If worker is waked and his flag has changed.
		if(check_number->flag != 0) {

			//Termination of worker.
			if(check_number->flag == 666) {
				mutex_lock(&mutex,__LINE__);
				printf("Terminating... thread: %d\n", check_number->counter);
				wait_workers--;
				if(wait_workers == 0) {
					cond_signal(&main_cond,__LINE__);
				}							//Number notification for main
				mutex_unlock(&mutex,__LINE__);
				return(NULL);
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
			mutex_lock(&mutex,__LINE__);
			printf("check_number is %d with id %d\n",check_number->numb,check_number->counter);
			wait_workers--;

			cond_signal(&workers_cond,__LINE__);
			
			wait_workers++;
			cond_wait(&workers_cond,&mutex,__LINE__);
			//When waked again resets in order to retry the recheck if he has data to process.
			continue;
		}
		
		mutex_lock(&mutex,__LINE__);
		if(wait_main != 0) {
			printf("I am going to unblock main after checking the number with id %d\n",check_number->counter);
			wait_main--;
			cond_signal(&main_cond,__LINE__);
		}

		wait_workers++;
		check_number->flag = 0;
		finished_numbers++;
		printf("finished numbers is %d, wait_workers is: %d and i'm going to block %d\n",finished_numbers, wait_workers, check_number->counter);
		
		//When all the numbers have been checked the last worker wakes main in order to enter finish mode.
		if((wait_workers == num_threads) && (finished_numbers == numbers)) {
			wait_main = 69;
			cond_signal(&main_cond,__LINE__);
		}

		cond_wait(&workers_cond,&mutex,__LINE__);
	}
}

int main(int argc, char *argv[]) {
	int iret1,i,j;
	
	num_threads = atoi(argv[1]);
	numbers = atoi(argv[2]);
	pthread_t thread1[num_threads];
	
	//creates the array of structs in which we save the data
	number_array = (number_dataT *)malloc(numbers*sizeof(number_dataT));
	if(number_array == NULL) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in malloc of struct" ANSI_COLOR_RESET "\n");
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
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in malloc of struct workers" ANSI_COLOR_RESET "\n");
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
		iret1 = pthread_create(&thread1[i],NULL,primesearch,&workers_array[i]);
		if(iret1 != 0 ) {
			fprintf(stderr,ANSI_COLOR_YELLOW "Error - The pthread_create returns code %d" ANSI_COLOR_RESET "\n",iret1);
			perror(ANSI_COLOR_YELLOW "Error :" ANSI_COLOR_RESET);
			//free(number_array);
			//free(workers_array);
			exit(EXIT_FAILURE);
		}
	}
	
	/*Assign the job for every available worker.*/
	i = 0;
	while(i < numbers) {
		//block main.
		mutex_lock(&mutex,__LINE__);
		if(wait_workers != num_threads) {
			wait_main++;
			
			printf("Before block main, wait is: %d\n", wait_main);
			cond_wait(&main_cond,&mutex,__LINE__);
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
						cond_signal(&workers_cond,__LINE__);
					}
				}
			}
		}
		printf("For-loop ended.\n");
		mutex_unlock(&mutex,__LINE__);
	}
	
	mutex_lock(&mutex,__LINE__);
	if(wait_main != 69) {
		printf("Main waits all workers to become available.\n");
		cond_wait(&main_cond,&mutex,__LINE__);
	}

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
	
	cond_broadcast(&workers_cond,__LINE__);
	
	
	cond_wait(&main_cond,&mutex,__LINE__);
	
	
	mutex_unlock(&mutex,__LINE__);
	printf("Main thread will terminate! :)\n");
	
	cond_destroy(&workers_cond,__LINE__);
	cond_destroy(&main_cond,__LINE__);
	mutex_destroy(&mutex,__LINE__);
	
	
	//free(workers_array);
	//free(number_array);
	
	/*workers_array = NULL;
	nuber_array = NULL;*/
	
	return(0);
}

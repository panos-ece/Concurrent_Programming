/*Program that checks if a number is prime or not, using CCR for concurrency.
 * Kofopoulos - Lymperis Stamatis 2548
 * Pavlidis Panagiotis 2608 
 */
#include "ccr.h"

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

CCR_DECLARE(R);

int numbers, num_threads;
int num_workers;
int wait_workers = 0;
int wait_main = 0;
int finished_numbers = 0;
int boolean = 0;
int flag = 0;

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
	
	//Creates and blocks all workers.
	
	CCR_EXEC(R,1,wait_workers++; 
		if(wait_workers == num_threads) {
			printf("All workers are created and wait for assignments.\n");
			boolean = 1;
		}
	);
	
	while(1) {
		//If worker is waked and his flag has changed.
		CCR_EXEC(R,check_number->flag != 0,wait_workers--;
		
            //Termination of worker.
            if(check_number->flag == 666) {
                printf("Terminating... thread: %d\n", check_number->counter); 
                if(wait_workers == 0) {
                    boolean = -1;
                } 
            }
        );
        
        if(check_number->flag == 666) {
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
		 
		CCR_EXEC(R,1,
			wait_workers++;
			check_number->flag = 0;
			finished_numbers++;
			printf("finished numbers are %d, wait_workers are: %d and i'm going to block %d\n",finished_numbers, wait_workers, check_number->counter);
			
			//When all the numbers have been checked the last worker wakes main in order to enter finish mode.
			if((wait_workers == num_threads) && (finished_numbers == numbers)) {
				wait_main = 69;
			} 
			boolean = 1;
		);
	}
}

int main(int argc, char *argv[]) {
	int iret1,i,j;
	
	num_threads = atoi(argv[1]);
	numbers = atoi(argv[2]);
	pthread_t thread1[num_threads];
	
    CCR_INIT(R);
    
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
		printf("Main thread id is %li\n",pthread_self());
		CCR_EXEC(R,((wait_workers == num_threads) && (boolean == 1)),
		//assign job.
		for(j = 0; j < num_threads; j++) {
			if((workers_array[j].flag == 0) && (j < numbers)) {
				if((i < numbers)) {
					workers_array[j].numb = number_array[i].numb;
					number_array[i].counter = workers_array[j].counter;
					workers_array[j].flag = -1;
					i++;
				}
			}
		}
		boolean = 0;
		printf("For-loop ended.\n"); );
	}
	
	CCR_EXEC(R,((wait_main == 69)),

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
	);
	
	CCR_EXEC(R,boolean == -1,printf("Main thread will terminate! :)\n"); );
		
	//free(workers_array);
	//free(number_array);
	
	/*workers_array = NULL;
	nuber_array = NULL;*/
	
	return(0);
}

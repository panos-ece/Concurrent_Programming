#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

int numbers;

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
	int test;

	thread = pthread_self();
	
	check_number = (number_dataT *) v;

	printf("Hello from worker from check_number number: %d   %lu   %d\n", check_number->counter,
		   thread, workers_array[check_number->counter].counter);

	while(1) {
		if(check_number->flag != 0) {
			if(check_number->flag == 666) {
				printf("Terminating... thread: %d\n", check_number->counter);
				check_number->flag = 69;							//Number notification for main
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

			if(number_array[num_counter].prime  == 0) {
				check_number->flag = 0;
				test = sched_yield();
				if(test!= 0 ) {
					fprintf(stderr,"Error - The sched_yield returns code %d\n",test);
					perror("Error :");
					exit(EXIT_FAILURE);
				}
				continue;
			}
			
			check_number->flag = 0;

			test = sched_yield();
			if(test!= 0 ) {
				fprintf(stderr,"Error - The sched_yield returns code %d\n",test);
				perror("Error :");
				exit(EXIT_FAILURE);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int iret1,i,j;
	int num_threads;
	int flags = 0;
	
	num_threads = atoi(argv[1]);
	numbers = atoi(argv[2]);
	pthread_t thread1[num_threads];

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
		workers_array[i].flag = 0;
		workers_array[i].counter = i;		//ID of threads.
	}

	/* creates the workers... Each worker is appointed in an empty struct in which
	the main copies the data from the number_array.*/ 
	for(i = 0; i < num_threads; i++) {
		iret1 = pthread_create(&thread1[i],NULL,primesearch,&workers_array[i]);
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
		for(j = 0; j < num_threads; j++) {
			if((workers_array[j].flag == 0) && (j < numbers)) {
				workers_array[j].numb = number_array[i].numb;
				number_array[i].counter = workers_array[j].counter;
				workers_array[j].flag = -1;
				i++;
			}
		}
	}

	/*Wait for all workers to become available in order to terminate.*/
	while(flags != num_threads) {
		flags= 0;	
		for(i = 0; i < num_threads; i++){
			if(workers_array[i].flag == 0) {
				flags++;
			}
		}
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
	
	
	/*Main is notified that all workers are terminated.*/
	flags = 0;
	while(flags != num_threads) {
		flags= 0;	
		for(i = 0; i < num_threads; i++){
			if(workers_array[i].flag == 69) {
				flags++;
			}
		}
	}

	printf("Main thread will terminate! :)\n");
	
	//free(workers_array);
	//free(number_array);
	
	/*workers_array = NULL;
	nuber_array = NULL;*/
	
	return(0);
}

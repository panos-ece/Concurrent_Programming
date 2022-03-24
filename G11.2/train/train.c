/*Program that controls the boarding and the getting of passengers in a train.
 * Kofopoulos-Lymperis Stamatis 2548
 * Pavlidis Panagiotis 2608
*/
#include "my_sem.h"
#include <pthread.h>

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int num_passengers;
int passenger_in_train;
int train_limit;
int waiting_passengers_on_train = 0;
int waiting_to_board = 0;

key_t sem_train;
key_t sem_passengers;
key_t sem_passengers_to_wait;
key_t sem_main;
key_t sem_mutex;


void train_func() {
	int i, check_up;
	
	printf("Train waiting for departure.\n");
	mysem_down(sem_train);
	printf("I am doing the route\n");
	sleep(1);
	printf("Route is finished\n");
	
	//Unblocks the boarded passengers when the route is finished, in order to leave from the train.
	if(waiting_passengers_on_train - train_limit >= 0) {
		for(i = 0; i < train_limit; i++) {
			check_up = mysem_up(sem_passengers);
			if(check_up == -1) {
				printf(ANSI_COLOR_YELLOW  "Error in up call for sem_passengers in if."  ANSI_COLOR_RESET "\n");
			}
			num_passengers--;
			printf("Num passengers is %d\n",num_passengers);
		}
	}
	
	//This else is used, if we want the passengers to enter the train if they are less than the train seat limit, in order to terminate normally the program.
	/*else {
		for(i = 0; i < waiting_passengers_on_train; i++) {
			check_up = mysem_up(sem_passengers);
			if(check_up == -1) {
				printf(ANSI_COLOR_YELLOW  "Error in up call for sem_passengers in else."  ANSI_COLOR_RESET "\n");
			}
			num_passengers--;
		}
			printf("Num passengers is %d\n",num_passengers);
	}*/
	
}

void passengers_func() {
	int check_up, i;
	int wake_passengers = 0;
	
	printf("Hello i am a passenger!\n");
	
	mysem_down(sem_mutex);
	passenger_in_train++;
	
	check_up = mysem_up(sem_mutex);
	if(check_up == -1) {
		printf(ANSI_COLOR_YELLOW  "Error in up call for sem_mutex while increase passengers"  ANSI_COLOR_RESET "\n");
	}
	//Block the passenger who are above the train seat limit.
	if(passenger_in_train > train_limit) {
		printf("I am blocked outside the train.\n");
		waiting_to_board++;
		mysem_down(sem_passengers_to_wait);
	}
	
	mysem_down(sem_mutex);
	waiting_passengers_on_train++;
	printf("I am blocked inside the train.\n");
	
	//Unblock the train in order to begin the route.
	if(waiting_passengers_on_train == train_limit) {
		check_up = mysem_up(sem_train);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW  "Error in up call for sem_train"  ANSI_COLOR_RESET "\n");
		}
	}
	
	check_up = mysem_up(sem_mutex);
	if(check_up == -1) {
		printf(ANSI_COLOR_YELLOW  "Error in up call for sem_mutex on waiting_passengers_on_train"  ANSI_COLOR_RESET "\n");
	}
	//Blocks the passengers who have boarded train.
	mysem_down(sem_passengers);
	
	printf("Passenger is leaving from the train\n");
	
	mysem_down(sem_mutex);
	
	waiting_passengers_on_train--;
	passenger_in_train--;
	
	//The last passenger that leaves the train unblocks some of the passengers who are blocked before in order to enter the train.
	if(waiting_passengers_on_train == 0) {
		if(waiting_to_board >= train_limit) {
			wake_passengers = train_limit;
			waiting_to_board = waiting_to_board - train_limit;
			for(i = 0; i < wake_passengers; i++) {
				check_up = mysem_up(sem_passengers_to_wait);
				if(check_up == -1) {
					printf(ANSI_COLOR_YELLOW  "Error in up call for sem_passengers_to_wait"  ANSI_COLOR_RESET "\n");
				}
			}
		}
		
		//This else is used, if we want the passengers to enter the train if they are less than the train seat limit, in order to terminate normally the program.
		/*else {
			wake_passengers = waiting_to_board;
			waiting_to_board = 0;
		}*/
		
	}
	
	check_up = mysem_up(sem_mutex);
	if(check_up == -1) {
		printf(ANSI_COLOR_YELLOW  "Error in up call for sem_mutex while decrease passengers"  ANSI_COLOR_RESET "\n");
	}
}

void *train(void *input) {
	
	while(1) {
		if(num_passengers != 0) {
			train_func();
		}
		if(num_passengers == 0){
			printf("I don't have any passengers here.\n");
			break;
		}
	}
	return(NULL);
}

void *passengers(void *v) {
	int check_up;
	
	passengers_func();
	
	//The last of all the passengers unblocks main.
	if((num_passengers == 0) && (passenger_in_train == 0)) {
		check_up = mysem_up(sem_main);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW  "Error in up call for sem_main."  ANSI_COLOR_RESET "\n");
		}
	}
	
	return(NULL);
}

int main(int argc, char *argv[]) {
	int check_thread, i;
	pthread_t thread_id;
	
	if(argc != 3) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Enter the numbers of passengers and the train's seat limit." ANSI_COLOR_RESET "\n");
		exit(EXIT_FAILURE);
	}
	
	num_passengers = atoi(argv[1]);
	train_limit = atoi(argv[2]);
	
	sem_train = mysem_create(0);
	sem_passengers = mysem_create(0);
	sem_passengers_to_wait = mysem_create(0);
	sem_main = mysem_create(0);
	sem_mutex = mysem_create(1);
	
	//Creates train thread.
	check_thread = pthread_create(&thread_id,NULL,train,NULL);
	if(check_thread != 0) {
		fprintf(stderr,ANSI_COLOR_YELLOW  "Error in creating the train thread"  ANSI_COLOR_RESET "\n");
		perror(ANSI_COLOR_YELLOW  "Error: "  ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	//Creates passengers threads.
	for(i = 0; i < num_passengers; i++) {
		check_thread = pthread_create(&thread_id,NULL,passengers,NULL);
		if(check_thread != 0) {
			fprintf(stderr,ANSI_COLOR_YELLOW  "Error in creating the passengers thread"  ANSI_COLOR_RESET "\n");
			perror(ANSI_COLOR_YELLOW  "Error: "  ANSI_COLOR_RESET);
			exit(EXIT_FAILURE);
		}
	}
	
	mysem_down(sem_main);
	
	mysem_destroy(sem_passengers_to_wait);
	mysem_destroy(sem_passengers);
	mysem_destroy(sem_train);
	mysem_destroy(sem_mutex);
	mysem_destroy(sem_main);
	
	return(0);
}

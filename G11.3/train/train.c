/*Program that controls the boarding and the getting of passengers in a train.
 * Kofopoulos-Lymperis Stamatis 2548
 * Pavlidis Panagiotis 2608
*/
#include "mutex_error.h"
#include <pthread.h>
#include <unistd.h>

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int num_passengers;
int passenger_in_train;
int train_limit;
int waiting_passengers_on_train = 0;
int waiting_to_board = 0;
int train_flag = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t train_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t passengers_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t passengers_to_wait_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;

void train_func() {
	int i;
	
	printf("Train waiting for departure.\n");
	mutex_lock(&mutex, __LINE__);
	if(train_flag == 0) {
		cond_wait(&train_cond, &mutex, __LINE__);
	}
	train_flag = 0;
	printf("I am doing the route\n");
	mutex_unlock(&mutex,__LINE__);
	sleep(1);
	mutex_lock(&mutex,__LINE__);
	printf("Route is finished\n");
	
	//Unblocks the boarded passengers when the route is finished, in order to leave from the train.
	for(i = 0; i < train_limit; i++) {
		cond_signal(&passengers_cond, __LINE__);
		num_passengers--;
		printf("Num passengers is %d\n",num_passengers);
	}
	
	mutex_unlock(&mutex, __LINE__);
}

void passengers_func() {
	int i;
	int wake_passengers = 0;
	
	printf("Hello i am a passenger!\n");
	
	mutex_lock(&mutex, __LINE__);
	passenger_in_train++;
	
	//Block the passenger who are above the train seat limit.
	if(passenger_in_train > train_limit) {
		printf("I am blocked outside the train.\n");
		waiting_to_board++;
		cond_wait(&passengers_to_wait_cond, &mutex, __LINE__);
	}
	
	waiting_passengers_on_train++;
	printf("I am blocked INSIDE the train.\n");
	
	//Unblock the train in order to begin the route.
	if(waiting_passengers_on_train == train_limit) {
		printf("Signaling train!\n");
		train_flag = 1;
		cond_signal(&train_cond, __LINE__);
	}
	
	//Blocks the passengers who have boarded train.
	cond_wait(&passengers_cond, &mutex, __LINE__);
	
	printf("Passenger is leaving from the train\n");
	
	waiting_passengers_on_train--;
	passenger_in_train--;
	
	//The last passenger that leaves the train unblocks some of the passengers who are blocked before in order to enter the train.
	if(waiting_passengers_on_train == 0) {
		if(waiting_to_board >= train_limit) {
			wake_passengers = train_limit;
			waiting_to_board = waiting_to_board - train_limit;
		}
		
		//This else is used, if we want the passengers to enter the train if they are less than the train seat limit, in order to terminate normally the program.
		else {
			wake_passengers = waiting_to_board;
			waiting_to_board = 0;
		}
		
		for(i = 0; i < wake_passengers; i++) {
			cond_signal(&passengers_to_wait_cond, __LINE__);
		}
	}
	
	mutex_unlock(&mutex, __LINE__);
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
	
	passengers_func();
	
	//The last of all the passengers unblocks main.
	if((num_passengers == 0) && (passenger_in_train == 0)) {
		mutex_lock(&mutex,__LINE__);
		cond_signal(&main_cond, __LINE__);
		mutex_unlock(&mutex,__LINE__);
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
	
	mutex_lock(&mutex, __LINE__);
	cond_wait(&main_cond, &mutex, __LINE__);
	mutex_unlock(&mutex, __LINE__);
	
	cond_destroy(&train_cond,__LINE__);
	cond_destroy(&passengers_cond,__LINE__);
	cond_destroy(&passengers_to_wait_cond,__LINE__);
	cond_destroy(&main_cond,__LINE__);
	mutex_destroy(&mutex,__LINE__);
	
	return(0);
}

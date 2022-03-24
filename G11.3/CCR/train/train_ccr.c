/*Program that controls the boarding and the getting of passengers in a train using CCR for concurrency.
 * Kofopoulos-Lymperis Stamatis 2548
 * Pavlidis Panagiotis 2608
*/

#include "ccr.h"

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

CCR_DECLARE(R);

int num_passengers;
int train_limit;
int waiting_passengers_on_train = 0;
int train_flag = 0;
int main_flag = 0;





void train_func() {
	if((waiting_passengers_on_train != train_limit)) {
		printf("Train waiting for departure.\n");
	}
	CCR_EXEC(R,((waiting_passengers_on_train == train_limit) && (train_flag == 0)),
					printf("I am doing the route\n"); sleep(1); printf("Route is finished\n"); 
					num_passengers -= train_limit; printf("Num passengers is %d\n",num_passengers); train_flag = 1;); 
	
	//Unblocks the boarded passengers when the route is finished, in order to leave from the train.
}

void passengers_func() {
	
	printf("Hello i am a passenger!\n");
	
	CCR_EXEC(R,((waiting_passengers_on_train < train_limit) && (train_flag == 0)),
		waiting_passengers_on_train++; printf("I am blocked INSIDE the train %d.\n",waiting_passengers_on_train););
	
	//Block the passenger who are above the train seat limit.
	//CCR_EXEC(R,waiting_passengers_on_train > train_limit,waiting_to_board++; printf("I am blocked outside the train.\n"););
	
	CCR_EXEC(R,(train_flag == 1),printf("Passenger is leaving from the train %d\n",waiting_passengers_on_train); waiting_passengers_on_train--; 
							if(waiting_passengers_on_train == 0) {
								train_flag = 0;
							} );
	
	//The last passenger that leaves the train unblocks some of the passengers who are blocked before in order to enter the train.
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
	
	CCR_EXEC(R,((num_passengers == 0) && (waiting_passengers_on_train == 0)),main_flag = 1;);
	
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
    
    CCR_INIT(R);
	
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
	
	CCR_EXEC(R,main_flag == 1,printf("Main thread terminates!\n"););
	
	return(0);
}

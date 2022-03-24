/* Program that controls the traffic on a narrow bridge which has only one street lane.
 Pavlidis Panagiotis 2608
 Kofopoulos-Lymperis Stamatis 2548*/

#include "mutex_error.h"
#include <pthread.h>
#include <unistd.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define CARS_POP 10

struct cars {
	int car_id;
	int car_colour;
};

typedef struct cars carsT;

carsT *new_cars;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t blue_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t red_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t bridge_limit_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;

int number_of_cars;
int bridge_limit;
int bridge_counter = 0;

int blue_in_move = -1;
int red_in_move = -1;

int red_in_bridge = 0;
int blue_in_bridge = 0;

int wait_counter_blue = 0;
int wait_counter_red = 0;


void *create_cars(void * input) {
	carsT *car;
	car = (carsT*) input;
	int i;

	mutex_lock(&mutex,__LINE__);
	
	//Checks which car colour enters first in the bridge.
	if((red_in_move == -1) && (blue_in_move == -1)) {
		if(car->car_colour == 1) {
			blue_in_move = car->car_colour;
		}
		else {
			red_in_move = car->car_colour;
		}
	}
	
	mutex_unlock(&mutex,__LINE__);

	if(car->car_colour == 1) {
		
		printf(ANSI_COLOR_BLUE "Hello from car." ANSI_COLOR_RESET "\n");
		
		//Blocks while the red cars are in the bridge.
		mutex_lock(&mutex,__LINE__);
		
		if(red_in_move != -1) {
			wait_counter_blue++;
			cond_wait(&blue_cond,&mutex,__LINE__);
		}
		
		//Blocks in order to change the car colour which will be in the bridge.
		
		if(bridge_counter >= CARS_POP) {
			
			bridge_counter++;
			printf("bridge_counter on blue is %d\n",bridge_counter);
			red_in_move = 0;
			wait_counter_blue++;
			
			cond_wait(&blue_cond,&mutex,__LINE__);
			red_in_move = -1;
		}
		
		bridge_counter++;
		
		blue_in_bridge++;
		
		//Blocks the same colour of cars if we reach the capacity of the bridge.
		if(blue_in_bridge > bridge_limit) {
			cond_wait(&bridge_limit_cond,&mutex,__LINE__);
		}
		
		mutex_unlock(&mutex,__LINE__);
		
		//The CS == The bridge !!
		printf(ANSI_COLOR_BLUE "Hello from the CS." ANSI_COLOR_RESET "\n");
		sleep(1);
		
		printf(ANSI_COLOR_BLUE "Goodbye from car." ANSI_COLOR_RESET "\n");
		
		mutex_lock(&mutex,__LINE__);
		//Unblocks a car if there is an available space in the bridge.
		if(blue_in_bridge > bridge_limit) {
			cond_signal(&bridge_limit_cond,__LINE__);
		}
		blue_in_bridge--;
		
		if(blue_in_bridge == 0) {
			blue_in_move = -1;
			red_in_move = 0;
			bridge_counter = 0;
			
			//Unblocks the same car colour if there are not any of the opposite cars colour.
			printf("Wait wait_counter_red in blue is %d\n",wait_counter_red);
			if((wait_counter_blue != 0) && (wait_counter_red == 0))  {
				blue_in_move =1;
				red_in_move = -1;
				for(i = 0; ((i < wait_counter_blue) && (i < CARS_POP)); i++) {
					cond_signal(&blue_cond,__LINE__);
				}
				wait_counter_blue = wait_counter_blue - i;
			}
			
			//Unblocks the opposite car colour to enter the bridge.
			printf("Wait wait_counter_blue is %d\n",wait_counter_blue);
			for(i = 0; ((i < wait_counter_red) && (i < CARS_POP)) ; i++) {
				cond_signal(&red_cond,__LINE__);
			}
			wait_counter_red = wait_counter_red - i;
		}
		
		mutex_unlock(&mutex,__LINE__);
	}
	else {
		printf(ANSI_COLOR_RED "Hello from car." ANSI_COLOR_RESET "\n");
		
		//Blocks while the blue cars are in the bridge.
		
		mutex_lock(&mutex,__LINE__);
		
		if(blue_in_move != -1) {
			wait_counter_red++;
			cond_wait(&red_cond,&mutex,__LINE__);
		}
		
		//Blocks in order to change the car colour which will be in the bridge.
		

		if(bridge_counter >= CARS_POP) {
			bridge_counter++;
			printf("bridge_counter on red is %d\n",bridge_counter);
			blue_in_move = 1;
			wait_counter_red++;
			
			cond_wait(&red_cond,&mutex,__LINE__);
			blue_in_move = -1;
		}
		
		bridge_counter++;

		
		red_in_bridge++;
		
		//Blocks the same colour of cars if we reach the capacity of the bridge.
		if(red_in_bridge > bridge_limit) {
			cond_wait(&bridge_limit_cond,&mutex,__LINE__);
		}
		
		mutex_unlock(&mutex,__LINE__);
		
		//The CS == The bridge!!
		printf(ANSI_COLOR_RED "Hello from the CS." ANSI_COLOR_RESET "\n");
		sleep(1);
		
		printf(ANSI_COLOR_RED "Goodbye from car." ANSI_COLOR_RESET "\n");
		
		mutex_lock(&mutex,__LINE__);
		
		//Unblocks a car if there is an available space in the bridge.
		if(red_in_bridge > bridge_limit) {
			cond_signal(&bridge_limit_cond,__LINE__);
		}
		red_in_bridge--;
		
		if(red_in_bridge == 0) {
			red_in_move = -1;
			blue_in_move = 1;
			bridge_counter = 0;
			
			//Unblocks the same car colour if there are not any of the opposite cars colour.
			printf("Wait wait_counter_blue in red is %d\n",wait_counter_blue);
			if((wait_counter_red != 0) && (wait_counter_blue == 0)) {
				red_in_move = 0;
				blue_in_move =-1;
				for(i = 0; ((i < wait_counter_red) && (i < CARS_POP)); i++) {
					cond_signal(&red_cond,__LINE__);
				}
				wait_counter_red = wait_counter_red - i;
			}
			
			//Unblocks the opposite car colour to enter the bridge.
			printf("Wait wait_counter_red is %d\n",wait_counter_red);
			for(i = 0; ((i < wait_counter_blue) && (i < CARS_POP)); i++) {
				cond_signal(&blue_cond,__LINE__);
			}
			wait_counter_blue = wait_counter_blue - i;
			
		}
		
		mutex_unlock(&mutex,__LINE__);
	}
	
	mutex_lock(&mutex,__LINE__);
	number_of_cars--;
	//When cars have passed the bridge unblocks main in order to terminate.
	if(number_of_cars == 0){
		cond_signal(&main_cond,__LINE__);
	}
	
	mutex_unlock(&mutex,__LINE__);
	return(NULL);
}

int main(int argc, char *argv[]) {
	int i;
	pthread_t threads_id[atoi(argv[1])];
	int check_thread;
	
	if(argc != 3) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Enter the number of cars and the bridge_limit!" ANSI_COLOR_RESET "\n");
		exit(EXIT_FAILURE);
	}
	
	number_of_cars = atoi(argv[1]);
	bridge_limit = atoi(argv[2]);
		
	new_cars = (carsT *)malloc(atoi(argv[1])*sizeof(carsT));
	if(new_cars == NULL) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in creation of the cars" ANSI_COLOR_RESET "\n");
		perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	srand(time(0));
	
	//creating cars-threads.
	for(i = 0; i < atoi(argv[1]); i++ ) {
		
		//1 is for blue cars and 0 for red.
		new_cars[i].car_colour = rand() % 2;
		
		check_thread = pthread_create(&threads_id[i], NULL, create_cars, &new_cars[i]);
		if(check_thread != 0) {
			fprintf(stderr,ANSI_COLOR_YELLOW "Error in creation of blue threads" ANSI_COLOR_RESET "\n");
			perror(ANSI_COLOR_YELLOW "Error: " ANSI_COLOR_RESET);
			exit(EXIT_FAILURE);
		}
	}
	
	//blocks main.
	mutex_lock(&mutex,__LINE__);
	cond_wait(&main_cond,&mutex,__LINE__);
	mutex_unlock(&mutex,__LINE__);
	
	cond_destroy(&red_cond,__LINE__);
	cond_destroy(&blue_cond,__LINE__);
	cond_destroy(&bridge_limit_cond,__LINE__);
	cond_destroy(&main_cond,__LINE__);
	mutex_destroy(&mutex,__LINE__);

	return(0);
}

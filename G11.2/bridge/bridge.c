/* Program that controls the traffic on a narrow bridge which has only one street lane.
 Pavlidis Panagiotis 2608
 Kofopoulos-Lymperis Stamatis 2548*/

#include "my_sem.h"
#include <pthread.h>
#include <time.h>


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

key_t sem_red;
key_t sem_blue;
key_t sem_mutex;
key_t sem_bridge_limit;
key_t sem_main;

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
	int check_up;
	int i;

	mysem_down(sem_mutex);
	
	//Checks which car colour enters first in the bridge.
	if((red_in_move == -1) && (blue_in_move == -1)) {
		if(car->car_colour == 1) {
			blue_in_move = car->car_colour;
		}
		else {
			red_in_move = car->car_colour;
		}
	}
	
	check_up = mysem_up(sem_mutex);
	if(check_up == -1) {
		printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in the beginning!!" ANSI_COLOR_RESET "\n");
	}

	if(car->car_colour == 1) {
		
		printf(ANSI_COLOR_BLUE "Hello from car." ANSI_COLOR_RESET "\n");
		
		//Blocks while the red cars are in the bridge.
		if(red_in_move != -1) {
			wait_counter_blue++;
			mysem_down(sem_blue);
		}
		
		//Blocks in order to change the car colour which will be in the bridge.
		mysem_down(sem_mutex);

		if(bridge_counter >= CARS_POP) {

			bridge_counter++;
			printf("bridge_counter on blue is %d\n",bridge_counter);
			red_in_move = 0;
			wait_counter_blue++;
			
			check_up = mysem_up(sem_mutex);
			if(check_up == -1) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in checking the case_change for blue!!" ANSI_COLOR_RESET "\n");
			}
			
			mysem_down(sem_blue);
			red_in_move = -1;
			mysem_down(sem_mutex);
		}
		
		bridge_counter++;
		
		check_up = mysem_up(sem_mutex);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in bridge_counter++ for blue!!" ANSI_COLOR_RESET "\n");
		}

		mysem_down(sem_mutex);
		blue_in_bridge++;
		check_up = mysem_up(sem_mutex);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in blue++!!" ANSI_COLOR_RESET "\n");
		}
		//Blocks the same colour of cars if we reach the capacity of the bridge.
		if(blue_in_bridge > bridge_limit) {
			mysem_down(sem_bridge_limit);
		}
		
		//The CS == The bridge !!
		printf(ANSI_COLOR_BLUE "Hello from the CS." ANSI_COLOR_RESET "\n");
		sleep(1);
		mysem_down(sem_mutex);
		
		printf(ANSI_COLOR_BLUE "Goodbye from car." ANSI_COLOR_RESET "\n");
		
		//Unblocks a car if there is an available space in the bridge.
		if(blue_in_bridge > bridge_limit) {
			check_up = mysem_up(sem_bridge_limit);
			if(check_up == -1) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_bridge_limit for blue!!" ANSI_COLOR_RESET "\n");
			}
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
					check_up = mysem_up(sem_blue);
					if(check_up == -1) {
						printf(ANSI_COLOR_YELLOW "Unused up call in sem_blue while unblock blue!!" ANSI_COLOR_RESET "\n");
					}
				}
				wait_counter_blue = wait_counter_blue - i;
			}
			
			//Unblocks the opposite car colour to enter the bridge.
			printf("Wait wait_counter_blue is %d\n",wait_counter_blue);
			for(i = 0; ((i < wait_counter_red) && (i < CARS_POP)) ; i++) {
				check_up = mysem_up(sem_red);
				if(check_up == -1) {
					printf(ANSI_COLOR_YELLOW "Unused up call in semid while unblock red!!" ANSI_COLOR_RESET "\n");
				}
			}
			wait_counter_red = wait_counter_red - i;
		}
		
		check_up = mysem_up(sem_mutex);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in bridge == 0!!" ANSI_COLOR_RESET "\n");
		}
	}
	else {
		printf(ANSI_COLOR_RED "Hello from car." ANSI_COLOR_RESET "\n");
		
		//Blocks while the blue cars are in the bridge.
		if(blue_in_move != -1) {
			wait_counter_red++;
			mysem_down(sem_red);
		}
		
		//Blocks in order to change the car colour which will be in the bridge.
		mysem_down(sem_mutex);

		if(bridge_counter >= CARS_POP) {
			bridge_counter++;
			printf("bridge_counter on red is %d\n",bridge_counter);
			blue_in_move = 1;
			wait_counter_red++;
			check_up = mysem_up(sem_mutex);
			if(check_up == -1) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in checking the case_change for red!!" ANSI_COLOR_RESET "\n");
			}
			mysem_down(sem_red);
			blue_in_move = -1;
			mysem_down(sem_mutex);
		}
		
		bridge_counter++;

		check_up = mysem_up(sem_mutex);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in bridge_counter++ for red!!" ANSI_COLOR_RESET "\n");
		}

		mysem_down(sem_mutex);
		red_in_bridge++;
		check_up = mysem_up(sem_mutex);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in red++!!" ANSI_COLOR_RESET "\n");
		}
		
		//Blocks the same colour of cars if we reach the capacity of the bridge.
		if(red_in_bridge > bridge_limit) {
			mysem_down(sem_bridge_limit);
		}
		
		//The CS == The bridge!!
		printf(ANSI_COLOR_RED "Hello from the CS." ANSI_COLOR_RESET "\n");
		sleep(1);
		mysem_down(sem_mutex);
		
		printf(ANSI_COLOR_RED "Goodbye from car." ANSI_COLOR_RESET "\n");
		
		//Unblocks a car if there is an available space in the bridge.
		if(red_in_bridge > bridge_limit) {
			check_up = mysem_up(sem_bridge_limit);
			if(check_up == -1) {
				printf(ANSI_COLOR_YELLOW "Unused up call in sem_bridge_limit for red!!" ANSI_COLOR_RESET "\n");
			}
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
					check_up = mysem_up(sem_red);
					if(check_up == -1) {
						printf(ANSI_COLOR_YELLOW "Unused up call in sem_red while unblock red!!" ANSI_COLOR_RESET "\n");
					}
				}
				wait_counter_red = wait_counter_red - i;
			}
			
			//Unblocks the opposite car colour to enter the bridge.
			printf("Wait wait_counter_red is %d\n",wait_counter_red);
			for(i = 0; ((i < wait_counter_blue) && (i < CARS_POP)); i++) {
				check_up = mysem_up(sem_blue);
				if(check_up == -1) {
					printf(ANSI_COLOR_YELLOW "Unused up call in semid while unblock blue!!" ANSI_COLOR_RESET "\n");
				}
			}
			wait_counter_blue = wait_counter_blue - i;
			
		}
		
		check_up = mysem_up(sem_mutex);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex in red--!!" ANSI_COLOR_RESET "\n");
		}
	}
	
	mysem_down(sem_mutex);
	number_of_cars--;
	check_up = mysem_up(sem_mutex);
	if(check_up == -1) {
		printf(ANSI_COLOR_YELLOW "Unused up call in sem_mutex before sem_main." ANSI_COLOR_RESET "\n");
	}
	//When cars have passed the bridge unblocks main in order to terminate.
	if(number_of_cars == 0){
		check_up = mysem_up(sem_main);
		if(check_up == -1) {
			printf(ANSI_COLOR_YELLOW "Unused up call in sem_main." ANSI_COLOR_RESET "\n");
		}
	}
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
	
	sem_blue = mysem_create(0);
	sem_red = mysem_create(0);
	sem_mutex = mysem_create(1);
	sem_bridge_limit = mysem_create(0);
	sem_main = mysem_create(0);
	
	printf("sem_blue is %d\n",sem_blue);
	printf("sem_red is %d\n",sem_red);
	printf("sem_bridge_limit is %d\n",sem_bridge_limit);
	printf("sem_mutex is %d\n",sem_mutex);
	
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
	mysem_down(sem_main);
	
	mysem_destroy(sem_blue);
	mysem_destroy(sem_red);
	mysem_destroy(sem_bridge_limit);
	mysem_destroy(sem_mutex);
	mysem_destroy(sem_main);

	return(0);
}

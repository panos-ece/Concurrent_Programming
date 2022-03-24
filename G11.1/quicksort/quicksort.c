#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

int *array;

//55 18 64 58 22 41 68 45
//threads

struct thread_element {
	int l_index;
	int h_index;
	int flag;
	pthread_t thread_id;
};

typedef struct thread_element thread_elementT;


void *quicksort(void *number_to_sort) {
	thread_elementT *thread_info;
	thread_elementT *baby_thread1;
	thread_elementT *baby_thread2;
	
	thread_info = (thread_elementT *)number_to_sort;

	printf("Hello from the new thread: l = %d, h = %d\n", thread_info->l_index, thread_info->h_index);

	int i = thread_info->l_index;
	int j = thread_info->h_index;
	int pivot = thread_info->h_index;
	int temp,iter,k;
	int check_yield;
	int check_thread1, check_thread2;

	
	if(thread_info->h_index - thread_info->l_index + 1 >= 2) {
		while(i < j) {
			while(i < j) {
				if(array[i] > array[pivot]) {
					break;
				}
				i++;
			}
			
			while(i < j) {
				if(array[j] < array[pivot]) {
					break;
				}
				j--;
			}
			
			if(i >= j) {
				temp = array[i];
				array[i] = array[pivot];
				array[pivot] = temp;
			}
			
			temp = array[i];
			array[i] = array[j];
			array[j] = temp;
			
			putchar('\n');
			
			for(k = thread_info->l_index; k <= thread_info->h_index; k++){
				printf("%d ", array[k]);
			}
		
			putchar('\n');
			
		}

		//thread_info->flag = 1 means it creates the left child.
		//thread_info->flag = -1 means it creates the right child.
		thread_info->flag = 1;

		for(iter = 0; iter < 2; iter++) {
			printf("Create children!!\n");
			if((thread_info->flag == 1)) {
				baby_thread1 = (thread_elementT*)malloc(sizeof(thread_elementT));
				if(baby_thread1 == NULL){
					fprintf(stderr, "Error in malloc of the baby_thread1!\n");
					exit(EXIT_FAILURE);
				}

				baby_thread1->l_index = thread_info->l_index;
				baby_thread1->h_index = i - 1;	
				
				check_thread1 = pthread_create(&baby_thread1->thread_id,NULL,quicksort,baby_thread1);
				if(check_thread1 != 0) {
					fprintf(stderr, "Error in creation of thread1\n");
					perror("Error :");
					free(baby_thread1);
					exit(EXIT_FAILURE);
				}
			}

			if((thread_info->flag == -1)) {
				baby_thread2 = (thread_elementT*)malloc(sizeof(thread_elementT));
				if(baby_thread2 == NULL){
					fprintf(stderr, "Error in malloc of the baby_thread2!\n");
					free(baby_thread1);
					exit(EXIT_FAILURE);
				}

				baby_thread2->l_index = i + 1;
				baby_thread2->h_index = thread_info->h_index;
				
				check_thread2 = pthread_create(&baby_thread2->thread_id,NULL,quicksort,baby_thread2);
				if(check_thread2 != 0) {
					fprintf(stderr, "Error in creation of thread2\n");
					perror("Error :");
					free(baby_thread1);
					free(baby_thread2);
					exit(EXIT_FAILURE);
				}
			}
			thread_info->flag = -1;
		}

		while(1) {
			if((baby_thread1->flag == 666) && (baby_thread2->flag == 666)) {
				thread_info->flag = 666;
				printf("Children have ended!!\n");
				free(baby_thread1);
				free(baby_thread2);
				break;
			}

			check_yield = sched_yield();
			if(check_yield == -1) {
				fprintf(stderr,"Error in yield\n");
				perror("Error :");
				free(baby_thread1);
				free(baby_thread2);
				exit(EXIT_FAILURE);
			}
		}
		return(NULL);
	}
	else {
		thread_info->flag = 666;
		printf("Exiting else command because: l = %d, h = %d\n", thread_info->l_index, thread_info->h_index);
	}
	return(NULL);
}

int main(int argc, char *argv[]) {
	int size = atoi(argv[1]);
	int i, iret1;
	thread_elementT *threads;
	int check_yield;
	
	//Allocate memory for data_array.
	array = (int *)malloc(size * sizeof(int));
	if(array == NULL) {
		fprintf(stderr,"Error in allocation of the array\n");
		exit(EXIT_FAILURE);
	}
	
	//Allocate memory for the initial thread.
	threads = (thread_elementT *)malloc(sizeof(thread_elementT));
	if(threads == NULL) {
		fprintf(stderr,"Error in allocation of the threads\n");
		free(array);
		exit(EXIT_FAILURE);
	}
	
	printf("Give the elements of the array: ");
	
	for(i = 0; i < size; i++) {
		scanf(" %d",&array[i]);
	}
	
	for(i = 0; i < size; i++){
		printf("%d ", array[i]);
	}
	
	putchar('\n');

	threads->l_index = 0;

	//size array that enters in quicksort.
	threads->h_index = size - 1;								
	
	iret1 = pthread_create(&threads->thread_id,NULL,quicksort,threads);
	if(iret1 != 0) {
		fprintf(stderr,"Error - in pthread_create returns code %d\n",iret1);
		perror("Error: ");
		free(array);
		free(threads);
		exit(EXIT_FAILURE);
	}

	/*Waiting dynamically quicksort to end.*/
	while(1) {
		if(threads->flag == 666){
			printf("All the threads have end with their job and the array is now sorted!\n");
			break;
		}

		check_yield = sched_yield();
		if(check_yield == -1) {
			fprintf(stderr,"Error in yield\n");
			perror("Error :");
		}
	}
	
	putchar('\n');
	
	for(i = 0; i < size; i++){
		printf("%d ", array[i]);
	}
	
	putchar('\n');

	free(threads);
	free(array);

	return(0);
}

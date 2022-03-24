#include "my_sem.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"


key_t mysem_create(int val) {
	key_t semid;
	
	if((semid = semget(IPC_PRIVATE,2,S_IRWXU)) == -1) {
		perror(ANSI_COLOR_YELLOW "Error in semget: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	
	if(semctl(semid,0,SETVAL,val) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in init first semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semctl return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	if(semctl(semid,1,SETVAL,1) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in init 2nd semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semctl return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	return(semid);
}

void mysem_down(key_t semid) {
	struct sembuf sops;
	int sem_val;
	
	sops.sem_num = 1;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	
	if(semop(semid,&sops,1) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in down mutex semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	sem_val = semctl(semid,0,GETVAL);
	if(sem_val == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in GETVAL in down semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Sem_val return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	if(sem_val == 0) {
		sops.sem_num = 1;
		sops.sem_op = 1;
		sops.sem_flg = 0;
		
		if(semop(semid,&sops,1) == -1) {
			fprintf(stderr,ANSI_COLOR_YELLOW "Error in up mutex when GETVAL = 0 semid %d." ANSI_COLOR_RESET "\n",semid);
			perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
			exit(EXIT_FAILURE);
		}
	}
	
	sops.sem_num = 0;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	sem_val--;
	if(semop(semid,&sops,1) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in down basic semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	if(sem_val == 0) {
		sops.sem_num = 1;
		sops.sem_op = 1;
		sops.sem_flg = 0;
		
		if(semop(semid,&sops,1) == -1) {
			fprintf(stderr,ANSI_COLOR_YELLOW "Error in up mutex after down basic semid %d." ANSI_COLOR_RESET "\n",semid);
			perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
			exit(EXIT_FAILURE);
		}
	}
}

int mysem_up(key_t semid) {
	int sem_val;
	struct sembuf sops;
	
	sops.sem_num = 1;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	
	if(semop(semid,&sops,1) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in down mutex in mysem_up semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	sem_val  = semctl(semid,0,GETVAL);
	if(sem_val == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Sem_val return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	if(sem_val >= 1) {
		sops.sem_num = 1;
		sops.sem_op = 1;
		sops.sem_flg = 0;
		
		if(semop(semid,&sops,1) == -1) {
			fprintf(stderr,ANSI_COLOR_YELLOW "Error in up mutex when sem_val >=1 in mysem_up semid %d." ANSI_COLOR_RESET "\n",semid);
			perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
			exit(EXIT_FAILURE);
		}
		return(-1);
	}
	
	sops.sem_num = 0;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	
	if(semop(semid,&sops,1) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	sops.sem_num = 1;
	sops.sem_op = 1;
	sops.sem_flg = 0;
		
	if(semop(semid,&sops,1) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in up mutex when exiting mysem_up semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Semop return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	
	return(1);
}

void mysem_destroy(key_t semid) {
	if(semctl(semid,0,IPC_RMID) == -1) {
		fprintf(stderr,ANSI_COLOR_YELLOW "Error in destroying semid %d." ANSI_COLOR_RESET "\n",semid);
		perror(ANSI_COLOR_YELLOW "Sem_destroy return code: " ANSI_COLOR_RESET);
		exit(EXIT_FAILURE);
	}
}


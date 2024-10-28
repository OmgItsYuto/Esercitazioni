#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#include "header.h"

#define N_PROC 11

int main(){
	pid_t pid_ex,pid;
	int st,val;

	//rand(time(NULL));

	//creare le risorse IPC necessarie

	key_t chiaveshm = ftok(".",'k');
	int ds_shm = shmget(chiaveshm,sizeof(BufferCircolare), IPC_CREAT|0664);
	if(ds_shm<0) { perror("SHM errore"); exit(1); }

	key_t chiavesem = ftok(".",'s');
	int ds_sem = semget(chiavesem, 2, IPC_CREAT|0664);
	if(ds_sem<0) { perror("SEM errore"); exit(1); }

	key_t chiavecnt = ftok(".",'l');
	int ds_shm2 = shmget(chiavecnt,sizeof(int), IPC_CREAT|0664);
	int *cnt = (int *) shmat(ds_shm2,NULL, 0);
	*cnt=0;

	semctl(ds_sem, SPAZIO_DISP, SETVAL, N);
	semctl(ds_sem, NUM_MSG, SETVAL, 0);
	semctl(ds_sem, MUTEX_P, SETVAL, 1);

	//istanziare i processi
	for(int i = 0; i < N_PROC;i++){
		pid = fork();
		if(pid == 0){
			if(i == 0){
				execl("./consumatore","./consumatore",NULL);
				exit(1);
				//exec consumatore
			}else{
				execl("./produttore","./produttore",NULL);
				exit(1);
				//exec produttore
			}
			_exit(0);			
		}
		val=(1+rand()%3);
		sleep(val);

	}
	
	for(int i = 0; i < N_PROC ; i++){
		pid_ex = wait(&st);
		printf("[MASTER] - Il processo %d ha terminato l'esecuzione\n",pid_ex);			
	}
	return 0;
}

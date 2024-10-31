#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#include "header.h"


int main(){
	pid_t pid_ex,pid;
	int st,i;
	int shmid,semid;
	BufferCircolare* buf;

	key_t key_shm,key_sem;
	key_shm = ftok(".",'k');
	key_sem = ftok(".",'m');

	shmid = shmget(key_shm,sizeof(BufferCircolare),IPC_CREAT|0664);
	semid = semget(key_sem,6,IPC_CREAT|0664);
	printf("[MASTER] - Shmid : %d, Semid: %d\n",shmid,semid);

	// TODO
	// Inizializzare risorse
	buf=shmat(shmid,NULL,0);
	for(int i=0;i<SIZE;i++){
		buf->stati[i]=LIBERO;
	}

	semctl(semid, SPAZIO_DISP, SETVAL, NPROD);
	semctl(semid, MSG_DISP1, SETVAL, 0);
	semctl(semid, MSG_DISP2, SETVAL, 0);
	semctl(semid, MSG_DISP3, SETVAL, 0);
	semctl(semid, MUTEXP, SETVAL, 1);
	semctl(semid, MUTEXC, SETVAL, 1);

	srand(time(NULL));
	int val;

	for(i = 0; i < NCONS+NPROD; i++){
		// TODO CREARE PROCESSI FIGLI (PRODUTTORI E CONSUMATORI)
		pid=fork();

		if(pid==-1){
			perror("Errore fork!");
		}else if(pid==0){
			if(i<NPROD){
				produci(buf,i+1,semid);
				exit(0);
			}else{
				consuma(buf,i-2,semid);
				exit(0);
			}
		}

		val =  (1+rand()%3);
		sleep(val);

	}
	
	for(i = 0; i < NCONS+NPROD ; i++){
		// TODO JOIN COMPLETAMENTO FIGLI
		pid_ex=wait(NULL);
		printf("[MASTER] - Il processo %d ha terminato l'esecuzione\n",pid_ex);			
	}

	// RIMOZIONE RISORSE
	semctl(semid,0,IPC_RMID);
	shmctl(shmid,IPC_RMID,NULL);

	return 0;
}

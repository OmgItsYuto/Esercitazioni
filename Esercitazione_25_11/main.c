#include "procedure.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

int main(){
    int id_shm,id_shm2;
    pid_t pid;

	id_shm = shmget(IPC_PRIVATE,sizeof(BufferCircolare),IPC_CREAT|0664);
	id_shm2 = shmget(IPC_PRIVATE,sizeof(pid_t)*20,IPC_CREAT|0664);


    if(id_shm < 0) {
		perror("errore creazione risorse");
		exit(1);
	}

	BufferCircolare *b = shmat(id_shm,NULL,0);
    pid_t *disk=shmat(id_shm2,NULL,0);

    for(int i=0;i<20;i++)
        disk[i]=0;

    init_monitor(&(b->m),2);
    b->cnt=0;
    b->coda=b->testa=0;

    for(int i=0;i<N_UTENTI;i++){
        pid=fork();
        if(pid<0){
            perror("Errore fork");
            exit(1);
        }else if(pid==0){
            utente(b);
            exit(0);
        }
    }

    pid=fork();
    if(pid<0){
        perror("Errore fork");
        exit(1);
    }else if(pid==0){
        schedulatore(b,disk);
        exit(0);
    }

    for(int j=0; j<N_UTENTI+1; j++){
		pid=wait(NULL);
		printf("Il processo figlio %d è terminato\n",pid);
	}

    printf("Output disco\n");
    for(int i=0;i<20;i++){
        printf("Locazione %d: %d\n",i,disk[i]);
    }

    remove_monitor(&(b->m));
	shmctl(id_shm,IPC_RMID,0);
	shmctl(id_shm2,IPC_RMID,0);
}
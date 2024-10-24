#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "procedure.h"

int main(){

    key_t chiave = ftok(".",'m');

	int ds_shm = shmget(chiave, sizeof(int), IPC_CREAT|0664);

	if(ds_shm<0) { perror("SHM errore"); exit(1); }

	int * p;

	p = (int *) shmat(ds_shm, 0, 0);



	key_t chiavesem = ftok(".",'s');

	int ds_sem = semget(chiavesem, 2, IPC_CREAT|0664);

	if(ds_sem<0) { perror("SEM errore"); return -1; }

    *p=0;
    semctl(ds_sem,SPAZIO_DISPONIBILE,SETVAL,1);
    semctl(ds_sem,MESSAGGIO_DISPONIBILE,SETVAL,0);
    
    consumatore(p,ds_sem);

    return 0;

}
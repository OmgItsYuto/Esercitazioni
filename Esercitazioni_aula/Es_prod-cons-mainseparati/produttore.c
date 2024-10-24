#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include "procedure.h"

int main(){

    key_t chiave = ftok(".",'m');

	int ds_shm = shmget(chiave, sizeof(int), 0);

	if(ds_shm<0) { perror("SHM errore"); exit(1); }

	int * p;

	p = (int *) shmat(ds_shm, 0, 0);

	key_t chiavesem = ftok(".",'s');

	int ds_sem = semget(chiavesem, 2, 0);

	if(ds_sem<0) { perror("SEM errore"); return -1; }

    produttore(p,ds_sem);

    return 0;

}
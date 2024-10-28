/* PROCESSO PRODUTTORE */

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "header.h"


int main(){
	printf("[PRODUTTORE] <%d> \n",getpid());

	key_t chiaveshm = ftok(".",'k');
	int ds_shm = semget(chiaveshm,sizeof(BufferCircolare), 0);
	if(ds_shm<0) { perror("SHM errore"); exit(1); }

	BufferCircolare *buf = (BufferCircolare *) shmat(ds_shm, 0, 0);


	key_t chiavesem = ftok(".",'s');
	int ds_sem = semget(chiavesem,2, 0);
	if(ds_sem<0) { perror("SEM errore"); return -1; }

	produci_elemento(ds_sem,buf);

	return 1;
}

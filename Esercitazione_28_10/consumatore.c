/*PROCESSO CONSUMATORE*/

#include "header.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>



int main(){
	printf("[Consumatore] Sono il processo consumatore con pid %d",getpid());

	key_t chiaveshm = ftok(".",'k');
	int ds_shm = shmget(chiaveshm,sizeof(BufferCircolare), 0);
	BufferCircolare *buf = (BufferCircolare *) shmat(ds_shm, 0, 0);

	key_t chiavesem = ftok(".",'s');
	int ds_sem = semget(chiavesem,2, 0);

	//consuma_elementi(ds_sem,buf);

	return 0;
}
